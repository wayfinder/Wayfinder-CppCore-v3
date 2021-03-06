/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "RegTestEnv"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

//#include "CoreDebugPrint.h"
#include "RegressionTestEnvironment.h"
#include <cassert>


#if defined __linux__
# include <gtk/gtk.h>

// # include <JTC/JTC.h>
// # include <JTC/JTCInit.h>
 #elif defined (IPHONE)
# include <IPhoneInitUtil.h>
// # include <JTC/JTC.h>
// # include <JTC/JTCInit.h>
// #elif defined (ANDROID)
// # include <stdio.h>
// # include <JTC/JTC.h>
// # include <JTC/JTCInit.h>
#endif

# include "PALThreading.h"
# include "PALEnvironment.h"

#include <string.h>

using namespace RegressionTestUtil;

static bool s_isMarathonTest = false;
static bool s_userSuppliedTests = false;
static RegTestSuite s_loadedRegTests;

void RegressionTestEnvironment::init(int argc, char** argv)
{
   
   if(argc == 2 && strcmp(argv[1], "-run_marathon_test") == 0)  {
//      coreprintln("Enabling marathon test");
      s_isMarathonTest = true;
   } else if(argc == 3 && strcmp(argv[1], "-f") == 0) {
      FILE* f = fopen(argv[2], "r");

      if(f) {
//         coreprintln("Reading file %s..", argv[2]);
         
         char curTest[255];
         
         s_userSuppliedTests = true;
         
         while(fgets(curTest, 255, f)) {
            std::string test(curTest);
         
            if(!test.empty()) {
               test = test.substr(0, test.length() - 1);
               s_loadedRegTests.push_back(test);
            }
         }
         
         fclose(f);         
      } else {
//         coreprintln("Failed to open %s..", argv[2]);   
      }
   }

   pal_thread_init();

#ifdef __linux__
   g_thread_init(NULL);
   gdk_threads_init();
   assert(g_thread_supported());

   gdk_threads_enter();

   gtk_init(&argc, &argv);
#elif defined(IPHONE)
   IPhoneInitUtil::initAutoReleasePool();
#endif
}

void RegressionTestEnvironment::close()
{
   pal_thread_deinit();

#ifdef __linux__
   gdk_threads_leave();
#endif

#ifdef IPHONE
   IPhoneInitUtil::destroyAutoReleasePool();
#endif
   
}

bool RegressionTestEnvironment::isMarathonTest()
{
   return s_isMarathonTest;
}

bool RegressionTestEnvironment::userSuppliedTests()
{
   return s_userSuppliedTests; 
}

const RegressionTestUtil::RegTestSuite& RegressionTestEnvironment::getUserSuppliedTests()
{
   return s_loadedRegTests;
}

bool RegressionTestEnvironment::conditionalWait(
   const bool& conditionVariable, unsigned int timeOutMs)
{   
   unsigned int startTime = environment::currentTimeMillis();
      

   while(!conditionVariable) {
      environment::doPlatformIteration();
      
      unsigned int elapsedTime =
         environment::currentTimeMillis() - startTime;
      
      if(elapsedTime >= timeOutMs) {
         return false;
      }
   }

   return true;
}

void RegressionTestEnvironment::sleep(unsigned int timeOutMs)
{
   unsigned int startTimeMs =
      environment::currentTimeMillis();

   environment::doPlatformIteration();
   
   while( environment::currentTimeMillis()
          - startTimeMs < timeOutMs) {
      environment::doPlatformIteration();
   }
}
