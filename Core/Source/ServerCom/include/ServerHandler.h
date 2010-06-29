/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H

#include "NGPMessageSender.h"
#include "NGPMessageReceiver.h"
#include "NGPMessageListenerRegistry.h"
#include "ConcurrentQueue.h"

//forward declarations
class ConnectionSupplier;
class RequestIdGenerator;
class RequestMapHolder;
class ReceiverMapHolder;

namespace wf_ngp {
class NavRequestPacket;
class NavReplyPacket;
}

typedef ConcurrentQueue<wf_ngp::NavReplyPacket*> messageQueue_t;

/**
 * ServerHandler manages the connection to the server, making sure the client
 * is connected and able to use the network. It is using the PALNetwork to set
 * up the network handling.
 *
 * ServerHandler uses the PALNetwork interface for network handling.
 * The ServerHandler will be used by the ServerParser.
 */
class ServerHandler : public NGPMessageSender,
                      public NGPMessageReceiver {

public:

   /**
    * Constructor.
    */
   ServerHandler();

   /**
    * Destructor.
    */
   ~ServerHandler();

   /**
    * Start the server handler. should be done some time after creating the
    * serverhandler object, before it is being used for something else.
    */
   void start();
   
   /**
    * Do not re-establish the socket.
    */
   bool isConnected();

   /**
    * Kill the connection and set m_socketContext to NULL so that
    * the next call to ensureConnection() will re-establish the
    * socket.
    *
    * If m_socketContext == NULL, this method has no effect.
    */
   void disconnect();

   /**
    * @see NGPMessageSender::sendNGPMessage.
    */
   virtual wf_uint32 sendNGPMessage(wf_ngp::NavRequestPacket* packet,
                                    ReceiverModuleId receiverId);

   /**
    * @see NGPMessageReceiver::receivedNGPPacket.
    */
   virtual void receivedNGPPacket(wf_ngp::NavReplyPacket* reply);
                                  
   /** 
    * Returns the registry where it is possible for listeners
    * to register and deregister themselves to a certain id.
    * 
    * @return A pointer to the interface
    */
   NGPMessageListenerRegistry* getListenerRegistry();

   /**
    * Checks the incoming queues and handles the objects that are put on the
    * queue, if any.
    */
   void waitAndCheckQueues(); 
   
protected:
private:

   /**
    * We do not allow any copying.
    */
   ServerHandler(const ServerHandler&);

   /**
    * We do not allow use of assignment operator.
    */
   const ServerHandler& operator=(const ServerHandler&);

   /**
    * Pretend to send a request packet.
    * Instead of actually sending this over the network, construct a phoney
    * reply packet and use it when calling the handleServerReply.
    *
    * @param packet The packet to pretend to send.
    * @param receiver the module id to associate with this request
    *
    * @return the request id that the ServerHandler assigns the request
    */
   wf_uint32 sendFakeBlocking(wf_ngp::NavRequestPacket& packet,
                              ReceiverModuleId receiver);

   bool sendBlocking(void* buf, unsigned int size);

   /**
    * Send a packet blocking.
    * Wrapper that serializes the packet.
    *
    * @param packet the packet. Not const since requestID
    * will be updated.
    */
   bool sendBlocking(wf_ngp::NavRequestPacket* packet);

   /**
    * Notifies the thread that it is time to wake up!
    */
   bool notify();


   bool ensureConnection();

/// Member variables
   
   /// For testing The ConnectionManager
   ConnectionSupplier* m_cm;

   /// The actual connection
   wf_uint32 m_connectionId;
   
   /// Typedef for the queue that contains NavRequests packets.
   typedef ConcurrentQueue<wf_ngp::NavRequestPacket*> InQueue;

   /// The queue that contains NGPRequests that should be sent to the server.
   InQueue m_msgQueue;

   /// Helper class that handles the request id generation thread safe.
   RequestIdGenerator* m_requestIdGenerator;

   /// Helper class that store request ids along with the receiver module id.
   /// The storage is thread safe.
   RequestMapHolder* m_requestMapHolder;

   /// Helper class that store module ids along with pointers to the modules
   /// that should receive server replies. The storage is thread safe.
   ReceiverMapHolder* m_receiverMapHolder;
};

#endif
