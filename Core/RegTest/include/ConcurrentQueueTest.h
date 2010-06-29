/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CONCURRENT_QUEUE_TEST_H
#define CONCURRENT_QUEUE_TEST_H

#include "RegressionTest.h"
#include "ConcurrentQueue.h"
#include "NotCopyable.h"

typedef void(*ThreadStartFunc)(void*);

/**
 * Simple class for putting something on the concurrent queue.
 */
class DataMessage
{
public:
   DataMessage() : m_data(0)
      {}
   
   explicit DataMessage(unsigned int data) :
      m_data(data)
      {}

   bool operator==(const DataMessage& comp);
   unsigned int m_data;
};

inline bool DataMessage::operator==(const DataMessage& comp) {
   return m_data == comp.m_data;
}

/**
 * Regression test that tests the concurrent queue. Performs two
 * basic tests.
 *
 * 1. Create and start two threads, one that uses the conditional
 *    wait function that the queue offers. What it does is that
 *    we wait for some time until data is pushed to the queue.
 *    The other thread is pushing messages to the queue.
 *
 * 2. Create and start two threads, one that uses the mutex read,
 *    polling the queue for something to read.
 *    The other thread does exactly as in the test above, it uses
 *    messages from the static vector and pushes copies of them
 *    to the queue.
 *
 * The reading thread uses messages that are put in a static vector
 * (s_msgVec) and puts these messages on the queue, on at a time with
 * some sleep in between.
 *
 * The reading thread stores the messages in a local vector and when
 * it is done it compares the local vector against the static vector
 * to make sure all the messages was read from the queue.
 */
class ConcurrentQueueTest : public RegressionTest,
                            private NotCopyable
{
public:

   /**
    * Constructor.
    *
    * @param name The name of the regression test, passes this
    *             to the super class.
    */
   ConcurrentQueueTest(const char* name);

   /**
    * Destructor.
    */
   virtual ~ConcurrentQueueTest();

   /**
    * Starts the test. Creates threads and data structures.
    * First we start one test and waits until a bool flag is set
    * by the reading thread, notifying that the first test is done.
    * We then start the next thread and wait until this is finished.
    */
   virtual void startTest();

   /**
    * Notification by the writing thread that all messages has
    * been pushed to the queue. We can now set the s_continueRead
    * flag to false. The reading thread will now finish its reading
    */
   void doneWritingToQueue();

   /**
    * Notification by the reading thread that performs the conditional
    * read.
    *
    * @param success True if all messages has been successfully read
    *                from the queue. False if not.
    */
   void doneReadingFromQueue(bool success);

   /**
    * Notification by the reading thread that performs the polling
    * read.
    *
    * @param success True if all messages has been successfully read
    *                from the queue. False if not.
    */
   void donePollingFromQueue(bool success);

protected:

   /**
    * Convenience function that creates and starts a thread.
    *
    * @param thread A reference to a thread pointer that should
    *               point to the created thread.
    * @param threadFunc A pointer to the function that the thread will call
    *                   once started.
    * @param threadData The datas struct containing data sent to the thread.
    * 
    * @return True if creating the and starting the thread was successful.
    */
   bool createAndStartThread(Thread*& thread,
                             ThreadStartFunc threadFunc,
                             void* threadData);

   /**
    * A pointer to the shared message queue.
    */
   ConcurrentQueue<DataMessage>* m_queue;

   /// Thread that performs the writing to the queue.
   Thread* m_condWriteThread;

   /// Thread that performs the conditinal read.
   Thread* m_condReadThread;

   /// Thread that performs writing to the queue.
   Thread* m_mutexWriteThread;

   /// Thread that performs the reading by polling the queue.
   Thread* m_mutexReadThread;

   /// Flag that is set to true once a test is finished.
   bool m_doneReadingFromQueue;
};

void readFunc(void* arg);
void pollReadFunc(void* arg);
void writeFunc(void* arg);

#endif
