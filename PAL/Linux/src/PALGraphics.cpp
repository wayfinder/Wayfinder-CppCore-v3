/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
*/

#include "PALGraphics.h"
#include "PALFont.h"
#include <gtk/gtk.h>
#include <cassert>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <cstdlib>
#include "stb_image.h"

#define DBG "[PALGraphicsLinux]: "


typedef struct {
   void *fontContext;
} PALGraphicsLinuxContext;


pstatus loadImageFromMemory(GraphicsContext context,
                            const wf_uint8* source,
                            wf_uint32 sourceSize,
                            ImageType type,
                            BufFormat format,
                            wf_uint32 desiredWidthPixels,
                            wf_uint32 desiredHeightPixels,
                            wf_uint32* widthPixels,
                            wf_uint32* heightPixels,
                            ResultBuffer* buffer)
{
   if(type != PNG) {
      return PAL_ERR_IMG_TYPE_NOT_SUPPORTED;
   }

   if(format != RGBA8) {
      return PAL_ERR_BUFFER_FORMAT_NOT_SUPPORTED;
   }

   assert(sizeof(stbi_uc) == sizeof(wf_uint8));

   int reqComp  = 4; // Always read RGBA
   int comp     = 0;

   wf_int32 width = 0;
   wf_int32 height = 0;

   // wf_uint8* buf =
      stbi_load_from_memory(source,
                            sourceSize,
                            &width,
                            &height,
                            &comp,
                            reqComp);

   assert(width >= 0);
   assert(height >= 0);
   
   *widthPixels = width;
   *heightPixels = height;

   assert(comp == reqComp);

   return PAL_OK;
}

pstatus loadImageFromFile(GraphicsContext context,
                          FILE* source,
                          ImageType type,
                          BufFormat format,
                          wf_uint32 desiredWidthPixels,
                          wf_uint32 desiredHeightPixels,
                          wf_uint32* widthPixels,
                          wf_uint32* heightPixels,
                          ResultBuffer* buffer)
{
   if(type != PNG) {
      return PAL_ERR_IMG_TYPE_NOT_SUPPORTED;
   }

   if(format != RGBA8) {
      return PAL_ERR_BUFFER_FORMAT_NOT_SUPPORTED;
   }

   assert(sizeof(stbi_uc) == sizeof(wf_uint8));

   int reqComp  = 4; // Always read RGBA
   int comp     = 0;

   wf_int32 width = 0;
   wf_int32 height = 0;

   unsigned char* buf =
      stbi_load_from_file(source, &width, &height, &comp, reqComp);

   *widthPixels = width;
   *heightPixels = height;

   assert(comp == reqComp);

   buffer->init(buf, (*widthPixels) * (*heightPixels) * 4, free);

   return PAL_OK;
}

pstatus drawTextLineToBuffer(GraphicsContext context,
                             const wf_char* utf8Text,
                             wf_int32 charPosition,
                             wf_int32 length,
                             BufFormat format,
                             wf_uint32* widthPixels,
                             wf_uint32* heightPixels,
                             ResultBuffer* buffer)
{
   PALGraphicsLinuxContext* linuxContext = (PALGraphicsLinuxContext*)(context);

   int            err = 0;
   int            aPenX;
   int            aPenY;
   int            ttWidth;
   int            ttHeight;
   unsigned int*  ttBitmap = NULL;
   PalFont_BBox_t aBBox;
      
   err = PalFont_RenderText( linuxContext->fontContext,
                        NULL, // unsigned     *aBitmap,
                        0, //int           aBitmapWidth,
                        0, //int           aBitmapHeight,
                        utf8Text, //char         *text,
                        0,
                        -1, //int           textLength,
                        NULL, //int          *aPen_x,
                        NULL, //int          *aPen_y,
                        &aBBox, //       *aBBox,
                        1 ); //int           resetBox );
   
   if ( err != 0 ) {
      return (pstatus)err;
   }

   // bounding box is in fractional units so round up to integer pixels
   ttWidth  = ( ( aBBox.xMax + 63 ) >> 6 ) - ( aBBox.xMin >> 6 );
   ttHeight = ( ( aBBox.yMax + 63 ) >> 6 ) - ( aBBox.yMin >> 6 );
   
   // allocate a bitmap just big enough
   if ( ttBitmap != NULL ) {
      free( ttBitmap );
      ttBitmap = NULL;
   }
   
   ttBitmap = ( unsigned int * )malloc( 4 * ttWidth * ttHeight );

   memset(ttBitmap, 0, ttWidth * ttHeight * 4);
   
   if ( ttBitmap == NULL ) {
      return PAL_OUT_OF_MEMORY;
   }

   // make sure we position the pen correctly
   aPenX = - ( aBBox.xMin >> 6 ) << 6;
   aPenY = - ( aBBox.yMin >> 6 ) << 6;
   
   err = PalFont_RenderText( linuxContext->fontContext, ttBitmap, ttWidth, ttHeight,
                  utf8Text, 0, -1, &aPenX, &aPenY, &aBBox, 1 );

   *widthPixels = ttWidth;
   *heightPixels = ttHeight;

   printf("widthPixels: %u\n", *widthPixels);
   printf("heightPixels: %u\n", *heightPixels);


   
   buffer->init(reinterpret_cast<wf_uint8*>(ttBitmap), ttWidth * ttHeight * 4, free);
   
   return PAL_OK;
}

