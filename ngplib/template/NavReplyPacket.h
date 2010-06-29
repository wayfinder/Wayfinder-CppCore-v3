/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef NAVREPLYPACKET_H
#define NAVREPLYPACKET_H

#include "NGPConfig.h"
#include "Common.h"
#include "NavPacket.h"
#include "Buffer.h"
#include <cstring>

namespace wf_ngp {


/**
 * Class that represents a Navigator reply packet.
 * 
 * @author Daniel Pettersson
 */
class NavReplyPacket : public NavPacket {
   public:

      /**
       * Constructor.
       *
       * @param protoVer The protocol version.
       * @param type The type of packet.
       * @param reqID The ID of the packet.
       * @param reqVer The version of request.
       * @param buff The parameter block buffer.
       * @param buffLen The length of the parameter block buffer.
       */
      NavReplyPacket( byte protoVer, uint16 type, byte reqID, 
                      byte reqVer, byte statusCode, 
                      const char* statusMessage, 
                      const byte* buff, uint32 buffLen );

      /**
       * Constructor.
       *
       * @param protoVer The protocol version.
       * @param type The type of packet.
       * @param reqID The ID of the packet.
       * @param reqVer The version of request.
       * @param buff The parameter block buffer.
       * @param buffLen The length of the parameter block buffer.
       */
      NavReplyPacket( byte protoVer, uint16 type, byte reqID, byte reqVer);
                   

      /**
       * Constructs from a Nav2 buffer with type and all in it.
       */
      NavReplyPacket( const byte* buff, uint32 buffLen );


      /**
       * Get the statusCode.
       */
      byte getStatusCode() const;


      /**
       * Set the statusCode.
       */
      void setStatusCode( byte statusCode );


      /**
       * Get the statusMessage.
       */
      const char* getStatusMessage() const;


      /**
       * Set the statusMessage.
       */
      void setStatusMessage( const char* statusMessage );


      /**
       * Get the type from a Nav2 buffer with type and all in it.
       */
       static PacketTypeEnum getRequestType( const uint8* buf, int size );


   protected:
      /**
       * Writes m_statusCode and m_statusMessage.
       */
      virtual void writeSpecificHeaderPart( std::vector<byte>& buff ) const;


   private:
      /// The status of the reply.
      byte m_statusCode;
   

      /// The status code as text.
      Buffer m_statusMessage;
};


// =======================================================================
//                                     Implementation of inlined methods =


inline byte 
NavReplyPacket::getStatusCode() const {
   return m_statusCode;
}


inline void 
NavReplyPacket::setStatusCode( byte statusCode ) {
   m_statusCode = statusCode;
}


inline const char* 
NavReplyPacket::getStatusMessage() const {
   return reinterpret_cast< const char* > (
      m_statusMessage.accessRawData( 0 ) );
}


inline void 
NavReplyPacket::setStatusMessage( const char* statusMessage ) {
   m_statusMessage.reserve( strlen( statusMessage ) + 1 );
   m_statusMessage.clear();
   m_statusMessage.writeNextCharString( statusMessage );
}


} /* namespace wf_ngp */

#endif // NAVREPLYPACKET_H

