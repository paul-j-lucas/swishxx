/*
**	PJL C++ Library
**	thread_pool.c
**
**	Copyright (C) 1998  Paul J. Lucas
**
**	This program is free software; you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation; either version 2 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program; if not, write to the Free Software
**	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifdef	MULTI_THREADED

// standard
#include <cstdlib>			/* for exit(2) */
#include <ctime>
#ifdef	DEBUG_threads
#include <iostream>
#endif

// local
#include "platform.h"
#include "thread_pool.h"
#include "util.h"

extern char const *me;

#ifndef	PJL_NO_NAMESPACES
using namespace std;
namespace PJL {
#endif

//
// Macros to wrap critical sections.
//
#define	DEFER_CANCEL() \
	{ int cancel_type; \
	::pthread_setcanceltype( PTHREAD_CANCEL_DEFERRED, &cancel_type )

#define	RESTORE_CANCEL() \
	::pthread_setcanceltype( cancel_type, 0 ); \
	::pthread_testcancel(); }

//
// Define a more robust mutex locking mechanism by ensuring that the mutex will
// be unlocked even if the thread is cancelled.  Also roll in optional critical
// section protection since we're doing pthread_setcanceltype() anyway.
//
// See also: Bradford Nichols, Dick Buttlar, and Jacqueline Proulx Farrell.
// "Pthreads Programming," O'Reilly & Associates, Sebastopol, CA, 1996.
// pp. 141-142.
//
#define	MUTEX_LOCK(M,D) { \
	int cancel_type; bool const defer_cancel = (D); \
	::pthread_setcanceltype( PTHREAD_CANCEL_DEFERRED, &cancel_type ); \
	pthread_cleanup_push( (void (*)(void*))::pthread_mutex_unlock, (M) ); \
	::pthread_mutex_lock( (M) ); \
	if ( D ) ; else { \
		::pthread_setcanceltype( cancel_type, 0 ); \
		::pthread_testcancel(); \
	}

#define	MUTEX_UNLOCK() \
	pthread_cleanup_pop( 1 ); \
	if ( defer_cancel ) { \
		::pthread_setcanceltype( cancel_type, 0 ); \
		::pthread_testcancel(); \
	} \
}

//*****************************************************************************
//
// SYNOPSIS
//
	void thread_pool_decrement_busy( void *p )
//
// DESCRIPTION
//
//	This is a clean-up function that is called to decrement the number of
//	threads that are "busy" performing a task.  It gets called either when
//	(1) thread::main() completes its task or (2) if the thread gets killed
//	while performing its task.  A clean-up function is used to guarantee
//	that this happends for case (2).
//
// NOTE
//
//	This function is declared extern "C" since it is called via the C
//	library function pthread_cleanup_push() and, because it's a C function,
//	it expects C linkage.
//
// PARAMETERS
//
//	p	Pointer to an instance of a thread_pool::thread.
//
//*****************************************************************************
{
	thread_pool::thread *const t = static_cast<thread_pool::thread*>( p );
	MUTEX_LOCK( &t->pool_.t_busy_lock_, true );
	--t->pool_.t_busy_;
	MUTEX_UNLOCK();
}

//*****************************************************************************
//
// SYNOPSIS
//
	void thread_pool_thread_cleanup( void *p )
//
// DESCRIPTION
//
//	This is the clean-up function that gets called upon a thread's death.
//	If the derived class author calls pthread_exit() directly, we must
//	ensure that the thread destructor gets called.
//
// NOTE
//
//	This function is declared extern "C" since it is called via the C
//	library function pthread_cleanup_push() and, because it's a C function,
//	it expects C linkage.
//
// PARAMETERS
//
//	p	Pointer to an instance of a thread_pool::thread.
//
//*****************************************************************************
{
	thread_pool::thread *const t = static_cast<thread_pool::thread*>( p );
#	ifdef DEBUG_threads
	cerr << "thread_pool_thread_cleanup(" << (long)t << ')' << endl;
#	endif
	if ( !t->destructing_ ) {
		//
		// The thread's destructor is not presently executing.  Change
		// the thread's state so it will not call pthread_cancel() and
		// then delete it.  Since thread::operator delete() is
		// overridden to do nothing, "delete" only calls the destructor
		// and no memory is deallocated.
		//
		t->destructing_ = true;
		delete t;
	}

	//
	// Since no memory is deallocated by thread::operator delete(), call
	// the global operator delete() to deallocate the storage for the
	// object.
	//
	::operator delete( t );
}

