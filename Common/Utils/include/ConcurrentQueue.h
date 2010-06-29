/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
*/

#ifndef CONCURRENT_QUEUE_H
#define CONCURRENT_QUEUE_H

#include "PALThreading.h"
#include "PALMachineTypes.h"
#include "Lock.h"
#include "NotCopyable.h"

#include <queue>

template <typename Type>
class ConcurrentQueue : private NotCopyable
{
public:

   /**
    * Creates a queue, a mutex and a conditional variable is created
    * in the constructor.
    */
   ConcurrentQueue() {
      m_mutex = pal_thread_create_mutex();
      m_condVar = pal_thread_create_conditional();
   }

   /**
    * Destroys the mutex and the conditional variable.
    */
   ~ConcurrentQueue() {
      pal_thread_destroy_mutex(m_mutex);
      pal_thread_destroy_conditional(m_condVar);
   }

   /**
    * Pushes a data object to the queue. After the object has been
    * pushed to the queue we release the mutex and then we notifies
    * the listener that is waiting for the message.
    *
    * @param data An object that should be pushed to the queue.
    */
   void push(Type const& data) {
      Lock lock(m_mutex);
      m_queue.push(data);
      lock.unlock();
      pal_thread_conditional_broadcast(m_condVar);
   }

   /**
    * Checks if the queue is empty or not.
    *
    * @return True if the queue is empty, false if not.
    */
   bool empty() const {
      Lock lock(m_mutex);
      return m_queue.empty();
   }

   /**
    * Tries to pop a value from the queue. The parameter
    * poppedValue will contain the oldest object on the
    * queue if this function returns true.
    *
    * @param poppedValue A reference to a data object that will contain
    *                    the data of the first object on the queue.
    * @return True if the queue wasn't empty, false if empty.
    */
   bool tryPop(Type& poppedValue) {
      Lock lock(m_mutex);
      if(m_queue.empty()) {
         return false;
      }
        
      poppedValue=m_queue.front();
      m_queue.pop();
      return true;
   }

   /**
    * Calling this function leads to the caller waiting
    * until there is a message on the queue. This will prevent
    * polling the queue.
    *
    * @param poppedValue A reference to a data object that will contain
    *                    the data of the first object on the queue.
    */
   void waitAndPop(Type& poppedValue) {
      Lock lock(m_mutex);
      while(m_queue.empty()) {
         pal_thread_conditional_wait(m_condVar, m_mutex,0);
      }
        
      poppedValue=m_queue.front();
      m_queue.pop();
   }

   /**
    * Calling this function leads to the caller waiting
    * until there is a message on the queue. This will prevent
    * polling the queue. This function basically do the same
    * as the waitAndPop(Type& poppedValue) but with addition this
    * function supports timeout. This means that the calling thread
    * will only wait until either a message gets pushed to the queue
    * or until the timout expires.
    *
    * @param poppedValue A reference to a data object that will contain
    *                    the data of the first object on the queue.
    * @return True if a message was pushed to the queue and we
    *         popped it. False if the timout expired and no value was
    *         retreived from the queue.
    */
   bool waitAndPop(Type& poppedValue, wf_uint32 timeout) {
      Lock lock(m_mutex);
      while(m_queue.empty()) {

         // Conditional wait with a timeout
         pstatus status =
            pal_thread_conditional_wait(m_condVar, m_mutex, timeout);
         
         if (status == PAL_CONDITIONAL_WOKE_UP_FROM_TIMEOUT) {
            return false;
         }
      }
      poppedValue=m_queue.front();
      m_queue.pop();
      return true;
   }

private:

   /**
    * A deque that will contain the type of data that this class
    * is initiated with.
    */
   std::queue<Type> m_queue;

   /**
    * Mutex used for doing this message class thread safe.
    */
   mutable Mutex* m_mutex;

   /**
    * This conditional member is used when doing a conditional
    * wait until a message has been pushed to the queue. When using
    * this, polling can be avoided.
    */
   Conditional* m_condVar;
};

#endif // End CONCURRENT_QUEUE_H
