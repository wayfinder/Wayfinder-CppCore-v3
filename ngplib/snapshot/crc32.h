#ifndef CRC32_H
#define CRC32_H

#include "arch.h"

namespace isab {

class crc32
{
public:
   static const uint32 staticrcMap[ 256 ]; 

   /**
    * XXX: Copied from MC2, MC2CRC32, 20040622 10:30.
    * Calculates the CRC for a buffer.
    *
    * @param buf The buffer to calculate CRC for.
    * @param len The length of buf.
    * @param crc The start value for the crc, default 0.
    * @return The CRC for buf.
    */
   static uint32 calcCrc32( const uint8* buf, uint32 len, uint32 crc = 0 );

};
} /* namespace isab */
#endif


