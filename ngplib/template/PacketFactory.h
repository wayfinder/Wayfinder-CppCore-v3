#ifndef PACKETFACTORY_H
#define PACKETFACTORY_H

#include "Common.h"
#include "NavReplyPacket.h"
#include "NavRequestPacket.h"

namespace wf_ngp {

/**
 * Used by checkSTXAndSetPacketLength() to indicate the validity of the STX
 * header value
 */
enum STXStatus {
   /// header is at least HEADER_LENGTH bytes and the STX value is correct
   STX_VALID,
   /// header is less than HEADER_LENGTH bytes
   STX_INVALID_SHORT_HEADER,
   /// header is at least HEADER_LENGTH bytes but the STX value is invalid
   STX_INVALID_BAD_VALUE
};


/**
 * The amount of bytes to be read and passed into the
 * checkSTXAndSetPacketLength method.
 *
 * @see checkSTXAndSetPacketLength()
 */
static const uint32 HEADER_LENGTH = 5;


/**
 * Reads and checks the STX value from the header.
 *
 * If STX_VALID is returned, the parameter remainingBytes will
 * be set to the length of the remaining packet size.
 *
 * @param data The byte* containing the packet data, must be at least
 * HEADER_LENGTH bytes
 * @param size The length of the packet data in bytes, must be at least
 * HEADER_LENGTH bytes
 * @param remainingBytes Will be set to the amount of remaining bytes in the
 * packet
 * @return one of the STXStatus values
 */
STXStatus checkSTXAndSetPacketLength(const byte* data, uint32 size,
                                     uint32& remainingBytes);

/**
 *  Generates a NavRequestPacket based on the packet type.
 *
 *  This method expects that the passed data has the first HEADER_LENGTH bytes
 *  (containing the STX value and the packet length value) removed. This method
 *  also expects the data to be encrypted.
 *
 *  Warning! This method will modify the data in the byte* passed into the
 *  method when decrypting the data.
 *
 *  @param data The byte* containing the packet data
 *  @param size The length of the packet data
 *  @return A NavRequestPacket ptr or 0 if no suitable packet could be created
 */
NavRequestPacket* createRequestPacketFromBuffer(byte* data, uint32 size);


/**
 *  Generates a NavReplyPacket from raw data
 *
 *  This method expects that the passed data has the first HEADER_LENGTH bytes
 *  (containing the STX value and the packet length value) removed. This method
 *  also expects the data to be encrypted.
 *
 *  Warning! This method will modify the data in the byte* passed into the
 *  method when decrypting the data.
 *
 *  @param data The byte* containing the packet data
 *  @param size The length of the packet data
 *  @return A NavReplyPacket ptr or 0 if no suitable packet could be created
 */
NavReplyPacket* createReplyPacketFromBuffer(byte* data, uint32 size);


}


#endif /* PACKETFACTORY_H */
