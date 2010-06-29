/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "NavReplyPacket.h"
#include <cstring>

namespace wf_ngp{


NavReplyPacket::NavReplyPacket( uint8 protoVer, uint16 type, uint8 reqID, 
                                uint8 reqVer, uint8 statusCode, 
                                const char* statusMessage, 
                                const uint8* buff, uint32 buffLen )
      : NavPacket( protoVer, type, reqID, reqVer, buff, buffLen ), 
        m_statusCode( statusCode )
{
   setStatusMessage( statusMessage );
}

NavReplyPacket::NavReplyPacket( byte protoVer, uint16 type, byte reqID, byte reqVer)
   : NavPacket(protoVer, type, reqID, reqVer),
     m_statusCode(), m_statusMessage(0)
{
   
}

NavReplyPacket::NavReplyPacket( const byte* data, uint32 size ) 
      : NavPacket( 0, 0, 0, 0 )
{
   NonOwningBuffer buff( data, size );

   setProtoVer(      buff.readNext8bit() );
   // XXX: see below!
   setType(          buff.readNextUnaligned16bit() );
   // added
   setReqID(         buff.readNext8bit() );
   setRequestVer(    buff.readNext8bit() );
   setStatusCode(    buff.readNext8bit() );
   setStatusMessage( buff.getNextCharString() );
   // XXX: crc must not be parsed as part of param block!
   m_params.assignToThis( 
      NParamBlock( buff.accessRawData(), buff.remaining() - 4, m_protoVer ) );
}

void
NavReplyPacket::writeSpecificHeaderPart( std::vector<byte>& buff ) const
{
   //     10               1    status_code: Typically 0x00
   buff.push_back( m_statusCode );

   //     11               strlen +1    status_message: Typically ""
   int n = m_statusMessage.getLength();
   const byte* rawStr = m_statusMessage.accessRawData(0);
   buff.insert(buff.end(), rawStr, rawStr + n);
   buff.push_back(0); // add terminating \0
}


PacketTypeEnum
NavReplyPacket::getRequestType( const uint8* data, int size ) {
   NonOwningBuffer buff( data, size );
   buff.readNext8bit(); // ProtoVer
   // XXX: changed from unaligned32. spec says 2 bytes!
   return PacketTypeEnum( buff.readNextUnaligned16bit() );
}


} // End namespace wf_ngp
