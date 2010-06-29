#include "PALThreading.h"
#include "JTC.h"
#include <cassert>

class PosixJTCThread : public JTCThread {
public:
   PosixJTCThread(ThreadStartFunc function, void* data, const char* name) :
      JTCThread(name),
      m_function(function),
      m_data(data),
      m_hasExited(false),
      m_hasBeenStarted(false),
      m_name(name)
      {
      }

   virtual ~PosixJTCThread() {
   }


   virtual void run() {
      m_hasBeenStarted = true;
      m_function(m_data);
      m_hasExited = true;
   }

   bool hasExited() {
      return m_hasExited;
   }

   bool hasBeenStarted() {
      return m_hasBeenStarted;
   }

   void setOwnerStruct(Thread* thread) {
      m_ownerStruct = thread;
   }

   Thread* getOwnerStruct() {
      return m_ownerStruct;
   }
   
   const char* getName() {
      return m_name;
   }

private:
   ThreadStartFunc m_function;
   void* m_data;
   bool m_hasExited;
   bool m_hasBeenStarted;
   Thread* m_ownerStruct;
   const char* m_name;
};

struct Thread {
   JTCHandleT<PosixJTCThread> thread;
};

struct Mutex {
   JTCMutex mutex;
};

struct RecursiveMutex {
   JTCRecursiveMutex recursiveMutex;
};

struct Conditional {
   JTCCond conditional;
};

pstatus pal_thread_init() {
   JTCInit::initJTC();
   return PAL_OK;
}

pstatus pal_thread_deinit() {
   delete JTCInit::g_jtcInitialize;
   JTCInit::g_jtcInitialize = NULL;
   return PAL_OK;
}

Thread* pal_thread_create_thread(ThreadStartFunc function,
                               void* data, const char* name) {
   Thread* thread;
   if((thread = new Thread())) {
      thread->thread = new PosixJTCThread(function, data, name);
      thread->thread->setOwnerStruct(thread);
      return thread;
   } else {
      return NULL;
   }
}

pstatus pal_thread_start_thread(Thread* thread)
{
   if(thread) {
      Thread* posixThread = thread;
      if(posixThread->thread) {
         posixThread->thread->start();
         return PAL_OK;
      } else {
         return PAL_ERR_CANNOT_START_BROKEN_THREAD;
      }
   } else {
      return PAL_ERR_CANNOT_START_NULL_THREAD;
   }
}

pstatus pal_thread_destroy_thread(Thread* thread)
{
   if(thread) {
      Thread* posixThread = thread;
      //actually the below code could be replaced with a simple 
      //delete posixThread;
      //but all these checks could be nice to have when replacing JTC with raw
      //pthreads, if we ever do that.

      
      //fist check if the thread has returned from it's run function:
      if (posixThread->thread && posixThread->thread->hasExited()) {
         //in this case the thread has cleaned itself up, so just delete the struct
         delete posixThread;
         return PAL_OK;
      }
      else {
         //it has not yet exited, so it might be still running, or it has not
         ///been started.
         if (posixThread->thread && posixThread->thread->hasBeenStarted()) { 
            return PAL_ERR_CANNOT_DESTROY_RUNNING_THREAD;           
         }
         else {
            //do not delete the JTC implementation, since it's cleaned up
            //automatically thanks to the autopointer style declaration in the
            //PosixThread struct
            //delete posixThread->thread;
            delete posixThread;
            return PAL_OK;
         }
      }
   }
   else {
      return PAL_ERR_CANNOT_DESTROY_NULL_THREAD;
   }
}

Thread* pal_thread_current_thread() {
   JTCThread* jtc = JTCThread::currentThread();
   PosixJTCThread* posixJTCThread = static_cast<PosixJTCThread*>(jtc);
   return posixJTCThread->getOwnerStruct();
}

const char* pal_thread_get_name(Thread* thread) {
   if(thread) {
      Thread* posixThread = thread;
      return posixThread->thread->getName();
   } else {
      return NULL;
   }
}

wf_int32 pal_thread_get_current_thread_id() {
   // TODO: This is a work around for getting the id for the thread on iPhone.
   return (wf_int32)JTCThreadId::self().getIdAsPthread();
//   return (wf_int32)JTCThreadId::self();
}

// void pal_thread_join(Thread thread) {
//    Thread* posixThread = thread;
//    posixThread->thread->join();
// }

// void pal_thread_join(Thread thread, long millis) {
//    Thread* posixThread = thread;
//    posixThread->thread->join(millis);
// }

void pal_thread_join(Thread* thread, long millis, int nanos) {
   Thread* posixThread = thread;
   posixThread->thread->join(millis, nanos);
}

