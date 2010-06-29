/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "NETWORKBEARER"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#define CORE_LOGGING_MODULE_LEVEL NONE

#include "CoreDebugPrint.h"
#include "NetworkBearer.h"

NetworkBearer::NetworkBearer( NetworkBearerType bearer ) : m_bearerType( bearer )
{
   coreprintln("NetworkBearer() Begin");

   // Not connected by default
   m_isConnected = false;
   // Not available by default
   m_isAvailable = false;

   // Subscribe to changes
   pal_bearer_addCallbackFunc(&NetworkBearer::CbearerCallback, this);

   coreprintln("NetworkBearer() End");
}

NetworkBearer::~NetworkBearer()
{
   coreprintln("~NetworkBearer() Begin");

   // Unsubscribe from changes
   pal_bearer_removeCallbackFunc(&NetworkBearer::CbearerCallback, this);

   coreprintln("~NetworkBearer() End");
}

bool NetworkBearer::isConnected() const
{
   coreprintln("NetworkBearer::isConnected Begin");
   coreprintln("NetworkBearer::isConnected End (%s)", m_isConnected?"TRUE":"FALSE");
   return m_isConnected;
}

bool NetworkBearer::isAvailable() const
{
   bool retVal = false;
   coreprintln("NetworkBearer::isAvailable Begin");
   if( m_isConnected ) {
      retVal = m_isConnected;
   }
   if( !retVal ) {
     retVal = m_isAvailable;
   }
   coreprintln("NetworkBearer::isAvailable End (%s)", retVal?"TRUE":"FALSE");
   return retVal;
}

NetworkBearerType NetworkBearer::getType() const
{
   coreprintln("NetworkBearer::getType Begin");
   coreprintln("NetworkBearer::getType End (%x)", m_bearerType);
   return m_bearerType;
}

void NetworkBearer::CbearerCallback(void* extendedErrorData,
                               unsigned int platformStatus,
                               void* data)
{
   coreprintln("NetworkBearer::CbearerCallback");
   NetworkBearer* networkBearer = static_cast<NetworkBearer*>(data);
   
   networkBearer->bearerCallback(extendedErrorData,
                                 platformStatus);
}

void NetworkBearer::bearerCallback(void* extendedErrorData,
                              unsigned int platformStatus)
{
   coreprintln("NetworkBearer::bearerCallback");
   // Callback
}

