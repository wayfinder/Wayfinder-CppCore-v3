/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
*/

#ifndef PALLOCATION_H_
#define PALLOCATION_H_

#ifdef __cplusplus
extern "C" {
#endif
   
#include "PAL.h"
#include "PALMachineTypes.h"

/*
 * Used to report the position
 */
typedef struct PALPosition PALPosition;
struct PALPosition {
   const wf_float64 m_wgs84Declat;
   const wf_float64 m_wgs84Declon;
   const wf_int32 m_altitudeMeters;
   const wf_int16 m_speedMPS;
   const wf_int16 m_courseDegrees;
   const wf_int16 m_accuracyMeters;
   const wf_int64 m_timestamp;
};

/*
 * Function pointer, called whenever a new position is available
 */
typedef void(*pal_loc_pos_updated_func)(PALPosition, void*);

enum ProviderState {
   UNAVAILABLE = 0,
   AVAILABLE
};

/*
 * Function pointer, called whenever the state of the provider has changed
 */
typedef void(*pal_loc_provider_state_changed_func)(ProviderState, void*);


enum ProviderType {
   NETWORKBASED,
   GPS_ASSISTED,
   GPS_AUTONOMOUS,
};

//all available location providers
typedef struct LocationProviders LocationProviders;
//forward declare
struct LocationProviders;

pstatus
pal_loc_create_providers(LocationProviders** locationProviders, int* size);

pstatus
pal_loc_print_provider(LocationProviders* provider, int providerIndex);

pstatus
pal_loc_start_provider(LocationProviders* provider, int providerIndex,
                       pal_loc_pos_updated_func updateFunc,
                       pal_loc_provider_state_changed_func stateFunc);

pstatus
pal_loc_get_provider_type(LocationProviders* provider, int providerIndex,
                          ProviderType* type);

pstatus
pal_loc_stop_provider(LocationProviders* provider, int providerIndex);

pstatus
pal_loc_destroy_providers(LocationProviders* providers);

#ifdef __cplusplus
} // End of extern "C"
#endif
      
#endif /* PALLOCATION_H_ */
