/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef MESSAGE_ROUTER_TEST_H
#define MESSAGE_ROUTER_TEST_H

#include "AsynchRegressionTest.h"
#include "NavReplyPacket.h"
#include "MessageListener.h"
#include "MessageRouter.h"
#include "ReceiverMapHolder.h"
#include "NotCopyable.h"
#include "WFString.h"
#include <vector>

class MessageRouterTest :  public AsynchRegressionTest,
                           public wfcore::MessageListener,
                           public wfcore::RouterShutdownListener,
                           private NotCopyable {

public:

  MessageRouterTest(const char* name);

  virtual ~MessageRouterTest();

  virtual void startTest();

  virtual void processSystemMessage(const wf_ngp::NavReplyPacket& message);

  virtual void messageRouterShutdown();

private:
   std::vector<wf_ngp::NavReplyPacket> m_incomingVec;
   unsigned short m_receivedPackets;

   wfcore::MessageRouter* m_msgRouter;
   ReceiverMapHolder* m_registry;
   bool m_errorOccurred;
};


#endif /* MESSAGE_ROUTER_TEST_H */
