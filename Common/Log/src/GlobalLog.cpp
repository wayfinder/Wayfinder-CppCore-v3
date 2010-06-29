/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#define CORE_LOGGING_MODULE_NAME "GlobalLog"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "GlobalLog.h"

#include "PALMachineTypes.h"
#include "PALThreading.h"
#include "PALTimeUtils.h"

#include "CoreDebugPrint.h"
#include "Lock.h"

using namespace std;

#define BUF_SIZE 512

vector<string> GlobalLog::m_logVec;
Mutex* GlobalLog::m_mutex = pal_thread_create_mutex();

GlobalLog::~GlobalLog()
{
   deleteMembers();
}

void GlobalLog::addLogMsg(const char* logMsg)
{
   Lock lock(m_mutex);

   palTime_t tm;
   pal_time_timeOfDay(tm);

   char time_string[40];
   sprintf(time_string, "[%d-%d-%d - %02d:%02d:%02d",
           tm.year, tm.month, tm.day, tm.hour, tm.min, tm.sec);

   static wf_uint32 s_millis = pal_time_millis();
   float diff = (pal_time_millis() - s_millis) / 1000.0;
   wf_uint32 milliseconds = pal_time_millis() % 1000;

   char formatBuf[BUF_SIZE];
   memset(formatBuf, 0, BUF_SIZE * sizeof(char));

   sprintf(formatBuf, "%s.%03d] - diff = %03.2f - %s",
           time_string, milliseconds, diff, logMsg);

   m_logVec.push_back(formatBuf);
}

void GlobalLog::dumpLog()
{
   coreprintln("===========================================================================\n");
   coreprintln("============= Starting dumping of log messages ============================\n");
   for (t_stringVec::const_iterator it = m_logVec.begin();
        it != m_logVec.end();
        ++it) {
      coreprintln("%s\n", it->c_str());
   }
   coreprintln("===========================================================================\n");
   coreprintln("============= Done dumping log messages ===================================\n");
   coreprintln("===========================================================================\n\n\n");
}

void GlobalLog::deleteMembers()
{
   pal_thread_destroy_mutex(m_mutex);
   m_mutex = NULL;
}
