/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
*/

#ifndef PALFONT_H
#define PALFONT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Defines */

#define SynthNone     ( 0 )
#define SynthBold     ( 1 )
#define SynthItalic   ( 2 )
#define SynthStroke   ( 4 )
#define SynthTwoColor ( 8 )

#define ONE_POINT_ZERO         ( 1 << 16 )                  /**< 1.0 in fixed 16.16 format */
#define DEFAULT_SIZE           ( 32 )                       /**< default point size */
#define DEFAULT_INTERIOR_ALPHA ( 0xFF )                     /**< default glyph alpha */
#define DEFAULT_EXTERIOR_ALPHA ( 0xFF )                     /**< default outline alpha */
#define DEFAULT_INTERIOR_COLOR ( 0x00FFFFFF )               /**< default glyph color */
#define DEFAULT_EXTERIOR_COLOR ( 0x00000000 )               /**< default outline color */

/* Exported Types */

typedef struct {
   int xMin;
   int yMin;
   int xMax;
   int yMax;
} PalFont_BBox_t;

typedef struct {
   int xx;
   int yy;
   int xy;
   int yx;
} PalFont_Matrix_t;

typedef struct {
   int m_Width;
   int m_Height;
   int m_OffsetToPenX;
   int m_OffsetToPenY;
} PalFont_BitmapMetrics_t;


/* Exported Function Prototypes */

/**
 * allocate and initialise context
 *
 * @param aContext pointer to void pointer context
 * @return         error
 */
int PalFont_Initialise(      void         **aContext );

/**
 * finalise font context
 *
 * @param aContext void context
 * @return         error
 */
int PalFont_Finalise(        void          *aContext );

/**
 * Renders or Measures some Text
 *
 * @param aContext      void context
 * @param aBitmap       pointer to bitmap ( if not provided will just measure )
 * @param aBitmapWidth  width of the provided bitmap
 * @param aBitmapHeight height of the provided bitmap
 * @param aTextString   utf-8 text string
 * @param aTextIndex    start drawing from this index
 * @param aTextLength   length of the string
 * @param aPen_x        pointer to pen position that will be updated ( optional )
 * @param aPen_y        pointer to pen position that will be updated ( optional )
 * @param aBBox         pointer to Bounding Box that will be updated ( optional )
 * @param resetBox      if non-zeroŽBounding Box will be reset before accumulating
 */
int PalFont_RenderText(      void          *aContext,
                             unsigned int  *aBitmap,
                             int            aBitmapWidth,
                             int            aBitmapHeight,
                       const char          *aTextString,
                             int            aTextIndex,
                             int            aTextLength,
                             int           *aPen_x,
                             int           *aPen_y,
                             PalFont_BBox_t*aBBox,
                             int            aResetBox );

// TODO: Comment
int PalFont_OutlineColor_Set( void         *aContext,
                              unsigned int  aColor );
   
/*
 * set the font color
 *
 * @param aContext void context
 * @param aColor   color in ARGB format
 * @return         error
 */
int PalFont_Color_Set(       void          *aContext,
                             unsigned int   aColor );

/*
 * set the font stroked outline color
 *
 * @param aContext void context
 * @param aColor   color in ARGB format
 * @return         error
 */
int PalFont_OulineColor_Set( void          *aContext,
                             unsigned int   aColor );

/**
 * set the font stroked outline color
 *
 * @param aContext void context
 * @param aColor   color in ARGB format
 * @return         error
 */
int PalFont_Size_Set(        void          *aContext,
                             unsigned int   aSize );

/**
 * set the font style
 *
 * @param aContext void context
 * @param aStyle   bitfield( 1==Bold, 2==Italic, 4==Hollow, 8==Outline )
 * @return         error
 */
int PalFont_Style_Set(       void          *aContext,
                             unsigned int   aStyle );

/**
 * set the interior transparency
 *
 * @param aContext void context
 * @param aAlpha   0 - 255, invisible - visible
 * @return         error
 */
int PalFont_Alpha_Set(       void          *aContext,
                             unsigned int   aAlpha );

/**
 * set the font stroked outline transparency
 *
 * @param aContext void context
 * @param aAlpha   0 - 255, invisible - visible
 * @return         error
 */
int PalFont_OutlineAlpha_Set( void          *aContext,
                              unsigned int   aAlpha );

/**
 * set the transform for doing rotation, scaling, condensed
 *
 * @param aContext void context
 * @param aMatrix  new matrix
 * @return         error
 */
int PalFont_Matrix_Set(       void             *aContext,
                              PalFont_Matrix_t  aMatrix );


/**
 * set the font face
 *
 * @param aContext void context
 * @param aPath    path to font file containing face
 * @return         error
 */
int PalFont_Face_Set(        void          *aContext,
                     const   char          *aPath );

#ifdef __cplusplus
} // extern "C"
#endif
      
#endif /* PALFONT_H */
