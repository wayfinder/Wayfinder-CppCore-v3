/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
*/


/*
 *  Filename    : PALFont.c
 *  Description : PalFont Freetype / Harfbuzz Implementation
 *
 *  Notes       : To get this working you need to have Harfbuzz to compile
 *                against.
 *                Harfbuzz should be located in PAL/Linux/harfbuzz and
 *                the files should be included in the makefile when compiling.
 */

/* System Includes */
#include <assert.h>

/* External Module Includes */

/* Freetype Includes */
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_STROKER_H

/* Harfbuzz Includes */
#error Harfbuzz is GPL2 which we cannot use in a product.
#include "harfbuzz-shaper.h"
#include "harfbuzz-global.h"
#include "harfbuzz-gpos.h"

#include "../contrib/harfbuzz-unicode.h"
#include "../contrib/harfbuzz-freetype.h"

/* Internal Module Includes */
#include "PALFont.h"
#include "outline.h"

/* defines */
#define MAX_CHARS ( 256 )

/* Local Types */

/**
 * private freetype font context
 */
typedef struct {
   unsigned int  m_tag;          /**< tag for type checking */
   FT_Library    m_library;      /**< freetype library instance */
   FT_Face       m_face;         /**< freetype face instance */
   unsigned int  m_fontColor;    /**< interior font color */
   unsigned int  m_fontColorTwo; /**< exterior font color */
   unsigned int  m_fontSize;     /**< font size in pixels */
   int           m_Synth;        /**< font style mask */
   int           m_Boldness;     /**< boldness factor for synthesized bold */
   int           m_StrokeWidth;  /**< stroke width for hollow and outline styles */
   FT_Matrix     m_Matrix;       /**< matrix for transforming text */
   int           m_Alpha1;       /**< alpha value for exterior of outlined glyphs */
   int           m_Alpha2;       /**< alpha value for normal and inteior of outline glyphs */
} ft_Context_t;

typedef struct {
   int                      n_Chars;
   uint16_t                     str[ MAX_CHARS ];
   HB_Glyph              out_glyphs[ MAX_CHARS ];
   HB_GlyphAttributes     out_attrs[ MAX_CHARS ];
   HB_Fixed                out_advs[ MAX_CHARS ];
   HB_FixedPoint        out_offsets[ MAX_CHARS ];
   unsigned short   out_logClusters[ MAX_CHARS ];
} HarfBuzz_t;


/* Static Functions */

/**
 * get unicode from a ut8 string and advance character pointer
 *
 * @param text pointer to pointer to utf8
 * @return     unicode
 */
inline static unsigned int getNextUnicodeFromUTF8( unsigned char **text )
{
  unsigned int result = 0;

  // NULL pointer
  if ( text == NULL ) {
    return result;
  }

  else if ( *text[ 0 ] < 0x80 ) {
    // U+0000→U+007F    0xxxxxxx
    result = *(*text)++;
  }
  else if ( *text[ 0 ] < 0xE0 ) {
    // U+0080→U+07FF    110xxxxx 10xxxxxx
    result  = ( ( *( *text )++ & 0x1f ) << 6 );
    result |= ( ( *( *text )++ & 0x3f ) );
  }
  else if ( *text[ 0 ] < 0xF0 ) {
    // U+0080→U+07FF    110xxxxx 10xxxxxx
    result  = ( ( *( *text )++ & 0x0f ) << 12 );
    result |= ( ( *( *text )++ & 0x3f ) << 6 );
    result |=     *( *text )++ & 0x3f;
  }
  else {
  // U+10000→U+10FFFF 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    result  = ( ( *( *text )++ & 0x0f ) << 18 );
    result |= ( ( *( *text )++ & 0x3f ) << 12 );
    result |= ( ( *( *text )++ & 0x3f ) << 6 );
    result |=     *( *text )++ & 0x3f;
  }

  return result;
}

/**
 * type check and return freetype context
 *
 * @param aContext void context
 * @return         freetype context
 */
inline static ft_Context_t *FT_CONTEXT( void *aContext ) {
   ft_Context_t *ftContext = ( ft_Context_t * )aContext;
   assert( ftContext != NULL );
   assert( ftContext->m_tag == ( unsigned int )"FREE" );
   return ftContext;
}

