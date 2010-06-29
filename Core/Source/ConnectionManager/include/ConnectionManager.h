/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include "NetworkBearer.h"
#include "NetworkBearerPDP.h"
#include "NetworkBearerWiFi.h"
#include "Connection.h"
#include "CMConnection.h"
#include "ConnectionSupplier.h"
#include <vector>
#include <map>

/**
 * The ConnectionManager handles Network Bearers and Network Connection
 *
 */
class ConnectionManager : public ConnectionSupplier {

public:

   /**
    * Constructor.
    */
   ConnectionManager();

   /**
    * Destructor.
    */
   virtual ~ConnectionManager();

   /**
    *
    * @param 
    */
   void enableBearer( NetworkBearerType bearer );
   void disableBearer( NetworkBearerType bearer );

   /**
    * Method for getting a Network Connection
    * Method will actually return a pointer to the out-queue of the Connection
    *
    * @param bearerType The bearer type requested for this connection
    */
   void getNetworkConnection( NetworkBearerType bearerType /*, toWhere */ );

   // /**
   //  *Get the connection for a specific bearer and type
   //  *
   //  * @param bearer
   //  * @param type
   //  * @param serverHandler XXX Please remove me
   //  * 
   //  */
   // CMConnection* getConnection( NetworkBearerType bearer, ConnectionType type, ServerHandler* serverHandler );

   // /**
   //  *Get the connection that represents a specific type
   //  *
   //  * @param type
   //  * @param serverHandler XXX Please remove me
   //  */
   // CMConnection* getConnection( ConnectionType type, ServerHandler* serverHandler );


   virtual void closeConnection( wf_uint32 connID );

   /**
    * Method for checking if any network bearer is connected
    *
    * @return bool True if a network bearer is setup and functional
    */
   bool haveNetworkBearer();

   /**
    * Method for checking if a specific bearer type (WiFi/PDP etc) is setup and functional
    *
    * @return bool True if the network bearer is setup and functional
    */
   bool haveNetworkBearer( NetworkBearerType bearerType );

   /**
    * Method for checking if a connection is setup and functional
    * Note this method does not know where the connection terminates
    *
    * @return bool True if a network connection is setup and functional
    *
    * TODO Add WHERE check !
    */
   bool haveNetworkConnection();

   virtual wf_uint32 getConnection(NetworkBearerType bearerType,
                                   ConnectionType connType,
                                   NGPMessageReceiver* msgReceiver);

   virtual WriteStatus writeBlocking(wf_uint32 connectionId,
                                     const void* const buffer,
                                     unsigned int sizeToWrite,
                                     unsigned int* sizeWritten);
   
   
private:

   /**
    * 
    *
    * @return
    */
   void closeAllConnections();

   /**
    * 
    *
    * @return 
    */
   void closeAllBearers();

   // A vector with all connected network bearers
   std::vector<NetworkBearer*> m_networkBearers;
   
   // // A vector with all established network connection
   typedef std::map<wf_uint32, Connection*> NetworkConnectionMap;
   NetworkConnectionMap m_networkConnections;

   wf_uint32 m_currConnId;
   // The Queue for requests / timer messages
   // TODO :: MessageQueue reqQueue;
};

#endif // CONNECTION_MANAGER_H
