
#include "PALConnection.h"
#include "PALSocket.h"
#include <string.h>
#define CORE_LOGGING_MODULE_NAME "PALCONNECTION"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL
#define CORE_LOGGING_MODULE_LEVEL NONE

struct ConnectionCallbackFunction {
   ConnectionCallbackFunction(ConnectionCallbackFunc cf, void* d) : 
      callbackFunc(cf), data(d) {}
   
   ConnectionCallbackFunc callbackFunc;
   void* data;
};

static void pal_connection_init(PALConnection* const connection)
{
   // Only support socket right now
   // TODO this needs to be resolved later so that the context type can be retrieved
   if( connection->context == NULL ) {
      //printf("NEW SOCKET CONTEXT\n");
      connection->context = (void*)new SocketContext();
   }
}

pstatus pal_connection_establish(PALConnection* const connection)
{
   // Testing PALSocket
   pstatus status;
   pal_connection_init( connection );
   
   // Will return a SocketContext
   status = pal_socket_create( (SocketContext*)connection->context );
   if(status == PAL_OK)
   {
      // Connect to the socket
      status = pal_socket_connect( (SocketContext*)connection->context );
   }
   return status;
}

pstatus pal_connection_terminate(const PALConnection* const connection)
{
   if(connection->context != NULL)
   {
      pal_socket_disconnect((SocketContext*)connection->context);
      // Free memory in SocketContext
      delete[] ((SocketContext*)connection->context)->serverAddress;
   }
   return PAL_OK;
}

pstatus pal_connection_setParameters(PALConnection* const connection, void *p)
{
   ServerParameters* pSp = (ServerParameters*)p;
   pal_connection_init( connection );
   switch(pSp->type)
   {
      case SERVER_PARAMETER_TYPE_CONNECTION:
      {
         // Save the host
         ((SocketContext*)connection->context)->serverAddressLength = pSp->spc.hostNameLength;
         ((SocketContext*)connection->context)->serverAddress = new char[pSp->spc.hostNameLength];
         memcpy( ((SocketContext*)connection->context)->serverAddress, pSp->spc.pHostName, pSp->spc.hostNameLength );
         // Save the port
         ((SocketContext*)connection->context)->serverPort = pSp->spc.port;
         break;
      }

      default:
         break;
   }
   return PAL_OK;
}

pstatus pal_connection_read(const PALConnection* const connection,
                            void* buffer,
                            unsigned int bytesToRead,
                            unsigned int* bytesRead)
{
   // Only support socket right now
   return pal_socket_read((SocketContext*)connection->context,
                          buffer,
                          bytesToRead,
                          bytesRead);
}

pstatus pal_connection_write(const PALConnection* const connection,
                             const void* const buffer,
                             unsigned int sizeToWrite,
                             unsigned int* sizeWritten)
{
   //printf("--------------------------------------------------CONN WRITE\n");
   // Only support socket right now
   return pal_socket_write((SocketContext*)connection->context,
                           buffer,
                           sizeToWrite,
                           sizeWritten);
}

pstatus pal_connection_addCallbackFunc(ConnectionCallbackFunc callbackFunc,
                                       void* data)
{
   /*
   networkContext->callbackFunctions.insert(
      new NetworkCallbackFunction(netCallbackFunc, data));
   */

   return PAL_OK;
}

pstatus pal_connection_removeCallbackFunc(ConnectionCallbackFunc callbackFunc,
                                           void* data)
{
   /*
   for(std::set<NetworkCallbackFunction*>::iterator it =
          networkContext->callbackFunctions.begin();
       it != networkContext->callbackFunctions.end(); ++it) {
      if((*it)->callbackFunc == netCallbackFunc) {
         delete (*it);
         networkContext->callbackFunctions.erase(it);
      }
   }
   */
   return PAL_OK;
}
