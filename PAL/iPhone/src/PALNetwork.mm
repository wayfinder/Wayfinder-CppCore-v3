/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
*/

#include "PALNetwork.h"
#include <string.h>
#include <stdio.h>
#include <set>

// Includes for creating a socket.
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

/// Included for making it possible to set the socket as non-blocking. 
#include <fcntl.h>

#define CORE_LOGGING_MODULE_NAME "PALNETWORK"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL 

struct NetworkCallbackFunction {
   NetworkCallbackFunction(NetworkCallbackFunc cf, void* d) : 
      callbackFunc(cf), data(d) {}
   
   NetworkCallbackFunc callbackFunc;
   void* data;
};

struct NetworkContext {
   NetworkContext() :
      networkState(UNINITIALIZED)
      {}
   
   NetConnectionState networkState;
   std::set<NetworkCallbackFunction*> callbackFunctions;
};

struct SocketContext {
   SocketContext() :
      fd(0), serverAddress(NULL),
      serverPort(0), socketMode(BLOCKING_SOCKET) {}

   int fd;
   SocketMode socketMode;
   char* serverAddress;
   unsigned int serverPort;
};



pstatus createNetworkContext(NetworkContext** networkContext)
{
   *networkContext = new NetworkContext();
   
   return PAL_OK;
}

pstatus setNetworkContextParameters(NetworkContext* networkContext,
                                    const char* connectionParameters)
{
   return PAL_OK;
}

pstatus initNetworkContext(NetworkContext* networkContext)
{
   networkContext->networkState = INITIALIZED;

   for(std::set<NetworkCallbackFunction*>::iterator it =
          networkContext->callbackFunctions.begin();
       it != networkContext->callbackFunctions.end(); ++it) {
      (*it)->callbackFunc(networkContext,
                          networkContext->networkState,
                          NULL,
                          0,
                          (*it)->data);
   }
   
   return PAL_OK;
}

pstatus destroyNetworkContext(NetworkContext** networkContext)
{
   (*networkContext)->networkState = UNINITIALIZED;

   for(std::set<NetworkCallbackFunction*>::iterator it =
          (*networkContext)->callbackFunctions.begin();
       it != (*networkContext)->callbackFunctions.end(); ++it) {
      (*it)->callbackFunc((*networkContext),
                          (*networkContext)->networkState,
                          NULL,
                          0,
                          (*it)->data);
   }

   //Remove all registered callback functions to be on the safe side.
   for(std::set<NetworkCallbackFunction*>::iterator it =
          (*networkContext)->callbackFunctions.begin();
       it != (*networkContext)->callbackFunctions.end(); ++it) {
      delete (*it);
      (*networkContext)->callbackFunctions.erase(it);
   }
   
   delete (*networkContext);
   (*networkContext) = NULL;

   return PAL_OK;
}

pstatus addNetworkCallbackFunc(NetworkContext* networkContext,
                               NetworkCallbackFunc netCallbackFunc,
                               void* data)
{
   networkContext->callbackFunctions.insert(
      new NetworkCallbackFunction(netCallbackFunc, data));

   return PAL_OK;
}

pstatus removeNetworkCallbackFunc(NetworkContext* networkContext,
                                  NetworkCallbackFunc netCallbackFunc,
                                  void* data)
{
   for(std::set<NetworkCallbackFunction*>::iterator it =
          networkContext->callbackFunctions.begin();
       it != networkContext->callbackFunctions.end(); ++it) {
      if((*it)->callbackFunc == netCallbackFunc) {
         delete (*it);
         networkContext->callbackFunctions.erase(it);
      }
   }
}

pstatus createSocket(SocketContext** socketContext)
{
   
   *socketContext = new SocketContext();

   int result = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
   if(result == -1) {
      switch (errno) {
      case EACCES:
         return PAL_SOCKET_PERMISSION_DENIED;
      case EINVAL:
      case EPROTONOSUPPORT:
         return PAL_PROTOCOL_NOT_SUPPORTED;
      case ENOBUFS:
      case ENOMEM:
         return PAL_OUT_OF_MEMORY;
      default:
         return PAL_SOCKET_ERROR;
         
      }
   }

   (*socketContext)->fd = result;
   return PAL_OK;
}

pstatus setSocketContextOptions(SocketContext* socketContext,
                                SocketContextOptions socketOptions)
{
   switch(socketOptions.type) {
   case SERVER_PARAMETERS:
      socketContext->serverAddress =
         new char[strlen(socketOptions.option.serverParameters.serverAddress) + 1];
      strcpy(socketContext->serverAddress,
             socketOptions.option.serverParameters.serverAddress);
      
      socketContext->serverPort = socketOptions.option.serverParameters.serverPort;
      break;
   case SOCKET_PARAMETERS:
      if(socketOptions.option.socketParameters.socketMode == NON_BLOCKING_SOCKET) {
         int res = fcntl(socketContext->fd, F_SETFL, O_NONBLOCK);
         if(res == 0) {
            socketContext->socketMode = NON_BLOCKING_SOCKET;
         } else {
            return PAL_FAILED_TO_SET_OPTION;
         }
      }
      break;
   default:
      break;
   }
   
   return PAL_OK;
}

