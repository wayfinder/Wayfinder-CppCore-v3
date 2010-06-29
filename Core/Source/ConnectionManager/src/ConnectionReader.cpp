/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "CONNECTIONREADER"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#define CORE_LOGGING_MODULE_LEVEL NONE

#include "CoreDebugPrint.h"
#include "ConnectionReader.h"
#include "ServerHandler.h"
#include "NavReplyPacket.h"
#include "NavRevGeocodingReply.h"
#include "PALThreading.h"
#include "PALConnection.h"
#include "PacketFactory.h"

#include <iostream>
#include <sstream>
#include <cstring>
#include <memory>
/**
 *
 */
ConnectionReader::ConnectionReader(NGPMessageReceiver* msgReceiver,
                                   PALConnection* palConnection) :
   m_msgReceiver(msgReceiver),
   m_palConnection(palConnection)
{
   coreprintln("ConnectionReader() Begin");

   m_data = new readerData_t;
   m_shouldRunMutex = pal_thread_create_mutex();
   m_data->connectionReader = this;
   setShouldRun(true);
   m_thread = pal_thread_create_thread(&threadFunc, m_data);
   pal_thread_start_thread(m_thread);

   coreprintln("ConnectionReader End");
}

/**
 *
 */
ConnectionReader::~ConnectionReader()
{
   coreprintln("~ConnectionReader Begin");

   setShouldRun(false);
   coreprintln("joining thread, shouldRun=%d...", getShouldRun());
   pal_thread_join(m_thread, 0, 0);
   delete m_data;
   pal_thread_destroy_thread(m_thread);
   m_thread = NULL;
   pal_thread_destroy_mutex(m_shouldRunMutex);

   coreprintln("~ConnectionReader End");
}

void ConnectionReader::run(void* arg)
{
   while(getShouldRun()) {
      wf_ngp::NavReplyPacket* packet = receiveBlocking();
      if (packet != NULL){
         coreprintln("packet received, sending to receiver");
         m_msgReceiver->receivedNGPPacket(packet);

      } else {
         coreprintln("could not read packet!");
      }
   }
   coreprintln("Reader thread leaving run");
}

void ConnectionReader::threadFunc(void* arg)
{
   readerData_t* data = static_cast<readerData_t*> (arg);
   data->connectionReader->run(arg);
}

bool ConnectionReader::getShouldRun() {
   Lock lock(m_shouldRunMutex);
   return m_shouldRun;
}

void ConnectionReader::setShouldRun(bool shouldRun) {
   Lock lock(m_shouldRunMutex);
   m_shouldRun = shouldRun;
}

// read as many as requested or err
bool ConnectionReader::receiveBlocking(void* buffer,
                                       unsigned int bytesToRead)
{
   bool done = false;

   unsigned int bytesRead;
   char* bufferChar = static_cast<char*>(buffer);
   while (!done){

      // Temporary for testing
      pstatus result = pal_connection_read(m_palConnection,
                                           bufferChar,
                                           bytesToRead,
                                           &bytesRead);

      if (result == PAL_SOCKET_END_OF_FILE) {
         coreprintln("receiveBlocking got end of file!!");
         setShouldRun(false);
         return false;
         
      } else if (result != PAL_OK) {
         // Something is wrong with the socket or network
         coreprintln("reading from the socket returns"
                     " pstatus error: %d",
                     result);

         //tell the reader queue to stop looping
         setShouldRun(false);
         return false;
         
      } else if (bytesRead < bytesToRead){
         coreprintln("receiveBlocking could not read everything at "
           "once, read %d of %d bytes. First byte: %x",
           bytesRead,
           bytesToRead,
           bufferChar[0]);
         
         bufferChar += bytesRead;
         bytesToRead -= bytesRead;

      } else {
         // No error found, and everything seems to be read.
         done = true;
         coreprintln("receiveBlocking successfully read all %d bytes"
                     ", exiting.", dbgBytesToRead);
      }
   }

   return done;
}

wf_ngp::NavReplyPacket* ConnectionReader::receiveBlocking()
{
   using namespace std;
   using namespace wf_ngp;

   byte headerbuf[HEADER_LENGTH];
   if(! receiveBlocking(headerbuf, HEADER_LENGTH)) {
      coreprintln("Could not read header");
      return NULL;
   }

   uint32 packLength;
   STXStatus status = checkSTXAndSetPacketLength(
      headerbuf, HEADER_LENGTH, packLength);

   if(status != STX_VALID) {
      ostringstream os;
      os << hex << showbase;
      for(uint32 i=0; i < HEADER_LENGTH; i++) {
         os << (int) headerbuf[i] << " ";
      }

      coreprintln("Got incorrect STX header, something is weird.."
                  " first %d bytes: %s",
                  wf_ngp::HEADER_LENGTH,
                  os.str().c_str());

      return NULL;
   }
   // allocating the buffer for the packet on the heap in case it's big
   // XXX: fix this, will use wrong delete call! auto_ptr uses
   // delete and not delete[].
   auto_ptr<byte> packetPtr(new byte[packLength]);
   coreprintln("receiveBlocking() blocking for %u bytes",
               packLength);
   bool receivedOk = receiveBlocking(packetPtr.get(), packLength);
   coreprintln("eceiveBlocking() read packet, calling "
               "createReplyPacketFromBuffer(%p, %u)",
               packetPtr.get(), packLength);
   if (receivedOk){
      NavReplyPacket* reply = createReplyPacketFromBuffer(packetPtr.get(),
                                                          packLength);
      coreprintln("receiveBlocking() created packet %p", reply);

      if(reply == 0) {
         coreprintln("Reply packet is not of a supported type");
         return NULL;
      }
      
      return reply;

   } else {
      // We could not receive everything! is something wrong?
      coreprintln("For some reason, we could not receive everything");
      return NULL;
   }
}