/**
 * blend destination pixel and glyf pixel with correct color and alpha
 *
 * @param backColor color read from bitmap pixel
 * @param glyfColor color of glyf
 * @param glyfAlpha antialias value from alpha mask
 * @param globAlpha global alpha of text
 * return           blended pixel value to be written
 */
inline static unsigned int blend( unsigned int backColor,
                                  unsigned int glyfColor,
                                  unsigned int glyfAlpha,
                                  unsigned int globAlpha ) {
   unsigned int c = 0;

   glyfAlpha = ( glyfAlpha * globAlpha ) / 255;

   c += ( ( ( ( glyfColor & 0x000000FF ) * glyfAlpha ) +
              ( backColor & 0x000000FF ) * ( 0xFF - glyfAlpha ) ) >> 8 ) & 0x000000FF;
   c += ( ( ( ( glyfColor & 0x0000FF00 ) * glyfAlpha ) +
              ( backColor & 0x0000FF00 ) * ( 0xFF - glyfAlpha ) ) >> 8 ) & 0x0000FF00;
   c += ( ( ( ( glyfColor & 0x00FF0000 ) * glyfAlpha ) +
              ( backColor & 0x00FF0000 ) * ( 0xFF - glyfAlpha ) ) >> 8 ) & 0x00FF0000;

   return c;
}

/**
 * check if character is latin
 *
 * @param ch unicode char
 * return    1 if latin, 0 if not
 */
inline static int isLatin( int ch ) {
   return  ( ch >= '0' && ch <= '9' ) ||
           ( ch >= 'a' && ch <= 'z' ) ||
           ( ch >= 'A' && ch <= 'Z' );
}

/**
 * reverse count chars from start to start + count
 *
 * @param text  pointer to utf16 string
 * @param start start index
 * @param count lenght of run to be reversed
 */
inline static void reverse_text( uint16_t *text, int start, int count ) {
   int i;
   for ( i = 0; i < count / 2; i++ ) {
      int temp = text[ start + i ];
      text[ start + i ] = text[ start + count - 1 - i ];
      text[ start + count - 1 - i ] = temp;
   }
}

/**
 * takes a utf16 string and runs Harfbuzz to create a list of glyphs
 *
 * @param aContext     void context
 * @param aUtf16String the utf16 string
 * @param aHarfBuzz    structure containing Harfbuzz stuff
 * @return             error
 */
static int Utf16_To_Harfbuzz( void       *aContext,
                              uint16_t   *aUtf16String,
                              int         nChars,
                              HarfBuzz_t *aHarfBuzz ) {
   ft_Context_t *ftContext = FT_CONTEXT( aContext );
   HB_FontRec    hbFont;
   HB_Face       hbFace = NULL;
   HB_ShaperItem hbShaperItem;

   memset( aHarfBuzz->str,             0, sizeof( aHarfBuzz->str ) );
   memset( aHarfBuzz->out_glyphs,      0, sizeof( aHarfBuzz->out_glyphs ) );
   memset( aHarfBuzz->out_attrs,       0, sizeof( aHarfBuzz->out_attrs ) );
   memset( aHarfBuzz->out_advs,        0, sizeof( aHarfBuzz->out_advs ) );
   memset( aHarfBuzz->out_offsets,     0, sizeof( aHarfBuzz->out_offsets ) );
   memset( aHarfBuzz->out_logClusters, 0, sizeof( aHarfBuzz->out_logClusters ) );

   memset( &hbFont,                    0, sizeof( HB_FontRec ) );
   memset( &hbFace,                    0, sizeof( HB_Face ) );

   hbFace = HB_NewFace( ftContext->m_face, hb_freetype_table_sfnt_get );

   memcpy( aHarfBuzz->str, aUtf16String, nChars * sizeof( uint16_t ) );

   aHarfBuzz->n_Chars = nChars;

   assert( ftContext->m_face != NULL );

   hbFont.klass    = &hb_freetype_class;
   hbFont.userData = ftContext->m_face;
   hbFont.x_ppem   = ftContext->m_face->size->metrics.x_ppem;
   hbFont.y_ppem   = ftContext->m_face->size->metrics.y_ppem;
   hbFont.x_scale  = ftContext->m_face->size->metrics.x_scale;
   hbFont.y_scale  = ftContext->m_face->size->metrics.y_scale;

   memset( &hbShaperItem, 0, sizeof( HB_ShaperItem ) );

   hbShaperItem.kerning_applied     = FALSE;
   hbShaperItem.string              = ( HB_UChar16 * )aHarfBuzz->str;
   hbShaperItem.stringLength        = nChars;
   hbShaperItem.item.bidiLevel      = 0;
   hbShaperItem.item.script         = HB_Script_Arabic;
   hbShaperItem.item.pos            = 0;
   hbShaperItem.item.length         = hbShaperItem.stringLength;
   hbShaperItem.shaperFlags         = 0;
   hbShaperItem.font                = &hbFont;
   hbShaperItem.face                = hbFace;
   hbShaperItem.glyphIndicesPresent = FALSE;
   hbShaperItem.initialGlyphCount   = 0;
   hbShaperItem.glyphs              = aHarfBuzz->out_glyphs;
   hbShaperItem.attributes          = aHarfBuzz->out_attrs;
   hbShaperItem.advances            = aHarfBuzz->out_advs;
   hbShaperItem.offsets             = aHarfBuzz->out_offsets;
   hbShaperItem.log_clusters        = aHarfBuzz->out_logClusters;
   hbShaperItem.num_glyphs          = MAX_CHARS;

   HB_ShapeItem( &hbShaperItem );
   HB_FreeFace( hbFace );

   return 0;
}