pstatus connect(SocketContext* socketContext)
{
   if(socketContext->serverAddress == NULL) {
      return PAL_NO_SOCKET_CONTEXT_PARAMS;
   }

   struct sockaddr_in sin;

   memset(&sin, 0, sizeof(sin));
   sin.sin_family = AF_INET;
   sin.sin_port = htons((u_short)socketContext->serverPort);
   struct hostent *hostEntry;
   hostEntry = gethostbyname(socketContext->serverAddress);
   if (hostEntry != 0) {
      memcpy(&(&sin)->sin_addr, hostEntry->h_addr, hostEntry->h_length);
   } else {
      
      printf("CONNECT ERROR: %d\n", errno);
      sin.sin_addr.s_addr = inet_addr(socketContext->serverAddress);
//      return PAL_CONNECT_ERROR;
   }
   
   int status = connect(socketContext->fd,
                        (struct sockaddr*)&sin, sizeof(struct sockaddr_in));
   
   if(status != 0) {
      printf("CONNECT ERROR: %d\n", errno);
      switch (errno) {
      case EINPROGRESS:
         return PAL_CONNECTING;
      case EALREADY:
         return PAL_CONNECTION_ALREADY_ONGOING;
      case ECONNREFUSED:
         return PAL_CONNECTION_REFUSED;
      case EISCONN:
         return PAL_SOCKET_ALREADY_CONNECTED;
      case ETIMEDOUT:
         return PAL_CONNECTION_ATTEMPT_TIMEDOUT;
      case EADDRNOTAVAIL:
         printf("oops!\n");
      default:
         return PAL_CONNECT_ERROR;
      }
   }
      
   return PAL_OK;
}

pstatus disconnectAndDestroy(SocketContext** socketContext)
{
   int status = shutdown((*socketContext)->fd, SHUT_RDWR);

   if(status != 0) {
      switch (errno) {
      case ENOTCONN:
         return PAL_SOCKET_NOT_CONNECTED;
      default:
         return PAL_DISCONNECT_ERROR;
      }
   }

   status = close((*socketContext)->fd);
   if(status == -1) {
      return PAL_DISCONNECT_ERROR;
   }
   (*socketContext)->fd = 0;
   
   delete (*socketContext);
   (*socketContext) = NULL;
   
   return PAL_OK;
}

pstatus select(SocketContext* socketContext,
               int* timeout, 
               bool* readReady,
               bool* writeReady)
{
   
   // Setup the file descriptors.
   fd_set fdescs;
   FD_ZERO(&fdescs);
   FD_SET(socketContext->fd, &fdescs);

   struct timeval tv;
   if(timeout != NULL) {
      tv.tv_sec = *timeout;
      tv.tv_usec = 0;
   }

   fd_set* read, *write;

   if(readReady != NULL && *readReady) {
      read = &fdescs;
      write = (fd_set *)0;
   } else if(writeReady != NULL && *writeReady){
      write = &fdescs;
      read = (fd_set *)0;
   }
   
   int status = select(getdtablesize(),
                       read,
                       write,
                       (fd_set *)0, &tv);
   if(status == -1) {
      return PAL_SELECT_ERROR;
   }
   if(readReady != NULL && *readReady){
      status = FD_ISSET(socketContext->fd, read);
      if(status) {
         // This means the socket is ready to be read.
         *readReady = true;
      } else {
         // This means there is nothing to read on the socket.
         *readReady = false;
      }
   } else if(writeReady != NULL && *writeReady){
      status = FD_ISSET(socketContext->fd, write);
      if(status) {
         // This means the socket is ready to be written to.
         *writeReady = true;
      } else {
         // This means it is not possible to write to the socket.
         *writeReady = false;
      }      
   }
   
   return PAL_OK;
}

pstatus read(SocketContext* socketContext,
             void* buffer,
             unsigned int bytesToRead,
             unsigned int* bytesRead)
{
   int result = ::read(socketContext->fd,
                       buffer,
                       (size_t)bytesToRead);
   
   if(result == -1) {
      return PAL_READ_ERROR;
   }
   *bytesRead = result;
   
   return PAL_OK;
}

pstatus write(SocketContext* socketContext,
              const void* buffer,
              unsigned int sizeToWrite,
              unsigned int* sizeWritten)
{
   int result  = ::write(socketContext->fd,
                         buffer,
                         (size_t)sizeToWrite);
   if(result == -1) {
      return PAL_WRITE_ERROR;
   }
   *sizeWritten = result;

   return PAL_OK;
}
