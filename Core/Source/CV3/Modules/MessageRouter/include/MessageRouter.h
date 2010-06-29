/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MESSAGE_ROUTER_H
#define MESSAGE_ROUTER_H

#include "Module.h"
#include "MessageRouterInt.h"
#include "MessageListener.h"
#include "Message.h"
#include "NotCopyable.h"
#include <set>
#include "ConcurrentQueue.h"
#include "ReceiverModuleId.h"
#include "NavReplyPacket.h"
#include "NotCopyable.h"
#include "NGPMessageReceiver.h"
#include "NGPMessageListenerRegistry.h"
#include "RouterShutdownListener.h"

namespace wfcore {

typedef ConcurrentQueue<wf_ngp::NavReplyPacket*> messageQueue_t;

/**
* The message route sends the received message from the server to the correct
* module/package within cv3.
* This Module will be created by the Main Module
*/
class MessageRouter : public MessageRouterInt,
                      public Module,
                      private NotCopyable,
                      public NGPMessageReceiver
{
public:

   /** 
    * Constructor.
    * 
    * @param msgRegistry The registry to register itself as a receiver of
    * NGPMessages.
    */
   MessageRouter(NGPMessageListenerRegistry* msgRegistry);

   /** 
    * Destructor.
    */
   ~MessageRouter();
   
   /**
    * A method to register a MessageListener with the MessageRouter
    * @param messageListener The MessageListener
    */
   virtual void registerListener(MessageListener* messageListener);

   /**
    * A method to register a MessageListener with the MessageRouter
    * @param messageListener The MessageListener
    */
   virtual void deregisterListener(MessageListener* messageListener);

   /**
    * A method to post messages to listeners
    * @param message The message to be posted
    */
   virtual void post(const Message& message);

   /**
    * Sets the shutdown listener which will be notified when the thread
    * dies. If the thread is already offline when this listener is registered,
    * no notification will be sent.
    *
    * If a RouterShutdownListener is set in the router, the threads final
    * action will be to call RouterShutdownListener.messageRouterShutdown().
    *
    * @param The RouterShutdownListener which should receive the notification
    * when the router main thread has died.
    * @see RouterShutdownListener
    */
   void setShutdownListener(RouterShutdownListener* listener);


   /**
    * Get the message queue that belongs to the router.
    * Typically you want this in order to put messages on the queue,
    * such as NavReplyPackets coming from the server.
    *
    * @return the message queue
    */
   messageQueue_t* getMessageQueue() const;

   /**
    * Get the module ID of the receiver.
    * The ServerHandler has a list of receivers, and in that list
    * it keeps track of what queue belongs to each receiver.
    * In order to enable this, the Message Router needs to provide this
    * functionality, i.e. expose its unique receiver id.
    *
    * @return the moduleId of this receiver
    */
   ReceiverModuleId getReceiverModuleId() const;

   /**
    * Checks the inqueue for messages and handles them.
    */
   void checkQueue();

   /**
    * @see NGPMessageReceiver.
    */
   virtual void receivedNGPPacket(wf_ngp::NavReplyPacket* packet);

   //--------------------------------------------------------------------------
   // threading

   /**
    * Starts the internal thread which will process any incoming messages
    * and route them to the correct receiver. If the thread is already active,
    * this method will have no effect.
    */
   void startThread();


   /**
    * Stops the internal thread. If a RouterShutdownListener is set in the
    * router, the threads final action will be to call
    * RouterShutdownListener.messageRouterShutdown().
    *
    * @param waitUntilJoined true if and only if this call should block until
    * the thread has died and joined with oblivion.
    */
   void stopThread(bool waitUntilJoined);


   /**
    * Blocks until the internal thread is dead.
    */
   void waitForThreadToJoin();


private:
   /// A Vector definition
   typedef std::set<MessageListener*> ListenerVector_t;

   /// A list of Listeners registered with the MessageRouter 
   ListenerVector_t m_listeners;

   /// The queue of incoming messages from the server
   messageQueue_t* m_messageQueue;

   /// The receiverId of this module, constant throughout its lifespan
   ReceiverModuleId m_receiverId;

   /// The interface used to register as a receiver of NGP messages.
   NGPMessageListenerRegistry* m_msgRegistry;

   /// The listener that will be called when the route shuts down
   RouterShutdownListener* m_shutdownListener;

   //-------------------------
   // threading

   /// internal main method for the thread
   void routerThreadMain();

   /// used for internal synch
   void setStayAliveBool(bool alive);

   /// used for internal synch
   void routerThreadWait();

   /// used for internal synch
   void routerThreadNotify();

   /// called from the pal threading
   static void CrouterThreadFunc(void* data);

   /// Conditional used by the internal thread
   Conditional* m_routerThreadCond;

   /// Mutex used for thread safety
   Mutex* m_routeMutex;

   /// The internal thread
   Thread* m_routerThread;

   /// true while the thread should keep running
   bool m_stayAlive;
};


} // namespace wfcore

#endif // MESSAGE_ROUTER_H
