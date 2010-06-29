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
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

// #include "CoreDebugPrint.h"
#include "ConcurrentQueueTest.h"
#include "PALThreadingTest.h"
#include "PAL.h"
#include "ConcurrentQueue.h"
#include <vector>

//#define coreprintln printf
#define coreprintln(...) ((void)0)
static std::vector<DataMessage> s_msgVec;
bool s_continueRead;

typedef struct data {
   ConcurrentQueueTest* threadTest;
   ConcurrentQueue<DataMessage>* queue;
   Thread* thread;
} data_t;

ConcurrentQueueTest::ConcurrentQueueTest(const char* name)
   : RegressionTest(name, 30000)
{
   m_queue = new ConcurrentQueue<DataMessage>();
   coreprintln("The pointer value of m_queue is = %p\n", m_queue);
   s_continueRead = true;
   m_doneReadingFromQueue = false;
}

ConcurrentQueueTest::~ConcurrentQueueTest()
{
   delete m_queue;
}

void ConcurrentQueueTest::startTest()
{
   for (unsigned short i = 0; i < 10; ++i) {
      DataMessage dataMessage(i);
      s_msgVec.push_back(dataMessage);
   }
   // pstatus pStatus;


   // Create a read data object and a read data thread, then start it.
   // The read thread will wait untill the write thread has pushed a
   // message to the queue.
   data_t* readData = new data_t;
   readData->threadTest = this;
   readData->queue = m_queue;
   bool res = createAndStartThread(m_condReadThread, &readFunc, readData);
   if (!res) {
      coreprintln("Something went wrong creating the thread, test has failed I will exit\n");
      delete readData;
      return;
   }

   // Create the write data and the write thread. The write thread
   // will push messages to the shared queue.
   data_t* writeData = new data_t;
   writeData->threadTest = this;
   writeData->queue = m_queue;
   res = createAndStartThread(m_condWriteThread, &writeFunc, writeData);
   if (!res) {
      coreprintln("Something went wrong creating the thread, "
                  "test has failed I will exit\n");
      delete writeData;
      return;
   }

   // wait for the threads to exit:
   pal_thread_join(m_condReadThread,0,0);
   pal_thread_join(m_condWriteThread,0,0);

   pal_thread_destroy_thread(m_condReadThread);
   pal_thread_destroy_thread(m_condWriteThread);

   delete readData;
   readData = NULL;
   
   readData = new data_t;
   readData->threadTest = this;
   readData->queue = m_queue;
   res = createAndStartThread(m_mutexReadThread, &pollReadFunc, readData);
   coreprintln("m_mutexReadThread = %p\n", m_mutexReadThread);
   if (!res) {
      coreprintln("Something went wrong creating the thread, "
                  "test has failed I will exit\n");
      delete readData;
      return;
   }

   // Create the write data and the write thread. The write thread
   // will push messages to the shared queue.
   delete writeData;
   writeData = NULL;

   writeData = new data_t;
   writeData->threadTest = this;
   writeData->queue = m_queue;
   res = createAndStartThread(m_mutexWriteThread, &writeFunc, writeData);
   if (!res) {
      coreprintln("Something went wrong creating the thread, "
                  "test has failed I will exit\n");
      delete writeData;
      return;
   }

   // wait for the threads to exit:
   pal_thread_join(m_mutexWriteThread,0,0);
   pal_thread_join(m_mutexReadThread,0,0);

   delete readData;
   readData = NULL;
   delete writeData;
   writeData = NULL;

   pal_thread_destroy_thread(m_mutexReadThread);
   pal_thread_destroy_thread(m_mutexWriteThread);

   setTestPassed();
}

bool ConcurrentQueueTest::createAndStartThread(Thread*& thread,
                                               ThreadStartFunc threadFunc,
                                               void* threadData)
{
   thread = pal_thread_create_thread(threadFunc, threadData);
   coreprintln("thread = %p\n", thread);
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

void ConcurrentQueueTest::doneWritingToQueue()
{
   s_continueRead = false;
}

void ConcurrentQueueTest::doneReadingFromQueue(bool success)
{
   s_continueRead = true;
   if (success) {
      m_doneReadingFromQueue = true;
   } else {
      setTestFailed("Failed when doing wait read, not all messages was read"
                    "from the queue");
   }
}

void ConcurrentQueueTest::donePollingFromQueue(bool success)
{
   if (success) {
      m_doneReadingFromQueue = true;
   } else {
      setTestFailed("Failed when doing polling read, not all messages was read"
                    "from the queue");
   }  
}

void pollReadFunc(void* arg)
{
   data_t* data = static_cast<data_t*> (arg);
   DataMessage message;
   std::vector<DataMessage> resVec;
   resVec.reserve(10);
   while (s_continueRead) {
      if (data->queue->tryPop(message) == false) {
         // Qeueu was empty, sleep for a while
         pal_thread_sleep(10);
      } else {
         // We got a message from the queue
         coreprintln("queue.waitAndPop returned true\n");
         coreprintln("DataMessage.m_data = %d\n", message.m_data);
         resVec.push_back(message);
      }
   }

   if (resVec.size() == s_msgVec.size() &&
       std::equal(s_msgVec.begin(), s_msgVec.end(), resVec.begin()) == true) {
      coreprintln("All messages that was pushed to the queue has "
                  "now been successfully read\n");
      data->threadTest->donePollingFromQueue(true);
   } else {
      coreprintln("Sorry but we did not read all the messages from the queue\n");
      data->threadTest->donePollingFromQueue(false);
   }
}

void readFunc(void* arg)
{
   unsigned int timeout = 500;
   data_t* data = static_cast<data_t*> (arg);
   DataMessage message;
   std::vector<DataMessage> resVec;
   resVec.reserve(10);
   coreprintln("The pointer value of data->queue is = %p\n", data->queue);
   while(s_continueRead) {
      if (data->queue->waitAndPop(message, timeout) == false) {
         coreprintln("queue.waitAndPop returned false\n");
      } else {
         coreprintln("queue.waitAndPop returned true\n");
         coreprintln("DataMessage.m_data = %d\n", message.m_data);
         resVec.push_back(message);
      }
   }
   coreprintln("Done reading from queue\n");

   if (resVec.size() == s_msgVec.size() &&
       std::equal(s_msgVec.begin(), s_msgVec.end(), resVec.begin()) == true) {
      coreprintln("All messages that was pushed to the queue has now been successfully read\n");
      data->threadTest->doneReadingFromQueue(true);
   } else {
      coreprintln("Sorry but we did not read all the messages from the queue\n");
      data->threadTest->doneReadingFromQueue(false);
   }
}

void writeFunc(void* arg)
{
   data_t* data = static_cast<data_t*> (arg);

   unsigned int index = 0;
   while (index < s_msgVec.size()) {
      // Create data and push
      if (index == s_msgVec.size() >> 1) {
         coreprintln(
            "index is = %d, we should sleep a while to test the read timeout\n", index);
         pal_thread_sleep(10);
      }
      
      // DataMessage message(index);
      DataMessage message = s_msgVec.at(index);
      data->queue->push(message);
      pal_thread_sleep(10);
      index++;
   }
   data->threadTest->doneWritingToQueue();
}
