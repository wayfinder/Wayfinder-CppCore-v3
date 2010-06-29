/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef RECEIVER_MAP_HOLDER_H
#define RECEIVER_MAP_HOLDER_H

#include "NGPMessageListenerRegistry.h"
#include "PALMachineTypes.h"
#include "ReceiverModuleId.h"
#include <map>

// Forward declarations
struct Mutex;

/** 
 * Class that maps receiverId against the receiving interface.
 * All operations are mutex protected, i.e thread safe.
 */
class ReceiverMapHolder : public NGPMessageListenerRegistry {
public:

   /** 
    * Contructor.
    */
   ReceiverMapHolder();

   /** 
    * Destructor.
    */
   virtual ~ReceiverMapHolder();   

   /** 
    * Registers a receiver Id and the interface of the receiving module.
    * 
    * @param messageReceiver The interface of the receiving module.
    * @param receiverId The Id of the receiver.
    */
   virtual void registerNGPMessageReceiver(
      NGPMessageReceiver* messageReceiver,
      ReceiverModuleId receiverId);
      
   /** 
    * Unregisters the receiver so it can no longer receive replies of
    * requests.
    * 
    * @param receiverId The Id of the receiver to unregister.
    */
   virtual void deregisterNGPMessageReceiver(ReceiverModuleId receiverId);

   /** 
    * Get the interface of a receiver. This is used for sending packets to
    * the receiver of a reply.
    * 
    * @param receiverId The Id of the receiver to receive a reply.
    * 
    * @return The interface to message receiver the id belongs to.
    */
   NGPMessageReceiver* getMessageReceiver(ReceiverModuleId receiverId);

private:
   
   /// The map of receivers id and their interfaces for retrieving replies.
   std::map<ReceiverModuleId, NGPMessageReceiver*> m_messageReceiverMap;
   /// Mutex for protecting the map containg id and interfaces for receivers.
   mutable Mutex* m_messageReceiverMapMutex;

};

#endif // End of RECEIVER_MAP_HOLDER_H
