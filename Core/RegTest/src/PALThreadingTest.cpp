/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "PALTHREADINGTEST"
#define CORE_LOGGING_MODULE_LEVEL NONE

// #include "CoreDebugPrint.h"
#include "PALThreadingTest.h"
#include "PAL.h"
#include <cstring>
#include "CoreDebugPrint.h"

//#define coreprintln printf

typedef struct data {
   int i;
   PALThreadingTest* threadTest;
   Thread* thread;
} data_t;

PALThreadingTest::PALThreadingTest(const char* name)
   : RegressionTest(name),
     m_res(0),
     m_data_ready(false)
{
   m_condmut = pal_thread_create_recursive_mutex();
   m_cond = pal_thread_create_conditional();
   //not needed if done in the RegressionTestEnvironment:
   //initThread();
}

PALThreadingTest::~PALThreadingTest()
{
   pal_thread_destroy_conditional(m_cond);
   pal_thread_destroy_recursive_mutex(m_condmut);

   //not needed if done in the RegressionTestEnvironment:
   //deinitThread();
}


void PALThreadingTest::startTest()
{
   pstatus pStatus;
   data_t* data1 = new data_t;

   //first try to create a thread that we never use:
   Thread* thread0 = pal_thread_create_thread(&threadFunc, data1);
   //if this gives a memory leak, something is wrong.
   pal_thread_destroy_thread(thread0);
   thread0 = NULL;
   
   //now create a mutex that the two threads will use to protect shared data:
   m_mutex = pal_thread_create_mutex();
   if(m_mutex == NULL) {
      setTestFailed("Create mutex failed");
      return;
   }
   //create a recursive mutex too:
   m_recursiveMutex = pal_thread_create_recursive_mutex();
   if(m_recursiveMutex == NULL) {
      setTestFailed("Create recursive mutex failed");
      return;
  }

   //set some data:
   data1->i = 3;
   data1->threadTest = this;

   //try to print a pointer to the main thread:
   coreprintln("main thread: currentThread()=%p\n", currentThread());
   coreprintln("main thread: getCurrentThreadID()=%d\n", getCurrentThreadID());
   
   //create the first thread to use for processing:
   //also, give this thread a name.
   Thread* thread1 = pal_thread_create_thread(&threadFunc, data1, "thread1");
   coreprintln("thread1=%p\n", thread1);
   if(thread1 == NULL) {
      setTestFailed("Create thread failed");
      return;
   }  else {
      int res = strcmp(pal_thread_get_name(thread1), "thread1");
      if(res != 0) {
         setTestFailed("getThreadName() broken");
         return;
      }
      data1->thread = thread1;
      pStatus = pal_thread_start_thread(thread1);
      if(pStatus != PAL_OK) {
         setTestFailed("Start thread failed");
         return;
      }
   }

   //create another set of data
   data_t* data2 = new data_t;
   data2->i = 3;
   data2->threadTest = this;
   Thread* thread2 = pal_thread_create_thread(&threadFunc, data2);
   coreprintln("thread2=%p\n", thread2);
   if(thread2 == NULL) {
      setTestFailed("Create thread failed");
      return;
   }  else {
      data2->thread = thread2;

      pStatus = pal_thread_start_thread(thread2);
      if(pStatus != PAL_OK) {
         setTestFailed("Start thread failed");
         return;
      }
   }

   int i = 0;
   int timeout = 10000; //10 seconds
   //now wait for the threads to do their work:
   while(m_res != 4 && i < timeout) {
      //do nothing
      usleep(1);
      i++;
   }
   if(i == timeout) {
      setTestFailed("Doing thread work failed");
      return;
   }
   //getting this far means mutexes and recursive mutexes work.
   

   
   //now lets try if the conditionals work
   //create another set of data
   //used for the conditional waiting thread
   data_t* data3 = new data_t;
   data3->i = 4;
   data3->threadTest = this;
   Thread* thread3 = pal_thread_create_thread(&threadFunc2, data3);
   coreprintln("thread3=%p\n", thread3);
   if(thread3 == NULL) {
      setTestFailed("Create thread failed");
      return;
   }  else {
      data3->thread = thread3;

      pStatus = pal_thread_start_thread(thread3);
      if(pStatus != PAL_OK) {
         setTestFailed("Start conditional testing thread failed");
         return;
      }
   }

   //sleep to let thread3 enter wait
   pal_thread_sleep(20);
   
   //create another set of data
   //used for the conditional signal thread
   data_t* data4 = new data_t;
   data4->i = 5;
   data4->threadTest = this;
   Thread* thread4 = pal_thread_create_thread(&threadFunc3, data4);
   coreprintln("thread4=%p\n", thread2);
   if(thread4 == NULL) {
      setTestFailed("Create thread failed");
      return;
   }  else {
      data4->thread = thread4;

      pStatus = pal_thread_start_thread(thread4);
      if(pStatus != PAL_OK) {
         setTestFailed("Start conditional testing thread 2 failed");
         return;
      }
   }

   pal_thread_join(thread4,0,0);
   pal_thread_sleep(100); // let the other threads finish


   //now clean up the resources we used:
   delete data1;
   data1 = NULL;
   delete data2;
   data2 = NULL;
   pal_thread_destroy_mutex(m_mutex);
   m_mutex = NULL;
   pal_thread_destroy_recursive_mutex(m_recursiveMutex);
   m_recursiveMutex = NULL;
   pal_thread_destroy_thread(thread1);
   thread1 = NULL;
   pal_thread_destroy_thread(thread2);
   thread2 = NULL;

   delete data3;
   data1 = NULL;
   delete data4;
   data2 = NULL;
   pal_thread_destroy_thread(thread3);
   thread3 = NULL;
   pal_thread_destroy_thread(thread4);
   thread4 = NULL;
}


