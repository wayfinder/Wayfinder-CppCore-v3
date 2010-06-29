#ifndef REVERSE_GEOCODE_TEST_H
#define REVERSE_GEOCODE_TEST_H

#include "MessageListener.h"

// forward declarations
class Core;
namespace wfcore {
   class MessageDispatcher;
   class MessageRouter;
}


namespace wfcoretestclient {

/**
 * Sends a request for reverse geocoding and prints the result.
 */
class ReverseGeocodeTest : public wfcore::MessageListener {
public:

   /**
    * core must be valid throughout this class' life time.
    * core must not be null.
    */
   ReverseGeocodeTest(Core* core);

   virtual ~ReverseGeocodeTest();

   void runTest();
   
   // see wfcore::MessageListener
   void processSystemMessage(const wf_ngp::NavReplyPacket& message);


private:

   // we send messages to Core through the dispatcher
   wfcore::MessageDispatcher& m_msgDispatcher;
   
   // we received messages from Core through the router.
   wfcore::MessageRouter& m_msgRouter; 

};
} // namespace

#endif // REVERSE_GEOCODE_TEST_H
