/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
*/

#ifndef PALSOCKET_H
#define PALSOCKET_H

#include "PAL.h"
#include "PALTypes.h"

struct SocketContext {
   SocketContext() :
      fd(0), 
      serverAddressLength(0), 
      serverAddress(NULL),
      serverPort(0) {}

   int fd;
   unsigned int serverAddressLength;
   char* serverAddress;
   unsigned int serverPort;
};

/**
 * Below are the basic primitives used for connect to the server. They need to
 * be accurate implemented for the network connection to work.
 */

/**
 * Create the socket to use for connections.
 * The socket should have a IPv4 domain, i.e. PF_INET on Linux and the
 * connection type should be sequenced, reliable, two-way, connection-based
 * byte streams. This function also creates the socket context used for this
 * socket.
 * 
 * @param networkContext The environment for the network connection.
 * @param socketContext Contains information about the created socket.
 *
 * @return PAL_OK if the socket was created correctly.
 * @return PAL_PROTOCOL_NOT_SUPPORTED if the desired protocol is not supported.
 * @return PAL_OUT_OF_MEMORY if there is not enough memory to create a socket.
 * @return PAL_SOCKET_ERROR if there is a general socket error.
 */
pstatus pal_socket_create(SocketContext* socketContext);

/**
 * This function will initiate the connection on a socket.
 * It should use the connection parameters that has been set to set up a
 * connection to the specified address. The socket context holds which 
 * socket that should be connected.
 *
 * @param socketContext The socket context to connect with.
 * 
 * @return PAL_OK if the connection attempt succeeded.
 * @return PAL_CONNECTION_REFUSED if no one is listening for the connection in
 *         the remote address.
 * @return PAL_SOCKET_ALREADY_CONNECTED The socket is already connected.
 * @return PAL_CONNECTION_ATTEMPT_TIMEDOUT The connection attempt timed out.
 * @return PAL_CONNECT_ERROR is there is a general connect error.
 */
pstatus pal_socket_connect(SocketContext* socketContext);

/**
 * This function will shutdown the connection and the appropriate steps to do
 * this needs to be handled here. It will also destroy the socket context.
 * After this function has been called, a new socket needs to be created
 * to be able to set up a new connection.
 *
 * @param socketContext Contains information about the socket to disconnect.
 *
 * @return PAL_OK if the disconnection succeeded.
 * @return PAL_SOCKET_NOT_CONNECTED if the socket was not connected.
 * @return PAL_DISCONNECT_ERROR if there was a general disconnnect error.
 */
pstatus pal_socket_disconnect(SocketContext* socketContext);

/**
 * This function will check the socket if they are ready for writing or
 * reading. The two last parameters will be used as both in and out
 * parameters. The corresponding platform specific select function should be
 * called when this function is called. For example this function can be called
 * with parameter readReady is set to true and writeReady set to NULL. This
 * means that a select shall be performed on the socketContext where socket
 * should be checked for if it is possible to read from it. If it is parameter
 * shall still be set to true and if it is not it should be set to false.
 * 
 * If timeout is set to 0 (zero) then select should return
 * immediately, if it is set to NULL, select can block indefinitely.
 * 
 * @param socketContext The socket to read from.
 * @param timeout The timeout value in seconds for this call, can be NULL.
 * @param readReady Out parameter if socket is ready to be read.
 * @param writeReady Out parameter if socket is ready to be written to.
 *
 * @return PAL_OK if the operation succeeded.
 * @return PAL_SELECT_ERROR if the operation failed.
 */
pstatus pal_socket_select(SocketContext* socketContext,
                          int* timeout, 
                          bool* readReady,
                          bool* writeReady);

/**
 * This function will read the data that was received from the server.
 *
 * @param socketContext The socket to r2ead from.
 * @param buffer Where the data should be written to.
 * @param sizeToWrite The number of bytes to read.
 * @param sizeWritten The number of byte that was read.
 * 
 * @return PAL_OK if the operation succeeded.
 * @return PAL_READ_ERROR if the operation failed.
 */
pstatus pal_socket_read(SocketContext* socketContext,
                        void* buffer,
                        unsigned int bytesToRead,
                        unsigned int* bytesRead);

/**
 * This function will write data to the socket so it will be sent to the 
 * server.
 * 
 * @param socketContext The socket to write to.
 * @param buffer Contains the data to write to the socket.
 * @param sizeToWrite The number of bytes to write to the socket.
 * @param sizeWritten The number of byte that was written.
 * 
 * @return PAL_OK if the operation succeeded.
 * @return PAL_WRITE_FAILED if the operation failed.
 */
pstatus pal_socket_write(SocketContext* socketContext,
              const void* const buffer,
              unsigned int bytesToWrite,
              unsigned int* bytesWritten);


#endif // end PALSOCKET_H

