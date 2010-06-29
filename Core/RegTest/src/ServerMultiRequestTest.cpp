/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "ServerMultiRequestTest.h"

#define CORE_LOGGING_MODULE_NAME "SERVER_MULTI_REQUEST_TEST"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL
#include "CoreDebugPrint.h"

#include "NavRevGeocodingReq.h"
#include "NavRevGeocodingReply.h"
#include "PALMachineTypes.h"
#include "ReceiverModuleId.h"
#include "ServerHandler.h"
#include "WGS84Coordinate.h"

#include <sstream>

using WFAPI::WGS84Coordinate;
using wf_ngp::PacketTypeEnum;
using wf_ngp::NavReplyPacket;
using wf_ngp::NavRevGeocodingReq;
using wf_ngp::NavRevGeocodingReply;

ServerMultiRequestTest::ServerMultiRequestTest(const char* name)
   : RegressionTest(name),
     m_receiverId(REGTEST_SERVER_CONNECTION_TEST)
{
   m_serverHandler.start();
}

ServerMultiRequestTest::~ServerMultiRequestTest()
{
}

// testing of reverse geocoding as such is done by ServerConnectionTest
// so this test focuses on the multi request things.
void ServerMultiRequestTest::startTest()
{
   const WGS84Coordinate malmoCoord(55.61347, 12.99158);
   const WGS84Coordinate paddingtonCoord(51.51934, -0.18157);

   const NavRevGeocodingReply* malmo =
      getAndCheckReply(malmoCoord, "Malmo");
   if (malmo == NULL) {
      // Office move was reverted ...
      return;
   }
   
   const NavRevGeocodingReply* paddington =
      getAndCheckReply(paddingtonCoord, "London");
   if (paddington == NULL) {
      return;
   }

   if (malmo == paddington) {
      setTestFailed("The requests did not yield different requets");
      return;
   }

   setTestPassed();

   delete paddington;
   delete malmo;
   return;
}

const NavRevGeocodingReply*
ServerMultiRequestTest::getAndCheckReply(
   const WFAPI::WGS84Coordinate& coord,
   const std::string& expectedCity) {

   static wf_uint32 lastRequestID = 0;
   
   m_serverHandler.getListenerRegistry()->registerNGPMessageReceiver(
      this, m_receiverId);
   NavRevGeocodingReq addressReq(0, coord);
   
   // try sending
   wf_uint32 requestID = 
      m_serverHandler.sendNGPMessage(&addressReq, m_receiverId);
   if (requestID == lastRequestID) {
      setTestFailed("Did not get a different request ID.");
      return NULL;
   }
   lastRequestID = requestID;
   
   NavReplyPacket* packetFromQ = NULL;
   m_messageQueue.waitAndPop(packetFromQ);

   if (packetFromQ->getType() != PacketTypeEnum::NAV_REV_GEOCODING_REPLY) {
      std::ostringstream os;
      os << std::hex << std::showbase
         << "Packet type " << packetFromQ->getType()
         << " not expected.";
      setTestFailed(os.str());
      return NULL;
   }

   const NavRevGeocodingReply* gpsAddressRep =
      static_cast<const NavRevGeocodingReply*>(packetFromQ);
   
   // Avoid tests that assume too much about map data, such as the
   // exact distance between two POI:s. But this should be pretty
   // safe except when the server fails to load maps.
   // This is also language sensitive!
   const std::string& cityActual =
      gpsAddressRep->getCityName();
   if (expectedCity != cityActual) { // overloaded operator
      std::ostringstream os;
      os << "Expected city \"" << expectedCity
         << "\" != \"" << cityActual << "\".";
      setTestFailed(os.str());
      return NULL;
   }

   return gpsAddressRep;
}

void ServerMultiRequestTest::receivedNGPPacket(wf_ngp::NavReplyPacket* packet)
{
   m_messageQueue.push(packet);
}
