#!/usr/bin/env python

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
import sys
from jinja2 import Environment,FileSystemLoader

class Generator:
    def __init__(self, output, doc):
        self.output = output
        self.doc = doc
        # Use the HTML templates
        self.env = Environment(loader=FileSystemLoader(["./doc_templates/html"]))
        self.globals = []
        self.global_params = {}

    def writeline(self, line):
        self.write(line)
        self.write("\n")

    def write(self, data):
        self.output.write(data)

    def handle_enum(self, enum):
        template = self.env.get_template("enum")
        self.write(template.render({"enum" : enum}))

    def handle_struct(self, struct):
        template = self.env.get_template("struct")
        self.write(template.render({"struct" : struct,
                                    "globals" : self.globals }))

    def handle_rpacket(self, packet):
        template = self.env.get_template("packet")
        self.write(template.render({"packet" : packet}))
        

    def generate(self):
        # setup params and global types to be referenced
        for param in doc.params.values():
            self.global_params[param.id] = param

        globaltypes = ( doc.enums.values(),
                        doc.common_enums,
                        doc.structs,
                        doc.common_structs
                        )
        for types in globaltypes:
            for subtypes in types:
                self.globals.append(subtypes.name)

        # Start rendering the documentation
        self.writeline(self.env.get_template("file_header").render())

        # first render some basic type info
        self.write(self.env.get_template("basic_types").render())
        # Ok, a bit HTML here, but this will change to a more generic way to
        # add titles
        # TODO: Change to a more generic way to insert titles
        self.writeline("<h1>Common enums</h1>")

        # now generate html for the types

        for enum in doc.enums.values():
            self.handle_enum(enum)

        for enum in doc.common_enums:
            self.handle_enum(enum)

        # Ok a bit HTML here, but will change to a more generic way
        # TODO: Change to a more generic way to insert titles
        self.writeline("<h1>Common structs</h1>")
        for struct in doc.structs:
            self.handle_struct(struct)

        for struct in doc.common_structs:
            self.handle_struct(struct)

        # TODO: see TODO above
        self.writeline("<h1>Packets</h1>")
        # Create packet files
        for packet in doc.requests.values():
            self.handle_rpacket(packet)

        for reply in doc.replies.values():
            self.handle_rpacket(reply)

        self.writeline(self.env.get_template("file_footer").render())
            
    

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print "Usage:",sys.argv[0],"input.xml output.html"
        sys.exit(0)

    input_xml = sys.argv[1]


    doc = parser_xml.parse(input_xml, "client_settings.xml")
    output_html = open(sys.argv[2], "w")
    if output_html is None:
        print "Failed to open:",sys.argv[2]
        sys.exit(1)
    
    gen = Generator(output_html, doc)
    gen.generate()
    
