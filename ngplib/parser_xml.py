#!/bin/env python

#
#  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
#  All rights reserved.

#  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

#  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
#  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
# 

import libxml2
import request

class Node:
    """Interface against backend for XML parsing.
    This class uses libxml2.
    Implement the following function if another parser is needed:
    attribute, value, nodes
    """

    def __init__(self, node):
        """<<node>> is the implementation specific node.
        """
        self.node = node

    def attribute(self, name):
        """Returns value of attribute with <<name>>
        """
        value_node = self.node.xpathEval("@"+name); 
        if not value_node:
            return ""
        return value_node[0].content

    def value(self, name):
        """Returns value of node with <<name>>
        """
        value_node = self.node.xpathEval(name)
        if not value_node:
            return ""
        return value_node[0].content

    def nodes(self, name):
        """Return a list of subnodes if self.node called <<name>>
        """
        return [ Node( x ) for x in self.node.xpathEval(name) ]

def parse(filename, filename_settings):
    """Returns a list of parsed packets
    """
    document = request.Document()
    xml_doc = libxml2.parseFile(filename)

    settings_doc = libxml2.parseFile(filename_settings)

    # Parse allowed packets
    for packet_node in settings_doc.xpathEval("/allowedPackets/packet"):
        allowed_packet = request.AllowedPacket(Node(packet_node))
        document.allowed_packets[allowed_packet.name] = allowed_packet.version

    # parse global params
    for param_node in xml_doc.xpathEval("/serverFormat/globalParams/param"):
        param = request.Param(Node(param_node))
        document.params[ param.id ] = param

    # parse global server version
    for ver_node in xml_doc.xpathEval("/serverFormat/version"):
        ver = request.ServerVersion(Node(ver_node))
        document.server_version = ver

    # Global param structs, like version etc
    for struct_node in xml_doc.xpathEval("/serverFormat/globalParams/struct"):
        struct = request.Struct(Node(struct_node))
        document.structs.append( struct )

    # Common enums
    for enum_node in xml_doc.xpathEval("/serverFormat/globalEnums/enum"):
        enum = request.Enum(Node(enum_node))
        document.common_enums.append( enum )

    # Common structs
    for struct_node in xml_doc.xpathEval("/serverFormat/globalStructs/struct"):
        struct = request.Struct(Node(struct_node))
        document.common_structs.append( struct )
#        document.common_structs = {}

    # parse requests
    for request_node in xml_doc.xpathEval("/serverFormat/reqPacket"):
        req = request.Packet(Node(request_node))
        document.requests[ ( req.name, req.version ) ] = req

    # parse replies
    for reply_node in xml_doc.xpathEval("/serverFormat/replyPacket"):
        # Yes, we use request class as reply
        reply = request.Packet(Node(reply_node))
        document.replies[ ( reply.name, reply.version ) ] = reply

    # parser enums (in serverformat, skipping the other enums for now)
    for enum_node in xml_doc.xpathEval("/serverFormat/enum"):
        enum = request.Enum(Node(enum_node))
        document.enums[ enum.name ] = enum
     
    # XXX: Add also for replyPacketBase and requestPacketBase

    return document;
