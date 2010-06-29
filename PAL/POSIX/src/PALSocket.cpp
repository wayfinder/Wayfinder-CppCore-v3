
#include "PALSocket.h"
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

#define CORE_LOGGING_MODULE_NAME "PALSOCKET"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL 

pstatus pal_socket_create(SocketContext* socketContext)
{
   if(socketContext == NULL)
   {
      return PAL_SOCKET_INPUT_PARAM;
   }
   
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

   socketContext->fd = result;
   
   return PAL_OK;
}

pstatus pal_socket_connect(SocketContext* socketContext)
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
      return PAL_CONNECT_ERROR;
   }
   
   int status = connect(socketContext->fd,
                        (struct sockaddr*)&sin, sizeof(struct sockaddr_in));
   
   if(status != 0) {
      switch (errno) {
      case ECONNREFUSED:
         return PAL_CONNECTION_REFUSED;
      case EISCONN:
         return PAL_SOCKET_ALREADY_CONNECTED;
      case ETIMEDOUT:
         return PAL_CONNECTION_ATTEMPT_TIMEDOUT;
      default:
         return PAL_CONNECT_ERROR;
      }
   }
      
   return PAL_OK;
}

pstatus pal_socket_disconnect(SocketContext* socketContext)
{
   int status = shutdown(socketContext->fd, SHUT_RDWR);

   if(status != 0) {
      switch (errno) {
      case ENOTCONN:
         return PAL_SOCKET_NOT_CONNECTED;
      default:
         return PAL_DISCONNECT_ERROR;
      }
   }

   status = close(socketContext->fd);
   if(status == -1) {
      return PAL_DISCONNECT_ERROR;
   }
   socketContext->fd = 0;
   
   return PAL_OK;
}

pstatus pal_socket_select(SocketContext* socketContext,
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

pstatus pal_socket_read(SocketContext* socketContext,
                        void* buffer,
                        unsigned int bytesToRead,
                        unsigned int* bytesRead)
{
   if( socketContext->fd == 0 )
   {
      return PAL_SOCKET_NOT_CONNECTED;
   }
   if( buffer == NULL )
   {
      return PAL_SOCKET_INPUT_PARAM;
   }
   int result = ::read(socketContext->fd,
                       buffer,
                       (size_t)bytesToRead);
   
   if(result == -1) {
      return PAL_READ_ERROR;
   }

   if(result == 0) {
      return PAL_SOCKET_END_OF_FILE;
   }

   *bytesRead = result;
   
   return PAL_OK;
}

pstatus pal_socket_write(SocketContext* socketContext,
                         const void* const buffer,
                         unsigned int sizeToWrite,
                         unsigned int* sizeWritten)
{
   if( socketContext->fd == 0 )
   {
      return PAL_SOCKET_NOT_CONNECTED;
   }
   if( buffer == NULL )
   {
      return PAL_SOCKET_INPUT_PARAM;
   }
   int result  = ::write(socketContext->fd,
                         buffer,
                         (size_t)sizeToWrite);

   if(result == -1) {
      return PAL_WRITE_ERROR;
   }
   *sizeWritten = result;

   return PAL_OK;
}

