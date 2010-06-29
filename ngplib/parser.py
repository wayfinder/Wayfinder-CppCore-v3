#
#  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
#  All rights reserved.

#  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

#  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
#  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
# 

import pprint
 
import xml.dom.minidom
from xml.dom.minidom import Node
 
doc = xml.dom.minidom.parse("protocol.xml")
 
mapping = {}
 
root = doc.getElementsByTagName("serverFormat")[0]

def genReqPacketCpp(node):
    name     = node.getAttribute("name")
    version  = node.getAttribute("version")

    # print "class %s : public RequestPacket { " % name;
    # print "public:"
    # print "   %s(" %name

    # print "};"

    print "%s - v%s" % (name, version)

    for param in node.getElementsByTagName("param"):
        print "   %s - %s" % (param.getAttribute("id"), param.getAttribute("name"))


def genReplyPacketCpp(node):
    name     = node.getAttribute("name")
    version  = node.getAttribute("version")
    print "%s - v%s" % (name, version)

    for param in node.getElementsByTagName("param"):
        print "   %s - %s" % (param.getAttribute("id"), param.getAttribute("name"))
    

def visitPacket(node):
    packetType = node.localName


    if(packetType == "reqPacket"):
        genReqPacketCpp(node)
    elif(packetType == "replyPacket"):
        genReplyPacketCpp(node)
    else:
        print("Invalid packet!")
    

# visitPacket(root.getElementsByTagName("reqPacket")[0])    

for node in root.getElementsByTagName("reqPacket"):
    visitPacket(node)

for node in root.getElementsByTagName("replyPacket"):
    visitPacket(node)
    # print "Description: %s" % desc


# for node in doc.getElementsByTagName("serverFormat"):
#   name = node.getAttribute("name")
#   print "apa"
#   # L = node.getElementsByTagName("title")
#   # for node2 in L:
#   #   title = ""
#   #   for node3 in node2.childNodes:
#   #     if node3.nodeType == Node.TEXT_NODE:
#   #       title += node3.data
#   #   mapping[isbn] = title
 
# mapping now has the same value as in the SAX example:
# pprint.pprint(mapping)