Mutex* pal_thread_create_mutex() {
   Mutex* posixMutex;
   if((posixMutex = new Mutex)) {
      return posixMutex;
   } else {
      //could not even create a PosixMutex
      return NULL;
   }
}

pstatus pal_thread_destroy_mutex(Mutex* mutex) {
   Mutex* posixMutex = mutex;
   delete posixMutex;
   return PAL_OK;
}

bool pal_thread_lock_mutex(Mutex* mutex) {
   Mutex* posixMutex = mutex;
   posixMutex->mutex.lock();
   //currently JTC always return true on lock operations
   return true;
}

bool pal_thread_unlock_mutex(Mutex* mutex) {
   Mutex* posixMutex = mutex;
   posixMutex->mutex.unlock();
   //currently JTC always return true on unlock operations
   return true;
}

bool pal_thread_try_lock_mutex(Mutex* mutex) {
   Mutex* posixMutex = mutex;
   return posixMutex->mutex.trylock();
}

RecursiveMutex* pal_thread_create_recursive_mutex() {
   RecursiveMutex* posixRecursiveMutex;
   if((posixRecursiveMutex = new RecursiveMutex)) {
      return posixRecursiveMutex;

   } else {
      //could not even create a PosixRecursiveMutex
      return NULL;
   }
}

pstatus pal_thread_destroy_recursive_mutex(RecursiveMutex* recursiveMutex) {
   RecursiveMutex* posixRecursiveMutex = recursiveMutex;
   delete posixRecursiveMutex;
   return PAL_OK;
}

bool pal_thread_lock_recursive_mutex(RecursiveMutex* recursiveMutex) {
   RecursiveMutex* posixRecursiveMutex = recursiveMutex;
   return posixRecursiveMutex->recursiveMutex.lock();
}

bool pal_thread_unlock_recursive_mutex(RecursiveMutex* recursiveMutex) {
   RecursiveMutex* posixRecursiveMutex = recursiveMutex;
   return posixRecursiveMutex->recursiveMutex.unlock();
}

bool pal_thread_try_lock_recursive_mutex(RecursiveMutex* recursiveMutex) {
   RecursiveMutex* posixRecursiveMutex = recursiveMutex;
   return posixRecursiveMutex->recursiveMutex.trylock();
}

Conditional* pal_thread_create_conditional() {
   Conditional* posixConditional;
   if((posixConditional = new Conditional)) {
         return posixConditional;
   } else {
      return NULL;
   }
}

pstatus pal_thread_destroy_conditional(Conditional* conditional) {
   Conditional* posixConditional = conditional;
   delete posixConditional;
   return PAL_OK;   
}

pstatus pal_thread_conditional_wait(Conditional* conditional,
                                    Mutex* mutex, wf_uint32 timeout) {
   Conditional* posixConditional = conditional; 
   Mutex* posixMutex = mutex;  

   if (timeout == 0){
      // Never time out
      posixConditional->conditional.wait(posixMutex->mutex);
      return PAL_OK;
   } else {
      // Valid time out value
      bool status;
      status = posixConditional->conditional.wait(posixMutex->mutex, timeout);
      
      if(status == false) {
         return PAL_CONDITIONAL_WOKE_UP_FROM_TIMEOUT;
      } else {
         return PAL_OK;
      }
   }

}


pstatus pal_thread_conditional_wait_recursive(Conditional* conditional,
                                              RecursiveMutex* recursiveMutex,
                                              wf_uint32 timeout) {
   Conditional* posixConditional = conditional; 
   RecursiveMutex* posixRecursiveMutex = recursiveMutex;

   if (timeout == 0){
      // Never time out
      posixConditional->conditional.wait(posixRecursiveMutex->recursiveMutex);
      return PAL_OK;
   } else {
      bool status;
      status =
         posixConditional->conditional.wait(posixRecursiveMutex->recursiveMutex,
                                            timeout);

      if(status == false) {
         return PAL_CONDITIONAL_WOKE_UP_FROM_TIMEOUT;
      } else {
         return PAL_OK;      
      }
   }

}

pstatus pal_thread_conditional_signal(Conditional* conditional) {
   Conditional* posixConditional = conditional;
   posixConditional->conditional.signal();
   return PAL_OK;
}

pstatus pal_thread_conditional_broadcast(Conditional* conditional) {
   Conditional* posixConditional = conditional;
   posixConditional->conditional.broadcast();
   return PAL_OK;
}

pstatus pal_thread_sleep(int mseconds) {
   JTCThread::sleep(mseconds);
   return PAL_OK;
}
