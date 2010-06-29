/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "NParam.h"
#include <cstdio>
#include <cstring>
#ifdef NGPLIB_SERVER
#include <Utility.h>
#endif

namespace wf_ngp {
using namespace std;

NParam::NParam( uint16 paramID, const byte* buff, uint16 len ) 
   : m_paramID( paramID ), m_buff( buff, buff + len )
{
}


NParam::NParam( const NParam& other ) {
   m_paramID = other.m_paramID;
   m_buff = other.m_buff;
}

NParam::NParam( uint16 paramID, int32 lat, int32 lon )
   : m_paramID( paramID )
{
   addInt32( lat );
   addInt32( lon );
}

NParam::NParam( uint16 paramID, const char* data )
   : m_paramID( paramID ), m_buff( data, data + strlen( data ) + 1 )
{
}

NParam::NParam( uint16 paramID, bool data ) 
   : m_paramID( paramID )
{
   if ( !data ) {
      m_buff.push_back( data );
   }
}


NParam::NParam( uint16 paramID, byte data ) 
   : m_paramID( paramID )
{
   m_buff.push_back( data );
}


NParam::NParam( uint16 paramID, uint16 data ) 
   : m_paramID( paramID )
{
   addUint16( data );
}


NParam::NParam( uint16 paramID, uint32 data ) 
   : m_paramID( paramID )
{
   addUint32( data );
}


NParam::NParam( uint16 paramID, int32 data ) 
   : m_paramID( paramID )
{
   addInt32( data );
}

#ifdef NGPLIB_SERVER
NParam::NParam( uint16 paramID, const char* data, bool usesLatin1 )
   : m_paramID( paramID )
{
   addString( data, usesLatin1 );
}

NParam::NParam( uint16 paramID, const MC2String& data, 
                bool usesLatin1 )
   : m_paramID( paramID )
{
   addString( data, usesLatin1 );
}
#endif

NParam::NParam( uint16 paramID, const uint16* data, uint16 len )
   : m_paramID( paramID )
{
   m_buff.reserve( len*2 );
   for ( uint16 i = 0 ; i < len ; ++i ) {
      addUint16( data[ i ] );
   }
}


NParam::NParam( uint16 paramID, const uint32* data, uint16 len )
   : m_paramID( paramID )
{
   m_buff.reserve( len*4 );
   for ( uint16 i = 0 ; i < len ; ++i ) {
      addUint32( data[ i ] );
   }
}


NParam::NParam( uint16 paramID, const int32* data, uint16 len )
   : m_paramID( paramID )
{
   m_buff.reserve( len*4 );
   for ( uint16 i = 0 ; i < len ; ++i ) {
      addInt32( data[ i ] );
   }
}

NParam::NParam( uint16 paramID )
   : m_paramID( paramID )
{
}

void
NParam::addByte( byte data ) {
   m_buff.push_back( data );
}

void NParam::addUint8( byte data )
{
   m_buff.push_back( data );
}

void 
NParam::addUint16( uint16 data ) {
   m_buff.push_back( ( data ) >> 8 );
   m_buff.push_back( ( data ) & 0xff );
}

void
NParam::addUint32ToByteVector( vector<byte>& v, uint32 d ) {
   v.push_back( (( d ) >> 24 ) & 0xff );
   v.push_back( (( d ) >> 16 ) & 0xff );
   v.push_back( (( d ) >> 8  ) & 0xff );
   v.push_back( (( d ) )       & 0xff );
}

void 
NParam::addUint32( uint32 data ) {
   addUint32ToByteVector( m_buff, data );
}

void 
NParam::addInt32( int32 data ) {
   addUint32( data );
}


void 
NParam::addByteArray( const byte* buff, uint16 len ) {
   m_buff.insert( m_buff.end(), buff, buff + len );
}

void 
NParam::addByteArray( const char* buff, uint16 len ) {
   m_buff.insert( m_buff.end(), reinterpret_cast<const byte*>( buff ), 
                  reinterpret_cast<const byte*>( buff ) + len );
}

#ifdef NGPLIB_SERVER
void
NParam::addString( const char* str, bool usesLatin1 ) {
   MC2String tmp;
   if( usesLatin1 ) {
      tmp = UTF8Util::mc2ToIso(MC2String(str));
   } else {
      tmp = UTF8Util::mc2ToUtf8(MC2String(str));
   }
   m_buff.insert( m_buff.end(), tmp.c_str(), tmp.c_str() + tmp.size() + 1 );
} 


void
NParam::addString( const MC2String& str, bool usesLatin1 ) {
   MC2String tmp;
   if( usesLatin1 ) {
      tmp = UTF8Util::mc2ToIso(str);
   } else {
      tmp = UTF8Util::mc2ToUtf8(str);
   }
   m_buff.insert( m_buff.end(), tmp.c_str(), tmp.c_str() + tmp.size() + 1);
}
#endif


uint16
NParam::getParamID() const {
   return m_paramID;
}


void
NParam::setParamID( uint16 paramID ) {
   m_paramID = paramID;
}


uint16 
NParam::getLength() const {
   return m_buff.size();
}


const byte* 
NParam::getBuff() const {
   return &m_buff.front();
}


