/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "MESSAGEROUTERTEST"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "MessageRouterTest.h"
#include "MessageRouter.h"
#include "NavReplyPacket.h"
#include "NavRevGeocodingReply.h"
#include <vector>
#include <iostream>

using namespace wfcore;
using std::vector;

MessageRouterTest::MessageRouterTest(const char* name)
   : AsynchRegressionTest(name), m_receivedPackets(0)
{
   m_errorOccurred = false;
   m_registry = new ReceiverMapHolder();
   m_msgRouter = new MessageRouter(m_registry);
   m_msgRouter->registerListener(this);
   m_msgRouter->setShutdownListener(this);
}


MessageRouterTest::~MessageRouterTest()
{
   m_msgRouter->deregisterListener(this);
   delete m_msgRouter;
   delete m_registry;
}


//---------------------
// regression test methods

void
MessageRouterTest::startTest()
{

   using namespace wf_ngp;

   // first a check to make sure that you can delete an unstarted router
   // Made due to the thread-killing failsafe in the router destructor
   ReceiverMapHolder* holder = new ReceiverMapHolder();
   MessageRouter* tmpRouter = new MessageRouter(holder);
   delete tmpRouter;
   delete holder;

   m_msgRouter->startThread();

   // create a bunch of messages and send them through the router
   for (unsigned short i = 0; i < 10; ++i) {
      NavRevGeocodingReply* reply =
            new NavRevGeocodingReply("city",
                                 "district",
                                 "topregion",
                                 "municipal",
                                 "addr",
                                 1);

      reply->setReqID(i);
      m_msgRouter->receivedNGPPacket(reply);
   }
   // wait for stuffz
   waitForLatch(5000);

   if(m_errorOccurred == true) {
      setTestFailed("Unable to cast the reply. It has become corrupted on the way");
   } else if(m_receivedPackets < 10) {
      setTestFailed("Some packets were lost in the bearing sea");
   } else {
      setTestPassed();
   }

   // clean up
   m_msgRouter->stopThread(true);
}


//---------------------
// incoming messages

void
MessageRouterTest::processSystemMessage(const wf_ngp::NavReplyPacket& message)
{
   using namespace wf_ngp;

   try {
      dynamic_cast<const NavRevGeocodingReply&>(message);
   } catch(std::bad_cast) {
      // oh jeez, something got messed up on the way
      m_errorOccurred = true;
   }
   m_receivedPackets++;
   if(m_receivedPackets == 10) {
      notifyAll();
   }
}


//----------------------
// shutdown notification

void
MessageRouterTest::messageRouterShutdown()
{
}



