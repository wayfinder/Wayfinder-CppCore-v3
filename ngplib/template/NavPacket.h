/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef NAVPACKET_H
#define NAVPACKET_H

#include "NGPConfig.h"

#include "NParamBlock.h"

#include <vector>

//obfuscate the function name in the symbol table:
#define wf_ngp_encrypt(...) printStatus(__VA_ARGS__)

namespace wf_ngp {

/**
 * This function simply encrypts data, which is needed before sending to
 * the server.
 *
 * @param buff The buffer to encrypt. In a NavReplyPacket, this should be
 *             offset to the status_code, i.e. 10 from the start of the packet.
 *             In a NavRequestPacket, this should be the offset of the
 *             parameter block, i.e. also 10 from the start of the packet.
 * @param length How many bytes to encrypt. 
 */
void wf_ngp_encrypt(unsigned char* buff, uint32 length);

/**
 * Class that represents a Navigator packet.
 * 
 * @author Daniel Pettersson
 */
class NavPacket {
   public:

      /**
       * Constructor.
       *
       * @param protoVer The protocol version.
       * @param type The type of packet.
       * @param reqID The ID of the packet.
       * @param reqVer The version of request.
       */
      NavPacket( byte protoVer, uint16 type, byte reqID, byte reqVer );


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
      NavPacket( byte protoVer, uint16 type, byte reqID, byte reqVer,
                 const byte* buff, uint32 buffLen );


   /**
    * Virtual destructor
    */
   virtual ~NavPacket(){};

   /**
    * Serialize function, to be derived by subclasses.
    *
    * Implementors must generate NParams from their data and
    * add those NParams to block.
    */
   virtual void serialize(NParamBlock& block) const = 0;

   /**
    * Writes the packet to a buffer.
    *
    * Implementors must write packet header, their paramblock and packet trail
    * (e.g. crc) to buff.
    *
    * @param buff The buffer to write to.
    */
   virtual void writeTo( std::vector<byte>& buff ) const;

   /**
    * Writes the packet and extra params to a buffer.
    *
    * Implementors must write
    * - packet header,
    * - a paramblock consisting of the extraParams + their own paramblock
    * - packet trail (e.g. crc)
    * to buff.
    */
   virtual void writeTo( std::vector<byte>& buff,
                         const std::vector<NParam>& extraParams ) const;
   
      /**
       * Get the protoVer.
       */
      byte getProtoVer() const;


      /**
       * Get the type.
       */
      uint16 getType() const;


      /**
       * Get the reqID.
       */
      byte getReqID() const;


      /**
       * Get the reqVer.
       */
      byte getReqVer() const;


      /**
       * Get the parameter block.
       */
      NParamBlock& getParamBlock();


      /**
       * Get the parameter block.
       */
      const NParamBlock& getParamBlock() const;

      /**
       * Set the protoVer.
       */
      void setProtoVer( byte protoVer );

      /**
       * Sets the type.
       */
      void setType( uint16 type );


      /**
       * Sets the reqVer.
       */
      void setRequestVer( byte reqVer );

      /**
       *
       */
      void setReqID(byte reqId);

      /**
       * Get the ProtoVer used in this client.
       */
      static uint8 getCurrentProtoVer();
      
      /**
       * Compare equal.
       */
      bool operator == ( const NavPacket& rhs ) const {
          return
             m_protoVer == rhs.m_protoVer &&
             m_type == rhs.m_type &&
             m_reqID == rhs.m_reqID &&
             m_reqVer == rhs.m_reqVer &&
             m_params == rhs.m_params;
      }

   protected:
       /// The protocol version.
       byte m_protoVer;


       /// The type of packet.
       uint16 m_type;


       /// The ID of the packet.
       byte m_reqID;


       /// The version of type.
       byte m_reqVer;


       /// The parameter block.
       NParamBlock m_params;

      /**
       * Request and Reply sub classes must override this
       * if they need to write a specific part of the header
       * after req_ver. They must use vector::push_back() to add
       * their data.
       */
      virtual void writeSpecificHeaderPart( std::vector<byte>& buff )
         const {};

   
   private:
      void writeToInternal( std::vector<byte>& buff,
                            const NParamBlock& params ) const;
};

// =======================================================================
//                                     Implementation of inlined methods =

inline byte
NavPacket::getProtoVer() const {
   return m_protoVer;
}


inline uint16 
NavPacket::getType() const {
   return m_type;
}

inline void
NavPacket::setProtoVer( byte protoVer ) {
   m_protoVer = protoVer;
}

inline void
NavPacket::setType( uint16 type ) {
   m_type = type;
}


inline void
NavPacket::setRequestVer( byte reqVer ) {
   m_reqVer = reqVer;
}


inline void 
NavPacket::setReqID(byte reqId) {
   m_reqID = reqId;
}


inline byte 
NavPacket::getReqID() const {
   return m_reqID;
}


inline byte 
NavPacket::getReqVer() const {
   return m_reqVer;
}


inline NParamBlock& 
NavPacket::getParamBlock() {
   return m_params;
}


inline const NParamBlock& 
NavPacket::getParamBlock() const {
   return m_params;
}


} /* namespace wf_ngp */

#endif // NAVPACKET_H

