/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "AsynchRegressionTest.h"
#include "Lock.h"

AsynchRegressionTest::AsynchRegressionTest(const char* name,
                                           unsigned int timeOutMs) :
                                           RegressionTest(name, timeOutMs)
{
   m_cond = pal_thread_create_conditional();
   m_mutex = pal_thread_create_mutex();
   m_latchActive = true;
}


AsynchRegressionTest::~AsynchRegressionTest()
{
   pal_thread_destroy_conditional(m_cond);
   pal_thread_destroy_mutex(m_mutex);
}


void
AsynchRegressionTest::waitForNotify(unsigned int timeOutMilliSecs)
{
   Lock lock(m_mutex);
   pal_thread_conditional_wait(m_cond, m_mutex, timeOutMilliSecs);
}


void
AsynchRegressionTest::waitForLatch(unsigned int timeOutMilliSecs)
{
   Lock lock(m_mutex);
   if(m_latchActive) {
      pal_thread_conditional_wait(m_cond, m_mutex, timeOutMilliSecs);
   }
}


void
AsynchRegressionTest::notifyAll()
{
   Lock lock(m_mutex);
   m_latchActive = false;
   pal_thread_conditional_broadcast(m_cond);
}
