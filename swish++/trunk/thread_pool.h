/*
**	PJL C++ Library
**	thread_pool.h
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

#ifndef	thread_pool_H
#define	thread_pool_H

// standard
#include <pthread.h>
#include <queue>
#include <set>

// local
#include "fake_ansi.h"			/* for std */

#ifndef	PJL_NO_NAMESPACES
namespace PJL {
#else
#define	PJL /* nothing */
#endif

extern "C" void*	thread_main( void* );
extern "C" void		thread_destroy( void* );

//*****************************************************************************
//
// SYNOPSIS
//
	class thread_pool
//
// DESCRIPTION
//
//	A thread_pool pre-creates and manages a pool of persistent threads to
//	do tasks taken from a queue.  A thread takes a task off the queue,
//	performs it, then returns to the idle state.  Except as noted below, a
//	thread persists forever.  This improves performance because a given
//	task does not incur the thread creation/destruction cost.
//
//	The pool size grows dynamically when there are more tasks in the queue
//	than threads.  Threads will die off if idle past a timeout and the pool
//	will return to its original size.  (Which threads die off is
//	indeterminate, i.e., the original set of threads created are not
//	guaranteed to persist forever.  Once there are more threads than the
//	original size, all threads are equally likely to die off.)
//
//	If, for whatever reason, you don't want the dynamic behavior, just set
//	max_threads to be equal to min_threads.
//
// SEE ALSO
//
//	Bradford Nichols, Dick Buttlar, and Jacqueline Proulx Farrell.
//	"Pthreads Programming," O'Reilly & Associates, Sebastopol, CA, 1996.
//
//*****************************************************************************
{
	class thread;
	class q_lock;
	friend class	thread;
	friend void*	thread_main( void* );
	friend void	thread_destroy( void* );
public:
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	class thread {
		//
		// A thread is an abstract base class wrapped around a POSIX
		// thread, but in a thread pool context, i.e., it does all of
		// the grunt work of waiting for tasks and coordinating with
		// the thread pool object to which it belongs.
		//
		// The derived class author only has to override the two pure
		// virtual member functions of create() and main().  The
		// create() function should be of the form:
		//
		//	virtual thread* create( thread_pool &p ) const {
		//		return new Derived_class_name( p );
		//	}
		//
		// The main() function should do whatever the author wants
		// given the argument taken from the task queue.
		//
		// The derived class can have additional member functions it
		// calls itself and also additional data members it uses
		// itself.
		//
	public:
		virtual ~thread();

		union argument_type {
			//
			// Ideally, we'd like to allow thread::main() to take
			// whatever arguments the derived class author pleases;
			// however, C++ forces us to establish the function
			// signature upon declaration.  We therefore define
			// this argument_type to be either a simple integer or
			// a pointer.  If the derived class author wants to
			// pass more data that can be accomodated by either of
			// those, then s/he can use the pointer to point to a
			// dynamically allocated structure containing the real
			// data.  (S/he must also remember to delete said
			// structure when done with it.)
			//
			argument_type( long  a ) : i( a ) { }
			argument_type( void *a ) : p( a ) { }
			long	i;
			void*	p;
		};

		void operator delete( void*, size_t ) { }
		//
		// Override "delete" for a thread to do nothing, i.e., do not
		// deallocate memory for the object, because the POSIX thread
		// clean-up function still needs to access the thread object
		// after its destructor has been called.  The clean-up function
		// deallocates the object.
		//
	protected:
		typedef void* (*start_function_type)( void* );

		thread( thread_pool&, start_function_type = thread_main );

		virtual thread*	create( thread_pool& ) const = 0;
		virtual void	main( argument_type ) = 0;
	private:
		pthread_t	thread_;		// our POSIX thread
		pthread_mutex_t	run_lock_;
		thread_pool&	pool_;			// to which we belong
		q_lock*		q_lock_;
		bool		destructing_;		// destructor called?

		void		run() { ::pthread_mutex_unlock( &run_lock_ ); }
		thread*		create_and_run() const {
					thread *const t = create( pool_ );
					t->run();
					return t;
				}

		friend class	thread_pool;
		friend void*	thread_main( void* );
		friend void	thread_destroy( void* );

		thread( thread const& );		// forbid copy
		thread& operator=( thread const& );	// forbid assignment
	};

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	thread_pool( thread *prototype,
		int min_threads, int max_threads, int timeout
	);
	~thread_pool();

	void		new_task( thread::argument_type );
	//		Supply a new task to be worked upon by a thread.

	void		max_threads( int n ) 		{ max_threads_ = n; }
	void		min_threads( int n ) 		{ min_threads_ = n; }
	void		timeout( int t )		{ timeout_ = t; }
	//		Adjust the thread pool's parameters.  Such adjustments
	//		do not occur immediately, however; rather, the pool
	//		slowly adjusts as tasks are completed.
private:
	class q_lock {
		//
		// A q_lock is a reference-counted mutex used for the task
		// queue that is shared by the thread_pool and all of its
		// threads.  A reference count is needed because the mutex has
		// to persist until all the POSIX thread clean-up functions run
		// and that can be after the thread_pool object is destroyed
		// (which is why q_lock can't be an ordinary data member like
		// t_lock_ below).
		//
	public:
		pthread_mutex_t mutex_;

		q_lock()		{ count_ = 1; }
		~q_lock()		{ ::pthread_mutex_destroy( &mutex_ ); }

		q_lock*	inc_ref();
		void	dec_ref();
	private:
		int	count_;
	};

	typedef std::set< thread* > thread_set;
	typedef std::queue< thread::argument_type > task_queue_type;

	int		min_threads_, max_threads_;
	thread_set	threads_;
	pthread_mutex_t	t_lock_;

	int		t_busy_;			// how many are busy
	pthread_mutex_t	t_busy_lock_;
	pthread_cond_t	t_idle_;			// a thread is idle

	task_queue_type	queue_;
	q_lock*		q_lock_;
	pthread_cond_t	q_not_empty_;			// a task is available

	bool		destructing_;			// destructor called?
	int		timeout_;
};

#ifndef	PJL_NO_NAMESPACES
}
#endif

#endif	/* thread_pool_H */

#endif	/* MULTI_THREADED */
