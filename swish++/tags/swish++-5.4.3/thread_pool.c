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
#include "exit_codes.h"
#include "platform.h"
#include "thread_pool.h"
#include "util.h"

extern char const*	me;

#ifndef	PJL_NO_NAMESPACES
using namespace std;
namespace PJL {
#endif

//*****************************************************************************
//
// SYNOPSIS
//
	void thread_destroy( void *p )
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
	cerr << "thread_destroy(" << (long)t << ')' << endl;
#	endif
	//
	// If this thread was cancelled, it was probably cancelled while in a
	// pthread_cond_wait() with the q_lock mutex.  Since the mutex lock is
	// reacquired, we have to unlock it.  However, we don't know for sure
	// that the mutex was locked, so this mutex was created to have the
	// PTHREAD_MUTEX_ERRORCHECK attribute that renders unlocking a mutex
	// that is not locked harmless.  Note that this isn't true under
	// FreeBSD, unfortunately, because it doesn't support said attribute.
	//
	::pthread_mutex_unlock( &t->q_lock_->mutex_ );

	//
	// Now that we've unlocked the q_lock mutex, we're done with it:
	// decrememt the reference count.
	//
	t->q_lock_->dec_ref();

	if ( !t->destructing_ ) {
		//
		// The thread's destructor is not presently executing.  Set the
		// destructing_ flag so it will not call pthread_cancel() and
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
	void* thread_main( void *p )
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
			<< ::strerror( result ) << "\n";
		::exit( Exit_No_Detach_Thread );
	}

	//
	// Add our clean-up function to this POSIX thread.  This will ensure
	// that we get removed from our thread pool's set of threads even if
	// the derived class author calls pthread_exit() directly.
	//
	// Side note: we don't use our normal style of prefixing standard
	// library funtions with "::" since pthread_cleanup_push() can be
	// implemented as a macro and "::" would most likely cause weird syntax
	// errors during compilation.
	//
	pthread_cleanup_push( thread_destroy, p );

	register thread_pool::thread *const
		t = static_cast<thread_pool::thread*>( p );

	//
	// We need to wait for the "run" mutex to become unlocked before
	// continuing to run the main() function to ensure that the thread pool
	// object to which we belong has been fully constructed before we
	// access its data members.  (It becomes unlocked when the run() member
	// function is called.)
	//
	::pthread_mutex_lock( &t->run_lock_ );

	//
	// Since this is a new running thread that will make use of the q_lock,
	// increment its reference count.
	//
	t->q_lock_ = t->pool_.q_lock_->inc_ref();

	while ( true ) {

#		ifdef DEBUG_threads
		cerr << "thread_main(): waiting for task\n";
#		endif

		result = 0;
		::pthread_mutex_lock( &t->q_lock_->mutex_ );
		while ( t->pool_.queue_.empty() ) {
			::pthread_mutex_lock( &t->pool_.t_lock_ );
			if ( t->pool_.threads_.size() <= t->pool_.min_threads_){
				::pthread_mutex_unlock( &t->pool_.t_lock_ );
				//
				// There are no threads beyond those originally
				// created: wait indefinitely for a task.
				//
				::pthread_cond_signal( &t->pool_.t_idle_ );
				::pthread_cond_wait(
					&t->pool_.q_not_empty_,
					&t->q_lock_->mutex_
				);
				result = 0;	// ignore possible prior timeout
				continue;
			}
			::pthread_mutex_unlock( &t->pool_.t_lock_ );
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
				::pthread_mutex_unlock( &t->q_lock_->mutex_ );
				delete t;
				internal_error
					<< "thread_main(): "
					   "thread exists after "
					   "alleged destruction\n"
					<< report_error;
			}
			//
			// Wait only a finite amount of time for a task to
			// become available.
			//
			struct timespec future;
			future.tv_sec = ::time( 0 ) + t->pool_.timeout_;
			future.tv_nsec = 0;
			::pthread_cond_signal( &t->pool_.t_idle_ );
			result = ::pthread_cond_timedwait(
				&t->pool_.q_not_empty_, &t->q_lock_->mutex_,
				&future
			);
			//
			// Loop around again to retest the condition that there
			// are still more threads than the minimum.  We want to
			// be absolutely sure before we commit suicide.
			//
		}

#		ifdef DEBUG_threads
		cerr << "thread_main(): got task\n";
#		endif

		::pthread_mutex_lock( &t->pool_.t_busy_lock_ );
		++t->pool_.t_busy_;
		::pthread_mutex_unlock( &t->pool_.t_busy_lock_ );

		thread_pool::thread::argument_type const
			arg = t->pool_.queue_.front();
		t->pool_.queue_.pop();
		::pthread_mutex_unlock( &t->q_lock_->mutex_ );

#		ifdef DEBUG_threads
		cerr << "thread_main(): performing task\n";
#		endif

		t->main( arg );			// do the real work

#		ifdef DEBUG_threads
		cerr << "thread_main(): completed task\n";
#		endif