void PALThreadingTest::threadJobfunction(void* arg) {
   int ii;
   data_t* data = (data_t*)arg;
   pal_thread_lock_mutex(m_mutex);
   coreprintln("locked mutex, m_res = %d\n", m_res);
   pal_thread_lock_recursive_mutex(m_recursiveMutex);
   coreprintln("locked recursive mutex, m_res = %d\n", m_res);
   data->i = 2;
   for(int i=0; i<=10; i++) {
      //coreprintln("Doing work in a thread...");
      ii=i/(i+1);
      pal_thread_sleep(1);
   }
   coreprintln("current thread=%p\n", data->thread);
   coreprintln("currentThread()=%p\n", currentThread());
   coreprintln("getThreadName(currentThread())=%s\n", getThreadName(currentThread()));
   coreprintln("work thread: getCurrentThreadID()=%d\n", getCurrentThreadID());
   
   m_res += 1;
   threadJobfunctionRecursive(data);
   pal_thread_unlock_recursive_mutex(m_recursiveMutex);
   coreprintln("unlocked recursive mutex\n");
   pal_thread_unlock_mutex(m_mutex);
   coreprintln("unlocked mutex\n");

}


void PALThreadingTest::threadJobfunctionRecursive(void* arg) {
   int ii;
   data_t* data = (data_t*)arg;
   pal_thread_lock_recursive_mutex(m_recursiveMutex);
   coreprintln("locked recursive mutex, m_res = %d\n", m_res);
   data->i = 2;
   for(int i=0; i<=10; i++) {
      //coreprintln("Doing work in a thread...");
      ii=i/(i+1);
      pal_thread_sleep(1);
   }
   coreprintln("current thread=%p\n", data->thread);
   m_res += 1;
   pal_thread_unlock_recursive_mutex(m_recursiveMutex);
   coreprintln("unlocked recursive mutex\n");

}

void threadFunc(void* arg)
{
   data_t* data = static_cast<data_t*> (arg);
   data->threadTest->threadJobfunction(arg);
}

void threadFunc2(void* arg)
{
   data_t* data = static_cast<data_t*> (arg);
   data->threadTest->wait_for_data_to_process();
}

void threadFunc3(void* arg)
{
   data_t* data = static_cast<data_t*> (arg);
   data->threadTest->prepare_data_for_processing();
}


void PALThreadingTest::process_data() {
   //do data processing here...
   //getting here means we passed the test
   setTestPassed();
}

void PALThreadingTest::wait_for_data_to_process()
{
   pal_thread_lock_recursive_mutex(m_condmut);
   while(!m_data_ready)
   {
      coreprintln("Entering conditional wait...\n");
      pal_thread_conditional_wait_recursive(m_cond, m_condmut,0);
      coreprintln("Leaving conditional wait!\n");
   }
   process_data();
   pal_thread_unlock_recursive_mutex(m_condmut);
}

void PALThreadingTest::prepare_data_for_processing()
{
   //retrieve_data();
   //prepare_data();
   pal_thread_lock_recursive_mutex(m_condmut);
   m_data_ready=true;
   pal_thread_unlock_recursive_mutex(m_condmut);

   coreprintln("About to signal the conditional\n");
   pal_thread_sleep(30);
   pal_thread_conditional_broadcast(m_cond);
}
