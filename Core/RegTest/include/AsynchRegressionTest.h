/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef ASYNCHREGRESSIONTEST_H
#define ASYNCHREGRESSIONTEST_H

#include "RegressionTest.h"
#include "PALThreading.h"

class AsynchRegressionTest : public RegressionTest {

public:
   AsynchRegressionTest(const char* name,
                        unsigned int timeOutMs = 10000);

   virtual ~AsynchRegressionTest();

   /**
    * Starts the test. The test will be finished when isTestFinished is
    * true.
    *
    * Since regression tests are typically run inside the GUI thread
    * context, you will need to "sleep" and let Nav2 propagate messages
    * upwards. The regression test logic will take place inside these
    * callback functions.
    *
    * This can be done using Environment::Wait(...).
    */
   virtual void startTest() = 0;


protected:

   /**
    * Blocks the execution until notify() is called.
    * @param timeOutMilliSecs Timeout in millisecs, 0 for no timeout
    */
   void waitForNotify(unsigned int timeOutMilliSecs);

   /**
    * Blocks the execution. This method will only hang until the first call to
    * notify has been made. Any calls to this method after notify has been
    * called will not block at all.
    * @param timeOutMilliSecs Timeout in millisecs, 0 for no timeout
    */
   void waitForLatch(unsigned int timeOutMilliSecs);

   /**
    * Releases any previously made call to waitForNotify() and
    * waitForLatch(). Will also disable the latch, making any calls to
    * waitForLatch() to no longer block.
    */
   void notifyAll(); // greetings from the Java team :D


private:

   Conditional* m_cond;
   Mutex* m_mutex;
   volatile bool m_latchActive;
};

#endif /* ASYNCHREGRESSIONTEST_H */
