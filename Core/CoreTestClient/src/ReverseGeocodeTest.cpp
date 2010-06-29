#include "ReverseGeocodeTest.h"

#define CORE_LOGGING_MODULE_NAME "REVERSE_GEOCODE_TEST"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL
#include "CoreDebugPrint.h"

#include "Core.h"
#include "MessageDispatcher.h"
#include "MessageRouter.h"
#include "NavRevGeocodingReply.h"
#include "NavRevGeocodingReq.h"
#include "WGS84Coordinate.h"

#include <iostream>

using std::cerr;
using std::cout;
using std::endl;

namespace wfcoretestclient {
ReverseGeocodeTest::ReverseGeocodeTest(Core* core)
   : m_msgDispatcher(core->getMessageDispatcher()),
     m_msgRouter(core->getMessageRouter())
{
}

ReverseGeocodeTest::~ReverseGeocodeTest()
{
}

void ReverseGeocodeTest::runTest()
{
   using WFAPI::WGS84Coordinate;

   m_msgRouter.registerListener(this);

   // 1m ~ 0.00001 deg
   const WGS84Coordinate malmoOffice(55.61347, 12.99158);
   const WGS84Coordinate paddingtonOffice(51.51934, -0.18157);

   cout << "Asking server for address of Malmo office @"
        << malmoOffice << endl;
   // TODO:: who will delete this request???
   wf_ngp::NavRevGeocodingReq* addressReq1
      = new wf_ngp::NavRevGeocodingReq(0, malmoOffice);

   m_msgDispatcher.sendNGPMessage(addressReq1);
   coreprintln("Sent a message");

   cout << "Asking server for address of Paddington office @"
        << paddingtonOffice << endl;
   
   // TODO:: who will delete this request???
   wf_ngp::NavRevGeocodingReq* addressReq2
      = new wf_ngp::NavRevGeocodingReq(0, paddingtonOffice);
   m_msgDispatcher.sendNGPMessage(addressReq2);
   coreprintln("Sent a message");

   // TODO: when multi-threaded Core, wait for the replies.
   // now the call-backs will come before createReverseGeocodeRequest
   // returns
}

void ReverseGeocodeTest::processSystemMessage(
   const wf_ngp::NavReplyPacket& message)
{
   using wf_ngp::NavRevGeocodingReply;

   coreprintln("processSystemMessage(%p) called",
               &message);

   // this is a client to show concepts that is intended to
   // run on a computer and thus it is reasonable to do
   // stream output rather than debug prints.
   if (message.getType() ==
       wf_ngp::PacketTypeEnum::NAV_REV_GEOCODING_REPLY) {

      const NavRevGeocodingReply* gpsAddressRep =
         static_cast<const NavRevGeocodingReply*>(&message);
         
      cout << "The address is:" << endl
           << "\tStreet address: "
           << gpsAddressRep->getAddressName() << endl
           << "\tDistrict:       "
           << gpsAddressRep->getDistrictName() << endl
           << "\tCity:           "
           << gpsAddressRep->getCityName() << endl
           << "\tMunicipal:      "
           << gpsAddressRep->getMunicipalName() << endl
           << "\tTop region:     "
           << gpsAddressRep->getTopRegionName() << endl
           << endl;
      
   } else {
      cerr << "ERROR: got an unexpected packet which I can't handle. "
           << "Type: "
           << std::hex << std::showbase << message.getType()
           << endl;
   }

}
} // namespace
