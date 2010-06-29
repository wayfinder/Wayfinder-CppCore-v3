/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "CONNECTION"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#define CORE_LOGGING_MODULE_LEVEL NONE

#include "CoreDebugPrint.h"
#include "Connection.h"
#include "PAL.h"
#include <string.h>

/**
 *
 */
Connection::Connection( const ConnectionInfo& connectionInfo ) :
   m_connectionInfo(connectionInfo),
   m_palConnection(NULL)
{
   coreprintln("Connection() Begin");

   m_isEstablished = false;
   m_palConnection = new PALConnection();

   // Set server parameters
   coreprintln("Connection() Set server parameters");
   ServerParameters sp;
   sp.type = SERVER_PARAMETER_TYPE_CONNECTION;

   sp.spc.port = connectionInfo.port;
   // Hostname is always in UTF8 so strlen will work
   sp.spc.hostNameLength = strlen( connectionInfo.hostName.c_str() );
   sp.spc.hostNameLength += 1; // Include 0 termintation
   sp.spc.pHostName = new char[sp.spc.hostNameLength];
   memcpy( sp.spc.pHostName, connectionInfo.hostName.c_str(), sp.spc.hostNameLength );
   coreprintln("Connection() %s", sp.spc.pHostName );
   coreprintln("Connection() %d", sp.spc.port);
   // Set the parameter values
   pal_connection_setParameters( m_palConnection, &sp );
   // Free the memory
   delete[] sp.spc.pHostName;

   // Subscribe to changes
   pal_connection_addCallbackFunc(&Connection::CconnectionCallback, this);

   coreprintln("Connection() End");
}

/**
 *
 */
Connection::~Connection()
{
   coreprintln("~Connection() Begin");
   // Unsubscribe from changes
   pal_connection_removeCallbackFunc(&Connection::CconnectionCallback, this);

   // Terminate any connection open
   terminate();
   coreprintln("~Connection() End");
}

// pstatus Connection::read(void* buffer,
//                       unsigned int bytesToRead,
//                       unsigned int*bytesRead)
// {
//    // Temporary for testing
//    pstatus status = pal_connection_read(m_palConnection,
//                                         buffer,
//                                         bytesToRead,
//                                         bytesRead);
//    if(status != PAL_OK) {
//       coreprintln("socket returned something else than PAL_OK: %d", status);
//       //tell the reader queue to stop looping
//       m_reader->setShouldRun(false);
//    }
//    return status;
// }

void Connection::write(const void* const buffer,
                       unsigned int sizeToWrite,
                       unsigned int* sizeWritten)
{
   // Temporary for testing
   (void)pal_connection_write(m_palConnection, buffer,
                              sizeToWrite, sizeWritten);
}

void Connection::startReader(NGPMessageReceiver* receiver) {
   m_reader = new ConnectionReader(receiver, m_palConnection);
   //start thread: (done in reader constructor right now)
   //m_reader->startThread();
}

/**
 *
 */
ConnectionStatus Connection::establish(NGPMessageReceiver* receiver)
{
   coreprintln("Connection::establish Begin");
   coreprintln("Connection::establish to %s:%d",
               m_connectionInfo.hostName.c_str(), 
               m_connectionInfo.port );

   // set m_isEstablished = true; upon callback
   pstatus status = pal_connection_establish(m_palConnection);
   if(status == PAL_OK) {
      
   }
   // Testing
   m_isEstablished = true;

   startReader(receiver);
   coreprintln("Connection::establish End (%d)", status);
   return CONNECTION_STATUS_OK;
}

/**
 *
 */
ConnectionStatus Connection::terminate()
{
   coreprintln("Connection::terminate Begin");

   if(isEstablished()) {
      //tell the reader queue to stop looping
      // this is just for safety sake, the read call above should handle this as well.
      m_reader->setShouldRun(false);

      pstatus status = pal_connection_terminate(m_palConnection);
      if(status == PAL_OK) {
      
      }

      // Testing
      m_isEstablished = false;

      delete m_reader;
      m_reader = NULL;
      
      coreprintln("Connection::terminate isEstablished End, connection terminate=%d", status);
   }
   coreprintln("Connection::terminate End");
   return CONNECTION_STATUS_OK;
}

/**
 *
 */
bool Connection::isEstablished()
{
   coreprintln("Connection::isEstablished Begin");

   coreprintln("Connection::isEstablished End (%s)", m_isEstablished?"TRUE":"FALSE");
   return m_isEstablished;
}

/**
 *
 */
const ConnectionInfo& Connection::getInfo()
{
   coreprintln("Connection::getInfo Begin");
   coreprintln("Connection::getInfo %s", m_connectionInfo.hostName.c_str());
   coreprintln("Connection::getInfo %d", m_connectionInfo.port);
   coreprintln("Connection::getInfo %s", m_connectionInfo.type==CONNECTION_TYPE_NONSECURE?"NONSECURE":"SECURE");
   coreprintln("Connection::getInfo %s", m_connectionInfo.bearer.isAvailable()?"Bearer Available":"Bearer Unavailable");
   coreprintln("Connection::getInfo %s", m_connectionInfo.bearer.isConnected()?"Bearer Connected":"Bearer Not connected");
   coreprintln("Connection::getInfo %s", m_connectionInfo.bearer.getType()==NETWORK_BEARER_PDP?"PDP":"WiFi");
   coreprintln("Connection::getInfo End");
   return m_connectionInfo;
}

void Connection::CconnectionCallback(void* extendedErrorData,
                                     unsigned int platformStatus,
                                     void* data)
{
   coreprintln("Connection::CconnectionCallback");
   Connection* connection = static_cast<Connection*>(data);
   
   connection->connectionCallback(extendedErrorData,
                                  platformStatus);
}

void Connection::connectionCallback(void* extendedErrorData,
                                    unsigned int platformStatus)
{
   coreprintln("Connection::connectionCallback");
}

