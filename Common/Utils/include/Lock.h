/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
*/

#ifndef LOCK_H
#define LOCK_H

#include "PALThreading.h"

/**
 * Handles synchronization of for example a function in an
 * easy to use way. When declaring the local variable Lock lock(mutexPtr)
 * the constructor locks the mutex and when the mutex goes out of
 * scope, typically when reaching end of function the destructor
 * will be called automatically, hence the unlockMutex will be called.
 */
class Lock
{
public:

   /**
    * Constructor that takes a PAL mutex pointer and tries to
    * aquire a lock on the mutex.
    *
    * @param A mutex retreived from PALThreading createMutex function.
    */
   explicit Lock(Mutex* mutexPtr) : m_mutexPtr(mutexPtr) { 
      //coreprintln("Attempting to lock\n");
      // acquire resource
      pal_thread_lock_mutex(m_mutexPtr);
      m_locked = true;
      //coreprintln("Lock was successful\n");
   }

   /**
    * The destructor will try to unlock the mutex.
    */
   ~Lock() {
      //coreprintln("Attempting to unlock\n");
      // release resource
      if (m_locked) {
         pal_thread_unlock_mutex(m_mutexPtr); 
         //coreprintln("Unlock was successful\n");
      } 
   }

   void unlock() {
      pal_thread_unlock_mutex(m_mutexPtr);
      m_locked = false;
   }
private:

   /**
    * This represents a mutex pointer that is retreived from PALThreading
    * The pointer will not be destroyed in the destructor.
    */
   Mutex* m_mutexPtr;

   bool m_locked;
};

#endif /* LOCK_H */
