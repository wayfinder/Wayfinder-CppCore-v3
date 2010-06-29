/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
*/

#ifndef PAL_UTILS_H
#define PAL_UTILS_H

#include "PALMachineTypes.h"

/**
 * Struct describing time.
 */
struct palTime_t {
   /// The year
   wf_uint32 year;

   /// The month
   wf_uint32 month;

   /// The day
   wf_uint32 day;

   /// The hour
   wf_uint32 hour;

   /// The minutes
   wf_uint32 min;

   /// The seconds
   wf_uint32 sec;
};

/**
 * Returns the time of the day.
 *
 * @param timeStruct Will contain the time of the day
 *                   according to the palTime_t struct
 *                   when returning.
 */
void pal_time_timeOfDay(palTime_t& timeStruct);

/**
 * Returns current time in milliseconds.
 *
 * @return Current time in milliseconds.
 */
wf_uint32 pal_time_millis();

/**
 * Returns current time in milliseconds, but does not compensate broken midnight
 *
 * @return Current time in milliseconds.
 */
wf_uint32 pal_current_time_millis();
   
#endif
