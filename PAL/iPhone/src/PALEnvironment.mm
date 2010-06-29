/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
*/

#include "PALEnvironment.h"
#include "PALTimeUtils.h"

#include <CoreFoundation/CFRunLoop.h>
#include <pthread.h>
#include <sched.h>

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#include <sys/time.h>

#include <iostream>


namespace environment {

   wf_uint32 currentTimeMillis()
   {
      //below call exists in POSIX pal:
      return  pal_current_time_millis();
   }

   void doPlatformIteration() {
      SInt32 res = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.1, false);

      switch(res) {
      case kCFRunLoopRunFinished:
         std::cout << "The run loop mode mode has no sources or timers." << std::endl;
         break;
      case kCFRunLoopRunStopped:
         std::cout << "The run loop was stopped with CFRunLoopStop." << std::endl;
         break;
      case kCFRunLoopRunTimedOut:
         std::cout << "The time interval seconds passed." << std::endl;
         break;
      case kCFRunLoopRunHandledSource:
         break;
      }

      sleep(0.016f);
   }

   void sleep(float time)
   {
      [NSThread sleepForTimeInterval:time];
   }

} // End of namespace environment