//*****************************************************************************
//
// SYNOPSIS
//
	void* thread_pool_thread_main( void *p )
//
// DESCRIPTION
//
//	This is the starting point of execution for a POSIX thread.  It waits
//	for a task to appear in its thread pool's task queue and performs the
//	task.  After completion, it waits for the next task.
//
//	If more threads currently exist than the minimum requested (because
//	more were created as a result of a high request load), then only wait a
//	finite amount of time for a task to appear.  If no task appears in that
//	amount of time, then the request load must've lessened so we can
//	destroy this POSIX thread.
//
// NOTE
//
//	This function is declared extern "C" since it is called via the C
//	library function pthread_create() and, because it's a C function, it
//	expects C linkage.
//
// PARAMETERS
//
//	p	Pointer to an instance of a thread_pool::thread.
//
// RETURN VALUE
//
//	This function never returns.  It goes away only when the thread is
//	destroyed.  The "return 0" at the end is just to make the C++ compiler
//	happy.
//
//*****************************************************************************
{
	int result = ::pthread_detach( ::pthread_self() );
	if ( result ) {
		error()	<< "could not detach thread: "
			<< ::strerror( result ) << endl;
		::exit( Exit_No_Detach_Thread );
	}

	//
	// Add our clean-up function to this POSIX thread.  This will ensure
	// that we get removed from our thread pool's set of threads even if
	// the derived class author calls pthread_exit() directly.
	//
	pthread_cleanup_push( thread_pool_thread_cleanup, p );

	register thread_pool::thread *const
		t = static_cast<thread_pool::thread*>( p );

	//
	// We need to wait for the "run" mutex to become unlocked before
	// continuing to run the main() function to ensure that the thread pool
	// object to which we belong has been fully constructed before we
	// access its data members.  (It becomes unlocked when the run() member
	// function is called.)
	//
	MUTEX_LOCK( &t->run_lock_, false );
	MUTEX_UNLOCK();

	while ( true ) {

#		ifdef DEBUG_threads
		cerr << "thread_pool_thread_main(): waiting for task" << endl;
#		endif

		thread_pool::thread::argument_type arg;
		result = 0;

		MUTEX_LOCK( &t->pool_.q_lock_, false );
		while ( t->pool_.queue_.empty() ) {
			bool no;
			MUTEX_LOCK( &t->pool_.t_lock_, false );
			no = t->pool_.threads_.size() <= t->pool_.min_threads_;
			::pthread_cond_signal( &t->pool_.t_idle_ );
			MUTEX_UNLOCK();
			if ( no ) {
				//
				// There are no threads beyond those originally
				// created: signal that we're idle and wait
				// indefinitely for a task.
				//
				::pthread_cond_wait(
					&t->pool_.q_not_empty_,
					&t->pool_.q_lock_
				);
				result = 0;	// ignore possible prior timeout
				continue;
			}
			//
			// More threads exist than the minimum.  Check to see
			// if we timed out waiting for a task on the last loop.
			//
			if ( result == ETIMEDOUT ) {
				//
				// No task became available: commit suicide by
				// deleting the instance of the thread: it will
				// exit the POSIX thread so the "delete" below
				// will never return.
				//
				delete t;
				internal_error
					<< "thread_pool_thread_main(): "
					   "thread exists after "
					   "alleged destruction"
					<< report_error;
			}
			//
			// Wait only a finite amount of time for a task to
			// become available.
			//
			struct timespec future;
			future.tv_sec = ::time( 0 ) + t->pool_.timeout_;
			future.tv_nsec = 0;
			result = ::pthread_cond_timedwait(
				&t->pool_.q_not_empty_, &t->pool_.q_lock_,
				&future
			);
			//
			// Loop around again to retest the condition that there
			// are still more threads than the minimum.  We want to
			// be absolutely sure before we commit suicide.
			//
		}

#		ifdef DEBUG_threads
		cerr << "thread_pool_thread_main(): got task" << endl;
#		endif

		DEFER_CANCEL();
		arg = t->pool_.queue_.front();
		t->pool_.queue_.pop();
		RESTORE_CANCEL();
		MUTEX_UNLOCK(); // t->pool_.q_lock_

		MUTEX_LOCK( &t->pool_.t_busy_lock_, true );
		++t->pool_.t_busy_;
		MUTEX_UNLOCK();
		pthread_cleanup_push( thread_pool_decrement_busy, t );

#		ifdef DEBUG_threads
		cerr << "thread_pool_thread_main(): performing task" << endl;
#		endif

		t->main( arg );			// do the real work

#		ifdef DEBUG_threads
		cerr << "thread_pool_thread_main(): completed task" << endl;
#		endif

		pthread_cleanup_pop( true );
	}

	//
	// Although we never get here due to the infinite loop above, the
	// Pthread standard requires that a pthread_cleanup_pop() appear at the
	// end of the same scope a pthread_cleanup_push() appears in.
	//
	pthread_cleanup_pop( false );

	return 0;				// just make the compiler happy
}

