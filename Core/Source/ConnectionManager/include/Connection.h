/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CONNECTION_H
#define CONNECTION_H

#include "ConnectionReader.h"
#include "ConnectionWriter.h"
#include "NetworkBearer.h"
#include "PAL.h"
#include "PALNetwork.h"
#include "PALConnection.h"
#include <string>

class NGPMessageReceiver;

/**
 * Enum to hold the type of connection
 */
typedef enum
{
   CONNECTION_TYPE_NONSECURE,    // Any connection will do
   CONNECTION_TYPE_SECURE        // A secure connection is required
} ConnectionType;

/**
 * Enum to hold the connection status codes
 */
typedef enum
{
   CONNECTION_STATUS_OK,
   CONNECTION_STATUS_ERROR_NO_CONNECTION,
   CONNECTION_STATUS_ERROR_UNKNOWN
} ConnectionStatus;

/**
 * A structur to hold all necessary information about a connection
 */
struct ConnectionInfo
{
   std::string hostName;         // The host to connect to
   wf_uint16 port;               // The port to connect to
   const ConnectionType type;    // Connection security type
   const NetworkBearer& bearer;  // Bearer information

   ConnectionInfo(std::string hostName,
                  wf_uint16 port,
                  const ConnectionType connectionType,
                  const NetworkBearer& networkBearer ) 
                  : hostName(hostName),
                    port(port),
                    type(connectionType), 
                    bearer(networkBearer)
   {
   }

   ConnectionInfo(const ConnectionInfo& connectionInfo) 
                  : hostName(connectionInfo.hostName),
                    port(connectionInfo.port),
                    type(connectionInfo.type), 
                    bearer(connectionInfo.bearer)
   {
   }
};

/**
 * The Connection class handles Network Connections such as
 * e.g. an HTTP Connection to the Wayfinder Server
 *
 */
class Connection {

public:

   /**
    * Constructor.
    *
    * @param connectionInfo a reference to a ConnectionInfo, which will be used
    *                       to connect to the right server
    */
   Connection( const ConnectionInfo& connectionInfo );
   
   /**
    * Destructor.
    */
   ~Connection();

   /** 
    * Method for establishing a connection
    * 
    * @param serverHandler Please remove me when designed properly
    * 
    * @return the status of the connection
    */
   ConnectionStatus establish(NGPMessageReceiver* receiver);

   /**
    * Method for terminating a connection
    *
    */
   ConnectionStatus terminate();

   // /**
   //  * Read data from this connection.
   //  *
   //  * @param buffer The buffer to read into from the network
   //  * @param bytesToRead The number of bytes we want to read in this run
   //  * @param bytesRead The actual number of bytes that was available for
   //  * reading
   //  * @return the pal status of the read.
   //  */
   // pstatus read(void* buffer,
   //           unsigned int bytesToRead,
   //           unsigned int*bytesRead);

   /**
    * Write data to this connection.
    *
    * @param buffer The buffer to write to the network
    * @param bytesToRead The number of bytes we want to write
    * @param bytesRead The actual number of bytes that was written
    * reading
    */
   void write(const void* const buffer,
              unsigned int sizeToWrite,
              unsigned int* sizeWritten);

   /**
    * Method for checking if a connection is established
    *
    * @return bool True if a connection is established
    */
   bool isEstablished();

   /**
    * Method for retrieving the connection information
    *
    * @return ConnectionInfo The information regarding the connection
    */
   const ConnectionInfo& getInfo();

   /**
    * Method for receiving pal_connection callbacks
    *
    * @return ConnectionInfo The information regarding the connection
    */
   static void CconnectionCallback(void* extendedErrorData,
                                   unsigned int platformStatus,
                                   void* data);
private:
   /**
    * Internal method for handling pal_connection callbacks
    *
    * @return ConnectionInfo The information regarding the connection
    */
   void connectionCallback(void* extendedErrorData,
                           unsigned int platformStatus);

   
   /**
    * Start the connection reader thread. This should be done after the associated
    * connection is started and established so we can read from it.
    *
    * @param serverHandler XXX: please remove me. Should be an interface instead.
    */
   void startReader(NGPMessageReceiver* receiver);
   
   
   // Is the connection established
   bool m_isEstablished;
   // A ConnectionInfo object holds all information reg. the connection destination
   ConnectionInfo m_connectionInfo;

   PALConnection* m_palConnection;

   // Queue
   // TODO add Queue
   // Reader
   ConnectionReader* m_reader;
   // Writer
   ConnectionWriter* m_writer;
};

#endif // CONNECTION_H
