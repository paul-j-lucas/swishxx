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

// local
#include "exit_codes.h"
#include "platform.h"
#include "thread_pool.h"
#include "util.h"

#ifndef	PJL_NO_NAMESPACES
using namespace std;
namespace PJL {
#endif

extern char const*	me;

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
#	ifdef DEBUG_threads
	cerr << "thread_destroy()" << endl;
#	endif

	thread_pool::thread *const t = static_cast<thread_pool::thread*>( p );
	if ( !t->destructing_ ) {
		//
		// The thread's destructor is not presently executing.  Set the
		// destructing_ flag so it will not call pthread_cancel() or
		// pthread_exit(), and then delete it.
		//
		t->destructing_ = true;
		delete t;
	}
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
			<< ::strerror( result ) << endl;
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

	while ( true ) {

#		ifdef DEBUG_threads
		cerr << "thread_main(): waiting for task" << endl;
#		endif

		result = 0;
		::pthread_mutex_lock( &t->pool_.q_lock_ );
		while ( t->pool_.queue_.empty() ) {
			::pthread_mutex_lock( &t->pool_.t_lock_ );
			if ( t->pool_.threads_.size() <= t->pool_.t_min_ ) {
				::pthread_mutex_unlock( &t->pool_.t_lock_ );
				//
				// There are no threads beyond those originally
				// created: wait indefinitely for a task.
				//
				::pthread_cond_signal( &t->pool_.t_idle_ );
				::pthread_cond_wait(
					&t->pool_.q_not_empty_,
					&t->pool_.q_lock_
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
				::pthread_mutex_unlock( &t->pool_.q_lock_ );
				delete t;
				::abort();	// should not get here
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
		cerr << "thread_main(): got task" << endl;
#		endif

		::pthread_mutex_lock( &t->pool_.t_busy_lock_ );
		++t->pool_.t_busy_;
		::pthread_mutex_unlock( &t->pool_.t_busy_lock_ );

		thread_pool::thread::argument_type const
			arg = t->pool_.queue_.front();
		t->pool_.queue_.pop();
		::pthread_mutex_unlock( &t->pool_.q_lock_ );

#		ifdef DEBUG_threads
		cerr << "thread_main(): performing task" << endl;
#		endif

		t->main( arg );			// do the real work

#		ifdef DEBUG_threads
		cerr << "thread_main(): completed task" << endl;
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
	thread_pool::thread::thread( thread_pool &p )
//
// DESCRIPTION
//
//	Construct (initialize) a thread by creating a POSIX thread and passing
//	it a pointer to ourselves.
//
//*****************************************************************************
	: pool_( p ), destructing_( false )
{
#	ifdef DEBUG_threads
	cerr << "thread::thread()" << endl;
#	endif

	int const result = ::pthread_create( &thread_, 0, thread_main, this );
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
	cerr << "thread::~thread()" << endl;
#	endif

	::pthread_mutex_lock( &pool_.destructing_lock_ );
	if ( pool_.destructing_ ) {
		::pthread_mutex_unlock( &pool_.destructing_lock_ );
		//
		// The thread pool to which we belong has had its destructor
		// called and is in the process of destroying itself and us.
		// Therefore, simply cancel our POSIX thread.
		//
		if ( !destructing_ ) {
			destructing_ = true;
			::pthread_cancel( thread_ );
		}
		return;
	}
	::pthread_mutex_unlock( &pool_.destructing_lock_ );

	//
	// Otherwise, we are committing suicide.  But first, we have to delete
	// the pointer to us in our thread pool's set of threads.
	//
	::pthread_mutex_lock( &pool_.t_lock_ );
	pool_.threads_.erase( this );
	::pthread_mutex_unlock( &pool_.t_lock_ );
	if ( !destructing_ ) {
		destructing_ = true;
		::pthread_exit( 0 );
		::abort();			// should not get here
	}
}

//*****************************************************************************
//
// SYNOPSIS
//
	thread_pool::thread_pool( thread const *prototype,
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
	: t_min_( min_threads ), t_max_( max_threads ),
	  destructing_( false ), timeout_( timeout )
{
	if (	::pthread_mutex_init( &t_busy_lock_, 0 ) ||
		::pthread_mutex_init( &destructing_lock_, 0 ) ||
		::pthread_mutex_init( &q_lock_, 0 ) ||
		::pthread_mutex_init( &t_lock_, 0 )
	) {
		error() << "could not init thread mutex" << endl;
		::exit( 1 );
	}
	if (	::pthread_cond_init( &q_not_empty_, 0 ) ||
		::pthread_cond_init( &t_idle_, 0 )
	) {
		error() << "could not init thread condition" << endl;
		::exit( 1 );
	}

	::pthread_mutex_lock( &t_lock_ );
	for ( int i = 0; i < t_min_; ++i )
		threads_.insert( prototype->create( *this ) );
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
	// anyway, and, more importantly, to make it call pthread_cancel() on
	// its POSIX thread rather than calling pthread_exit() on our POSIX
	// thread.
	//
	::pthread_mutex_lock( &destructing_lock_ );
	destructing_ = true;
	::pthread_mutex_unlock( &destructing_lock_ );

	::pthread_mutex_lock( &t_lock_ );
	for ( thread_set::iterator
		t = threads_.begin(); t != threads_.end(); ++t
	)
		delete *t;
	::pthread_mutex_unlock( &t_lock_ );

	::pthread_cond_destroy( &q_not_empty_ );
	::pthread_cond_destroy( &t_idle_ );
	::pthread_mutex_destroy( &t_lock_ );
	::pthread_mutex_destroy( &q_lock_ );
	::pthread_mutex_destroy( &destructing_lock_ );
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
	cerr << "thread_pool::new_task()" << endl;
#	endif

	::pthread_mutex_lock( &q_lock_ );
	queue_.push( arg );
	::pthread_cond_signal( &q_not_empty_ );
	::pthread_mutex_unlock( &q_lock_ );

	::pthread_mutex_lock( &t_lock_ );
	::pthread_mutex_lock( &t_busy_lock_ );
	if ( t_busy_ == threads_.size() ) {
		//
		// All the existing threads are busy.
		//
		::pthread_mutex_unlock( &t_busy_lock_ );
		if ( threads_.size() < t_max_ ) {
			//
			// We haven't maxed-out the number of threads we can
			// make, so create another one to handle the request.
			//
			threads_.insert( (*threads_.begin())->create( *this ) );
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
