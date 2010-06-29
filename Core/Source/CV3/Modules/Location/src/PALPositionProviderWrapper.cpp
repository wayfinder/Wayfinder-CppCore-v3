/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "PALPositionProviderWrapper.h"
#include "PALLocation.h"
#include "ProviderProxy.h"
#include "PALMachineTypes.h"
#include "PositionProvider.h"

namespace wfcore {

PALPositionProviderWrapper::PALPositionProviderWrapper(
                             const ProviderProxy& proxy,
                             const LocationProviders** provider,
                             const wf_int providerIndex) :
                             //FIXME Get real provider type from PAL
                             PositionProvider(proxy, GPS_AUTONOMOUS),
                             m_provider(provider),
                             m_providerIndex(providerIndex)
{

}


PALPositionProviderWrapper::~PALPositionProviderWrapper()
{
}


void
PALPositionProviderWrapper::startPositioning(LocationConfig& config)
{
}


void
PALPositionProviderWrapper::stopPositioning()
{
}


void
PALPositionProviderWrapper::positionUpdated(PALPosition* palPosition)
{


}


void
PALPositionProviderWrapper::providerStateChanged(ProviderState state)
{

}


//--------------------------------------------------------------------------
// static methods called by the PAL implementation


void
PALPositionProviderWrapper::CpositionUpdated(PALPosition* palPosition,
                                             void* receiver)
{
   PALPositionProviderWrapper* provider
                         = static_cast<PALPositionProviderWrapper*>(receiver);
   provider->positionUpdated(palPosition);
}


void
PALPositionProviderWrapper::CproviderStateChanged(ProviderState state,
                                                  void* receiver)
{
   PALPositionProviderWrapper* provider
                         = static_cast<PALPositionProviderWrapper*>(receiver);
   provider->providerStateChanged(state);
}



}