/**
 * Renders or Measures some Text
 *
 * @param aContext      void context
 * @param aBitmap       pointer to bitmap ( if not provided will just measure )
 * @param aBitmapWidth  width of the provided bitmap
 * @param aBitmapHeight height of the provided bitmap
 * @param aHarfBuzz     Harfbuzz structure containing glyph indices
 * @param aIndex        start with this character index
 * @param aCount        render / measure aCount characters
 * @param aPen_x        pointer to pen position that will be updated ( optional )
 * @param aPen_y        pointer to pen position that will be updated ( optional )
 * @param aBBox         pointer to Bounding Box that will be updated ( optional )
 * @param resetBox      if non-zero�Bounding Box will be reset before accumulating
 * @param isArabic      if non-zero will treat as right to left
 * @param outlineStage  0 = draw ouside, 1 = draw inside of synth outline style
 */
 static int PalFont_RenderHarfBuzz( void         *aContext,
                                   unsigned int *aBitmap,
                                   int           aBitmapWidth,
                                   int           aBitmapHeight,
                                   HarfBuzz_t   *aHarfBuzz,
                                   int           aIndex,
                                   int           aLength,
                                   int          *aPen_x,
                                   int          *aPen_y,
                                   PalFont_BBox_t*aBBox,
                                   int           resetBox,
                                   int           isArabic,
                                   int           outlineStage ) {


    printf("PalFont_RenderHarfBuzz: context is %p\n", aContext);
   ft_Context_t  *ftContext = FT_CONTEXT( aContext );
   FT_GlyphSlot   slot;
   PalFont_BBox_t theBBox;
   unsigned int   i, j = outlineStage, k;
   int            thePen_x        =  0;
   int            thePen_y        =  0;
   FT_UInt        aGlyphIndex     =  0;
   FT_UInt        aPrevGlyphIndex =  0;

   if ( ftContext == NULL ) {
      return -1;
   }

   if ( aPen_x != NULL ) {
     thePen_x = *aPen_x;
   }

   if ( aPen_y != NULL ) {
     thePen_y = *aPen_y;
   }

   slot = ftContext->m_face->glyph;

   if ( resetBox != 0 ) {
      /* make an invalid bounding box to start with */
      theBBox.xMin = INT_MAX;
      theBBox.yMin = INT_MAX;
      theBBox.xMax = INT_MIN;
      theBBox.yMax = INT_MIN;
   }

   for ( i = 0; i < aHarfBuzz->n_Chars;
         thePen_x += ( int )( ( ( double )ftContext->m_Matrix.xx *
                                ( double )slot->advance.x +
                                ( double )ftContext->m_Matrix.xy *
                                ( double )slot->advance.y ) / 65536.0 ),
         thePen_y += ( int )( ( ( double )ftContext->m_Matrix.yy *
                                ( double )slot->advance.y +
                                ( double )ftContext->m_Matrix.yx *
                                ( double )slot->advance.x ) / 65536.0 ),
         aPrevGlyphIndex = aGlyphIndex,
         i++ ) {

      FT_Glyph   aGlyph;
      int        y;
      int        x;

      k = isArabic ? ( aHarfBuzz->n_Chars - i - 1 ) : i;

      aGlyphIndex = aHarfBuzz->out_glyphs[ k ];

      /* apply kerning if there is any */
      if ( aPrevGlyphIndex != 0 && aGlyphIndex != 0 ) {
         FT_Vector delta;

         /* ignore errors */
         if ( FT_Get_Kerning( ftContext->m_face,
                              aPrevGlyphIndex,
                              aGlyphIndex,
                              FT_KERNING_DEFAULT,
                              &delta ) == 0 ) {
            thePen_x += ( int )( ( ( double )ftContext->m_Matrix.xx *
                                   ( double )delta.x ) / ( 65536.0 ) );
         }
      }

      /* load glyph image into the slot (erase previous one) */
      if ( FT_Load_Glyph( ftContext->m_face,
                           aGlyphIndex,
                           FT_LOAD_DEFAULT ) != 0 ) {
         continue; /* ignore errors */
      }

      // if we are synthesising italic then shear outline
      if ( ftContext->m_Synth & SynthItalic ) {
         FT_Matrix m;
         m.xx = 65536;
         m.yy = 65536;
         m.xy = 19660;
         m.yx = 0;
         FT_Outline_Transform( &slot->outline, &m );
      }

      /* if we are doing first pass of two color then fatten */
      if ( ( ftContext->m_Synth & SynthTwoColor ) && j == 0 ) {
         if ( FT_Outline_Embolden( &slot->outline,
                                    ftContext->m_StrokeWidth << 2 ) != 0 ) {
            continue;
         }
      }

      // if we are doing second pass of two color then translate
      if ( ( ftContext->m_Synth & SynthTwoColor ) && j == 1 ) {
         FT_Outline_Translate( &slot->outline,
                                ftContext->m_StrokeWidth * 2,
                                ftContext->m_StrokeWidth * 2 );
      }

      /* if we are synthesising bold then perform fattening operation */
      if ( ftContext->m_Synth & SynthBold ) {
         if ( FT_Outline_Embolden( &slot->outline,
                                    ftContext->m_Boldness << 1 ) != 0 ) {
            continue;
         }
      }

      /* if we are synthesising stroke font then stroke it */
      if ( ftContext->m_Synth & SynthStroke ) {
         if ( ft_OutlineStroke( ftContext->m_library,
                           &slot->outline, ftContext->m_StrokeWidth ) != 0 ) {
            continue;
         }
      }

      FT_Outline_Transform( &slot->outline, &ftContext->m_Matrix );

      /* if bounding box is supplied then
         accumulate the glyphs bounding boxes */
      if ( aBBox != NULL )  {

         FT_BBox theCBox;
         FT_Outline_Get_CBox( &slot->outline, &theCBox );

         theCBox.xMin += thePen_x;
         theCBox.xMax += thePen_x;
         theCBox.yMin += thePen_y;
         theCBox.yMax += thePen_y;

         if ( theCBox.xMin < theBBox.xMin ) {
            theBBox.xMin = theCBox.xMin;
         }
         if ( theCBox.yMin < theBBox.yMin ) {
            theBBox.yMin = theCBox.yMin;
         }
         if ( theCBox.xMax > theBBox.xMax ) {
            theBBox.xMax = theCBox.xMax;
         }
         if ( theCBox.yMax > theBBox.yMax ) {
            theBBox.yMax = theCBox.yMax;
         }
      }

      /* if bitmap is supplied then draw into it */
      if ( aBitmap == NULL ) {
         continue;
      }

      /* convert to an anti-aliased bitmap */
      if ( FT_Render_Glyph( slot, FT_RENDER_MODE_NORMAL ) != 0 )
         continue;

      /* render the bitmap...  */
      for ( y = 0; y < slot->bitmap.rows; y++ ) {
         for ( x = 0; x < slot->bitmap.width; x++ ) {
            unsigned int by = (-(thePen_y>>6)-slot->bitmap_top+y+aBitmapHeight);
            unsigned int bx = (thePen_x>>6)+slot->bitmap_left+x;
            if ( bx < aBitmapWidth && by < aBitmapHeight ) {
               unsigned int *pixel = &aBitmap[ by * aBitmapWidth + bx ];
               *pixel = blend( *pixel,
               j ? ftContext->m_fontColor : ftContext->m_fontColorTwo,
               slot->bitmap.buffer[y*slot->bitmap.pitch+x],
               j ? ftContext->m_Alpha1 : ftContext->m_Alpha2 );
            }
         }
      }
   }

   if ( aBBox != NULL ) {
      *aBBox = theBBox;
   }
   if ( aPen_x != NULL ) {
      *aPen_x = thePen_x;
   }
   if ( aPen_y != NULL ) {
      *aPen_y = thePen_y;
   }
   return 0;
}

