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
import os
import tempfile
import copy_templates


def cast_to(what_to_cast, type_to_cast_to, original_type):
    """Return a string which performs c++ style casting
    If type_to_cast_to is empty, then what_to_cast will be returned
    """
    if type_to_cast_to != "" and type_to_cast_to != original_type:
        return type_to_cast_to + "(" + what_to_cast + ")"
    else:
        return what_to_cast

def transform_packet_name(name):
    """Transforms and return <<name>> from UPPER_CASE_NAME to UpperCaseName.
    """
    return ''.join([ x.capitalize() for x in name.split('_') ])

def transform_to_upper(type):
    """Transforms "varibleName" to "VariableName", often used to concatenate "get" etc 
       to "getVariableName"
    """
    return type[0].upper() + type[1:]

def transform_type_to_adder(type):
    """Transforms for instance uint8 to addUint8 """
    return "add" + transform_to_upper(type)

def transform_type_to_getter(type):
    """Transforms for instance uint8 to getUint8 """
    return "get" + transform_to_upper(type)

def transform_type_to_inc_getter(type):
    """Transforms for instance uint8 to incGetUint8 """
    ret = transform_type_to_getter(type)
    return "inc" + transform_to_upper(ret)

def strip_array_from_type(type):
    """Retrieves the element type of an array. For instance strip_array_from_type("uint8[]")  
       will return uint8. If the type is not an array, it will simply return the type itself.
    """
    index_of_array = type.rfind("[]")    
    if index_of_array != -1:
        return type[0:index_of_array]
    else:
        return type

def get_size_from_ref_or_const(name): 
    """Given a size attribute this function will attempt to resolve either a reference 
       to another variable or to a constant value. Input argument can either be in the 
       form of "@nbrItems" or "12". If in the first form, it refers to a previously 
       declared variable. In this case, the @ is stripped and the remainder of the string
       is returned. If in the second form, it is a numerical constant, and the value itself
       self (12) should be returned. """

    index_of_ref = name.rfind("@")    
    if index_of_ref != -1:
        return name[index_of_ref+1:]
    else:
        return name

def expand_to_pod(name, type):
    """Certain types are used to wrap primitive data types. When these complex types 
       are to be serialized into a NParam entry, they will have to be converted into
       the primitive form once again. For instance, the string class will need to 
       return a pointer to its internal character array via c_str()."""
    if type == "string":
        return name + ".c_str()"
    else:
        return name

