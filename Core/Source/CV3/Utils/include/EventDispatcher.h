/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef EVENT_DISPATCHER_H
#define EVENT_DISPATCHER_H

/**
 *
 * Note: New description at end of comment field, old description kept for legacy
 *
 *
 * MessageDispatcher will receive a RequestPacket from some module/package in
 * Core (the RequestPacket could contain the required security level, e.g
 * requires a secure line).
 *
 * MessageDispatcher calls serialize on the RequestPacket object which returns
 * a RequestParams object, the data in this object will be based on the
 * following format:.
 * |-----------------+--------------+------+----------------------------------|
 * | Function        |       Offset | Size | Description                      |
 * |-----------------+--------------+------+----------------------------------|
 * | STX             |            0 |    1 | Always 0x02                      |
 * | protver         |            5 |    1 | protocol version                 |
 * | length          |            1 |    4 | Total package length             |
 * | type            |            6 |    2 | Message request type             |
 * | req_id          |            8 |    1 | Request ID                       |
 * | req_ver         |            9 |    1 | Request type version             |
 * | parameter block |           10 |    X | The payload                      | 
 * | crc             | 10 + X == -4 |    4 | CRC of packet                    |
 * |-----------------+--------------+------+----------------------------------|
 *
 * MessageDispatcher sends the RequestParams to the correct ServerParser
 * (based on the security level which can be fetched by the RequestPacket
 * object).
 *
 *
 *
 * The EventDispatcher sends Events between different system entities
 * (eg. between CV3 and ServerCommunication, between CV3 and ThreadXYZ)
 *
 *
 */
class EventDispatcher {
   
};

#endif // EVENT_DISPATCHER_H
