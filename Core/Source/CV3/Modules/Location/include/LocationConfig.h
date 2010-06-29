/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef LOCATIONCONFIG_H
#define LOCATIONCONFIG_H

#include "PALMachineTypes.h"


namespace wfcore {

struct ProviderUsage {
   bool m_useNetworkBasedCellID;
   /* For future
   bool m_useNetworkBasedWiFi;
   bool m_useNetworkBasedRFID;
   */
   bool m_useAssistedGPS;
   bool m_useAutonomousGPS;

   ProviderUsage()
   : m_useNetworkBasedCellID(true),
     m_useAssistedGPS(true),
     m_useAutonomousGPS(true) {}
};


class LocationConfig {

public:

   LocationConfig();

   ~LocationConfig();


   //--------------------------------------------------------------------------
   // config methods - global values that affect everything

   void setProviderUsage(ProviderUsage level);

   ProviderUsage getProviderUsage() const;


   //--------------------------------------------------------------------------
   // config methods - values that only affect network based positioning

   void setNetworkUpdateInterval(wf_int32 networkUpdateIntervalMilliSecs);

   wf_int32 getNetworkUpdateInterval() const;


   //--------------------------------------------------------------------------
   // config methods - values that only affect GPS based positioning

   void setGPSUpdateInterval(wf_int32 gpsUpdateIntervalMilliSecs);

   wf_int32 getGPSUpdateInterval() const;


private:
   ProviderUsage m_level;
   wf_int32 m_networkIntervalMilliSecs;
   wf_int32 m_gpsIntervalMilliSecs;
};


}




#endif /* LOCATIONCONFIG_H_ */
