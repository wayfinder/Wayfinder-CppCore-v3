/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "CONNECTIONMANAGER"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#define CORE_LOGGING_MODULE_LEVEL NONE

#include "CoreDebugPrint.h"
#include "ConnectionManager.h"
#include "ServerHandler.h"

static NetworkBearerPDP bearerPDP = NetworkBearerPDP();

ConnectionManager::ConnectionManager()
   : m_currConnId(1) // id 0 means invalid connection
{
   coreprintln("ConnectionManager() Begin ---------------------------------------------------------------------------");

   coreprintln("ConnectionManager() End");
}


ConnectionManager::~ConnectionManager()
{
   coreprintln("~ConnectionManager() Begin");

   // Close all connections
   closeAllConnections();
   
   // Close all bearers
   closeAllBearers();

   coreprintln("~ConnectionManager() End");
}

void ConnectionManager::enableBearer( const NetworkBearerType bearer )
{
   coreprintln("ConnectionManager::enableBearer Begin");

   switch( bearer ) {
      case NETWORK_BEARER_NONE:
         // disable all bearers
         disableBearer( bearer );
         break;
      case NETWORK_BEARER_PDP:
         m_networkBearers.push_back( new NetworkBearerPDP() );
         break;
      case NETWORK_BEARER_WIFI:
         m_networkBearers.push_back( new NetworkBearerWiFi() );
         break;
      case NETWORK_BEARER_ANY:
         m_networkBearers.push_back( new NetworkBearerPDP() );
         m_networkBearers.push_back( new NetworkBearerWiFi() );
         break;
      default: 
         break;
   }

   coreprintln("ConnectionManager::enableBearer End");
}

void ConnectionManager::disableBearer( const NetworkBearerType bearer )
{
   coreprintln("ConnectionManager::disableBearer Begin");

   switch( bearer ) {
      case NETWORK_BEARER_ANY:
      case NETWORK_BEARER_NONE:
         // disable all bearers
         break;
      case NETWORK_BEARER_PDP:
         break;
      case NETWORK_BEARER_WIFI:
         break;
      default: 
         break;
   }

   coreprintln("ConnectionManager::disableBearer End");
}



void ConnectionManager::closeConnection( wf_uint32 connId )
{
   coreprintln("ConnectionManager::closeConnection Begin");

   // Close the connection
    
   coreprintln("ConnectionManager::closeConnection End");
}

bool ConnectionManager::haveNetworkBearer()
{
   return haveNetworkBearer( NETWORK_BEARER_ANY );
}
bool ConnectionManager::haveNetworkBearer( NetworkBearerType bearerType )
{
   coreprintln("ConnectionManager::haveNetworkBearer Begin");
   bool haveNetworkBearer = false;
   NetworkBearerType foundBearer = NETWORK_BEARER_NONE;

   std::vector<NetworkBearer*>::iterator it = m_networkBearers.begin();
   while( !haveNetworkBearer && (it < m_networkBearers.end()) ) {
      // Check for connected bearers
      if( (*it)->isConnected() ) {
         if( ((foundBearer = (*it)->getType()) & bearerType) ) {
            // Found matching bearer
            haveNetworkBearer = true;
         }
      }
      it++;
   }

   coreprintln("ConnectionManager::haveNetworkBearer End (%s, %x)", haveNetworkBearer?"TRUE":"FALSE", foundBearer);
   return haveNetworkBearer;
}

bool ConnectionManager::haveNetworkConnection()
{
   coreprintln("ConnectionManager::haveNetworkConnection Begin");

   // If m_networkConnections is NOT empty, we have a network connection
   return !m_networkConnections.empty();

}

void ConnectionManager::closeAllConnections()
{
   coreprintln("ConnectionManager::closeAllConnections Begin");

   NetworkConnectionMap::iterator it = m_networkConnections.begin();
   while(it != m_networkConnections.end()) {
      delete it->second;
      it++;
   }

   // We must clear the whole map to get size 0
   m_networkConnections.clear();
   
   coreprintln("ConnectionManager::closeAllConnections End");
}

void ConnectionManager::closeAllBearers()
{
   coreprintln("ConnectionManager::closeAllBearers Begin");

   std::vector<NetworkBearer*>::iterator it = m_networkBearers.begin();
   while(it < m_networkBearers.end()) {
      delete (*it);
      it++;
   }

   coreprintln("ConnectionManager::closeAllBearers End");
}

wf_uint32 ConnectionManager::getConnection(NetworkBearerType bearerType,
                                           ConnectionType connType,
                                           NGPMessageReceiver* msgReceiver)
{
   coreprintln("ConnectionManager::getConnection Begin (%d %d)",
               bearerType, connType);

#ifdef IPHONE
   const char* serverHostname = "oss-nav.services.wayfinder.com";
   wf_uint16 serverPort = 7655;
#else
   const char* serverHostname = "oss-nav.services.wayfinder.com";
   wf_uint16 serverPort = 7655;
#endif

   // Temporary for testing
   ConnectionInfo ci = ConnectionInfo( serverHostname, 
                                       serverPort, 
                                       connType, 
                                       bearerPDP );
   Connection* c = new Connection( ci );
   wf_uint32 currConnId = m_currConnId++;
   m_networkConnections[currConnId] = c;
   // Testing - connect 
   c->establish(msgReceiver);
   coreprintln("ConnectionManager::getConnection End");
   // For testing
   return currConnId;
}

WriteStatus ConnectionManager::writeBlocking(wf_uint32 connectionId,
                                             const void* const buffer,
                                             unsigned int sizeToWrite,
                                             unsigned int* sizeWritten)
{
   if (m_networkConnections.find(connectionId) == m_networkConnections.end()){
      // No such connection available!
      return NO_SUCH_CONNECTION_AVAILABLE;
   }
   m_networkConnections[connectionId]->write(buffer,
                                             sizeToWrite,
                                             sizeWritten);
   return SUCCESSFULLY_WROTE_EVERYTHING;
}