/* Exported Functions */

int PalFont_Initialise( void **aContext ) {
   ft_Context_t *ftContext;
   int           err = 0;

   assert( aContext != NULL );

   ftContext = ( ft_Context_t * )malloc( sizeof( ft_Context_t ) );
   if ( ftContext == NULL ) {
      return -1;
   }

   memset( ftContext, 0, sizeof( ft_Context_t ) );

   ftContext->m_tag = ( unsigned int )"FREE";
   ftContext->m_library      = NULL;
   ftContext->m_face         = NULL;
   ftContext->m_fontColor    = DEFAULT_INTERIOR_COLOR;
   ftContext->m_fontColorTwo = DEFAULT_EXTERIOR_COLOR;
   ftContext->m_fontSize     = DEFAULT_SIZE;
   ftContext->m_Synth        = SynthNone;

   /* identity matrix */
   ftContext->m_Matrix.xx    = ONE_POINT_ZERO;
   ftContext->m_Matrix.yy    = ONE_POINT_ZERO;
   ftContext->m_Matrix.xy    = 0;
   ftContext->m_Matrix.yx    = 0;

   ftContext->m_Alpha1       = DEFAULT_EXTERIOR_ALPHA;
   ftContext->m_Alpha2       = DEFAULT_INTERIOR_ALPHA;

   err = FT_Init_FreeType( &ftContext->m_library );
   if ( err != 0 ) {
      return err;
   }

   /* err = PalFont_Face_Set( ftContext, DEFAULT_FACE ); */
   /* if ( err != 0 ) { */
   /*    return err; */
   /* } */

   /* err = PalFont_Size_Set( ftContext, DEFAULT_SIZE ); */
   /* if ( err != 0 ) { */
   /*    return err; */
   /* } */

   *aContext = ftContext;

   return 0;
}