//*****************************************************************************
//
// SYNOPSIS
//
	thread_pool::thread::thread(
		thread_pool &p, start_function_type start_func
	)
//
// DESCRIPTION
//
//	Construct (initialize) a thread by creating a POSIX thread and passing
//	it a pointer to ourselves.
//
// PARAMETERS
//
//	p		The thread_pool to which this thread belongs.
//
//	start_func	The function that is called upon thread creation.
//
//*****************************************************************************
	: destructing_( false ), pool_( p )
{
#	ifdef DEBUG_threads
	cerr << "thread::thread(" << (long)this << ')' << endl;
#	endif

	//
	// Create a locked "run" mutex that the soon-to-be-created thread will
	// wait for before running the code in the start function that accesses
	// our data members.  The only reason this is necessary is because the
	// prototype thread that is passed to the thread_pool constructor is
	// created before the thread_pool is fully constructed.
	//
	if ( ::pthread_mutex_init( &run_lock_, 0 ) ) {
		error() << "could not init thread mutex" << endl;
		::exit( Exit_No_Init_Thread_Mutex );
	}
	::pthread_mutex_lock( &run_lock_ );

	int const result = ::pthread_create( &thread_, 0, start_func, this );
	if ( result ) {
		error()	<< "could not create thread: "
			<< ::strerror( result ) << endl;
		::exit( Exit_No_Create_Thread );
	}
}

//*****************************************************************************
//
// SYNOPSIS
//
	/* virtual */ thread_pool::thread::~thread()
//
// DESCRIPTION
//
//	Destroy a thread.
//
//*****************************************************************************
{
#	ifdef DEBUG_threads
	cerr << "thread::~thread(" << (long)this << ')' << endl;
#	endif

	DEFER_CANCEL();
	::pthread_mutex_destroy( &run_lock_ );

	if ( !pool_.destructing_ ) {
		//
		// We are committing suicide.  But first, we have to delete the
		// pointer to us in our thread pool's set of threads.
		//
		MUTEX_LOCK( &pool_.t_lock_, false );
		pool_.threads_.erase( this );
		MUTEX_UNLOCK();
	} else {
		//
		// The thread pool to which we belong has had its destructor
		// called and is in the process of destroying itself and us.
		// Therefore, we don't have to do anything.
		//
	}
	RESTORE_CANCEL();

	if ( !destructing_ ) {
		//
		// We're not being called via the clean-up function, so change
		// state to prevent the clean-up function from calling us when
		// it eventually runs.
		//
		destructing_ = true;
		if ( ::pthread_equal( thread_, ::pthread_self() ) ) {
			//
			// This thread is committing suicide because there are
			// more than the minimum number of threads and it timed
			// out waiting for a task: call pthread_exit() in this
			// case since it's cleaner than pthread_cancel().
			//
			::pthread_exit( 0 );
		} else
			::pthread_cancel( thread_ );
	}
}

//*****************************************************************************
//
// SYNOPSIS
//
	thread_pool::thread_pool( thread *prototype,
		int min_threads, int max_threads, int timeout
	)
