/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MESSAGEDISPATCHER_H_
#define MESSAGEDISPATCHER_H_

#include "WGS84Coordinate.h"
#include "PALMachineTypes.h"
#include "NGPMessageSender.h"
#include "ReceiverModuleId.h"
#include "RouterShutdownListener.h"

// forward declarations outside namespace
class ServerHandler;

namespace wf_ngp {
class NavRequestPacket;
}

namespace wfcore {

class MessageDispatcher : public RouterShutdownListener
{
public:
   
   /**
    * The parameters must not be null.
    * The pointed-to-objects must be valid through
    * this class life time.
    *
    * @param msgSender The interface used for sending packets to server.
    * @param receiverId The Id of the receiver of the replies.
    */
   MessageDispatcher(NGPMessageSender* msgSender,
                     ReceiverModuleId receiverId);

   /**
    * Destructor.
    */
   virtual ~MessageDispatcher();

   /** 
    * Will send an NGP request packet to the server.
    * 
    * @param packet The request packet that will be sent.
    * 
    * @return The request id that will be used in the reply from the server.
    */
   wf_uint32 sendNGPMessage(wf_ngp::NavRequestPacket* packet);


   /**
    * Sends a notification to all listeners, informing them that the
    * message router main thread has died of natural causes.
    *
    * This method is inherited from the RouterShutdownListener ifc
    * @see RouterShutdownListener
    */
   virtual void messageRouterShutdown();

private:

   /**
    * The Id of the receiver which will receive the replies of the request
    * that the dispatcher sends.
    */
   ReceiverModuleId m_receiverId;

   ///The interface used for sending packets to server.
   NGPMessageSender* m_msgSender;
};

}

#endif /* MESSAGEDISPATCHER_H_ */
