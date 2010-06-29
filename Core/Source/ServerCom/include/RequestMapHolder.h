/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef REQUEST_MAP_HOLDER_H
#define REQUEST_MAP_HOLDER_H

#include "PALMachineTypes.h"
#include "ReceiverModuleId.h"
#include <map>

// Forward declarations
struct Mutex;

/** 
 * Class that can map requests against the receiver of the reply.
 * All operations are protected by a mutex, i.e all operations are thread
 * safe. If adding any functions make sure it is thread safe.
 */
class RequestMapHolder {
public:

   /** 
    * Contructor.
    */
   RequestMapHolder();

   /** 
    * Destructor.
    */
   virtual ~RequestMapHolder();

   /** 
    * Register which receiver to receive a reply to a specific request.
    * 
    * @param reqId The request Id of the request.
    * @param receiverId The Id of the receiving module. I.e this module will
    * later receive the reply of the request.
    */
   void registerReceiverId(wf_uint32 reqId, ReceiverModuleId receiverId);

   /** 
    * Get the receiver of a specific request. This function will also remove
    * the mapping between the requestId and receiver.
    * 
    * @param reqId The Id of the request to find the receiver of.
    * 
    * @return The receiverId of the module who should get the reply of the
    * request.
    */
   ReceiverModuleId getAndRemoveReceiver(wf_uint32 reqId);
   
private:
   
   /// Mutex for protecting the map containing requestId mapped to receiver.
   mutable Mutex* m_requestMapMutex;

   /// The map of requests and which receiver should get the reply
   std::map<wf_uint32, ReceiverModuleId> m_requestMap;
   
};

#endif /* REQUEST_MAP_HOLDER_H */
