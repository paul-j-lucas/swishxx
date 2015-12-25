/*
**      PJL C++ Library
**      thread_pool.h
**
**      Copyright (C) 1998-2015  Paul J. Lucas
**
**      This program is free software; you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation; either version 2 of the License, or
**      (at your option) any later version.
**
**      This program is distributed in the hope that it will be useful,
**      but WITHOUT ANY WARRANTY; without even the implied warranty of
**      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**      GNU General Public License for more details.
**
**      You should have received a copy of the GNU General Public License
**      along with this program; if not, write to the Free Software
**      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef thread_pool_H
#define thread_pool_H

// standard
#include <pthread.h>
#include <queue>
#include <set>

namespace PJL {

///////////////////////////////////////////////////////////////////////////////

extern "C" {
  typedef void* (*thread_start_function_type)( void* );

  void  thread_pool_decrement_busy( void* );
  void  thread_pool_thread_data_cleanup( void* );
  void* thread_pool_thread_main( void* );
  void  thread_pool_thread_once();
}

/**
 * A %thread_pool pre-creates and manages a pool of persistent threads to do
 * tasks taken from a queue.  A thread takes a task off the queue, performs it,
 * then returns to the idle state.  Except as noted below, a thread persists
 * forever.  This improves performance because a given task does not incur the
 * thread creation/destruction cost.
 *
 * The pool size grows dynamically when there are more tasks in the queue than
 * threads.  Threads will die off if idle past a timeout and the pool will
 * return to its original size.  (Which threads die off is indeterminate, i.e.,
 * the original set of threads created are not guaranteed to persist forever.
 * Once there are more threads than the original size, all threads are equally
 * likely to die off.)
 *
 * If, for whatever reason, you don't want the dynamic behavior, just set
 * \c max_threads to be equal to \c min_threads.
 *
 * See also:
 *    Bradford Nichols, Dick Buttlar, and Jacqueline Proulx Farrell.
 *    "Pthreads Programming," O'Reilly & Associates, Sebastopol, CA, 1996.
 */
class thread_pool {
public:
  class thread;
  friend class  thread;
  friend void   thread_pool_decrement_busy( void* );
  friend void*  thread_pool_thread_main( void* );

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  /**
   * A %thread is an abstract base class wrapped around a POSIX thread, but in
   * a thread pool context, i.e., it does all of the grunt work of waiting for
   * tasks and coordinating with the thread pool object to which it belongs.
   * 
   * The derived class author only has to override the two pure virtual member
   * functions of \c create() and \c main().  The \c create() function should
   * be of the form:
   * \code
   *    virtual thread* create( thread_pool &p ) const {
   *      return new Derived_class_name( p );
   *    }
   * \endcode
   * The \c main() function should do whatever the author wants given the
   * argument taken from the task queue.
   * 
   * The derived class can have additional member functions it calls itself and
   * also additional data members it uses itself.
   */
  class thread {
  public:
    virtual ~thread();

    /**
     * Ideally, we'd like to allow \c thread::main() to take whatever arguments
     * the derived class author pleases; however, C++ forces us to establish
     * the function signature upon declaration.  We therefore define this
     * %argument_type to be either a simple integer or a pointer.  If the
     * derived class author wants to pass more data that can be accomodated by
     * either of those, then s/he can use the pointer to point to a dynamically
     * allocated structure containing the real data.  (S/he must also remember
     * to delete said structure when done with it.)
     */
    union argument_type {
      argument_type( long  a ) : i( a ) { }
      argument_type( void *a ) : p( a ) { }
      long  i;
      void *p;
    private:
      argument_type() { }
      friend void* thread_pool_thread_main( void* );
    };

  protected:
    /**
     * Constructs (initializes) a thread by creating a POSIX thread and passing
     * it a pointer to ourselves.
     *
     * @param pool The thread_pool to which this thread belongs.
     * @param start_func The function that is called upon thread creation.
     */
    thread( thread_pool &pool,
            thread_start_function_type start_func = thread_pool_thread_main );

    virtual thread* create( thread_pool& ) const = 0;
    virtual void main( argument_type ) = 0;

  private:
    bool                  in_cleanup_;  // in clean-up func?
    thread_pool&          pool_;        // our owning pool
    pthread_mutex_t       run_lock_;
    pthread_t             thread_;      // our POSIX thread
    static pthread_key_t  thread_obj_key_;// thread object

    void run() {
      ::pthread_mutex_unlock( &run_lock_ );
    }

    thread* create_and_run() const {
      thread *const t = create( pool_ );
      t->run();
      return t;
    }

    friend class  thread_pool;
    friend void   thread_pool_decrement_busy( void* );
    friend void   thread_pool_thread_data_cleanup( void* );
    friend void*  thread_pool_thread_main( void* );
    friend void   thread_pool_thread_once();

    thread( thread const& );            // forbid copy
    thread& operator=( thread const& ); // forbid assignment
  };

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 /**
  * Constructs (initializes) a %thread_pool creating as many threads as
  * requested.
  *
  * @param prototype A pointer to an instance of a class derived from thread
  * used to create new instances of itself.
  * @param min_threads The minimum number of threads to keep available.
  * @param max_threads The maximum number of threads to allow.
  * @param timeout The number of seconds for a thread to wait for a task before
  * committing suicide, but only if more than \a min_threads presently exist.
  */
  thread_pool( thread *prototype, int min_threads, int max_threads,
               int timeout );

  ~thread_pool();

  /**
   * Supply a new task to be worked upon by a thread.
   *
   * @param arg The agument to pass to the new thread.
   * @param block Whether to block if all the threads are busy and no more can
   * be created.
   * @return Returns \c true if there is a thread to service the new task;
   * return \c false only if \a block is \c false and a new task can not be
   * queued.  (If \a block is \c true and all the threads are busy and no more
   * can be created, then the calling thread will block until a thread becomes
   * available.)
   */
  bool new_task( thread::argument_type arg, bool block = false );

  void max_threads( int n ) { max_threads_ = n; }
  void min_threads( int n ) { min_threads_ = n; }
  void timeout( int t )     { timeout_ = t; }
  //      Adjust the thread pool's parameters.  Such adjustments do not occur
  //      immediately, however; rather, the pool slowly adjusts as tasks are
  //      completed.

private:
  typedef std::set<thread*> thread_set;
  typedef std::queue<thread::argument_type> task_queue_type;

  int volatile    min_threads_, max_threads_;
  thread_set      threads_;
  pthread_mutex_t t_lock_;

  int volatile    t_busy_;              // how many are busy
  pthread_mutex_t t_busy_lock_;
  pthread_cond_t  t_idle_;              // a thread is idle

  task_queue_type queue_;
  pthread_mutex_t q_lock_;
  pthread_cond_t  q_not_empty_;         // a task is available

  bool            destructing_;         // destructor called?
  int volatile    timeout_;
};

///////////////////////////////////////////////////////////////////////////////

} // namespace PJL

#endif /* thread_pool_H */
/* vim:set et sw=2 ts=2: */
