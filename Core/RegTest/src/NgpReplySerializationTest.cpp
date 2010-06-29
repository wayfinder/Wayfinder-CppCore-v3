/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "NgpReplySerializationTest.h"

#include "NavRevGeocodingReply.h"

#include "Buffer.h"
#include "NavReplyPacket.h"
#include "PacketFactory.h"
#include <sstream>
#include <vector>


using std::hex;
using std::ostringstream;
using std::showbase;
using std::string;

using wf_ngp::uint32;
using wf_ngp::byte;

using wf_ngp::PacketTypeEnum;
using wf_ngp::NavReplyPacket;
using wf_ngp::NavRevGeocodingReply;

namespace {
const std::string argCityName("SomeCity");
const std::string argDistrictName("SomeDistrict");
const std::string argTopRegionName("Some Topregion");
const std::string argMunicipalName("Some Municipal");
const std::string argAddressName("Some Address");
const uint32 argTopRegionID(12345);
const uint32 argReqID(42);

typedef std::vector<byte> ByteVector;
}


NgpReplySerializationTest::NgpReplySerializationTest(const char* name)
   : RegressionTest(name),
     m_parsedReply(NULL)
{
}

NgpReplySerializationTest::~NgpReplySerializationTest()
{
   delete m_parsedReply;
}

void NgpReplySerializationTest::startTest()
{
   NavRevGeocodingReply reply0 =
      NavRevGeocodingReply(argCityName,
                           argDistrictName,
                           argTopRegionName,
                           argMunicipalName,
                           argAddressName,
                           argTopRegionID);
   
   reply0.setReqID(argReqID);
   reply0.serialize(reply0.getParamBlock());
   ByteVector buf;
   reply0.writeTo(buf);
   const uint32 TOTAL_LENGTH = buf.size();

   const ByteVector& bufC = buf;
   const byte* bufCp = &bufC[0]; // most methods in ngplib expect byte*

   //--------------------------------------------------------------------------
   // test the STX and packet length reading

   const uint32 EXPECTED_REMAINING_LENGTH = TOTAL_LENGTH - wf_ngp::HEADER_LENGTH;

   uint32 remainingBytes;
   wf_ngp::STXStatus status = wf_ngp::checkSTXAndSetPacketLength(
                                              bufCp, TOTAL_LENGTH,
                                              remainingBytes);

   switch(status)
   {
   case wf_ngp::STX_VALID:
      // valid
      break;

   case wf_ngp::STX_INVALID_SHORT_HEADER:
      setTestFailed("NGP Header size was too short, must be at least 5 bytes");
      return;

   case wf_ngp::STX_INVALID_BAD_VALUE:
      setTestFailed("NGP magic STX byte was not 0x02");
      return;

   default:
      setTestFailed("Unknown STX status code returned");
      return;
   }

   if (remainingBytes != EXPECTED_REMAINING_LENGTH) {
      ostringstream os;
      os << "Expected remaining length: " << EXPECTED_REMAINING_LENGTH
         << " actual remaining length: " << remainingBytes;
      setTestFailed(os.str());
      return;
   }

   //--------------------------------------------------------------------------
   // test the reading and handling of the remaining packet

   // symbolizes that the user has read the remaining bytes, discarding
   // the header length
   ByteVector remainBuf(&bufC[wf_ngp::HEADER_LENGTH], &bufC[TOTAL_LENGTH]);
   NavReplyPacket* reply = wf_ngp::createReplyPacketFromBuffer(&remainBuf[0],
                                                               remainingBytes);

   if(reply == 0) {
      setTestFailed("createReplyPacketFromBuffer() returned 0");
      return;
   }

   if (reply->getType() != PacketTypeEnum::NAV_REV_GEOCODING_REPLY) {
      ostringstream os;
      os << "Expected packet type: "
         << hex << showbase
         << PacketTypeEnum::NAV_REV_GEOCODING_REPLY
         << " actual packet type: " << reply->getType();
      setTestFailed(os.str());
      return;
   }

   m_parsedReply = dynamic_cast<NavRevGeocodingReply*>(reply);
   if(m_parsedReply == 0) {
      setTestFailed("dynamic cast of reply returned 0");
   }


   if (failIfDifferent("reqID", argReqID, m_parsedReply->getReqID())
       || failIfDifferent("cityName", argCityName,
                          m_parsedReply->getCityName())
       || failIfDifferent("districtName", argDistrictName,
                          m_parsedReply->getDistrictName())
       || failIfDifferent("topRegionName", argTopRegionName,
                          m_parsedReply->getTopRegionName())
       || failIfDifferent("MunicipalName", argMunicipalName,
                          m_parsedReply->getMunicipalName())
       || failIfDifferent("AddressName", argAddressName,
                          m_parsedReply->getAddressName())
       || failIfDifferent("TopRegionID", argTopRegionID,
                          m_parsedReply->getTopRegionID())

      ) {
      return;
   }

   setTestPassed();
   return;
}
