#include "CoreFactory.h"
#include "Core.h"
#include "ReverseGeocodeTest.h"
#include "WGS84Coordinate.h"

#include <cstdio>

using namespace wfcoretestclient;

int main(int argc, char** argv) {

   printf("Creating CoreConfig\n");
   corefactory::CoreConfig config(
         "oss-nav.services.wayfinder.com",
         80,
         "wf-linux-demo");

   printf("Creating Core\n");
   Core* core = corefactory::createCore(config);
   printf("Core has been created. :D\n");

   ReverseGeocodeTest test(core);
   test.runTest();

   printf("CoreTestClient DONE!\n");
}
