/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "NavPacket.h"
//#include "GlobalConstants.h"
#include "crc32.h"


//obfuscate the variable names in the symbol table:
#define magicPos printerStatus
#define MAGICBYTES printerCounter

#define XORSTRING                                                       \
   "\x37\x77\x89\x05\x28\x72\x5b\x2a\xce\xe4\x44\x1a"                   \
   "\x28\x72\x5b\x2a\xce\xe4\x44\x1a\x79\x8a\xdb\x90"                   \
   "\xce\xe4\x44\x1a\x79\x8a\xdb\x90\x2f\x5b\xcc\xd1"                   \
   "\x79\x8a\xdb\x90\x2f\x5b\xcc\xd1\x6e\x72\xb4\x9a"                   \
   "\x2f\x5b\xcc\xd1\x6e\x72\xb4\x9a\x79\x9b\xa5\x2b"                   \
   "\x6e\x72\xb4\x9a\x79\x9b\xa5\x2b\x45\x6a\xc1\x7c"                   \
   "\x79\x9b\xa5\x2b\x45\x6a\xc1\x7c\xe0\x4a\x81\x08"                   \
   "\x45\x6a\xc1\x7c\xe0\x4a\x81\x08\xbc\xdb\x31\x8a"                   \
   "\xe0\x4a\x81\x08\xbc\xdb\x31\x8a\xbe\x75\xa3\x36"                   \
   "\xbc\xdb\x31\x8a\xbe\x75\xa3\x36\xfe\x7d\xc6\x2c"                   \
   "\xbe\x75\xa3\x36\xfe\x7d\xc6\x2c\xd8\x91\xfc\x45"                   \
   "\xfe\x7d\xc6\x2c\xd8\x91\xfc\x45\x03\xb0\xde\x7b"                   \
   "\xd8\x91\xfc\x45\x03\xb0\xde\x7b\x4b\x82\xa6\x90"                   \
   "\x03\xb0\xde\x7b\x4b\x82\xa6\x90\xeb\x67\x0c\xcb"                   \
   "\x4b\x82\xa6\x90\xeb\x67\x0c\xcb\xb0\x8c\xd2\x6c"                   \
   "\xeb\x67\x0c\xcb\xb0\x8c\xd2\x6c\x67\x03\xf5\x24"                   \
   "\xb0\x8c\xd2\x6c\x67\x03\xf5\x24\x77\x98\x5a\x74"                   \
   "\x67\x03\xf5\x24\x77\x98\x5a\x74\x14\x1f\x9f\xec"                   \
   "\x77\x98\x5a\x74\x14\x1f\x9f\xec\xb0\x9b\x30\xb2"                   \
   "\x14\x1f\x9f\xec\xb0\x9b\x30\xb2\x4b\x0e\x2d\x95"                   \
   "\xb0\x9b\x30\xb2\x4b\x0e\x2d\x95\x90\xd3\x25\x7a"                   \
   "\x4b\x0e\x2d\x95"
#define XORLEN 256


namespace wf_ngp{

void 
wf_ngp_encrypt( unsigned char* buff, uint32 length )
{
   //xorCode(params, NAV_SERVER_COM_REQUEST_HEADER_SIZE);
   uint32 magicPos = 0;
   const char* MAGICBYTES = XORSTRING;
   for ( uint32 i = 0 ; i < length ; i++ ) {
      buff[ i ] = buff[ i ] ^ MAGICBYTES[ magicPos ];
      magicPos++;
      if ( magicPos >= XORLEN ) {
         magicPos = 0;
      }
   }
}


NavPacket::NavPacket( uint8 protoVer, uint16 type, uint8 reqID, uint8 reqVer )
      : m_protoVer( protoVer ), m_type( type ), m_reqID( reqID ), 
        m_reqVer( reqVer )
{
}


NavPacket::NavPacket( uint8 protoVer, uint16 type, uint8 reqID, uint8 reqVer,
                      const uint8* buff, uint32 buffLen )
      : m_protoVer( protoVer ), m_type( type ), m_reqID( reqID ), 
        m_reqVer( reqVer ), m_params( buff, buffLen, protoVer )
{
}


void
NavPacket::writeToInternal( std::vector<byte>& buff,
                      const NParamBlock& params ) const
{

   // offset    size (bytes)    description
   //      0               1    STX: always 0x02
   buff.push_back( 0x02 );
   //      1               4    length: total packet length
   //                                   STX, ..., crc
   //                           (Real value written last)
   buff.push_back( 0 );
   buff.push_back( 0 );
   buff.push_back( 0 );
   buff.push_back( 0 );
   //      5               1    protover: Protocol version
   //                           overall things like paramblock layout -
   //                           not tied to individual request types
   buff.push_back( m_protoVer );
   //      6               2    type: Message request type,
   //                                 describes payload.
   buff.push_back( ( m_type ) >> 8 );
   buff.push_back( ( m_type ) & 0xff );
   //      8               1    req_id: Request ID
   buff.push_back( m_reqID );
   //      9               1    req_ver: format version of the particular
   //                           request, as defined by "type"".
   buff.push_back( m_reqVer );

   writeSpecificHeaderPart( buff );

   //                      X    parameter_block: payload
   params.writeParams( buff, m_protoVer );

   // Write length
   // note addition of 4 to include crc which is not yet written
   buff[ 1 ] = (( buff.size() + 4 ) >> 24 ) & 0xff;
   buff[ 2 ] = (( buff.size() + 4 ) >> 16 ) & 0xff;
   buff[ 3 ] = (( buff.size() + 4 ) >> 8  ) & 0xff;
   buff[ 4 ] = (( buff.size() + 4 ) )       & 0xff;

   // (-4 from end)        4    crc: crc of entire packet including STX.
   uint32 crc = crc32::calcCrc32( &buff.front(), buff.size() );
   buff.push_back( (( crc ) >> 24 ) & 0xff );
   buff.push_back( (( crc ) >> 16 ) & 0xff );
   buff.push_back( (( crc ) >> 8  ) & 0xff );
   buff.push_back( (( crc ) )       & 0xff );

   // Encryption starts at offset 10 which is at
   //    parameter_block for requests
   //    status_code for replies.
   // the STX, ..., req_ver is in clear.
   wf_ngp_encrypt( &(buff[10]), buff.size() -10 );
}

void
NavPacket::writeTo( std::vector<byte>& buff ) const {
   
   writeToInternal(buff, m_params);
}

void
NavPacket::writeTo( std::vector<byte>& buff,
                    const std::vector<NParam>& extraParams ) const
{
   // this is not very efficient but the nice fix is to split
   // out the reading/writing of paramblock header from writing of the
   // params. This must also take into account trying to gzip etc.
   NParamBlock params;
   params.assignToThis(m_params);
   for(std::vector<NParam>::const_iterator p = extraParams.begin();
       p < extraParams.end();
       ++p) {
      params.addParam(*p);
   }

   writeToInternal(buff, params);
}


uint8
NavPacket::getCurrentProtoVer() {
   // Get value from "GlobalConstants.h" or similar (from parsed xml).
   //return ServerParser::getCurrentProtoVer();
   return 0xc;
}


} // End namespace wf_ngp
