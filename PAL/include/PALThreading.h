/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
*/

#ifndef PALTHREADING_H
#define PALTHREADING_H

#ifdef __cplusplus
extern "C" {
#endif
   
#include "PAL.h"
#include "PALTypes.h"

/**
 * This file contains declarations for functions dealing
 * with threading related operations.
 */

//a helper typedef to make function pointing a bit easier to use
typedef void(*ThreadStartFunc)(void*);

struct Thread;
struct Mutex;
struct RecursiveMutex;
struct Conditional;

/**
 * Initialize the thread system. This needs to be called before doing any thread related
 * work.
 */
pstatus pal_thread_init();

/**
 * Deinitialize the thread system. This needs to be called before exiting the program, or it
 * will leak.
 */
pstatus pal_thread_deinit();
   
/**
 * Create a new thread.
 * 
 * @param function A function pointer for the thread to start in
 * @param data A parameter containing data to be passed as an argument to above
 *        function
 * @param name The name of the thread.
 * 
 * @return A Thread, or NULL if no thread could be created.
 */
Thread* pal_thread_create_thread(ThreadStartFunc function,
                                 void* data, const char* name = NULL);

/**
 * Start a thread.
 * 
 * @param thread The thread to start
 * 
 * @return The thread starting status
 */
pstatus pal_thread_start_thread(Thread* thread);

/**
 * Destroy a thread, as in remove the resources it occupies. Cannot be used to
 * stop a running thread.
 * 
 * @param thread The thread to destroy
 * 
 * @return The thread destruction status
 */
pstatus pal_thread_destroy_thread(Thread* thread);

/**
 * Get the currently executing thread.
 *
 * @return A pointer to the thread currently executing, or 0 when executed by the main thread.
 */
Thread* pal_thread_current_thread();

/**
 * Get the name of a thread
 *
 * @param thread the thread to get the name from
 *
 * @return the name of the thread
 */
const char* pal_thread_get_name(Thread* thread);

/**
 * Get the ID of the current thread
 *
 * @return the Thread ID
 */
wf_int32 pal_thread_get_current_thread_id();

/**
 * Join with another thread, try for at most a given amount of time.
 *
 * @param thread The thread to join with, ie the one that you want to wait for
 * to terminate.
 * @param millis the amount of milliseconds to wait for the join
 * @param nanos the amount of nanoseconds to wait for the join
 */
void pal_thread_join(Thread* thread, long millis, int nanos);

/**
 * Create a mutex.
 * 
 * @return The newly created mutex
 */
Mutex* pal_thread_create_mutex();

/**
 * Destroy a mutex.
 *
 * @param Mutex The mutex to destroy
 *
 * @return The Mutex destruction status
 */
pstatus pal_thread_destroy_mutex(Mutex* mutex);

/**
 * Lock a mutex.
 * 
 * @param thread The mutex to lock
 * 
 * @return The mutex lock status
 */
bool pal_thread_lock_mutex(Mutex* mutex);

/**
 * Unlock a mutex.
 * 
 * @param thread The mutex to unlock
 * 
 * @return The mutex unlock status
 */
bool pal_thread_unlock_mutex(Mutex* mutex);

/**
 * Trylock a mutex.
 * 
 * @param thread The mutex to trylock
 * 
 * @return The mutex trylock status
 */
bool pal_thread_try_lock_mutex(Mutex* mutex);

/**
 * Create a recursive mutex.
 * 
 * @return The newly created recursive mutex
 */
RecursiveMutex* pal_thread_create_recursive_mutex();

/**
 * Destroy a recursive mutex.
 *
 * @param recursiveMutex The recursive mutex to destroy
 *
 * @return The recursiveMutex destruction status
 */
pstatus pal_thread_destroy_recursive_mutex(RecursiveMutex* recursiveMutex);

/**
 * Lock a recursive mutex.
 * 
 * @param thread The recursive mutex to lock
 * 
 * @return The recursiveMutex lock status
 */
bool pal_thread_lock_recursive_mutex(RecursiveMutex* recursiveMutex);

/**
 * Unlock a recursive mutex.
 * 
 * @param thread The recursive mutex to unlock
 * 
 * @return The recursiveMutex unlock status
 */
bool pal_thread_unlock_recursive_mutex(RecursiveMutex* recursiveMutex);

/**
 * Trylock a recursive mutex.
 * 
 * @param thread The recursive mutex to trylock
 * 
 * @return The recursiveMutex trylock status
 */
bool pal_thread_try_lock_recursive_mutex(RecursiveMutex* recursiveMutex);

/**
 * Create a new conditional variable.
 * 
 * @return A Conditional, or NULL if no conditional could be created.
 */
Conditional* pal_thread_create_conditional();

/**
 * Destroy a conditional.
 * 
 * @param conditional The conditional to destroy
 * 
 * @return The conditional destruction status
 */
pstatus pal_thread_destroy_conditional(Conditional* conditional);

/**
 * Wait on a conditional. This will unlock the mutex before going into wait
 * mode, and locks the mutex when leaving wait mode.
 * The conditional is woken up by either signal, broadcast or the timeout
 * milliseconds.
 * 
 * @param conditional the conditional variable to use for waiting
 * @param mutex the mutex to use for locking associated with the this
 *              conditional waiting
 * @param timeout the amount of milliseconds to wait before waking up if no
 *                signal or broadcast has arrived. Infinite timeout can be achieved
 *                by passing 0 here.
 * 
 * @return PAL_OK if the conditional woke up from a signal or broadcast,
 *         PAL_CONDITIONAL_WOKE_UP_FROM_TIMEOUT if waking up from timeout
 */
pstatus pal_thread_conditional_wait(Conditional* conditional,
                                    Mutex* mutex, wf_uint32 timeout);

/**
 * Wait on a conditional. This will unlock the recursive mutex before going into wait
 * mode, and locks the recursive mutex when leaving wait mode.
 * The conditional is woken up by either signal, broadcast or the timeout
 * milliseconds.
 * 
 * @param conditional the conditional variable to use for waiting
 * @param recursiveMutex the recursiveMutex to use for locking associated with the this
 *              conditional waiting
 * @param timeout the amount of milliseconds to wait before waking up if no
 *                signal or broadcast has arrived. Infinite timeout can be achieved
 *                by passing 0 here.
 * 
 * @return PAL_OK if the conditional woke up from a signal or broadcast,
 *         PAL_CONDITIONAL_WOKE_UP_FROM_TIMEOUT if waking up from timeout
 */
pstatus pal_thread_conditional_wait_recursive(Conditional* conditional,
                                              RecursiveMutex* recursiveMutex,
                                              wf_uint32 timeout);

/**
 * Send a signal to a conditional, telling it to wake up
 *
 * @param conditional the conditional variable to wake up
 *
 * @return always returns PAL_OK
 */
pstatus pal_thread_conditional_signal(Conditional* conditional);

/**
 * Send a broadcast to a conditional, telling all threads waiting on it it to wake up
 *
 * @param conditional the conditional variable to wake up
 *
 * @return always returns PAL_OK
 */
pstatus pal_thread_conditional_broadcast(Conditional* conditional);

/**
 * Sleep the current thread
 * 
 * @param usecond The amount of milliseconds to sleep
 */
pstatus pal_thread_sleep(int mseconds);


#ifdef __cplusplus
} // End of extern "C"
#endif
#endif /* PALTHREADING_H */
