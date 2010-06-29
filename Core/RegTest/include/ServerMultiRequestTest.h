/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef SERVERMULTIREQUESTTEST_H
#define SERVERMULTIREQUESTTEST_H

#include "RegressionTest.h"

#include "ConcurrentQueue.h"
#include "ReceiverModuleId.h"
#include "ServerHandler.h"

#include <string>

typedef ConcurrentQueue<wf_ngp::NavReplyPacket*> messageQueue_t;

// forward declarations
namespace wf_ngp {
   class NavReplyPacket;
   class NavRevGeocodingReply;
}

namespace WFAPI {
   class WGS84Coordinate;
}


/**
 * Tests that we can send more than one request to server without
 * crashing.
 */
class ServerMultiRequestTest : public RegressionTest,
                               public NGPMessageReceiver {
public:

   ServerMultiRequestTest(const char* name);

   virtual ~ServerMultiRequestTest();

   virtual void startTest();

   /**
    * @see NGPMessageReceiver::receivedNGPPacket
    */
   virtual void receivedNGPPacket(wf_ngp::NavReplyPacket* packet);

private:

   /// interface for sending requests to the server
   ServerHandler m_serverHandler;

   /// The queue of incoming messages from the server
   messageQueue_t m_messageQueue;

   /// The receiverId of this module, constant throughout its lifespan
   // The ID:s are defined in
   // Core/Source/ServerCom/include/ReceiverModuleId.h 
   ReceiverModuleId m_receiverId;

   // --------------------------------------------------------
   // utilities

   /// Caller owns the returned object and should delete it.
   const wf_ngp::NavRevGeocodingReply*
   getAndCheckReply(const WFAPI::WGS84Coordinate& coord,
                    const std::string& expectedCity);
};

#endif /* SERVERMULTIREQUESTTEST_H */
