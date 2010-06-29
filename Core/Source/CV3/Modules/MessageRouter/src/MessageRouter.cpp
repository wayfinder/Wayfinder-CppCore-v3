/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "MSGROUTER"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#define CORE_LOGGING_MODULE_LEVEL NONE
#include "CoreDebugPrint.h"
#include "MessageRouter.h"
#include "MessageRouterInt.h"
#include "MessageListener.h"
#include "Message.h"


namespace wfcore {


MessageRouter::MessageRouter(NGPMessageListenerRegistry* msgRegistry) :
   m_receiverId(CV3_MSG_ROUTER), m_msgRegistry(msgRegistry)
{
   m_msgRegistry->registerNGPMessageReceiver(this, m_receiverId);
   
   // Create our message queue
   m_messageQueue = new messageQueue_t();
   // Initialize the listener vector

   // Initialize the message queue

   m_shutdownListener = NULL;
   // Prepare for the thread
   m_routerThreadCond = pal_thread_create_conditional();
   m_routeMutex = pal_thread_create_mutex();
   m_routerThread = pal_thread_create_thread(&MessageRouter::CrouterThreadFunc,
                                             this);

}

MessageRouter::~MessageRouter()
{
   // Deregister as a NGP message receiver.
   m_msgRegistry->deregisterNGPMessageReceiver(m_receiverId);
   
   // Check for listeners
   // Listeners should already have deregister themselves on the SHUTDOWN message

   // destroy thread
   // you're actually not allowed to destroy this until the thread is dead,
   // but just in case it might be better to wait until the joining...
   stopThread(true);
   pal_thread_destroy_thread(m_routerThread);
   pal_thread_destroy_conditional(m_routerThreadCond);
   pal_thread_destroy_mutex(m_routeMutex);

   // This module cannot send the SHUTDOWN message
   delete m_messageQueue;
}

void MessageRouter::registerListener(MessageListener* messageListener)
{
   // New listener - add to listener vector
   coreprintln("registerListener(%p)", messageListener);
   m_listeners.insert(messageListener);
   coreprintln("# listeners after insert into %p: %d",
               &m_listeners, m_listeners.size());
}

void MessageRouter::deregisterListener(MessageListener* messageListener)
{
   // Remove listener - remove from listener vector
   coreprintln("deregisterListener(%p)", messageListener);
   m_listeners.erase(messageListener);
   coreprintln("# listeners after erase into %p: %d",
               &m_listeners, m_listeners.size());
}


void
MessageRouter::setShutdownListener(RouterShutdownListener* listener)
{
   m_shutdownListener = listener;
}



void MessageRouter::post(const Message& message)
{
   // New message to be sent to listeners
}

messageQueue_t* MessageRouter::getMessageQueue() const
{
   return m_messageQueue;
}

ReceiverModuleId MessageRouter::getReceiverModuleId() const
{
   return m_receiverId;
}

void MessageRouter::checkQueue()
{
   if (m_messageQueue->empty()){
      coreprintln("Checked queue, but it was empty");
   } else {
      coreprintln("Something on the queue!");
      while(!m_messageQueue->empty()) {
         wf_ngp::NavReplyPacket* res = NULL;
         if(m_messageQueue->tryPop(res)) {
            coreprintln("Got replyPacket: %d", res->getType());

            coreprintln("#listeners in %p: %d",
                  &m_listeners,
                  m_listeners.size());
            for (ListenerVector_t::const_iterator it = m_listeners.begin();
                  it != m_listeners.end();
                  ++it) {
               coreprintln("Calling (%p)->processSystemMessage()",
                     *it);
               (*it)->processSystemMessage(*res);
            }

            // all listeners are required to make copy if they need to
            // we own the single pointer to the packet (see also
            // ServerHandler::handleServerReply(wf_ngp::NavReplyPacket*)
            delete res;
         }
      }
   }
}

void MessageRouter::receivedNGPPacket(wf_ngp::NavReplyPacket* packet)
{
   coreprintln("Received message from ServerHandler! nice!");
   m_messageQueue->push(packet);
   routerThreadNotify();
}


//-----------------------------------------------------------------------------
// thread - class members

// NOTE!
// Right now thread safetly between start/stop is protected by using the
// mutex in startThread(), stopThread() and


void
MessageRouter::startThread()
{
   Lock lock(m_routeMutex);
   if(!m_stayAlive) {
      m_stayAlive = true;
      pal_thread_start_thread(m_routerThread);
   }
}

void
MessageRouter::stopThread(bool waitUntilJoined)
{
   Lock lock(m_routeMutex);
   m_stayAlive = false;
   //if we ever change this implementation to have multiple threads waiting on
   //the m_routerThreadCond, we should do broadcast below instead
   pal_thread_conditional_signal(m_routerThreadCond);
   if(waitUntilJoined) {
      lock.unlock();
      waitForThreadToJoin();
   }
}


void
MessageRouter::waitForThreadToJoin()
{
   pal_thread_join(m_routerThread, 0, 0);
}


void
MessageRouter::routerThreadMain()
{
   coreprintln("Router thread alive");
   while(m_stayAlive) {
      checkQueue();
      routerThreadWait();
   }
   coreprintln("Router thread exiting");
   if(m_shutdownListener) {
      //XXX: TODO: Fix crash here
      // m_shutdownListener->messageRouterShutdown();
   }
}

//-----------------------------------------------------------------------------
// thread - class members for thread synchronization


void
MessageRouter::routerThreadWait()
{
   Lock lock(m_routeMutex);
   if(m_stayAlive){
      pal_thread_conditional_wait(m_routerThreadCond, m_routeMutex, 0);
   }
}


void
MessageRouter::routerThreadNotify()
{
   Lock lock(m_routeMutex);
   //if we ever change this implementation to have multiple threads waiting on
   //the m_routerThreadCond, we should do broadcast below instead
   pal_thread_conditional_signal(m_routerThreadCond);
}



//-----------------------------------------------------------------------------
// thread - static for use with pal

void
MessageRouter::CrouterThreadFunc(void* data)
{
   MessageRouter* router = static_cast<MessageRouter*>(data);
   router->routerThreadMain();
}


} // namespace wfcore

