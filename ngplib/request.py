#
#  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
#  All rights reserved.

#  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

#  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
#  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
# 

class ServerVersion:
    def __init__(self, node):
        self.version = node.attribute("value")

class DerivesFrom: 
    """Type used to represent the relationship between two packets"""
    def __init__(self, node):
        self.name = node.attribute("name")
        self.version = node.attribute("version")

class AllowedPacket:
    """Type used to represent a packet that is valid for use """
    
    def __init__(self, node):
        self.name = node.attribute("name")
        self.version = node.attribute("version")

class Enum:
    """Constant number representation
    The enum values are stored in a list of pairs.
    """
    def __init__(self, node):
        self.containing_struct = node.attribute("name")
        self.name = "enum_t"
        self.comment = node.attribute("comment")
        self.values = []
        for value_node in node.nodes("enumVal"):
            self.values.append( ( value_node.attribute("name"),
                                  value_node.attribute("value"),
                                  value_node.attribute("comment") ) )
class LabeledVariable:
    def __init__(self, type, data_type, name, shortname, comment, is_array):
        self.type = type
        if (type == ""):
           self.type = data_type
        self.data_type = data_type
        self.name = name
        self.shortname = shortname
        self.comment = comment
        self.is_array = is_array

class Member(LabeledVariable):
    """ A simple data type in Struct
    """
    def __init__(self, node):
        LabeledVariable.__init__(self, 
                                 node.attribute("enum"), 
                                 node.attribute("type"),
                                 node.attribute("name"), 
                                 node.attribute("name"),
                                 node.attribute("comment"),
                                 False )

        self.size = node.attribute("size")
        self.ref_type = node.attribute("refType")
        self.enum = ""
        plain_enum_name = node.attribute("enum")
        if (plain_enum_name != ""):
            self.enum = node.attribute("enum") + "::enum_t"
           
        self.comment = node.attribute("comment")

class Struct:
    """A complex data type from params.
    """
    def __init__(self, node):
        self.name = node.attribute("name")
        self.members = []
        self.enums = {}

        for member_node in node.nodes("member"):
            self.members.append( Member(member_node) )
        for enum_node in node.nodes("enum"):
            enum = Enum(enum_node)
            self.enums[ enum.name ] = enum


class ParamRef:
    """Holds information about a referenced (global) parameter)"""
    def __init__(self, node):
        self.id = node.attribute("id")

class Param(LabeledVariable):
    """Holds information about one request or reply parameter.
    """
    def __init__(self, node):
        LabeledVariable.__init__(self, 
                                 node.attribute("enum"),
                                 node.attribute("type"),
                                 node.attribute("name"), 
                                 node.attribute("shortname"),
                                 node.attribute("description"),
                                 node.attribute("arrayStride") != "")

        self.id = node.attribute("id")

        self.shortname = node.attribute("shortname")
        self.default = node.attribute("defaultVal")
        self.array_stride = node.attribute("arrayStride")
        self.array_end = node.attribute("arrayEnd")

class Packet:
    """Request consist of name, version, comment and a set
    of parameters.
    """
    def __init__(self, node):
        self.name = node.attribute("name")
        self.version = node.attribute("version")
        self.comment = node.value("comment")
        self.params = {}
        self.param_refs = {}
        self.enums = {}
        self.structs = {}
        self.derives = {}

        # for comment in node.nodes("comment"):
        #     self.comment += comment.value("")

        for param_derive in node.nodes("derivesFrom"):
            der = DerivesFrom(param_derive)
            self.derives[ (der.name, der.version) ] = der

        for param_node in node.nodes("paramRef"):
            param = ParamRef( param_node )
            self.param_refs[ param.id ] = param

        for param_node in node.nodes("param"):
            param = Param( param_node )
            self.params[ param.id ] = param

        for struct_node in node.nodes("struct"):
            struct = Struct(struct_node)
            self.structs[ struct.name ] = struct

        for enum_node in node.nodes("enum"):
            enum = Enum(enum_node)
            self.enums[ enum.containing_struct ] = enum

class PacketBase:
    """The base class for all packets.
    """
    def __init__(self, node):
        self.enums = {}
        for enum_node in node.nodes("enum"):
            enum = Enum(enum_node)
            self.enums[ enum.name ] = enum

class Document:
    """Holds all requests, replies, enums, and global params.
    """
    def __init__(self):
        self.requests = {}
        self.replies = {}
        self.params = {}
        self.enums = {} # This one should be removed later?
        # Global enums like things referenced by global structs
        self.common_enums = []
        # Global structs like version setting
        self.structs = []
        # Common structs like MC2Coordinate etc
        self.common_structs = []
        self.packet_base = None
        # Allowed packets in the document
        self.allowed_packets = {}
