/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef MESSAGE_ROUTER_INT_H
#define MESSAGE_ROUTER_INT_H

#include "MessageListener.h"
#include "Message.h"

namespace wfcore {

/**
* The message route sends the received message from the server to the correct
* module/package within cv3.
* This Module will be created by the Main Module
*/
class MessageRouterInt
{
public:
   /**
    * Virtual destructor
    */
   virtual ~MessageRouterInt() {}

   /**
    * A method to register a MessageListener with the MessageRouter
    * @param messageListener The MessageListener
    */
   virtual void registerListener(MessageListener* messageListener) = 0;

   /**
    * A method to register a MessageListener with the MessageRouter
    * @param messageListener The MessageListener
    */
   virtual void deregisterListener(MessageListener* messageListener) = 0;

   /**
    * A method to post messages to listeners
    * @param message The message to be posted
    */
   virtual void post(const Message& message) = 0;
};

} // namespace wfcore

#endif // MESSAGE_ROUTER_INT_H
