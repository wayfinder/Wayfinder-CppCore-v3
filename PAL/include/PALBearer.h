/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
*/

#ifndef PALBEARER_H
#define PALBEARER_H

#include "PAL.h"
#include "PALTypes.h"
#include <stdio.h>

typedef enum
{
  PAL_NETWORK_BEARER_NONE,
  PAL_NETWORK_BEARER_PDP,
  PAL_NETWORK_BEARER_WIFI,
  PAL_NETWORK_BEARER_ANY
} PALNetworkBearerType;

/**
 *
 * @param extendedErrorData Data specific to the networkContextState.
 * @param platformStatus This can be used to send up a platform specific error
 *                       code to the UI.
 * @param data Contains scope information.
 */
typedef void (*BearerCallbackFunc)(void* extendedErrorData,
                                   unsigned int platformStatus,
                                   void* data);

/**
 *
 * @return PAL_OK if the establish call succeeded
 * @return PAL_NO_CONTEXT_PARAMS if the connection is not setup
 */
pstatus pal_bearer_connect(PALNetworkBearerType type);

/**
 *
 * @return PAL_OK if the establish call succeeded
 * @return PAL_NO_CONTEXT_PARAMS if the connection is not setup
 */
pstatus pal_bearer_disconnect(PALNetworkBearerType type);

/**
 *
 * @return PAL_OK if the establish call succeeded
 * @return PAL_NO_CONTEXT_PARAMS if the connection is not setup
 */
pstatus pal_bearer_setParameters(void *p);

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
pstatus pal_bearer_addCallbackFunc(BearerCallbackFunc callbackFunc,
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
pstatus pal_bearer_removeCallbackFunc(BearerCallbackFunc callbackFunc,
                                      void* data);

#endif // end PALBEARER_H