int PalFont_Color_Set( void         *aContext,
                       unsigned int  aColor ) {
   ft_Context_t *ftContext = FT_CONTEXT( aContext );
   ftContext->m_fontColor  = aColor;
   return 0;
}

int PalFont_OutlineColor_Set( void         *aContext,
                              unsigned int  aColor ) {
   ft_Context_t *ftContext = FT_CONTEXT( aContext );
   ftContext->m_fontColorTwo = aColor;
   return 0;
}

int PalFont_Size_Set( void         *aContext,
                      unsigned int  aSize ) {
   ft_Context_t *ftContext = FT_CONTEXT( aContext );
   ftContext->m_fontSize    = aSize;
   ftContext->m_Boldness    = ( aSize * 3 ) / 2;
   ftContext->m_StrokeWidth = ( aSize * 3 ) / 4;
   return FT_Set_Pixel_Sizes( ftContext->m_face, 0, aSize );
}

int PalFont_Style_Set( void         *aContext,
                       unsigned int  aStyle ) {
   ft_Context_t *ftContext = FT_CONTEXT( aContext );
   ftContext->m_Synth      = aStyle;
   return 0;
}

int PalFont_Alpha_Set( void         *aContext,
                       unsigned int  aAlpha ) {
   ft_Context_t *ftContext = FT_CONTEXT( aContext );
   ftContext->m_Alpha2     = aAlpha;
   return 0;
}

int PalFont_OutlineAlpha_Set( void         *aContext,
                              unsigned int  aAlpha ) {
   ft_Context_t *ftContext = FT_CONTEXT( aContext );
   ftContext->m_Alpha1     = aAlpha;
   return 0;
}

