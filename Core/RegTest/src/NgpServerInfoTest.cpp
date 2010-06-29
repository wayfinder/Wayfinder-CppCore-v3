/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "NGPSERVERINFOTEST"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#define CORE_LOGGING_MODULE_LEVEL NONE
#include "CoreDebugPrint.h"

#include "NgpServerInfoTest.h"

#include "CoreConfig.h"
#include "CoreFactory.h"
#include "MessageDispatcher.h"
#include "MessageRouter.h"

#include "NavServerInfoReply.h"
#include "NavServerInfoReq.h"

namespace {
const char* bool2str(bool b)
{
   if (b) {
      return "true";
   }

   return "false";
}
}


NgpServerInfoTest::NgpServerInfoTest(const char* name)
   : AsynchRegressionTest(name),
     m_core(NULL),
     m_request(NULL)
     //m_reply(NULL)
{
   // config has no effect anyway
   const corefactory::CoreConfig config("", 80, "");
   m_core = corefactory::createCore(config);
}

NgpServerInfoTest::~NgpServerInfoTest()
{
   m_core->getMessageRouter().deregisterListener(this);
   delete m_core;
   // the request is deleted when reply comes back because
   // the pointer may otherwise still be living in ServerCom
   // delete m_reply;
}


void NgpServerInfoTest::startTest()
{
   m_core->getMessageRouter().registerListener(this);
   m_request = new wf_ngp::NavServerInfoReq;

   wf_uint32 reqId =
      m_core->getMessageDispatcher().sendNGPMessage(m_request);
   
   setTestFailed("Timeout without receiving any reply.");
   coreprintln("Sent request %u. Waiting for reply...", reqId);
   waitForNotify(getTimeOutMs()/2);

   // avoid unused variable warning when compiling with debug prints off
   reqId = 0;

   // time out: failure reason set above
   //
   // conversion failed: processSystemMessage() has set fail reason.
   // 
   // test pass: processSystemMessage() called setTestPassed()
   return;
}


void
NgpServerInfoTest::processSystemMessage(const wf_ngp::NavReplyPacket& message)
{
   coreprintln("NgpServerInfoTest::processSystemMessage(): %p", &message);
   delete m_request;
   if (failIfDifferent("Not a SERVER_INFO_REPLY packet",
                       wf_ngp::PacketTypeEnum::NAV_SERVER_INFO_REPLY,
                       message.getType())) {
      notifyAll();
      return;
   }


   // turns out that there is no copy ctor or assignment operator so
   // we can't make a copy without writing the packet to a byte array
   // and de-serialize it. So we do the test here...

   // safe - we have checked type above
   const wf_ngp::NavServerInfoReply reply =
      dynamic_cast<const wf_ngp::NavServerInfoReply&>(message);

   coreprintln("CategoriesCrc: 0x%x", reply.getCategoriesCrc());
   coreprintln("LatestNewsCrc: 0x%x", reply.getLatestNewsCrc());
   coreprintln("SubscriptionTimeLeftSeconds: 0x%x",
               reply.getSubscriptionTimeLeftSeconds());
   coreprintln("TopRegionChecksum: 0x%x",
               reply.getTopRegionChecksum());
   coreprintln("ServerListChecksum: 0x%x",
               reply.getServerListChecksum());
   coreprintln("PublicUserName: \"%s\"",
               reply.getPublicUserName().c_str());
   coreprintln("CallCenterChecksum: 0x%x",
               reply.getCallCenterChecksum());
   coreprintln("UpgradeId: \"%s\"", reply.getUpgradeId().c_str());
   coreprintln("ForceClientUpgrade: %s",
               bool2str(reply.getForceClientUpgrade()));
   coreprintln("PinCrc: 0x%x", reply.getPinCrc());
   coreprintln("UtcTime: 0x%x", reply.getUtcTime());
   coreprintln("LatestNewsId: \"%s\"",
               reply.getLatestNewsId().c_str());
   coreprintln("FavoritesCrc: \"%s\"",
               reply.getFavoritesCrc().c_str());
   coreprintln("SearchDescCrc: 0x%x",
               reply.getSearchDescCrc().c_str());

   // some lame tests - don't want to become too dependant on
   // server config.
   if (reply.getTopRegionChecksum() == 0) {
      setTestFailed("Top region checksum at server is 0 - unexpected.");
      return;
   }

   if (reply.getUtcTime() == 0) {
      setTestFailed("Server's clock is in 1970");
      return;
   }
   
   setTestPassed();
   
   notifyAll();
   return;
}      
