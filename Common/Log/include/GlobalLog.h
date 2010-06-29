/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef GLOBAL_LOG_H
#define GLOBAL_LOG_H

#include "PALThreading.h"
#include <vector>
#include <string>
#include <cstring>

/**
 * Logging class that should be used as a global static object.
 * Members are initialized during startup and when a log print
 * should be added simply call addLogMsg. When you want the output
 * to the console call dumpLog.
 *
 * Note that when exiting you should call deleteMembers to do
 * proper cleanup.
 */
class GlobalLog
{
public:

   /**
    * Destructor, just in case.
    */
   ~GlobalLog();
      
   /**
    * Pushes a log message to a vector as a formated string.
    * The format will be year-month-day hour-minute-second.millisecod -
    * diff = the difference in seconds from the first log message -
    * the log message.
    *
    * @param logMsg The message to be added to the vector, this message will
    *               be concatenated with the date, time and time diff.
    */
   static void addLogMsg(const char* logMsg);

   /**
    * Dumps all the log messages that have been put in the vector
    * to the console.
    */
   static void dumpLog();

   /**
    * Does cleanup, delete members of this class that are located on
    * the heap.
    */
   static void deleteMembers();

private:

   /// Vector containing log messages.
   typedef std::vector<std::string> t_stringVec;
   static t_stringVec m_logVec;

   /// Mutex to lock while adding a log message
   static Mutex* m_mutex;
#endif
};