 bool 
NParam::getBool() const {
   if ( getLength() == 0 ) {
      return true;
   } else {
      return m_buff[ 0 ] != 0;
   }
}

 byte 
NParam::getByte( uint16 index ) const {
   return m_buff[ index ];
}

 byte 
NParam::incGetByte( uint32& index ) const {
   return m_buff[ index++ ];
}

 byte 
NParam::getUint8( uint16 index ) const {
   return getByte( index );
}

 byte 
NParam::incGetUint8( uint32& index ) const {
   return incGetByte( index );
}

 uint16 
NParam::getUint16( uint16 index ) const {
  return ( (uint16( m_buff[ index + 0 ]) << 8) |
           (uint16( m_buff[ index + 1 ])     ) );
//   return ntohs(*((uint16 *)(&(m_buff.front())+index)));
}

 uint16 
NParam::incGetUint16( uint32& index ) const {
   uint16 r = ntohs(*((uint16 *)(&(m_buff.front())+index)));
   index += 2;
   return r;
}

 uint32
NParam::getUint32FromByteArray( const byte* b, uint16 index ) {
  return ( (uint32(b[ index + 0 ]) << 24) |
           (uint32(b[ index + 1 ]) << 16) |
           (uint32(b[ index + 2 ]) <<  8) |
           (uint32(b[ index + 3 ])      ) );
//    return ntohl(*((uint32 *)(b+index)));
}

 uint32 
NParam::getUint32( uint16 index ) const {
   return getUint32FromByteArray( &m_buff.front(), index );
}

