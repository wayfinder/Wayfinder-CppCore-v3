/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "ServerConnectionTest.h"

#define CORE_LOGGING_MODULE_NAME "SERVER_CONNECTION_TEST"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL
#define CORE_LOGGING_MODULE_LEVEL NONE
#include "CoreDebugPrint.h"

#include "NavRevGeocodingReq.h"
#include "NavRevGeocodingReply.h"
#include "PALMachineTypes.h"
#include "ReceiverModuleId.h"
#include "ServerHandler.h"
#include "WGS84Coordinate.h"

#include <sstream>

using wf_ngp::PacketTypeEnum;
using wf_ngp::NavReplyPacket;
using wf_ngp::NavRevGeocodingReq;
using wf_ngp::NavRevGeocodingReply;

ServerConnectionTest::ServerConnectionTest(const char* name)
   : RegressionTest(name),
     m_receiverId(REGTEST_SERVER_CONNECTION_TEST)
{
   m_messageQueue = new messageQueue_t();
}

ServerConnectionTest::~ServerConnectionTest()
{
   delete m_messageQueue;
}

void ServerConnectionTest::startTest()
{
   ServerHandler sh;
   sh.start();
   sh.getListenerRegistry()->registerNGPMessageReceiver(this, m_receiverId);

   WFAPI::WGS84Coordinate office(55.613131, 12.992237);
   NavRevGeocodingReq addressReq(0, office);

   // try sending
   sh.sendNGPMessage(&addressReq, m_receiverId);

   NavReplyPacket* packetFromQ = NULL;
   m_messageQueue->waitAndPop(packetFromQ);


   if (packetFromQ->getType() != PacketTypeEnum::NAV_REV_GEOCODING_REPLY) {
      std::ostringstream os;
      os << std::hex << std::showbase
         << "Packet type " << packetFromQ->getType()
         << " not expected.";
      setTestFailed(os.str());
      return;
   }

   const NavRevGeocodingReply* gpsAddressRep =
      static_cast<const NavRevGeocodingReply*>(packetFromQ);
   
   // Avoid tests that assume too much about map data, such as the
   // exact distance between two POI:s. But this should be pretty
   // safe except when the server fails to load maps.
   // This is also language sensitive!
   std::string cityExpected("Malmo");
   std::string cityActual(gpsAddressRep->getCityName()); // make copy
   if (cityExpected != cityActual) { // overloaded operator
      std::ostringstream os;
      os << "Expected city \"" << cityExpected
         << "\" != \"" << cityActual << "\".";
      setTestFailed(os.str());
      return;
   }

   // dump the data for logging
   coreprintln("City name: %s",
               gpsAddressRep->getCityName().c_str());
   coreprintln("District name: %s",
               gpsAddressRep->getDistrictName().c_str());
   coreprintln("Top region name: %s",
               gpsAddressRep->getTopRegionName().c_str());
   coreprintln("Municipal name: %s",
               gpsAddressRep->getMunicipalName().c_str());
   coreprintln("Address name: %s",
               gpsAddressRep->getAddressName().c_str());

   setTestPassed();

   delete packetFromQ;
   return;
}

void ServerConnectionTest::receivedNGPPacket(wf_ngp::NavReplyPacket* packet)
{
   m_messageQueue->push(packet);
}

