/*
 *    Copyright 2004-2005, Wayfinder Systems AB.
 */

#ifndef NPARAM_H
#define NPARAM_H

#include "NGPConfig.h"

#ifdef NGPLIB_SERVER

#include <MC2String.h>
#include <UTF8Util.h>
#include <netinet/in.h>
#include <iomanip>

#else

#ifdef UNDER_CE
# include <Winsock.h>
#else
# include <netinet/in.h>
#endif // UNDER_CE

#endif // NGPLIB_SERVER

#include <vector>

namespace wf_ngp {

/**
 * Class holding a NavServerProt, v10+, param.
 * All complex values (16 bit, 32 bit) are considered to have
 * the most significant byte first. All data is in network
 * order that is.
 *
 * @author Daniel Pettersson
 */
class NParam {
public:
   /**
    * The types of NParam content, not used directly.
    */
   enum NParam_t {
      /**
       * The length may be 0 or 1. If 0 length then value is true. 
       * If length is 1 then the value is false if the byte is 0
       * true if not.
       */
      Bool,
      /// Length of 1, values 0-255 are possible. 
      Byte,
      /// Length of 2, values 0-65535 are possible.
      Uint16,
      /// Length of 4, values 0-4294967295 are possible.
      Uint32,
      /// Length of 4, values -2147483648 to 2147483647 are possible.
      Int32,
      /// Array of bytes ended by an null-byte, integer value of 0.
      /// Strings are utf-8 encoded.
      String,
      /// An array of length bytes.
      Byte_array,
      /// An array of length/2 uint16s.
      Uint16_array,
      /// An array of length/4 uint32s.
      Uint32_array,
      /// An array of length/4 int32s.
      Int32_array,
   };


   /**
    * Constructor.
    * 
    * @param paramID The parameter ID.
    * @param buff The buffer with the parameter data.
    * @param len The length of the parameter data.
    */
   NParam( uint16 paramID, const byte* buff, uint16 len );


   /**
    * Constructor from bool.
    */
   NParam( uint16 paramID, bool data );


   /**
    * Constructor from byte.
    */
   NParam( uint16 paramID, byte data );


   /**
    * Constructor from uint16.
    */
   NParam( uint16 paramID, uint16 data );


   /**
    * Constructor from uint32.
    */
   NParam( uint16 paramID, uint32 data );


   /**
    * Constructor from int32.
    */
   NParam( uint16 paramID, int32 data );

   /**
    * Constructor from string.
    */
   NParam( uint16 paramID, const char* data );

#ifdef NGPLIB_SERVER
   
   /**
    * Constructor from string.
    */
   NParam( uint16 paramID, const char* data, bool usesLatin1 = false );
   
   /**
    * Constructor from MC2String.
    */
   NParam( uint16 paramID, const MC2String& data, bool usesLatin1 = false );
#endif // NGPLIB_SERVER

   /**
    * Constructor from uint16 array.
    */
   NParam( uint16 paramID, const uint16* data, uint16 len );


   /**
    * Constructor from uint32 array.
    */
   NParam( uint16 paramID, const uint32* data, uint16 len );


   /**
    * Constructor from int32 array.
    */
   NParam( uint16 paramID, const int32* data, uint16 len );

   /**
    * Constructor from Coordinate.
    */
   NParam( uint16 paramID, int32 lat, int32 lon );
   
   /**
    * Constructor for param without data.
    */
   NParam( uint16 paramID );


   /**
    * Get the paramID.
    */
   uint16 getParamID() const;


   /**
    * Set the paramID.
    */
   void setParamID( uint16 paramID );


   /**
    * Get the buffer length.
    */
   uint16 getLength() const;


   /**
    * Get const referense to buffer.
    */
   const byte* getBuff() const;


   /**
    * Get buffer as a bool.
    */
   bool getBool() const;


   /**
    * Get buffer as a byte.
    */
   byte getByte( uint16 index = 0 ) const;

   /**
    * Get buffer as a byte. Update the index.
    */
   byte incGetByte( uint32& index ) const;

   /**
    * Get buffer as a byte.
    */
   byte getUint8( uint16 index = 0 ) const;

   /**
    * Get buffer as a byte. Update the index.
    */
   byte incGetUint8( uint32& index ) const;

   /**
    * Get buffer as an uint16.
    */
   uint16 getUint16( uint16 index = 0 ) const;

