/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
*/

#define CORE_LOGGING_MODULE_NAME "PALLOCATION"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

//#include "CoreDebugPrint.h"

#include "PAL.h"
#include "PALLocation.h"
#include <vector>
#include <string.h>
#include <malloc.h>

#define LOCATIONPROVIDERS 2
#define GPSDESCSTRING "Fake GPS"
#define CELLIDDESCSTRING "Fake GoogleCellID"

struct LocationProvider {
   int id;
   char* providerDesc;
};

struct LocationProviders {
   wf_uint8 nbrOfProviders;
   struct LocationProvider provider[LOCATIONPROVIDERS];
};

pstatus
pal_loc_create_providers(struct LocationProviders** locationProviders,
                         int* size) {

   *locationProviders = (struct LocationProviders*)malloc(sizeof(LocationProviders));

   //STATICALLY define this target systems location providers:
   (*locationProviders)->provider[0].providerDesc =
      (char*)malloc(strlen(GPSDESCSTRING) + 1*sizeof(char));
   strcpy((*locationProviders)->provider[0].providerDesc, GPSDESCSTRING);

   (*locationProviders)->provider[1].providerDesc =
      (char*)malloc(strlen(CELLIDDESCSTRING) + 1*sizeof(char));
   strcpy((*locationProviders)->provider[1].providerDesc, CELLIDDESCSTRING);
   
   (*locationProviders)->nbrOfProviders = LOCATIONPROVIDERS;

   *size = LOCATIONPROVIDERS;
   return PAL_OK;
}

pstatus
pal_loc_print_provider_desc(LocationProviders* providers, int providerIndex) {
   printf("%s\n", providers->provider[providerIndex].providerDesc);
   return PAL_OK;
}

pstatus
pal_loc_start_provider(struct LocationProviders* provider,
                       int providerIndex,
                       pal_loc_pos_updated_func updateFunc,
                       pal_loc_provider_state_changed_func stateFunc) {
   printf("provider started");
   return PAL_OK;
}

pstatus
pal_loc_stop_provider(LocationProviders* provider, int providerIndex) {
   printf("provider stopped");
   return PAL_OK;
}

pstatus
pal_loc_get_provider_type(LocationProviders* provider, int providerIndex,
                        ProviderType* type) {
   if(providerIndex == 0) {
      *type = GPS_AUTONOMOUS;
   } else {
      *type = NETWORKBASED;
   }
   return PAL_OK;
}

pstatus
pal_loc_destroy_providers(struct LocationProviders* providers) {
   int j = providers->nbrOfProviders;
   for(int i = 0; i < j; ++i) {
      free(providers->provider[i].providerDesc);
   }
   free(providers);
   return PAL_OK;
}

//example usage:
// struct LocationProviders* locationProviders;
// int size;
// pal_loc_create_providers(&locationProviders, &size);
// for(int i=0; i < size ; ++i) {
//    pal_loc_print_provider_desc(locationProviders, i);      
// }
// pal_loc_destroy_providers(locationProviders);