class Generator:
    def __init__(self, path, code_type, copyright):
        self.indent = 0
        self.path = path
        self.code_type = code_type
        self.copyright = copyright
        self.one_indent_level = 3
        self.max_line_length = 79

        # Special types that needs to be converted, for some code types
        # type conversion is a dictionary which maps type to a tuple which
        # contains the real type and the headers that needs to be included.
        # The first object in the tuple is always the type
        if self.code_type == "MC2":
            self.type_conversion = { "string" : ( "MC2String", "MC2String.h" ) }
        else:
            self.type_conversion = { "string" : ( "std::string" , "<string>" ) }

        self.type_conversion["Coordinate"] = ("Coordinate",
                                              "Coordinate.h")
        # DEBUG print self.type_conversion
        self.global_include = "NGP.h"

        # Is set to the headers needed for each file,
        # Reset it after each file is done.
        # Note: We should actually use a set() here, but thats for Python 2.4
        # and above, and since my old crappy dist (centos 4) does not have that
        # ...
        self.need_headers = []

        # Plain Old Data types
        self.pod_types = ( "uint8", "int8", "uint32", "int32", "uint16", "int16",
                           "int", "char", "byte", "bool" )

        # Used to generate the getters for members via NParam objects. 
        self.simple_getters = [ x for x in self.pod_types ]
        self.simple_getters.append("string")

        self.global_params = dict()
        self.allowed_packets = dict()

        self.current_file = ""

        # Make sure we have '/' last in path
        if self.path[-1] != '/':
            self.path = self.path + '/'

        # Create the path if it does not exist
        # and ignore error if it does...or maybe not
        # we should catch permission denied errors...
        # TODO: do it.
        try:
            os.makedirs(self.path)
        except:
            pass

    def const_ref(self, typestring):
        """Returns a const reference of <<typestring>>, if its not a POD type.
        Use this to set value const reference.
        """

        # If POD type then ignore const reference as return type
        if typestring in self.pod_types:
            return typestring
        return "const " + typestring + "&"
    
    def indent_increase(self):
        self.indent += self.one_indent_level
        
    def indent_decrease(self):
        self.indent -= self.one_indent_level

    def add_include(self, include):
        """Adds <<include>> header to be used as #include filename
        when generating code.
        """

        # dont add empty includes
        if len(include) == 0:
            return

        # only add if unique include.
        # We should use a set() here instead
        if include not in self.need_headers:
            self.need_headers.append(include)

    def get_type(self, typestring, is_param_array):
        """Transform type to their real types. For example std::vector and MC2String.

           is_param_array denotes whether the typestring represents a type which is
           encapsulated in a so called NParam array. Instead of representing
           a single NParam, the typestring instead represents a collection of 
           objects, all having the same definition as the first NParam. The array
           is represented by an increasing series of NParam ids.

           So an NParam with ID 5600 could contain the first object in the array,
           and 5602 the second etc. To deal with this strange way of handling arrays,
           which operates on the parameter level, above regular arrays (uint16[] for 
           instance), we have introduced the is_param_array attribute. Whenever this 
           is True, we will add an enclosing vector.
        """

        # reverse find for brackets
        index_of_array = typestring.rfind("[]")
        
        prefix = ""
        postfix = ""

        if is_param_array:
            # Add enclosing vector for NParam arrays
            prefix = "std::vector< "
            postfix = " >"

        if index_of_array == -1:
            # Try "array"-string
            # reverse find for string " array"
            index_of_array = typestring.rfind(" array")

        if index_of_array != -1:
            self.add_include("<vector>")

            # if array was found, create an array and determine the content by
            # recursion of subtype
            return prefix + "std::vector< " + \
                self.get_type(typestring[0:index_of_array], False) + " >" + postfix

        # No array type, see if we have special conversion, else just
        # return the type as is.
        if typestring in self.type_conversion:
            # get real type and the headers needed.
            datatype = self.type_conversion[typestring]
            # Copy header data tuple and only if it does not
            # exist
            for include in datatype[1:]:
                self.add_include(include)

            # Add common include if this type is global type
            if datatype[0] in self.common_header_types:
                self.add_include("Common.h")

            return prefix + datatype[0] + postfix

        # Add common include if this type is global type
        if typestring in self.common_header_types:
            self.add_include("Common.h")

        return prefix + typestring + postfix

    def handle_enum(self, enum):
        """ Will output an enum declaration. The enum declarations are somewhat special.
            The enums are not stored as regular enums, but rather contained within a
            struct. This struct contains the enum declaration and one member variable 
            (m_t), which is of the same type as the enum. This solves the issue with
            enums polluting the namespace.

            Some clever templated constructor tricks then enables us to use this 
            struct as you would with the regular enum - you can typecast it properly,
            evaluate it against the enum constants but also do other stuff like forward
            declare it. 

            The only caveat is that you have to print m_t in gdb or whatever instead of 
            the variable itself.
        """

        # Add namespace to enum.
        self.write_line("struct " + enum.containing_struct + " {")
        self.indent += self.one_indent_level
        
        self.write_documentation(enum.comment)
        self.write_line("enum " + enum.name + " {")
        self.indent += self.one_indent_level
        for value in enum.values:
            if self.code_type == "MC2":
                # old version - don't know if MC2 actually want it this
                # way.
                comment = ""
                if value[2] != "":

                    # The comment field is not empty, so we add it to the end
                    # to the line.
                    comment = " ///< %s" % value[2]

                # Write the enum value with the possibly empty comment string.
                self.write_line(value[0].upper() + " = "
                                + value[1]
                                + "," + comment)
            else:
                if value[2] != "":
                    self.write_line("/// " + value[2])

                self.write_line(value[0].upper() + " = " + value[1]
                                + ",")

        self.indent -= self.one_indent_level
        self.write_line("};")
        self.write_line("")

        # Create template magic
        self.write_line("// This is for simple forward declares and casts of enums.")

        # First create default constructor
        self.write_line("%s():m_t(enum_t(0)) {}" % enum.containing_struct)

        # Then create template constructor which will allow us to cast any
        # POD variable which supports the conversion into the enum. I.e. uint8,
        # uint16 etc
        self.write_line("template <typename T>")
        self.write_line("inline %s( T t ):m_t(enum_t(t)) {}" % enum.containing_struct)
        # Allow the struct enum to be treated like the primitive enum value
        self.write_line("operator enum_t() const { return m_t; }")
        self.write_line("enum_t m_t;")

        self.indent -= self.one_indent_level
        self.write_line("}; // struct " + enum.containing_struct)
        self.write_line("")
        
    def is_allowed_packet(self, packetName, packetVersion):
        # See if the packet matches any of the allowed packets
        return packetName in self.allowed_packets and \
               packetVersion == self.allowed_packets[packetName]
        
    def write_custom_enum(self, enumName, doc, valueList):    
        """ Outputs a simple enum declaration.

        """
        
        self.write_documentation(doc)
        self.write_line("enum " + enumName + " {")
        self.indent += self.one_indent_level
        for value in valueList:
            # Write the enum value
            self.write_line(value + ",")

        self.indent -= self.one_indent_level
        self.write_line("};")
        self.write_line("")
        
        
        
    
    def write_object_getters(self, objects):
        """Constructs code for parameters getters.
        For example:
        MC2String getSomeValue() { return m_someValue; }
        """

        if len(objects) > 0:
            self.write_line("// Object getters")
            
        for obj in objects:
            self.write_line("/// @return %s" % obj.name)
            implementation = " { return m_%s; }" % obj.shortname
            getter_name = "get" + obj.shortname[0].upper() + obj.shortname[1:] + "() const"
            self.write_line("%s %s" %
                            (self.const_ref(self.get_type(obj.type, 
                                                          obj.is_array)),
                             getter_name + implementation))

            self.write_line("")

    def write_member_serialization(self, member, is_struct):
        """
        This method will write a statement or a block which completely
        serializes a labeled variable onto a NParam. It takes the special
        NParam array situation into account (see get_type for further explanation).

        is_struct denotes whether or not the entity that contains the member
        is a packet class or a struct.
        """

        if is_struct:
            # If we are inside a struct, we already have access to the
            # target NParam. It is passed as an argument to its serialize
            # function.
            param = "param"
            varname = member.shortname
        else:
            # If we are instead inside of a class, we must create a new NParam
            # from the NParamBlock which is passed as an argument to its serialize
            # function. Thus the variable name is 
            param  = "tmp%s" % transform_to_upper(member.shortname)
            varname = "m_%s" % member.shortname

        if member.is_array:
            # If we are dealing with an NParam array, we need to add one extra level
            # of array indexing and an enclosing for loop. 
            array_ind = "[i]"
            self.write_line("")
            self.write_line("for(unsigned int i = 0; i < %s.size(); i++) {" % varname)
            self.indent += self.one_indent_level

            # The NParam id is no longer constant, but it depends upon the
            # enclosing for loop.
            array_inc = " + i * %s" % member.array_stride
        else:
            # If not, we can use a single for loop and a single index into the array.
            array_ind = ""
            array_inc = ""
            

        if not is_struct:
            # First write the declaration which adds a new parameter to the
            # parent NParamBlock if we are writing for a class.
            self.write_line("NParam& %s = block.addParam(NParam(%s%s));" % (param, 
                                                                            member.id,
                                                                            array_inc))

        # Find out if we are dealing with arrays, and also find out the array
        # element type
        index_of_array = member.type.rfind("[]")
        stripped_data_type = strip_array_from_type(member.data_type)

        if index_of_array != -1:
            # If we are dealing with an arrays, add for loop block
            self.write_line("")
            self.write_line("for(unsigned int j = 0; j < %s%s.size(); j++) {" % 
                            (varname, array_ind))
            self.indent += self.one_indent_level

            if stripped_data_type in self.simple_getters:
                # If the type is a type which can be directly added to the 
                # NParam block, do so.
                self.write_line("%s.%s(%s%s[j]);" % 
                                (param, 
                                 transform_type_to_adder(stripped_data_type),
                                 varname, array_ind))
            else:
                # If not, invoke the serialization function of the complex object.
                self.write_line("%s%s[j].serialize(%s);" % (varname, 
                                                            array_ind,
                                                            param))

            self.indent -= self.one_indent_level
            self.write_line("}")
        elif member.data_type in self.simple_getters:
            # If we are not dealing with an array and the type is a simple 
            # object which can be directly added to the NParam block, do so here.
            self.write_line("%s.%s(%s);" % 
                            (param,
                             transform_type_to_adder(member.data_type),
                             expand_to_pod(varname, member.data_type)))
        else:
            # If the object is non-array and of a complex type, invoke its
            # serialize function.
            self.write_line("%s.serialize(%s);" % (varname, param))

        # If we are dealing with NParam arrays, close the enclosing for loop
        # block.
        if member.is_array:
            self.indent -= self.one_indent_level
            self.write_line("}")

    def handle_struct_constructor_member_assignment(self, member):
        """
        Handles an assignment which takes place in a struct constructor.
        """
        index_of_array = member.type.rfind("[]")

        if index_of_array != -1:
            # If we are dealing with array type, create for loop which extracts 
            # the individual values and inserts them into the member vector

            arr_size = get_size_from_ref_or_const(member.size)
            self.write_line("")
            self.write_line("// Load vector")
            self.write_line("%s.reserve(%s);" % (member.name, arr_size))
            self.write_line("for(unsigned int i = 0; i < %s; i++) {" % 
                            (arr_size))
            self.indent += self.one_indent_level

            stripped_type = strip_array_from_type(member.type)
            stripped_data_type = strip_array_from_type(member.data_type)
            
            if stripped_data_type in self.simple_getters:
                # Not the distinction between the type and the data type: 
                # the data type contains the primitive type behind the enum or 
                # typedef.

                # We only need to reference that when we are doing
                # serialization.

                assignment_data = "param->%s(index)" % (transform_type_to_inc_getter(stripped_data_type))

                # Create temporary variable by fetching directly from NParam
                self.write_line("%s tmp = %s;" %
                                (self.get_type(stripped_type, False),
                                 assignment_data))
            else:
                # Create temporary variable by invoking the complex constructor 
                # with the NParam as argument
                self.write_line("%s tmp(%s(param, index));" 
                                % (stripped_type, stripped_type))

            # Push back the temporary variable unto the vector
            self.write_line("%s.push_back(tmp);" % (member.name))

            # Close the for loop scope
            self.indent -= self.one_indent_level
            self.write_line("}")
        elif member.data_type in self.simple_getters:
            # If we are dealing with simple type, just fetch directly from NParam
            assignment_data = "param->%s(index)" % transform_type_to_inc_getter(member.data_type)
            self.write_line("%s = %s;" % (member.name, cast_to(assignment_data, 
                                                               member.type,
                                                               member.data_type)))
        else:
            # Otherwise, go into another struct constructor which will handle
            # the parsing
            self.write_line("%s = %s(param, index);" % (member.name, member.type))

    def write_documentation(self, doc):
        """Write documentation lines.
        """

        # TODO:
        # split each line at first space before
        # self.max_line_length
        doc_comment = "/// "
        for comment_line in doc.split('\n'):
            self.write_line(doc_comment + comment_line)

    def write_includes(self):
        """Write code for headers that are needed for the current file.
        """
        
        # Add global includes.
        self.write_line("#include " + "\"" + self.global_include + "\"")

        # Sort in reverse order, so we get system includes last.
        # The system includes should always be included last, so we
        # can detect if there are any missing include in local includes.
        self.need_headers.sort()
        # Again, reverse=True can work in sort() but not in old Python <= 2.3
        # So we do this:
        self.need_headers.reverse()

        for include in self.need_headers:
            # if include begins and ends with brackets then
            # its a system include and should not have any
            # quotes
            if include[0] == '<' and include[-1] == '>':
                self.write_line("#include " + include)
            else:
                self.write_line('#include "' + include + '"')


    def write(self, string):
        """
        Write a string to the current file without any indentation.
        """
        self.current_file.write(string)

    def write_line(self, line):
        """
        Write a line to the current file
        """

        # Write indentation first
        [ self.write(" ") for space in range(self.indent) ]
        self.write(line + "\n")
        
    def insert_empty_lines(self, nbrOfEmptyLines):
        """
        Insert a number of empty lines into the file
        """
        [ self.write("\n") for count in range(nbrOfEmptyLines - 1) ]    
        

    def write_struct_constructor(self, name, members):
        # Create NParam constructor
        self.write_line("/**")
        self.write_line(" *  %s NParam constructor" % (name))
        self.write_line(" *  @param param The parameter which contains the data for the members.")
        self.write_line(" *  @param index The current index for the parameter.")
        self.write_line(" */")
        self.write_line(("%s(const NParam* param, " +
                         self.get_type("uint32", False)+"& index) {") %
                        (name)) 
        self.indent += self.one_indent_level
        for member in members:
            self.handle_struct_constructor_member_assignment(member)
        self.indent -= self.one_indent_level
        self.write_line("}")
        self.write_line("")

    def handle_struct(self, struct):
        """This function will write the entire body of a struct type.
        """

        # Open the struct declaration
        self.write_line("struct " + struct.name + " {")
        self.indent += self.one_indent_level

        self.write_line("")

        # Write out all the enum declarations that are contained within the struct
        for enum in struct.enums.values():
            self.handle_enum(enum)
        
        # Write the different constructors for the struct
        self.write_struct_constructor(struct.name, struct.members)
        self.write_object_constructor(struct.name, struct.members, True, "")
        self.write_default_constructor(struct.name, struct.members, True, "")

        # Write equal compare
        self.write_equal_compare(struct.name, struct.members, True, None)
        
        # Write the serialization function for the struct
        self.write_serialize_function(struct.name, struct.members, True)

        # Write all of our member variables
        self.write_member_vars(struct.members, True)

        # Close the struct declaration
        self.indent -= self.one_indent_level

        self.write_line("};")
        self.write_line("")


    def write_header(self, name):
        """Write header data to current file
        """
        self.write(self.copyright)
        ifdef = name.upper() + "_H"
        self.write_line("#ifndef " + ifdef)
        self.write_line("#define " + ifdef)

    def write_footer(self, name):
        """Write foot data to current file.
        """

        ifdef = name.upper() + "_H"
        self.write_line("#endif // " + ifdef)

    def write_object_constructor(self, entityName, objects, is_struct, super_constructor):
        """
        Writes a constructor body for a struct or packet class. This constructor
        takes all of its member variables as arguments and it makes it possible to
        create an object without an NParamBlock source.
        """

        # Write the comment for the constructor
        self.write_line("/**")
        self.write_line(" * %s object constructor." % entityName)
        self.write_line(" * ")
        for obj in objects:
            # TODO: Fix so that complete comment is shown
            self.write_line(" * @param %s %s..." % (obj.shortname,
                                                    obj.comment[0:40]))
        self.write_line(" */")
        
        # If we don't have any objects, write super constructor declaration on
        # single line
        if len(objects) == 0:
            self.write_line("%s() :" % entityName)
        else:
            self.write_line("%s(" % entityName)

        self.indent += self.one_indent_level

        # Create argument list, prefix all arguments with "arg" string
        for obj in objects:
            delim = ") : "
            if obj != objects[-1]:
                delim = ","
                
            self.write_line("%s arg%s%s" %
                            (self.const_ref(self.get_type(obj.type,
                                                          obj.is_array)),
                             transform_to_upper(obj.shortname), delim))

            
        # Write call to super class constructor    
        if not is_struct:
            if len(objects) == 0:
                self.write_line("%s" % super_constructor)
            else:
                self.write_line("%s," % super_constructor)

        # Write constructor initialization list
        for obj in objects:
            delim = ""
            # Add commas for all objects but the last 
            if obj != objects[-1]:
                delim = ","
            var_name = obj.shortname
            
            # Packet classes have "m_" prefix
            if not is_struct:
                var_name = "m_%s" % obj.shortname

            self.write_line("%s(arg%s)%s" % (var_name,
                                             transform_to_upper(obj.shortname), delim))

        # Close constructor body
        self.indent -= self.one_indent_level
        self.write_line("{}")
        self.write_line("")

    def write_default_constructor(self, entityName, objects, is_struct, super_constructor):
        """
        Writes the body for a default constructor function. Works for both packets which
        are classes and regular structs.
        """

        # Write comment body for default constructor
        self.write_line("/**")
        self.write_line(" *  %s default constructor" % (entityName))
        self.write_line(" *  ")
        for obj in objects:
            self.write_line(" *  @param %s %s.." % (obj.shortname, obj.comment[0:40]))
        self.write_line(" */")

        # Open the constructor body
        self.write_line("%s() : " % entityName)

        # Write out invocation of super class constructor
        if not is_struct:
            self.write_line("%s," % super_constructor)

         # Write initialization list, all calls are of the form m_var()
        self.indent += self.one_indent_level
        for obj in objects:
            delim = ""
            if obj != objects[-1]:
                delim = ","

            if not is_struct:
                var_name = "m_%s" % obj.shortname
            else:
                var_name = obj.shortname

            self.write_line("%s()%s" % (var_name, delim))

        # Close the constructor body
        self.indent -= self.one_indent_level
        self.write_line("{}")
        self.write_line("")

    def write_equal_compare(self, entity_name, objects, is_struct,
                            super_constructor):
        """ Write equal compare operator:
        bool operator == ( const entity_name& rhs ) const
        """

        if len(objects) == 0:
           return

        self.write_line("/**")
        self.write_line(" * Compare equals.")
        self.write_line(" */")
        self.write_line("bool operator == (const %s& rhs) const {" %
                        ( entity_name ) )
        self.indent += self.one_indent_level

        # Determine prefix for object variables
        if not is_struct:
            prefix = "m_"
        else:
            prefix = ""

        self.write_line("return")

        self.indent += self.one_indent_level;

        if not is_struct and super_constructor != None:
            self.write_line(super_constructor.split("(")[0] +
                            "::operator == (rhs) &&");

        compare_str = "%s == rhs.%s"
        # Write all equal compare except the last one, which should end with ";"
        for obj in objects[:-1]:
            self.write_line((compare_str + " &&") %
                            (prefix + obj.shortname, prefix + obj.shortname))

        if len(objects) > 0:
            self.write_line((compare_str + ";") %
                            (prefix + objects[-1].shortname,
                             prefix + objects[-1].shortname))

        self.indent -= self.one_indent_level
        self.indent -= self.one_indent_level
        self.write_line("}")
        self.write_line("")

    def write_serialize_function(self, entityName, objects, is_struct):
        """
        Write a serialization function which will serialize all members of 
        a given struct or class
        """

        # Write opening commment
        self.write_line("/**")
        self.write_line(" *  %s serialization function" % (entityName))
        self.write_line(" *  @param param The param that will be filled with data.")
        self.write_line(" */ ")

        # Write function declaration
        if is_struct:
            # Structs are delegated a parameter to read from via a class or another struct
            self.write_line("void serialize(NParam& param) const {" )
            self.indent += self.one_indent_level
        else:
            # Classes get the parameter block and are the first stop of any serialization
            self.write_line("virtual void serialize(NParamBlock& block) const {")
            self.indent += self.one_indent_level

        # Serialize each individual object
        for obj in objects:
            # if is_struct:
            self.write_member_serialization(obj, is_struct)
        
        # Close function scope    
        self.indent -= self.one_indent_level
        self.write_line("}")
        self.write_line("")

    def write_npblock_constructor_member_assignment(self, param):
        """
        This method writes the assignment of a single member variable
        of a parameter class. It is quite complex since it needs to 
        deal with the 
        """
        index_of_array = param.type.rfind("[]")

        stripped_type = strip_array_from_type(param.type)
        stripped_data_type = strip_array_from_type(param.data_type)

        # If we are dealing with a parameter array, we need to do an enclosing
        # for loop that increments the NParam id. We also need a temporary
        # variable that will be pushed into the vector of the member variable.
        if param.is_array:
            # Name the temporary variable that will be used to push_back items
            # into the outermost vector
            root_varname = "arrayTmp" 

            # Write for block
            self.write_line("for(int i = %s; i < %s; i+=%s) { /* Array stride block */ " % 
                            (param.id, param.array_end, param.array_stride))
            self.indent += self.one_indent_level
            self.write_line("")

            # Note that we are adding elements to the param vector. Thus we
            # need the type of the element, not the type of the vector itself.
            # That is what we would have gotten if we had forwarded param.is_array
            # to the get_type function, therefore we unconditionally pass False
            # to it.
            self.write_line("%s arrayTmp;" % self.get_type(param.type, False))
            self.write_line("")
            # We want to keep the structure of the internal assignment, and
            # thus we do the end condition checking here
            self.write_line("if(m_params.getParam(i) == NULL) {")
            self.indent += self.one_indent_level
            self.write_line("break;")
            self.indent -= self.one_indent_level
            self.write_line("}")
            offset = "i"
        else:
            # Otherwise, we read data directly into the variable
            root_varname = "m_%s" % param.shortname;
            offset = param.id

        if index_of_array != -1:
            # The param contains entirely of an array. Loop until there is no
            # more data, no need to predetermine size etc.
            
            # TODO: This can be made more efficient for certain types -
            # i.e. memcpy'ing the internal byte buffer directly.
            
            self.write_line("")
            self.write_line("{ /* Array block */ ")
            self.indent += self.one_indent_level
            # Retrieve the source NParam
            self.write_line("const NParam* param = m_params.getParam(%s);" % (offset))
            self.write_line("")
            self.write_line(self.get_type("uint32", False) + " arrIndex = 0;")
            self.write_line("")
            # Reserve some space for the vector
            self.write_line("%s.reserve(param->getLength() / sizeof(%s));" % 
                            (root_varname, stripped_type))
            self.write_line("")
            self.write_line("while(arrIndex < param->getLength()) {")
            self.indent += self.one_indent_level
            
            # Are we dealing with a simple type?
            if stripped_data_type in self.simple_getters:
                # If so, just read directly from param
                self.write_line("%s tmp = param->%s(arrIndex);" % 
                                (stripped_type, 
                                 cast_to(transform_type_to_getter(stripped_data_type), 
                                         stripped_type,
                                         stripped_data_type)))
            else:
                # Otherwise, call on struct constructor which will deal with
                # the initialization
                self.write_line("%s tmp(param, arrIndex);" 
                                % (stripped_type))

            # Close scope
            self.write_line("%s.push_back(tmp);" % root_varname)
            self.indent -= self.one_indent_level
            self.write_line("}");
            self.indent -= self.one_indent_level
            self.write_line("}")
            self.write_line("")

        elif param.data_type in self.simple_getters:
            # If we are dealing with simple type, just read directly from param
            assignment_data = "m_params.getParam(%s)->%s()" % (param.id,
                                                            transform_type_to_getter(param.data_type))
            # Check if getParam returns NULL before using the pointer.
            # TODO: check if this is a performance issue - getting the param
            #       twice.
            self.write_line("if(m_params.getParam(%s) != NULL) {" % param.id)
            self.indent += self.one_indent_level
            self.write_line("%s = %s;" % (root_varname, cast_to(assignment_data, 
                                                               param.type,
                                                               param.data_type)))
            self.indent -= self.one_indent_level
            self.write_line("}")

        else:
            # Otherwise, call upon the struct constructor to do the dirt work
            self.write_line("%s = %s(m_params.getParam(%s), index);" % (root_varname, 
                                                                     param.type, 
                                                                     param.id))

        # If dealing with NParam array, close scope
        if param.is_array:
            self.write_line("m_%s.push_back(%s);" % (param.shortname, root_varname))
            self.indent -= self.one_indent_level
            self.write_line("}")


    def write_buf_packet_constructor(self, packet, is_packet):
        """
        This method creates the constructor body for a packet. The constructor takes
        two arguments - a const byte* buff and a uint32 bufflen. 

        TODO: extend with status code and status message for reply packets?
        """
        name = transform_packet_name(packet.name)

        """ 
        The super constructor will handle all of the protocol specifics
        for the reply packets and the request packets.
        """
        if is_packet:
            super_constructor = "NavRequestPacket(buff, buffLen)"
        else:
            super_constructor = "NavReplyPacket(buff, buffLen)"
            
        # Write initial comment
        self.write_line("")
        self.write_line("/**")
        self.write_line(" * Buffer constructor.")
        self.write_line(" * ")
        self.write_line(" * @param buff The buffer containing the data for the")
        self.write_line(" *             packet, assuming STX and length is stripped.")
        self.write_line(" * @param buffLen The length of the buffer in bytes.")
        self.write_line(" */")
        
        # Write constructor declaration
        self.write_line(name + "(const byte* buff, uint32 buffLen)")

        # Write the super constructor invocation
        if is_packet:
            self.write_line(" : %s " % super_constructor)
        else:
            self.write_line(" : %s " % super_constructor)

        # Start writing the main constructor body     
        self.write_line("{")
        self.indent += self.one_indent_level

        # Index is used for the incremental adding functions in NParam
        self.write_line(self.get_type("uint32", False) + " index = 0;")

        # Create assignment statements for all the members of the block
        for param in packet.params.values():
            self.write_npblock_constructor_member_assignment(param)

        # For some packets, the index winds up never being used. It is not
        # sufficient to check if packet.params.values() is empty in the beginning
        # since some types do not use the incremental get* functions. To
        # avoid this warning, we simply reference the variable at the end.   
        self.write_line("index = 0; // To avoid unused variable warning")

        # Close the constructor body
        self.indent -= self.one_indent_level
        self.write_line("}")
        self.write_line("")
        
    def write_method_start(self, returnValue, methodName, parameterList):
        """ Writes the method definition
        """
        self.write_line(returnValue)
        
        # create string with all parameters
        parameterStr = "";
        for parameter in parameterList:
            if len(parameterStr) > 0:
                parameterStr += ", "
            parameterStr += parameter[0] + " " + parameter[1]
        
        self.write_line(methodName + "("+ parameterStr + ")")
        self.write_line("{")

    def write_member_vars(self, objects, is_struct):
        """Writes the member variable declarations for a packet class. 
           I.e. "string m_packetString;" with comment on top
        """        

        for obj in objects:
            self.write_documentation(obj.comment)

            typename = self.get_type(obj.type, obj.is_array)

            # Class variables are prefixed with m_
            if is_struct:
                line = typename + " " + obj.shortname + ";"
            else:
                line = typename + " m_" + obj.shortname + ";"

            self.write_line(line)

    def handle_rpacket(self, packet, is_packet):
        """Creates a packet/reply file containing packet/reply class.
        """
        
        name = transform_packet_name(packet.name)

        # See if the packet matches any of the allowed packets
        if not packet.name in self.allowed_packets:
            print "* New packet available: %s" % packet.name
            return
        elif packet.version != self.allowed_packets[packet.name]:
            if int(packet.version) > int(self.allowed_packets[packet.name]):
                print "* Newer version of %s available (%d)" % (packet.name, 
                                                                int(packet.version))
            return
        
        name = transform_packet_name(packet.name)
        self.need_headers = []
        self.indent = 0

        # Start writing struct/class data to temporary file
        # which we then use to construct real file later
        self.current_file = tempfile.TemporaryFile()

        self.need_headers.append("Common.h")

        # Include correct super class
        if is_packet:
            self.need_headers.append("NavRequestPacket.h")
        else:
            self.need_headers.append("NavReplyPacket.h")

        self.write_line("namespace wf_ngp {")
        self.write_line("");

        # Write main class comment
        self.write_line("/**")
        self.write_line(" *  %s " % (packet.name))
        self.write_line(" *")

        # multi-line comments can look weird due to
        # preserved indenting from protocol.xml.
        # Try to fix it ...
        # TODO: re-format lines to max 79 chars (or maybe
        # that should be already in protocol.xml?)
        commentlines = str.strip(packet.comment)
        for line in commentlines.split("\n"):
            self.write_line(" *    %s" % str.strip(line))

        self.write_line(" */")

        # Write the class declaration
        self.write("class " + name + " : public ") 

        # Determine what we need to inherit from
        if is_packet: 
            self.write_line("NavRequestPacket {")
        else:
            self.write_line("NavReplyPacket {")

        # Open the public scope
        self.write_line("public:")
        self.indent += self.one_indent_level


        # Start by declaring all the enum values of the packet
        for enum in packet.enums.values():
            self.handle_enum(enum)

        # Then declare the internal structs
        for struct in packet.structs.values():
            self.handle_struct(struct)

        # Create super constructor string    
        super_constructor = ""
        if is_packet:
            super_constructor = \
                """NavRequestPacket(NGP_PROTOCOL_VERSION, PacketTypeEnum::%s, 0, %s)""" \
                % (packet.name, packet.version)
        else:
            super_constructor = \
                """NavReplyPacket(NGP_PROTOCOL_VERSION, PacketTypeEnum::%s, 0, %s)""" \
                % (packet.name, packet.version)
            
        # Write the constructor that takes a buf and buflength as input.
        self.write_buf_packet_constructor(packet, is_packet)

        # Write virtual destructor    
        self.write_line("virtual ~%s() {}" % name)
        self.write_line("")
        
        # Write object constructor
        self.write_object_constructor(name, packet.params.values(), False, super_constructor)

        # Only write default constructor if we have more than one value,
        # otherwise the object constructor and default constructor will be
        # exactly the same
        if(len(packet.params.values()) > 0):
            self.write_default_constructor(name, packet.params.values(), False, super_constructor)

        # Write equal compare
        self.write_equal_compare(name, packet.params.values(), False,
                                 super_constructor)

        # Write our serialization function    
        self.write_serialize_function(name, packet.params.values(), False)         
        
        # Write getters for all of our objects
        self.write_object_getters(packet.params.values())
        
        # Create the private section of the package
        old_indent = self.indent
        self.indent = 0
        self.write_line("private:")
        self.indent = old_indent
        
        # Write our member variable declarations
        self.write_member_vars(packet.params.values(), False)

        self.indent -= self.one_indent_level
        # end class
        self.write_line("};\n")

        # close namespace
        self.write_line("} // namespace wf_ngp")

        self.finalize_header_file(name)

    def finalize_header_file(self, name):
        """Write the final file, since the self.current_file at this point
        should have been a temporary file.
        This is done so include files can be written at the right position.
        """

        # Now write header, include files, req/reply data, and footer
        # to the real file
        temp_file = self.current_file

        self.current_file = open(self.path + name + ".h", 'w')

        self.write_header(name)
        self.write_line("")
        self.write_includes()
        self.write_line("")

        # copy packet file data
        temp_file.seek(0)
        self.write(temp_file.read())
        self.write_footer(name)

        self.current_file.close()
        temp_file.close()
    
    def finalize_cpp_file(self, name):
        """Write the final file, since the self.current_file at this point
        should have been a temporary file.
        This is done so include files can be written at the right position.
        """

        # Now write header, include files, req/reply data, and footer
        # to the real file
        temp_file = self.current_file

        self.current_file = open(self.path + name + ".cpp", 'w')

        self.write(self.copyright)
        self.write_line("")
        self.write_includes()
        self.write_line("")

        # copy packet file data
        temp_file.seek(0)
        self.write(temp_file.read())

        self.current_file.close()
        temp_file.close()
        
        

    def merge_packet(self, packet, other_packets):

        # Merges packets that are based on older versions
        while len(packet.derives) > 0:
            for key, der in packet.derives.items():
                other_packet = other_packets[( der.name, der.version )]                

                # print("%s-v%s derives from %s-v%s" % (packet.name, 
                #                                       packet.version,
                #                                       other_packet.name, 
                #                                       other_packet.version))

                packet.params.update(other_packet.params)
                packet.enums.update(other_packet.enums)
                packet.structs.update(other_packet.structs)
                packet.derives.update(other_packet.derives)
                packet.param_refs.update(other_packet.param_refs)
                packet.comment = other_packet.comment + "\n\n" + packet.comment
                packet.derives.pop(key)

        for param_ref in packet.param_refs.values():
            real_param = self.global_params[param_ref.id]
            # print "Resolving global parameter reference %s (%s)" % (real_param.id, 
            #                                                         real_param.name)
            packet.params[real_param.id] = real_param;
        
    def handle_server_version(self, ver):
        self.write_line("static const int NGP_PROTOCOL_VERSION = %s;" % ver.version)
        self.write_line("")

    def handle_doc(self, doc):
        """Handles the main documenten node and creates files for each
        subnode.
        """

        # Setup global parameters and structs.
        name = "Common"

        # Start writing struct/class data to temporary file
        # which we then use to construct real file later
        self.current_file = tempfile.TemporaryFile()

        # open the namespace for the common types.
        # we can not do this per enum/struct as the structs may contain
        # enums and we can't have namespaces in structs of course.
        self.write_line("namespace wf_ngp {")

        self.handle_server_version(doc.server_version)

        self.allowed_packets = doc.allowed_packets
        
        for param in doc.params.values():
            self.global_params[param.id] = param

        self.common_header_types = []
        # Generate enum and common struct, and add their
        # names to the common_header_types so we can determine
        # whether to include Common.h in reply/request header
        # files
        for enum in doc.enums.values():
            self.common_header_types.append(enum.name)
            self.handle_enum(enum)

        for enum in doc.common_enums:
            self.common_header_types.append(enum.name)
            self.handle_enum(enum)

        for struct in doc.structs:
            self.common_header_types.append(struct.name)
            self.handle_struct(struct)

        for struct in doc.common_structs:
            self.common_header_types.append(struct.name)
            self.handle_struct(struct)

        # close the namespace for common types
        self.write_line("} // namespace")

        self.finalize_header_file(name)

        # Merge packets so that derived packets get all necessary values
        for packet in doc.requests.values():
            self.merge_packet(packet, doc.requests)

        for reply in doc.replies.values():
            self.merge_packet(reply, doc.replies)

        # Create packet files
        for packet in doc.requests.values():
            self.handle_rpacket(packet, True)

        for reply in doc.replies.values():
            self.handle_rpacket(reply, False)
            
    
    #### *********   FACTORY   *********
    
    
    # Writes the 'PacketFactory.cpp' file        
    def writeFactoryCppFile(self, doc):            
        """Writes the factory class which can generate correct packets
        based on the type
        """
        
        # Start writing struct/class data to temporary file
        # which we then use to construct real file later
        self.current_file = tempfile.TemporaryFile()
        
        # add includes we know we need
        self.add_include("Buffer.h")
        self.add_include("PacketFactory.h")
        
        ## ***** write namespace, enum etc  *******
        self.write_line("namespace wf_ngp {")
        
        ## ***  write the methods  ***
        
        self.insert_empty_lines(2)
        self.write_STXCheckMethod(doc)
        
        self.insert_empty_lines(2)
        self.write_createRequestPacketFromBuffer_method(doc)
        
        self.insert_empty_lines(2)
        self.write_createReplyPacketFromBuffer_method(doc)
            
        ## **** ENDING OF FILE ****    
            
        # close namespace
        self.write_line("} // end namespace wf_ngp")    
        
        # write actual file
        self.finalize_cpp_file("PacketFactory")
            
    def write_STXCheckMethod(self, doc):
        self.write_method_start("STXStatus", 
                                "checkSTXAndSetPacketLength", 
                                [ ["const byte*", "data" ], 
                                  ["uint32","size"],
                                  ["uint32&", "packLen"] ]);
                                
        self.indent_increase()                        
        self.write_line("if(size < HEADER_LENGTH) {")
        self.indent_increase()
        self.write_line("return STX_INVALID_SHORT_HEADER;")
        self.indent_decrease()
        self.write_line("}")

        self.write_line("NonOwningBuffer buff( data, size );")
        self.write_line("if(buff.readNext8bit() == 0x02) {")
        self.indent_increase()
        self.write_line("//Set the remaining bytes")
        self.write_line("packLen = buff.readNextUnaligned32bit() - HEADER_LENGTH;")
        self.write_line("return STX_VALID;")
        self.indent_decrease()
        self.write_line("} else {")
        self.indent_increase()
        self.write_line("packLen = 0;")
        self.write_line("return STX_INVALID_BAD_VALUE;")
        self.indent_decrease()
        self.write_line("}")
                                
        # Close function scope    
        self.indent_decrease()
        self.write_line("}")
        self.write_line("")
                                
                                

    def write_createReplyPacketFromBuffer_method(self, doc):
        self.write_method_start("NavReplyPacket*", 
                                "createReplyPacketFromBuffer", 
                                [ ["byte*", "data" ], ["uint32","size"] ]); 
        
        self.write_createPacketFromBufferMethodBody(doc.replies.values())
            
            
    def write_createRequestPacketFromBuffer_method(self, doc):
        self.write_method_start("NavRequestPacket*", 
                                "createRequestPacketFromBuffer", 
                                [ ["byte*", "data" ], ["uint32","size"] ]); 
        
        self.write_createPacketFromBufferMethodBody(doc.requests.values())
        
        
    def write_createPacketFromBufferMethodBody(self, values):
        self.indent_increase()

        self.write_line("// decrypt the packet. the first 5 bytes are not encrypted")
        self.write_line("wf_ngp_encrypt(&data[5], size-5);")
        self.write_line("// read the type of the packet")
        self.write_line("NonOwningBuffer buff( data, size );")
        self.write_line("buff.readNext8bit();")
        self.write_line("PacketTypeEnum type(buff.readNextUnaligned16bit());")
        
        # write the supermeganinja switch
        self.write_line("switch(type)")
        self.write_line("{")
        for packet in values:
            if self.is_allowed_packet(packet.name, packet.version):
                self.write_line("case PacketTypeEnum::%s:" % packet.name)
                self.indent_increase()
                className = transform_packet_name(packet.name)
                self.write_line("return new %s (data, size);" % className)
                self.indent_decrease()
                self.add_include(className + ".h")
        
        self.write_line("default:")
        self.indent_increase()
        self.write_line("return 0;")
        self.indent_decrease()
                
        self.write_line("}")
        
        # should not be able to get this far...
        self.write_line("return 0;")
        
        # Close function scope    
        self.indent_decrease()
        self.write_line("}")
        self.write_line("")
        

import sys

if __name__ == "__main__":
    code_type = ""

    if len(sys.argv) < 2 or "-h" in sys.argv or "--help" in sys.argv:
        print "Usage:",sys.argv[0],"[code_type]"
        print "Where:"
        print 'code_type\t "MC2" (server) or "CC" (cppcore)'
        print
        print "Example:"
        print sys.argv[0],"MC2"
        sys.exit(0)

    if len(sys.argv) == 2:
        code_type = sys.argv[1]

    copyright = """/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */
"""

    doc = parser_xml.parse("protocol.xml", "client_settings.xml")
    generator = Generator('NGP', code_type, copyright)
    generator.handle_doc(doc)
    generator.writeFactoryCppFile(doc)

    # Copy the templates for base classes etc.
    copy_templates.copy_templates(code_type)