pstatus createGraphicsContext(GraphicsContext* context) {

   PALGraphicsLinuxContext *linuxContext =
      ( PALGraphicsLinuxContext *)malloc( sizeof( PALGraphicsLinuxContext ) );

   if ( linuxContext == NULL ) {
      return PAL_OUT_OF_MEMORY;
   }

   if ( PalFont_Initialise( &linuxContext->fontContext ) != 0 ) {
      return PAL_ERR_FONT_FAILED_TO_INITIALIZE;
   }

   *context = ( GraphicsContext )linuxContext;

   return PAL_OK;
}

pstatus destroyGraphicsContext(GraphicsContext context) {

   pstatus err = PAL_OK;
   PALGraphicsLinuxContext *linuxContext = ( PALGraphicsLinuxContext * )context;

   if ( linuxContext == NULL ) {
      return PAL_ERR_GRAPHICS_CONTEXT_INVALID;
   }

   if(PalFont_Finalise( linuxContext->fontContext ) != 0) {
      err = PAL_ERR_FONT_FAILED_TO_FINALIZE;
   }

   // Free structure
   free ( linuxContext );
   return err;
}

pstatus setFontColorForContext( GraphicsContext context,
                                wf_uint8 r,
                                wf_uint8 g,
                                wf_uint8 b)
{
   //TODO: join r, g, b to one
   unsigned int rgb = 0xFFFFFFFF;
   PALGraphicsLinuxContext* linuxContext = (PALGraphicsLinuxContext*)(context);
   PalFont_Color_Set( linuxContext->fontContext, rgb );
   return PAL_OK;
}

pstatus setFontOutlineColorForContext( GraphicsContext context,
                                       unsigned int    rgb ) {
   PALGraphicsLinuxContext* linuxContext = (PALGraphicsLinuxContext*)(context);
   PalFont_OutlineColor_Set( linuxContext->fontContext, rgb );
   return PAL_OK;
}

pstatus setFontSizeForContext( GraphicsContext context,
                               int             size ) {
   PALGraphicsLinuxContext* linuxContext = (PALGraphicsLinuxContext*)(context);
   PalFont_Size_Set( linuxContext->fontContext, size );
   return PAL_OK;
}

pstatus setFontStyleForContext( GraphicsContext context,
                                int             style ) {
   PALGraphicsLinuxContext* linuxContext = (PALGraphicsLinuxContext*)(context);
   PalFont_Style_Set( linuxContext->fontContext, style );
   return PAL_OK;
}

pstatus setFontAlphaForContext( GraphicsContext context,
                                int             alpha ) {
   PALGraphicsLinuxContext* linuxContext = (PALGraphicsLinuxContext*)(context);
   PalFont_Alpha_Set( linuxContext->fontContext, alpha );
   return PAL_OK;
}

pstatus setFontOutlineAlphaForContext( GraphicsContext context,
                                       int             alpha ) {
   PALGraphicsLinuxContext* linuxContext = (PALGraphicsLinuxContext*)(context);
   PalFont_OutlineAlpha_Set( linuxContext->fontContext, alpha );
   return PAL_OK;
}

pstatus setFontMatrixForContext( GraphicsContext  context,
                                 PalFont_Matrix_t matrix ) {
   PALGraphicsLinuxContext* linuxContext = (PALGraphicsLinuxContext*)(context);
   PalFont_Matrix_Set( linuxContext->fontContext, matrix );
   return PAL_OK;
}

pstatus setFontForContext(GraphicsContext context,
                          const wf_char* fontFamily,
                          const wf_char* fontStyle,
                          wf_uint32 fontSize)
{
   setFontSizeForContext(context, fontSize);
   //TODO: implement
   return PAL_OK;
}

pstatus setFontFaceForContext(GraphicsContext  context,
                              const wf_char* path)
{
   PALGraphicsLinuxContext* linuxContext = (PALGraphicsLinuxContext*)(context);
   PalFont_Face_Set( linuxContext->fontContext, path );
   return PAL_OK;
}

pstatus runGraphicsTests(GraphicsContext context) {
   int ap = 22;
   ap++;
   return PAL_OK;
}



