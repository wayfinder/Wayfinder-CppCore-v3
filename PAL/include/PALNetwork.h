/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
*/

#ifndef PALNETWORK_H
#define PALNETWORK_H

#include "PAL.h"
#include "PALTypes.h"
#include <stdio.h>

/**
 * Typedef for the PALNetwork callback function.
 *
 * @param extendedErrorData Data specific to the networkContextState.
 * @param platformStatus This can be used to send up a platform specific error
 *                       code to the UI.
 * @param data Contains scope information.
 */
typedef void (*NetworkCallbackFunc)(void* extendedErrorData,
                                    unsigned int platformStatus,
                                    void* data);

/**
 * A Function for establishing a connection
 * The data on where to connect to is set via the set() function call
 *
 * @return PAL_OK if the establish call succeeded
 * @return PAL_NO_CONTEXT_PARAMS if the connection is not setup
 */
pstatus establishConnection(void);

/**
 * A Function for terminating a connection
 *
 * @return PAL_OK if the terminate call succeeded
 * @return PAL_CONNECTION_ERROR if the terminate call failed
 * @return PAL_NOT_CONNECTED if there is no connection to terminate
 */
pstatus terminateConnection(void);

/**
 * A Function for setting up a connection
 *
 * @return PAL_OK if the parameters were accepted
 */
pstatus setConnectionParameters(void *p);

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
pstatus addNetworkCallbackFunc(NetworkCallbackFunc netCallbackFunc,
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
pstatus removeNetworkCallbackFunc(NetworkCallbackFunc netCallbackFunc,
                                  void* data);


#endif // end PALNETWORK_H
