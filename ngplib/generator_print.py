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

import parser_xml

def print_enum(enum):
    print "Enum: ",enum.name
    for value in enum.values:
        print value[0],"=",value[1]

def print_param(param):
    print "id:",param.id,"name:",param.name,"default:",param.default, \
        "description:",param.description

def print_member(member):
    print "Name:",member.name
    print "Type:",member.type
    print "Comment:",member.comment

def print_struct(struct):
    print "Name:",struct.name
    for member in struct.members:
        print_member(member)

def print_request(req):
    print "Name:",req.name,"version:",req.version
    for param in req.params.values():
        print_param(param)

    for struct in req.structs.values():
        print_struct(struct)

    for enum in req.enums.values():
        print_enum(enum)

def print_doc(doc):
    print "---- Global Params ----"
    for param in doc.params.values():
        print_param(param)

    print "---- Global Enums ----"
    for enum in doc.enums.values():
        print_enum(enum)

    print "---- Requests ----"
    for request in doc.requests.values():
        print_request(request)

    print "---- Replies ----"
    for reply in doc.replies.values():
        print_request(reply)


doc = parser_xml.parse("protocol.xml")
print_doc(doc)
