/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CMCONNECTION_H
#define CMCONNECTION_H

#include "Connection.h"
#include "PAL.h"

class ServerHandler;

/**
 *
 */
class CMConnection {

public:

   /**
    * Constructor.
    */
   CMConnection(Connection* c);
   
   /**
    * Destructor.
    */
   ~CMConnection();

   /**
    * Read data from this connection.
    *
    * @param buffer The buffer to read into from the network
    * @param bytesToRead The number of bytes we want to read in this run
    * @param bytesRead The actual number of bytes that was available for
    * reading
    * @return the pal status of the read.
    */
   pstatus read(void* buffer,
             unsigned int bytesToRead,
             unsigned int*bytesRead);

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

   // /**
   //  * Start the connection reader thread. This should be done after the associated
   //  * connection is started and established so we can read from it.
   //  *
   //  * @param serverHandler XXX: please remove me. Should be an interface instead.
   //  */
   // void startReader(ServerHandler* serverHandler);
   
private:
   /// Member holding the associated connection
   Connection* m_connection;
};

#endif // CMCONNECTION_H
