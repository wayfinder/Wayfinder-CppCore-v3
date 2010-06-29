/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "MessageDispatcherTest.h"

#include "MessageDispatcher.h"
#include "MessageRouter.h"
#include "ServerHandler.h"
#include "WGS84Coordinate.h"
#include "NavRevGeocodingReq.h"
#include  <iostream>

MessageDispatcherTest::MessageDispatcherTest(const char* name)
   : AsynchRegressionTest(name)
{
   // TODO: have a common Core-creator with config for all regression
   // tests.
   // copy-paste from CoreFactory.cpp
   m_sh = new ServerHandler();
   m_sh->start();
   m_msgRouter = new wfcore::MessageRouter(m_sh->getListenerRegistry());
   m_msgRouter->startThread();
   m_msgDisp =
      new wfcore::MessageDispatcher(m_sh, m_msgRouter->getReceiverModuleId());   

   m_msgRouter->registerListener(this);
}

MessageDispatcherTest::~MessageDispatcherTest()
{
   // Copy-pasted from Core::~Core
   delete m_msgRouter;
   delete m_msgDisp;
   delete m_sh;
}

void MessageDispatcherTest::startTest()
{
   
   //request for lund office
   WFAPI::WGS84Coordinate coord(55.71721757, 13.18893587);
   // TODO:: who will delete this request???
   wf_ngp::NavRevGeocodingReq* addressReq
      = new wf_ngp::NavRevGeocodingReq(0, coord);

   m_msgDisp->sendNGPMessage(addressReq);

   waitForLatch(5000);
   if(m_responseReceived) {
      setTestPassed();
   } else {
      setTestFailed("No response received");
   }
}

void MessageDispatcherTest::processSystemMessage(const wf_ngp::NavReplyPacket& message)
{
   m_responseReceived = true;
   notifyAll();
}
