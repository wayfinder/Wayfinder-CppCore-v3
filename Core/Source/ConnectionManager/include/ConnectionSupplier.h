/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CONNECTIONSUPPLIER_H
#define CONNECTIONSUPPLIER_H

#include "NetworkBearer.h"
#include "Connection.h" // XXX: TODO: refactor enums to separate headers.

enum WriteStatus {
   NO_SUCH_CONNECTION_AVAILABLE = 0,
   SUCCESSFULLY_WROTE_EVERYTHING,
   // SUCCESSFULLY_WROTE_SOMETHING
};

class ConnectionSupplier {
public:

   virtual ~ConnectionSupplier() {};

   /** 
    * Method for getting a network connection.
    * 
    * @param bearerType The wanted bearer type for the connection
    *                   (pdp, wifi, any).
    * 
    * @return The id for the requested connection, if no such connection
    *         available, 0 will be returned.
    */
   wf_uint32 getConnection(NetworkBearerType bearerType);

   /** 
    * Method for getting a network connection.
    * 
    * @param connType The wanted type of connection (secure / nonsecure).
    * 
    * @return The id for the requested connection, if no such connection
    *         available, 0 will be returned.
    */
   wf_uint32 getConnection(ConnectionType connType);

   /** 
    * Method for getting a network connection.
    * 
    * @param bearerType The wanted bearer type for the connection
    *                   (pdp, wifi, any).
    * @param connType The wanted type of connection (secure / nonsecure).
    * 
    * @return The id for the requested connection, if no such connection
    *         available, 0 will be returned.
    */
   virtual wf_uint32 getConnection(NetworkBearerType bearerType,
                                   ConnectionType connType,
                                   NGPMessageReceiver* msgReceiver) = 0;

   
   /**
    * Close connection
    *
    * @param connID The connection that will no longer be used.
    */
   virtual void closeConnection(wf_uint32 connID) = 0;


   virtual WriteStatus writeBlocking(wf_uint32 connectionId,
                                     const void* const buffer,
                                     unsigned int sizeToWrite,
                                     unsigned int* sizeWritten) = 0;


   // FIXME: maybe this should not reside in this interface?
   virtual void enableBearer(NetworkBearerType bearer) = 0;
   
};

#endif /* CONNECTIONSUPPLIER_H */
