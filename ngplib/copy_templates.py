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

import shutil
import glob
import os


def copy_templates(code_type):
   """Copy the templates.

   Assumes that the current directory is the base directory
   for ngplib.
   """
   dest_dir = "NGP"

   for file in glob.glob("template/*.h"):
      shutil.copy(file, dest_dir)

   for file in glob.glob("template/*.cpp"):
      shutil.copy(file, dest_dir)

   for file in glob.glob("template/*.frag"):
      shutil.copy(file, dest_dir)

   for file in glob.glob("template/*.CC"):
      if file == "template/Makefile.CC" :
         new_name = dest_dir + "/Makefile"
         shutil.copyfile(file, new_name)

   copy_overrides(code_type, dest_dir)


def copy_overrides(code_type, dest_dir):
   """Copy the override files for code_type into dest_dir.

   Will crash if there are uncopyable files in the source
   directory. Does not handle any sub directories.

   """
   override_path = "template/overrides/" + code_type
   for globexp in ["*.h", "*.cpp", "*.frag"]:
      for in_file in glob.glob(override_path + "/" + globexp):
         # print in_file
         out_file = os.path.basename(in_file)
         if out_file == "":
            continue

         out_fd = open(dest_dir + "/" + out_file, "w")
         out_fd.write("/* This comes from " + in_file + " */\n\n")
         in_fd = open(in_file, "r")
         out_fd.write(in_fd.read())
         in_fd.close()
         out_fd.close()
