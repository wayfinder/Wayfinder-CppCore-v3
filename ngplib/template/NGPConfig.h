/*
 * Copyright 2010, Wayfinder Systems AB
 */

#ifndef NGPCONFIG_H
#define NGPCONFIG_H

/*
 * Define platform independent data types.
 *
 * A more proper way could be to set a fixed file name for
 * the typedefs and then use compiler -I-switches to include
 * from the right directory based on machine type.
 *
 * But we only share this between C++ Core and MC2 Server. And it
 * is not likely that we will send NGP-lib in source form to third
 * parties or compile it stand-alone for inclusion as static
 * lib+headers.
 */

#ifdef NGPLIB_SERVER
   // mc2: Shared/Utility/include/config.h
   //    includes
   // mc2: Shared/Utility/include/Types.h
   // which also fixes compiler/machine type abstraction.
   #include <config.h>

#else
   // Not possible to compile Core V3 without this anyway.
   // includes abstractions for Symbian
   #include <PALMachineTypes.h>

   /* but the names are different - might be changed in the future.
    */

   // avoid polluting the global namespace.
   namespace wf_ngp {
      typedef wf_uint8 uint8;
      typedef wf_int16 int16;
      typedef wf_uint16 uint16;
      typedef wf_byte byte;
      typedef wf_int32 int32;
      typedef wf_uint32 uint32;
   }

   // MC2 code use a define instead of static const for this
   // so we can't namespace it without making up new names for
   // min/max-values and use them in the template code.
   static const wf_ngp::uint32 MAX_INT32 = WF_MAX_INT32;
#endif // NGPLIB_SERVER

#endif // NGPCONFIG_H
