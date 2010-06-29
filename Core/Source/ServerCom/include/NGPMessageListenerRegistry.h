/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef NGP_MESSAGE_LISTENER_REGISTRY_H
#define NGP_MESSAGE_LISTENER_REGISTRY_H

/// Forward declarations
#include "NGPMessageReceiver.h"
#include "ReceiverModuleId.h"

/**
 * Interface describing how MessageReceiver register themself as
 * listeners. This means that the MessageReceiver only needs to know about this
 * interface and not the whole ServerHandler.
 * 
 * ServerHandlers needs to implement this interface to be able to let message
 * receiver to register themself as listeners.
 */

class NGPMessageListenerRegistry {

public:

   virtual ~NGPMessageListenerRegistry() {};

   /**
    * Function used by a NGPMessageRecevier to register itself as a
    * listener for messages received by the ServerHandler.
    * @param messageReceiver The receiver to register.
    * @param receiverId The id of the receiving module.
    */
   virtual void registerNGPMessageReceiver(NGPMessageReceiver* messageReceiver,
                                           ReceiverModuleId receiverId) = 0;

   /**
    * Function used by the MessageRecevier to register itself as a listener for
    * messages received by the ServerHandler.
    * @param messageReceiver The receiver to register. 
    */
   virtual void deregisterNGPMessageReceiver(ReceiverModuleId receiverId) = 0;
};

#endif /// End NGP_MESSAGE_LISTENER_REGISTRY_H
