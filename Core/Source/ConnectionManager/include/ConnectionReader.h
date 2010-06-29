/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CONNECTIONREADER_H
#define CONNECTIONREADER_H
#include "PALMachineTypes.h"

//forward declares
class NGPMessageReceiver;
struct Thread;
struct Mutex;
struct PALConnection;
namespace wf_ngp {
class NavReplyPacket;
}


/**
 * A Reader
 *
 */
class ConnectionReader {

public:

   /**
    * Constructor.
    *
    * @param ServerHandler XXX: please remove me
    * @param palConnection The pal connection to read from
    */
   ConnectionReader(NGPMessageReceiver* msgReceiver,
                    PALConnection* palConnection);

   /**
    * Destructor.
    */
   ~ConnectionReader();

   /**
    * The method that the thread executes in
    *
    * @param arg Data to pass to the thread. Typically contains the class we
    *            want to execute in.
    */
   void run(void* arg);

   /**
    * Thread helper function
    * 
    * @param arg Data to pass to the thread
    */ 
   static void threadFunc(void* arg);

   /**
    * gets the member that decides if the thread should stop executing or not
    *
    * @return true if the thread should continue to run, false if not
    */
   bool getShouldRun();

   /**
    * sets the member that decides if the thread should stop executing or not
    *
    * @param shouldRun true if the thread should continue to run, false if not
    */
   void setShouldRun(bool shouldRun);


private:
   wf_ngp::NavReplyPacket* receiveBlocking();
   
   bool receiveBlocking(void* buffer,
                        unsigned int bytesToRead);
   //XXX: the serverhandler that gets our packets from the server. please
   //remove and (re)design
   NGPMessageReceiver* m_msgReceiver;

   ///member holding the thread executing policy
   bool m_shouldRun;

   ///The mutex that protects the m_shouldRun member
   Mutex* m_shouldRunMutex;
   
   ///the pal thread associated with this class
   Thread* m_thread;

   ///data to pass to the thread, used to get the thread into the right class
   struct readerData* m_data;

   ///the PAL connection that this thread operates on
   PALConnection* m_palConnection;
};

//the thread data structure
typedef struct readerData {
   ConnectionReader* connectionReader;
} readerData_t;

#endif // CONNECTIONREADER_H
