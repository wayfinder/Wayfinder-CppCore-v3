
#include "PALBearer.h"

#define CORE_LOGGING_MODULE_NAME "PALBEARER"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL 

// TODO Not C-style
struct BearerCallbackFunction {
   BearerCallbackFunction(BearerCallbackFunc cf, void* d)
     : callbackFunc(cf), 
       data(d) 
   {}
   
   BearerCallbackFunc callbackFunc;
   void* data;
};

pstatus pal_bearer_connect(PALNetworkBearerType type)
{
   return PAL_OK;
}

pstatus pal_bearer_disconnect(PALNetworkBearerType type)
{
   return PAL_OK;
}

pstatus pal_bearer_setParameters(void *p)
{
   return PAL_OK;
}

pstatus pal_bearer_addCallbackFunc(BearerCallbackFunc callbackFunc,
                                   void* data)
{
   /*
   networkContext->callbackFunctions.insert(
      new NetworkCallbackFunction(netCallbackFunc, data));
   */

   return PAL_OK;
}

pstatus pal_bearer_removeCallbackFunc(BearerCallbackFunc callbackFunc,
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
