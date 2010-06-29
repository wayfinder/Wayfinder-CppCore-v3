/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
*/

#ifndef PALCONNECTION_H
#define PALCONNECTION_H

#include "PAL.h"
#include "PALTypes.h"
#include <string>

enum ServerParameterTypes
{
   SERVER_PARAMETER_TYPE_CONNECTION
};

struct ServerParametersConnection
{
   wf_uint8 hostNameLength;      // The host name length incl. the trailing 0
   char* pHostName;              // The UTF8 host name
   wf_uint16 port;               // The port to connect to
};

struct ServerParameters
{
   ServerParameterTypes type;

   union {
      ServerParametersConnection spc;
   };
};

/**
 *
 * A structure to hold information about a PALConnection
 *
 */
struct PALConnection
{
   PALConnection() :
      context(NULL) {}

   void* context;                // Pointing to the Context (e.g. SocketContext)
};

/**
 * Typedef for the PALNetwork callback function.
 *
 * @param extendedErrorData Data specific to the networkContextState.
 * @param platformStatus This can be used to send up a platform specific error
 *                       code to the UI.
 * @param data Contains scope information.
 */
typedef void (*ConnectionCallbackFunc)(void* extendedErrorData,
                                       unsigned int platformStatus,
                                       void* data);

/**
 * A Function for establishing a connection
 * The data on where to connect to is set via the set() function call
 *
 * @param connection a PALConnection object
 *
 * @return PAL_OK if the establish call succeeded
 * @return PAL_NO_CONTEXT_PARAMS if the connection is not setup
 */
pstatus pal_connection_establish(PALConnection* const connection);

/**
 * A Function for terminating a connection
 * 
 * @param connection a PALConnection object
 *
 * @return PAL_OK if the terminate call succeeded
 * @return PAL_CONNECTION_ERROR if the terminate call failed
 * @return PAL_NOT_CONNECTED if there is no connection to terminate
 */
pstatus pal_connection_terminate(const PALConnection* const connection);

/**
 * A Function for setting up a connection
 *
 * @param connection a PALConnection object
 * @param parameters a Parameters object
 *
 * @return PAL_OK if the parameters were accepted
 */
pstatus pal_connection_setParameters(PALConnection* const connection, void *parameters);

/**
 * A Function for setting up a connection
 *
 * @param connection a PALConnection object
 * @param buffer the buffer to write data to
 * @param bytesToRead the number of bytes to read
 * @param bytesRead the number of bytes actually read
 * 
 * @return PAL_OK if the read was successfull
 */
pstatus pal_connection_read(const PALConnection* const connection,
                            void* buffer,
                            unsigned int bytesToRead,
                            unsigned int* bytesRead);

/**
 * A Function for setting up a connection
 *
 * @param connection a PALConnection object
 * @param buffer the buffer to write
 * @param bytesToWrite the number of bytes to write
 * @param bytesWritten the number of bytes actually written
 *
 * @return PAL_OK if the write was successfull
 */
pstatus pal_connection_write(const PALConnection* const connection,
                             const void* const buffer,
                             unsigned int bytesToWrite,
                             unsigned int* bytesWritten);

/**
 * This function adds a callback that PALNetwork uses to send network status
 * updates to CV3. Observe that when destroyNetworkContext is called all
 * callbacks that is still registered will be removed.
 *
 * @param netCallbackFunc The callback function to call when a status update should be sent.
 * @param data The context to call the callback function on.
 *
 * @return PAL_OK if the callback was added correctly.
 */
pstatus pal_connection_addCallbackFunc(ConnectionCallbackFunc callbackFunc,
                                       void* data);

/**
 * This function removes a callback that PALNetwork uses to send network
 * status updates to.
 *
 * @param listener The listener to remove
 * @param data The context to call listener on.
 *
 * @return PAL_OK if the callback was removed correctly.
 */
pstatus pal_connection_removeCallbackFunc(ConnectionCallbackFunc netCallbackFunc,
                                          void* data);

#endif // end PALCONNECTION_H
