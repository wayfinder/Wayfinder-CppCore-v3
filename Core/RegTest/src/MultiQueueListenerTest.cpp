/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "MULTIQUEUELISTENERTEST"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

// #include "CoreDebugPrint.h"
#include "MultiQueueListenerTest.h"
#include "PALThreadingTest.h"
#include "PAL.h"
#include "ConcurrentQueue.h"
#include <vector>

#define coreprintln printf

static std::vector<DataMessageInt> s_intMsgVec;
static std::vector<DataMessageString> s_strMsgVec;

typedef struct data {
   MultiQueueListenerTest* threadTest;
   MultiQueueListener* mql;
   Thread* thread;
} data_t;




MultiQueueListenerTest::MultiQueueListenerTest(const char* name)
   : RegressionTest(name, 30000)
{
   m_mql = new MultiQueueListener;
}

MultiQueueListenerTest::~MultiQueueListenerTest()
{
   delete m_mql;
}

void MultiQueueListenerTest::startTest()
{
   // Create some dummy so that we can push something to the queue handler
   for (unsigned short i = 0; i < 10; ++i) {
      DataMessageInt dataMessage(i);
      s_intMsgVec.push_back(dataMessage);
   }
   
   for (unsigned short i = 0; i < 10; ++i) {
      DataMessageString dataMessage("test");
      s_strMsgVec.push_back(dataMessage); 
   }

   // Create the write data and the write thread. The write thread
   // will push data to the queue handler
   data_t* writeData = new data_t;
   writeData->threadTest = this;
   writeData->mql = m_mql;
   bool res = createAndStartThread(m_condWriteThread, &mqlWriteFunc, writeData);
   if (!res) {
      coreprintln("Something went wrong creating the thread, "
                  "test has failed I will exit\n");
      delete writeData;
      return;
   }

   m_mql->startListen();

   setTestPassed();
}

bool MultiQueueListenerTest::createAndStartThread(Thread*& thread,
                                               ThreadStartFunc threadFunc,
                                               void* threadData)
{
   thread = pal_thread_create_thread(threadFunc, threadData);
   //coreprintln("thread = %p\n", thread);
   if(thread == NULL) {
      setTestFailed("Create thread failed");
      return false;
   }  else {
      static_cast<data_t*>(threadData)->thread = thread;

      pstatus pStatus = pal_thread_start_thread(thread);
      if(pStatus != PAL_OK) {
         setTestFailed("Start thread failed");
         return false;
      }
   }
   return true;
}

void MultiQueueListenerTest::doneWritingToQueue()
{
   m_mql->setDone();
}

void mqlWriteFunc(void* arg)
{
   pal_thread_sleep(10);
   data_t* data = static_cast<data_t*> (arg);

   unsigned int index = 0;
   while (index < s_intMsgVec.size() && index < s_strMsgVec.size()) {
      // DataMessage message(index);
      DataMessageInt message = s_intMsgVec.at(index);
      data->mql->pushQueue(message);
      // Sleep some, hopefully this forces a context switch
      pal_thread_sleep(10);

      DataMessageString strMsg = s_strMsgVec.at(index);
      data->mql->pushQueue(strMsg);
      pal_thread_sleep(10);
      index++;
   }
   // We are done pushing, the queue handler is no longer needed.
   data->threadTest->doneWritingToQueue();
}

/********************************************************************************/
/******************* MultiQueueListener *****************************************/
/********************************************************************************/
MultiQueueListener::MultiQueueListener() :
   m_continue(true)
{
   m_cond = pal_thread_create_conditional();
   m_mutex = pal_thread_create_mutex();
}

MultiQueueListener::~MultiQueueListener()
{
   // FIXME: join on threads and delete them!
   pal_thread_destroy_conditional(m_cond);
   pal_thread_destroy_mutex(m_mutex);
}

void MultiQueueListener::startListen()
{
   // Untill someone says that we are done we will wait for data on the queues
   while (m_continue) {
      pal_thread_conditional_wait(m_cond, m_mutex, 0);
      // Got here, this means someone put data on my queus, better check it out
      checkQueues();
   }
}

void MultiQueueListener::checkQueues()
{
   DataMessageInt intMsg;
   if (m_intQueue.tryPop(intMsg)) {
      //printf("Got data from intqueue: %d\n", intMsg.m_data);
   }
   DataMessageString strMsg;
   if (m_stringQueue.tryPop(strMsg)) {
      //printf("Got data from intqueue: %s\n", strMsg.m_data.c_str());
   }
}

void MultiQueueListener::pushQueue(DataMessageInt message)
{
   // Oops, got some data. We better put it on the queue. Thread contexts are a
   // bitch and we need to do parsing in the correct thread
   m_intQueue.push(message);
   // Hey, wake up. You need to take care of your queue
   notify();
}

void MultiQueueListener::pushQueue(DataMessageString message)
{
   // Another data, better put it on the other queue
   m_stringQueue.push(message);
   // Better wake him up
   notify();
}

void MultiQueueListener::notify()
{
   // Really wake them up, those lazy sons of ...
   pal_thread_conditional_broadcast(m_cond);
}

void MultiQueueListener::setDone()
{
   // Finaly, we are done and yet we passed the test
   m_continue = false;
   notify();
}