   /**
    * Get buffer as an uint16. Update the index.
    */
   uint16 incGetUint16( uint32& index ) const;

   /**
    * Get an uint32 from a byte vector.
    */
   static uint32 getUint32FromByteArray( const byte* b,
                                         uint16 index = 0 );

   /**
    * Get buffer as an uint32. 
    */
   uint32 getUint32( uint16 index = 0 ) const;

   /**
    * Get buffer as an uint32. Update the index.
    */
   uint32 incGetUint32( uint32& index ) const;

   /**
    * Get buffer as an int32.
    */
   int32 getInt32( uint16 index = 0 ) const;

   /**
    * Get buffer as an int32. Update the index.
    */
   int32 incGetInt32( uint32& index ) const;

   /**
    * Get buffer as a string.
    */
   const char* getString( uint16 index = 0 ) const;

   /**
    * Get buffer as a string. Update the index.
    */
   const char* incGetString( uint32& index ) const;
   
#ifdef NGPLIB_SERVER
   /**
    * Get buffer as a string.
    */
   MC2String getString( bool usesLatin1 = false ) const;

   /**
    * Get buffer as a string. Update the index.
    */
   MC2String incGetString( bool usesLatin1, uint32& index ) const;
#endif // NGPLIB_SERVER

   /**
    * Get buffer as byte array. Is getLength bytes long.
    */
   const byte* getByteArray() const;


   /**
    * Get uint16 at position index.
    */
   uint16 getUint16Array( uint16 index ) const;


   /**
    * Get uint32 at position index.
    */
   uint32 getUint32Array( uint16 index ) const;


   /**
    * Get int32 at position index.
    */
   int32 getInt32Array( uint16 index ) const;


   /**
    * Append a byte.
    */
   void addByte( byte data );

   /**
    * Append an uint8.
    */  
   void addUint8( byte data );


   /**
    * Append an uint16.
    */
   void addUint16( uint16 data );


   /**
    * Append an uint32.
    */
   void addUint32( uint32 data );


   /**
    * Append an int32.
    */
   void addInt32( int32 data );


   /**
    * Append a byte array.
    */
   void addByteArray( const byte* buff, uint16 len );

   /**
    * Append a byte array.
    */
   void addByteArray( const char* buff, uint16 len );

   /**
    * Append a string.
    */
   void addString( const char* str );
   

   /**
    * Appends a bool
    */
   void addBool(bool boll);


#ifdef NGPLIB_SERVER
   /**
    * Append a string.
    */
   void addString( const char* str, bool usesLatin1 = false );

   /**
    * Append a string.
    */
   void addString( const MC2String& str, bool usesLatin1 = false );
#endif // NGPLIB_SERVER
   
   /**
    * Write to a byte buffer.
    */
   void writeTo( std::vector< byte >& buff ) const;


   /**
    * Comparison operator <.
    */
   bool operator < ( const NParam& b ) const;


   /**
    * Comparison operator >.
    */
   bool operator > ( const NParam& b ) const;


   /**
    * Comparison operator !=.
    */
   bool operator != ( const NParam& b ) const;


   /**
    * Comparison operator ==.
    */
   bool operator == ( const NParam& b ) const;


   /**
    * Copy cont for debugprinting
    */
   NParam( const NParam& other );

#ifdef NGPLIB_SERVER
   /**
    * Dump param to out.
    */
   void dump( ostream& out, bool dumpValue = false, 
              bool singleLine = false, uint32 maxLen = 25 ) const;
#endif // NGPLIB_SERVER

   /**
    * Get the type as string.
    */
   static const char* getTypeAsString( NParam_t type );


   /**
    * Set the content.
    */
   void setBuff( const std::vector< byte > buff );


   /**
    * Get the buffer as vector< byte >.
    */
   const std::vector< byte >& getVector() const;


   /**
    * Clear the param from any content.
    */
   void clear();

   /**
    * Static method to add an uint32 to a byte array.
    */
   static void addUint32ToByteVector( std::vector<byte>& v, uint32 d );

#ifdef NGPLIB_SERVER
   /**
    * Read param from dump string.
    */
   int readParam( const char* str );
#endif // NGPLIB_SERVER

private:
   /// The paramID.
   uint16 m_paramID;


   /// The buffer.
   std::vector<byte> m_buff;
};


} // end namespace wf_ngp

#endif // NPARAM_H

