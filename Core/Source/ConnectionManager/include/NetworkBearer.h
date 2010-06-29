/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef NETWORK_BEARER_H
#define NETWORK_BEARER_H

#include "PAL.h"
#include "PALBearer.h"

typedef enum {
   NETWORK_BEARER_NONE = 0x00,
   NETWORK_BEARER_PDP  = 0x01,
   NETWORK_BEARER_WIFI = 0x02,
   NETWORK_BEARER_ANY  = 0xff
} NetworkBearerType;


/**
 * The Network Bearer class handles all bearer related logic such
 * as connecting to PDP and connection to WiFi
 *
 */
class NetworkBearer {

public:
   /**
    * Constructor.
    */
   NetworkBearer( NetworkBearerType bearer );

   /**
    * Destructor.
    */
   virtual ~NetworkBearer();

   /**
    * TODO
    * @param status
    */
   virtual void connect() = 0;

   /**
    * TODO
    * @param status
    */
   virtual void disconnect() = 0;

   /**
    * TODO
    * @param status
    */
   virtual void getStatus() const = 0;

   /**
    * Method for checking if a network is connected
    *
    * @return bool True if a network is connected
    */
   bool isConnected() const;

   /**
    * Method for checking if a bearer type (this) is available 
    * Eg. When a WiFi network is nearby by currently not connected to this method will return true
    * Eg. When a WiFi network is connected this method will return true
    *
    * @return bool True if a network is available (any network including PDP)
    */
   bool isAvailable() const;

   /**
    * Method for retrieving the type of bearer being used
    *
    * @return NetworkBearerType The type of bearer used
    */
   NetworkBearerType getType() const;

   /**
    * Method for receiving pal_network_bearer callbacks
    *
    * @return NetworkBearerType The type of bearer used
    */
   static void CbearerCallback(void* extendedErrorData,
                               unsigned int platformStatus,
                               void* data);

private:

   /**
    * Internal method for handling pal_network_bearer callbacks
    *
    * @return NetworkBearerType The type of bearer used
    */
   void bearerCallback(void* extendedErrorData,
                       unsigned int platformStatus);
   
   
   // The type of bearer represented in the object
   const NetworkBearerType m_bearerType;
   // Is the bearer connected
   bool m_isConnected;
   // Is the bearer available, e.g. is WiFi nearby?
   bool m_isAvailable;
};

#endif // NETWORK_BEARER_H
