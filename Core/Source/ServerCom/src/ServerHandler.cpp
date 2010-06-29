/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "SERVERHANDLER"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#define CORE_LOGGING_MODULE_LEVEL NONE

#include "ServerHandler.h"
#include "CoreDebugPrint.h"
#include "RequestIdGenerator.h"
#include "RequestMapHolder.h"
#include "ReceiverMapHolder.h"
#include "NavRequestPacket.h"
#include "NavReplyPacket.h"
#include "PacketFactory.h"

#include "ConnectionManager.h"
#include <cstring>
#include <map>
#include <iostream>
#include <sstream>

using wf_ngp::byte;
using wf_ngp::uint32;

using wf_ngp::PacketTypeEnum;
using wf_ngp::NavReplyPacket;


ServerHandler::ServerHandler()
   : m_cm(NULL)
{
   coreprintln("ServerHandler() Begin");
   m_requestIdGenerator = new RequestIdGenerator();
   m_requestMapHolder = new RequestMapHolder();
   m_receiverMapHolder = new ReceiverMapHolder();
   m_cm = new ConnectionManager();
   
   coreprintln("ServerHandler() End");
}


ServerHandler::~ServerHandler()
{
   delete m_requestIdGenerator;
   delete m_requestMapHolder;
   delete m_receiverMapHolder;
   m_cm->closeConnection( m_connectionId );
   delete m_cm;
}

void ServerHandler::start() {
   // Allow the bearer
   m_cm->enableBearer( NETWORK_BEARER_ANY );
   // Get a connection
   m_connectionId = m_cm->getConnection(NETWORK_BEARER_ANY,
                                        CONNECTION_TYPE_NONSECURE,
                                        this);
}

bool ServerHandler::sendBlocking(wf_ngp::NavRequestPacket* packet)
{
   using wf_ngp::NParam;

   std::vector<unsigned char> buf;
   packet->serialize(packet->getParamBlock());

   std::vector<NParam> fakeID;
   const char* fakeIMEI = "LinuxNav2APITestIMEI001";
   const char* fakeIMEIType = "imei";
   fakeID.push_back(NParam(3, fakeIMEI));
   fakeID.push_back(NParam(29, fakeIMEIType));
   
   packet->writeTo(buf, fakeID);


   return sendBlocking(&buf[0], buf.size());
   //pal_thread_sleep(1000);  
}

bool ServerHandler::sendBlocking(void* buffer,
                                 unsigned int size)
{
   bool done = false;
   pstatus result = PAL_OK;
   if (ensureConnection()){
      unsigned int written;
      char* bufferChar = static_cast<char*>(buffer);
      while (!done){
         // For testing only !
         m_cm->writeBlocking(m_connectionId,
                             bufferChar,
                             size,
                             &written);
         // For testing
         result = PAL_OK;
         if (result != PAL_OK){
            // Something is wrong with the socket or network
            coreprintln("writing to the socket returns pstatus error: %d",
                        result);
            // kill the socket so that we open a new one next time
            disconnect();

            return false;
            
         } else if (written < size){
            coreprintln("sendBlocking could not write everything at "
                        "once, wrote %d of %d bytes",written, size);
            bufferChar += written;
            size -= written;

         } else {
            done = true;
            coreprintln("sendBlocking successfully wrote everything, exiting.");
         }
      }
   }

   return done;
}



bool ServerHandler::ensureConnection()
{
   coreprintln("::ensureConnection - TEST CODE ! ConnectionManager "
               "should guarantee connection");
   return true;
}


void ServerHandler::disconnect() {
   m_cm->closeConnection( m_connectionId );
}


wf_uint32
ServerHandler::sendNGPMessage(wf_ngp::NavRequestPacket* packet,
                              ReceiverModuleId receiverId)
{
   wf_uint32 requestId = m_requestIdGenerator->generateNewRequestId();
   packet->setReqID(requestId);

   // Register the receiver module id to this request id until
   // the reply is received.
   m_requestMapHolder->registerReceiverId(requestId, receiverId);
   
   m_msgQueue.push(packet);
   //notify will not be needed if there are seperate write threads later:
   bool workresult = notify();
   if(workresult == false) {
      return 0;
   } else {
      return requestId;      
   }
}

bool
ServerHandler::notify()
{
   //try to pop a packet from the write queue.
   //this will use the calling thread to ensure the connection is available and
   //to do the network write calls, ie. we steal cpu time from calling thread
   wf_ngp::NavRequestPacket* packet = NULL;
   if (m_msgQueue.tryPop(packet)) {
      coreprintln("Got message in queue! yeah!");
      return sendBlocking(packet);
   } else {
      coreprintln("Why did you wake me up? I am still tired!");
      return false;
   }
}

// void
// ServerHandler::waitAndCheckQueues() {
//    coreprintln("waitAndCheckQueues begin");
//    //sendBlocking(packet);
//    NavReplyPacket* reply = NULL;
//    reply = receiveBlocking();
//    coreprintln("after receiveBlocking");
//    if(reply) {
//       coreprintln("Got message from the net! yeah!");
//       ReceiverModuleId receiverModuleId =
//          m_requestMapHolder->getAndRemoveReceiver(reply->getReqID());

//       NGPMessageReceiver* receiver =
//          m_receiverMapHolder->getMessageReceiver(receiverModuleId);

//       if (receiver == NULL){
//          coreprintln("Oh no, received a reply without anyone waiting for it!");
//       } else {
//          coreprintln("Sending nav reply packet to NGPMessageReceiver");
//          receiver->receivedNGPPacket(reply);
//       }
//    } else {
//       coreprintln("Reply was NULL, so we were probably terminated.");
//    }
   
// }


NGPMessageListenerRegistry* ServerHandler::getListenerRegistry()
{
   return m_receiverMapHolder;
}

void ServerHandler::receivedNGPPacket(wf_ngp::NavReplyPacket* reply)
{
   coreprintln("receivedNGPPacket begin");

   if(reply) {
      coreprintln("Got message from the net! yeah!");
      ReceiverModuleId receiverModuleId =
         m_requestMapHolder->getAndRemoveReceiver(reply->getReqID());

      NGPMessageReceiver* receiver =
         m_receiverMapHolder->getMessageReceiver(receiverModuleId);

      if (receiver == NULL){
         coreprintln("Oh no, received a reply without anyone waiting for it!");
      } else {
         coreprintln("Sending nav reply packet to NGPMessageReceiver");
         receiver->receivedNGPPacket(reply);
      }
   } else {
      coreprintln("Reply was NULL, so we were probably terminated.");
   }
    
}