		::pthread_mutex_lock( &t->pool_.t_busy_lock_ );
		--t->pool_.t_busy_;
		::pthread_mutex_unlock( &t->pool_.t_busy_lock_ );
	}

	//
	// Although we never get here due to the infinite loop above, the
	// Pthread standard requires that a pthread_cleanup_pop() appear at the
	// end of the same scope a pthread_cleanup_push() appears in.
	//
	pthread_cleanup_pop( 0 );

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
	: pool_( p ), destructing_( false )
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
		error() << "could not init thread mutex\n";
		::exit( Exit_No_Init_Thread_Mutex );
	}
	::pthread_mutex_lock( &run_lock_ );

	int const result = ::pthread_create( &thread_, 0, start_func, this );
	if ( result ) {
		error()	<< "could not create thread: "
			<< ::strerror( result ) << "\n";
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
	::pthread_mutex_destroy( &run_lock_ );

	if ( !pool_.destructing_ ) {
		//
		// We are committing suicide.  But first, we have to delete the
		// pointer to us in our thread pool's set of threads.
		//
		::pthread_mutex_lock( &pool_.t_lock_ );
		pool_.threads_.erase( this );
		::pthread_mutex_unlock( &pool_.t_lock_ );
	} else {
		//
		// The thread pool to which we belong has had its destructor
		// called and is in the process of destroying itself and us.
		// Therefore, we don't have to do anything.
		//
	}

	if ( !destructing_ ) {
		//
		// We're not being called via the clean-up function, so set a
		// flag to prevent the clean-up function from calling us when
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
	void thread_pool::q_lock::dec_ref()
//
// DESCRIPTION
//
//	Decrement the reference count for the q_lock mutex.  If the count
//	becomes zero, delete ourselves.
//
//*****************************************************************************
{
	::pthread_mutex_lock( &mutex_ );
	if ( !--count_ )
		delete this;
	else
		::pthread_mutex_unlock( &mutex_ );
}

//*****************************************************************************
//
// SYNOPSIS
//
	thread_pool::q_lock* thread_pool::q_lock::inc_ref()
//
// DESCRIPTION
//
//	Increment the reference count for the q_lock mutex.
//
// RETURN VALUE
//
//	Returns "this" for convenience.
//
//*****************************************************************************
{
	::pthread_mutex_lock( &mutex_ );
	++count_;
	::pthread_mutex_unlock( &mutex_ );
	return this;
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
	  q_lock_( new q_lock ), destructing_( false ), timeout_( timeout )
{
	pthread_mutexattr_t at;
	if (	::pthread_mutexattr_init( &at ) ||
#ifndef	FreeBSD
		//
		// Apparantly, FreeBSD doesn't support "error checking" mutexes
		// so we can't do the line below which may result in undefined
		// behavior.  Oh well.
		//
		::pthread_mutexattr_settype( &at, PTHREAD_MUTEX_ERRORCHECK ) ||
#endif
		::pthread_mutex_init( &t_busy_lock_, 0 ) ||
		::pthread_mutex_init( &q_lock_->mutex_, &at ) ||
		::pthread_mutex_init( &t_lock_, 0 )
	) {
		error() << "could not init thread mutex\n";
		::exit( Exit_No_Init_Thread_Mutex );
	}
	::pthread_mutexattr_destroy( &at );
	if (	::pthread_cond_init( &q_not_empty_, 0 ) ||
		::pthread_cond_init( &t_idle_, 0 )
	) {
		error() << "could not init thread condition\n";
		::exit( Exit_No_Init_Thread_Condition );
	}

	::pthread_mutex_lock( &t_lock_ );
	threads_.insert( prototype );
	prototype->run();
	for ( int i = 1; i < min_threads_; ++i )
		threads_.insert( prototype->create_and_run() );
	::pthread_mutex_unlock( &t_lock_ );
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
	//
	// Set the destructing_ flag to prevent the thread destructor from
	// removing itself from our set since we're deleting the whole set
	// anyway.
	//
	destructing_ = true;

	::pthread_mutex_lock( &t_lock_ );
	for ( thread_set::iterator
		t = threads_.begin(); t != threads_.end(); ++t
	)
		delete *t;
	::pthread_mutex_unlock( &t_lock_ );

	//
	// Since this thread_pool is being destroyed, decrement the reference
	// count on the q_lock.  It may not become zero right now because there
	// are threads that are still in the process of being cancelled (which
	// is why a reference count is needed).
	//
	q_lock_->dec_ref();

	::pthread_cond_destroy( &t_idle_ );
	::pthread_cond_destroy( &q_not_empty_ );
	::pthread_mutex_destroy( &t_lock_ );
	::pthread_mutex_destroy( &t_busy_lock_ );
}

//*****************************************************************************
//
// SYNOPSIS
//
	void thread_pool::new_task( thread::argument_type arg )
//
// DESCRIPTION
//
//	Add a new task to the task queue for threads to work on.
//
// PARAMETERS
//
//	arg	The argument to pass to thread_main().
//
//*****************************************************************************
{
#	ifdef DEBUG_threads
	cerr << "thread_pool::new_task()\n";
#	endif

	::pthread_mutex_lock( &q_lock_->mutex_ );
	queue_.push( arg );
	::pthread_cond_signal( &q_not_empty_ );
	::pthread_mutex_unlock( &q_lock_->mutex_ );

	::pthread_mutex_lock( &t_lock_ );
	::pthread_mutex_lock( &t_busy_lock_ );
	if ( t_busy_ == threads_.size() ) {
		//
		// All the existing threads are busy.
		//
		::pthread_mutex_unlock( &t_busy_lock_ );
		if ( threads_.size() < max_threads_ ) {
			//
			// We haven't maxed-out the number of threads we can
			// make, so create another one to handle the request by
			// using the first thread in the pool as a prototype.
			//
			thread *const prototype = *threads_.begin();
			threads_.insert( prototype->create_and_run() );
		} else {
			//
			// We've maxed out the number of threads we can make,
			// so just wait until one becomes idle.
			//
			::pthread_cond_wait( &t_idle_, &t_lock_ );
		}
	} else
		::pthread_mutex_unlock( &t_busy_lock_ );
	::pthread_mutex_unlock( &t_lock_ );
}

#ifndef	PJL_NO_NAMESPACES
}
#endif

#endif	/* MULTI_THREADED */