int PalFont_Matrix_Set(       void             *aContext,
                              PalFont_Matrix_t  aMatrix ) {
   ft_Context_t *ftContext = FT_CONTEXT( aContext );
   ftContext->m_Matrix.xx  = aMatrix.xx;
   ftContext->m_Matrix.yy  = aMatrix.yy;
   ftContext->m_Matrix.xy  = aMatrix.xy;
   ftContext->m_Matrix.yx  = aMatrix.yx;
}

int PalFont_Face_Set( void *aContext,
                      const char *aPath ) {
   ft_Context_t *ftContext = FT_CONTEXT( aContext );
   if ( ftContext->m_face != NULL ) {
      FT_Done_Face( ftContext->m_face );
   }
   return FT_New_Face( ftContext->m_library,
                       aPath, 0,
                      &ftContext->m_face );
}

int PalFont_Finalise( void *aContext ) {
   ft_Context_t *ftContext = FT_CONTEXT( aContext );
   return FT_Done_FreeType( ftContext->m_library );
}


int PalFont_RenderText( void         *aContext,
                        unsigned int *aBitmap,
                        int           aBitmapWidth,
                        int           aBitmapHeight,
                        const char   *text,
                        int           textIndex,
                        int           textLength,
                        int          *aPen_x,
                        int          *aPen_y,
                        PalFont_BBox_t*aBBox,
                        int           resetBox ) {
   ft_Context_t *ftContext = FT_CONTEXT( aContext );
   HarfBuzz_t    aHarfBuzz;
   int           err       = 0;
   uint16_t      str[ MAX_CHARS ];
   int           i;
   int           nChars;
   int           isArabic  = 0;
   int           savePenX, savePenY;

   /* firstly convert to UTF-16 in str */
   for ( i = 0; i < MAX_CHARS; i++ ) {
      int aCharCode = getNextUnicodeFromUTF8( ( unsigned char ** )&text );
      if ( aCharCode == 0 ) {
         break;
      }
      str[ i ] = aCharCode;
   }

   /* we cant handle more than MAX_CHARS */
   if ( i == MAX_CHARS ) {
      return -1;
   }

   /* add termination, not sure if needed */
   str[ i ] = '\0';
   nChars   = i;

   /* do we have any Arabic? */
   for ( i = 0; i < nChars; i++ ) {
      if ( code_point_to_script( str[ i ] ) == HB_Script_Arabic ) {
         isArabic = 1;
         break;
      }
   }

   /* if we do then reverse any runs of Latin */
   if ( isArabic != 0 ) {
      int j;
      for ( j = 0 ; j < nChars; ) {
        /* set i to the start of latin */
        for ( i = j; i < nChars; i++ ) {
           if ( isLatin( str[ i ] ) != 0 ) {
              break;
           }
        }
        /* set j to the end of latin + 1 */
        for ( j = i; j < nChars; j++ ) {
           if ( isLatin( str[ j ] ) == 0 ) {
               break;
            }
         }
         reverse_text( str, i, j - i );
      }
   }

   err = Utf16_To_Harfbuzz( aContext, str, nChars, &aHarfBuzz );
   if ( err != 0 ) {
      return err;
   }

   if ( aPen_x != NULL ) {
      savePenX = *aPen_x;
   }

   if ( aPen_y != NULL ) {
      savePenY = *aPen_y;
   }

   err = PalFont_RenderHarfBuzz( ftContext,
                                 aBitmap,
                                 aBitmapWidth,
                                 aBitmapHeight,
                                &aHarfBuzz,
                                 textIndex,
                                 textLength,
                                 aPen_x,
                                 aPen_y,
                                 aBBox,
                                 resetBox,
                                 isArabic,
                                 0 );
   if ( err != 0 ) {
      return err;
   }

   if ( ( ftContext->m_Synth & SynthTwoColor ) != 0 ) {
      resetBox = 0;

      if ( aPen_x != NULL ) {
         *aPen_x = savePenX;
      }

      if ( aPen_y != NULL ) {
         *aPen_y = savePenY;
      }

      err = PalFont_RenderHarfBuzz( ftContext,
                                    aBitmap,
                                    aBitmapWidth,
                                    aBitmapHeight,
                                    &aHarfBuzz,
                                    textIndex,
                                    textLength,
                                    aPen_x,
                                    aPen_y,
                                    aBBox,
                                    resetBox,
                                    isArabic,
                                    1 );
      if ( err != 0 ) {
         return err;
      }
   }
   return 0;
}