//
// DESCRIPTION
//
//	Construct (initialize) a thread_pool creating as many threads as
//	requested.
//
// PARAMETERS
//
//	prototype	A pointer to an instance of a class derived from
//			"thread" used to create new instances of itself.
//
//	min_threads	The minimum number of threads to keep available.
//
//	max_threads	The maximum number of threads to allow.
//
//	timeout		The number of seconds for a thread to wait for a task
//			before committing suicide, but only if more than
//			min_threads presently exist.
//
//*****************************************************************************
	: min_threads_( min_threads ), max_threads_( max_threads ),
	  t_busy_( 0 ), destructing_( false ), timeout_( timeout )
{
	if (	::pthread_mutex_init( &t_busy_lock_, 0 ) ||
		::pthread_mutex_init( &q_lock_, 0 ) ||
		::pthread_mutex_init( &t_lock_, 0 )
	) {
		error() << "could not init thread mutex" << endl;
		::exit( Exit_No_Init_Thread_Mutex );
	}
	if (	::pthread_cond_init( &q_not_empty_, 0 ) ||
		::pthread_cond_init( &t_idle_, 0 )
	) {
		error() << "could not init thread condition" << endl;
		::exit( Exit_No_Init_Thread_Condition );
	}

	MUTEX_LOCK( &t_lock_, true );
	threads_.insert( prototype );
	prototype->run();
	for ( int i = 1; i < min_threads_; ++i )
		threads_.insert( prototype->create_and_run() );
	MUTEX_UNLOCK();
}

//*****************************************************************************
//
// SYNOPSIS
//
	thread_pool::~thread_pool()
//
// DESCRIPTION
//
//	Destroy a thread_pool by destroying all the threads in it.
//
//*****************************************************************************
{
	DEFER_CANCEL();
	//
	// Set the destructing_ flag to prevent the thread destructor from
	// removing itself from our set since we're deleting the whole set
	// anyway.
	//
	destructing_ = true;

	MUTEX_LOCK( &t_lock_, false );
	for ( thread_set::iterator
		t = threads_.begin(); t != threads_.end(); ++t
	)
		delete *t;
	MUTEX_UNLOCK();

	::pthread_cond_destroy( &t_idle_ );
	::pthread_cond_destroy( &q_not_empty_ );
	::pthread_mutex_destroy( &t_lock_ );
	::pthread_mutex_destroy( &t_busy_lock_ );
	::pthread_mutex_destroy( &q_lock_ );
	RESTORE_CANCEL();
}

//*****************************************************************************
//
// SYNOPSIS
//
	bool thread_pool::new_task( thread::argument_type arg, bool block )
//
// DESCRIPTION
//
//	Add a new task to the task queue for threads to work on.
//
// PARAMETERS
//
//	arg	The argument to pass to thread_pool_thread_main().
//
//	block	If all the threads are busy and no more can be created and
//		block = true, then the calling thread will block until a thread
//		becomes available.
//
// RETURN VALUE
//
//	Returns false only if block = false and the task can noe be queued
//	because all the threads are busy.
//
//*****************************************************************************
{
#	ifdef DEBUG_threads
	cerr << "thread_pool::new_task()" << endl;
#	endif

	bool all_are_busy, queue_task = true;

	MUTEX_LOCK( &t_lock_, false );
	MUTEX_LOCK( &t_busy_lock_, false );
	all_are_busy = t_busy_ == threads_.size();
	MUTEX_UNLOCK();

	if ( all_are_busy ) {
		if ( threads_.size() < max_threads_ ) {
			//
			// We haven't maxed-out the number of threads we can
			// make, so create another one to handle the request by
			// using the first thread in the pool as a prototype.
			//
#			ifdef DEBUG_threads
			cerr << "creating a new thread" << endl;
#			endif
			thread *const prototype = *threads_.begin();
			DEFER_CANCEL();
			threads_.insert( prototype->create_and_run() );
			RESTORE_CANCEL();
		} else if ( block ) {
			//
			// We've maxed out the number of threads we can make,
			// so just wait until one becomes idle.
			//
#			ifdef DEBUG_threads
			cerr << "waiting for idle thread" << endl;
#			endif
			::pthread_cond_wait( &t_idle_, &t_lock_ );
		} else
			queue_task = false;
	}
	MUTEX_UNLOCK(); // t_lock_

	if ( queue_task ) {
		MUTEX_LOCK( &q_lock_, true );
		queue_.push( arg );
		::pthread_cond_signal( &q_not_empty_ );
		MUTEX_UNLOCK();
	}

	return queue_task;
}

#ifndef	PJL_NO_NAMESPACES
}
#endif

#endif	/* MULTI_THREADED */