 uint32 
NParam::incGetUint32( uint32& index ) const {
   uint32 r = getUint32FromByteArray( &m_buff.front(), index );
   index += 4;
   return r;
}

int32 
NParam::getInt32( uint16 index ) const {
   return ntohl( (int32(m_buff[ index + 0 ]) << 24) |
                 (int32(m_buff[ index + 1 ]) << 16) |
                 (int32(m_buff[ index + 2 ]) <<  8) |
                 (int32(m_buff[ index + 3 ])      ) );

//   return ntohl(*((int32 *)(&(m_buff.front())+index)));
}

int32 
NParam::incGetInt32( uint32& index ) const {
   int32 r = ntohl(*((int32 *)(&(m_buff.front())+index)));
   index += 4;
   return r;
}
#ifdef NGPLIB_SERVER
MC2String 
NParam::getString( bool usesLatin1 ) const {
   MC2String str = reinterpret_cast<const char*> (&m_buff.front());
   if( usesLatin1 ) {
      return UTF8Util::isoToMc2(str);
   } else {
      return UTF8Util::utf8ToMc2(str);
   }
}


MC2String 
NParam::incGetString( bool usesLatin1, uint32& index ) const {
   MC2String str = reinterpret_cast<const char*> (&m_buff.front() + index);
   index += str.length() + 1;
   if( usesLatin1 ) {
      return UTF8Util::isoToMc2(str);
   } else {
      return UTF8Util::utf8ToMc2(str);
   }
}
#endif

const byte* 
NParam::getByteArray() const {
   return &m_buff.front();
}


uint16
NParam::getUint16Array( uint16 index ) const {
   return ntohs(*(((uint16 *)&(m_buff.front()))+index));
}


uint32
NParam::getUint32Array( uint16 index ) const {
   return ntohl(*(((uint32 *)&(m_buff.front()))+index));
}


int32
NParam::getInt32Array( uint16 index ) const {
   return ntohl(*(((int32 *)&(m_buff.front()))+index));
}


void
NParam::writeTo( vector< byte >& buff ) const {
   buff.push_back( ( m_paramID ) >> 8 );
   buff.push_back( ( m_paramID ) & 0xff );
   buff.push_back( ( m_buff.size() ) >> 8 );
   buff.push_back( ( m_buff.size() ) & 0xff );
   buff.insert( buff.end(), m_buff.begin(), m_buff.end() );
}


bool 
NParam::operator < ( const NParam& b ) const {
   return m_paramID < b.m_paramID;
}


bool 
NParam::operator > ( const NParam& b ) const {
   return m_paramID > b.m_paramID;
}


bool 
NParam::operator != ( const NParam& b ) const {
   return m_paramID != b.m_paramID;
}


bool 
NParam::operator == ( const NParam& b ) const {
   return m_paramID == b.m_paramID;
}


void
NParam::setBuff( const vector< byte > buff ) {
   m_buff = buff;
}


const vector< byte >&
NParam::getVector() const {
   return m_buff;
}


void
NParam::clear() {
   m_buff.clear();
}

void
NParam::addString( const char* str ) {
   m_buff.insert( m_buff.end(), str, str + strlen( str ) + 1 );
}


void
NParam::addBool(bool b) {
   if(!b) {
      addByte(0);
   }
}


const char* 
NParam::getString( uint16 index ) const {
   return reinterpret_cast<const char*> (&m_buff.front()+index);
}

const char* 
NParam::incGetString( uint32& index ) const {
   const char* str = reinterpret_cast<const char*> (&m_buff.front() + index);
   index += strlen(str) + 1;
   return str;
}


#ifdef NGPLIB_SERVER
void
NParam::dump( ostream& out, bool dumpValue, bool singleLine, 
              uint32 maxLen ) const
 {
   if ( !singleLine ) {
      out << "   ParamID " << getParamID() << " len " 
          << getLength() << endl;
      #if 0
      if ( dumpValue ) {
         Utility::hexDump( out, const_cast<byte*>( getBuff() ), 
                           getLength(), "      " );
      }
      #endif
   } else {
      out << getParamID();
      bool allPrintable = true;
      bool anyPrintable = false;
      for ( uint32 i = 0 ; i < MIN( getLength(), maxLen ) ; ++i ) {
         if ( !isprint( getVector()[ i ] ) || getVector()[ i ] == ':' ||
              getVector()[ i ] == ';' ) {
            if ( i + 1 == getLength() && getVector()[ i ] == '\0' &&
                 getLength() > 1 )
            {
               // Ok is stringlike
            } else {
               allPrintable = false;
            }
         } else {
            anyPrintable = true; 
         }
      }
      if ( !allPrintable ) {
         out << ":0x" << hex << setfill( '0' );
         for ( uint32 i = 0 ; i < MIN( getLength(), maxLen ) ; ++i ) {
            out << setw( 2 ) << uint32( getVector()[ i ] );
         }
         out << dec;
         out << ";";
      } // else // Else here and no if ( anyPrintable ) it's either 0x or A
      if ( anyPrintable ) {
         if ( allPrintable ) {
            out << ":";
         }
         for ( uint32 i = 0 ; i < MIN( getLength(), maxLen ) ; ++i ) {
            if ( isprint( getVector()[ i ] ) && getVector()[ i ] != ':' &&
                 getVector()[ i ] != ';' ) {
               out << getVector()[ i ];
            } else {
               out << ".";
            }
         }
      }
      if ( getLength() > maxLen ) {
         out << "(" << getLength() << ")"; 
      }
   }
}


int
NParam::readParam( const char* str ) {
   #if 0
   mc2dbg4 << "str " << str << endl;
   #endif
   int res = 0;
   // 4:wf6-s-60-v2-br. 5:none. 6:0x0001 28:NoCRC!.
   const uint32 strLen = strlen( str ) + 1;
   char data[ strLen ];
   char tmp[ strLen ];
   tmp[ 0] = '\0';
   uint16 paramID = 0;
   uint32 extraLen = 0;
   if ( sscanf( str, "%hu:0x%[0-9A-Fa-f];%[^\n]", &paramID, data + 2, tmp ) 
        == 3 ) {
      data[ 0 ] = '0';
      data[ 1 ] = 'x';
      extraLen += 1 + strlen( tmp );
      res = 1;
   } else if ( sscanf( str, "%hu:0x%[0-9A-Fa-f];", &paramID, data + 2 ) == 2 )
   {
      data[ 0 ] = '0';
      data[ 1 ] = 'x';
      extraLen += 1;
      res = 1;
   } else if ( sscanf( str, "%hu:%[^\n]", &paramID, data ) == 2 ) {
      res = 1;
   } else if ( sscanf( str, "%hu:%[^\n]", &paramID, data ) == 1 ) {
      data[ 0 ] = '\0';
      res = 1;
   } else {
      res = 0;
   }

   if ( res > 0 ) {
      m_paramID = paramID;
      const uint32 dataLen = strlen( data );
      uint32 paramIDLen = 1;
      // Improve this only early test
      while ( paramID >= 10 ) {
         ++paramIDLen;
         paramID /= 10;
      }
      res = dataLen + extraLen + 1 + paramIDLen;
      if ( dataLen > 1 && data[ 0 ] == '0' && data[ 1 ] == 'x' ) {
         // Hex
         m_buff.clear();
         int pos = 2; // 0x
         while ( data[ pos ] != '\0' && data[ pos ] != ';' ) {
            byte a = isdigit( data[ pos ] ) ? (data[ pos ]-48) :
               (isupper( data[ pos ] ) ? (data[ pos ]-55) : 
                (data[ pos ]-87));
            byte b = isdigit( data[ pos + 1 ] ) ? (data[ pos + 1 ]-48) :
               (isupper( data[ pos + 1 ] ) ? (data[ pos + 1 ]-55) : 
                (data[ pos + 1 ]-87));
            m_buff.push_back( (a<<4) + b );
            pos += 2;
         }
      } else if ( dataLen > 0 && data[ dataLen -1 ] == '.' ) {
         m_buff.insert( m_buff.begin(), data, data + dataLen );
         m_buff.back() = '\0';
      } else {
         m_buff.insert( m_buff.begin(), data, data + dataLen );
      }
   }

   return res;
}
#endif

const char*
NParam::getTypeAsString( NParam_t type ) {
   switch ( type ) {
      case Bool :
         return "Bool";
      case Byte :
         return "Byte";
      case Uint16 :
         return "Uint16";
      case Uint32 :
         return "Uint32";
      case Int32 :
         return "Int32";
      case String :
         return "String";
      case Byte_array :
         return "Byte_array";
      case Uint16_array :
         return "Uint16_array";
      case Uint32_array :
         return "Uint32_array";
      case Int32_array :
         return "Int32_array";
   }

   return "Unknown type";
}

} // end namespace wf_ngp

