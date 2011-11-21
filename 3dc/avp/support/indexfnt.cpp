/*******************************************************************
 *
 *    DESCRIPTION: 	indexfnt.cpp
 *
 *    AUTHOR: David Malcolm
 *
 *    HISTORY:  Created 18/11/97
 *
 *******************************************************************/

/* Includes ********************************************************/
#include "3dc.h"
#include "inline.h"
#include "indexfnt.hpp"
#include "tallfont.hpp"

extern "C"
{
#include "d3d_hud.h"
};
#define UseLocalAssert Yes
#include "ourasert.h"


/* Imported function prototypes ************************************/
extern void D3D_RenderHUDString(char *stringPtr,int x,int y,int colour);
extern void D3D_RenderHUDString_Clipped(char *stringPtr,int x,int y,int colour);

/* Imported data ***************************************************/
#ifdef __cplusplus
	extern "C"
	{
#endif
		extern unsigned char *ScreenBuffer;
		extern long BackBufferPitch;
		extern LPDIRECTDRAWSURFACE lpDDSBack;
		extern DDPIXELFORMAT DisplayPixelFormat;
		extern int CloudTable[128][128];
		extern int CloakingPhase;
#ifdef __cplusplus
	};
#endif

/* Exported globals ************************************************/
IndexedFont* IndexedFont :: pIndexedFont[ IndexedFonts_MAX_NUMBER_OF_FONTS ];


/* Exported function definitions ***********************************/
// class IndexedFont
// public:
 void IndexedFont :: UnloadFont( FontIndex I_Font_ToGet )
{

	GLOBALASSERT( pIndexedFont[I_Font_ToGet ] );

	IndexedFont* pDelete = pIndexedFont[I_Font_ToGet ];
	pIndexedFont[ I_Font_ToGet ] = NULL;
	delete pDelete;
}

/*virtual*/ IndexedFont :: ~IndexedFont()
{
	// Inform the SCString code there's been a change...
	SCString :: UpdateAfterFontChange( I_Font_Val );

	pIndexedFont[ I_Font_Val ] = NULL;
}


OurBool IndexedFont :: bCanRenderFully( ProjChar* pProjCh )
{
	// returns true iff all characters in the string are renderable by the font

	// Assumes one byte-per-character:
	while ( *pProjCh )
	{
		if ( !bCanRender( *pProjCh ) )
			return No;
		
		pProjCh++;
	}

	return Yes;
}

//  :: Render_Clipped_Report ()
void IndexedFont :: Render_Clipped_Report
(
	const struct r2pos& R2Pos_Cursor,
	const struct r2rect& R2Rect_Clip,
	int,// FixP_Alpha,
	const SCString& SCStr
) const
{
	textprint
	(
		"IndexedFont(%i)::RenderString_Clipped() at(%i,%i) clip(%i,%i,%i,%i) \"%s\"\n",
		I_Font_Val,
		R2Pos_Cursor . x,
		R2Pos_Cursor . y,
		R2Rect_Clip . x0,
		R2Rect_Clip . y0,
		R2Rect_Clip . x1,
		R2Rect_Clip . y1,
		SCStr . pProjCh()
	);
}


// protected:
// Protected constructor since abstract class
IndexedFont :: IndexedFont ( FontIndex I_Font_New )
{
	GLOBALASSERT( I_Font_New < IndexedFonts_MAX_NUMBER_OF_FONTS );
	I_Font_Val = I_Font_New;
	pIndexedFont[ I_Font_New ] = this;
	
}



// class IndexedFont_Proportional : public IndexedFont
// public:
// :: RenderString_Clipped ()
void IndexedFont_Proportional :: RenderString_Clipped
(
	struct r2pos& R2Pos_Cursor,
	const struct r2rect& R2Rect_Clip,
	int FixP_Alpha,// FixP_Alpha,
	const SCString& SCStr
) const
{


	ProjChar* pProjChar_I = SCStr . pProjCh();

	while ( *pProjChar_I )
	{
		const ProjChar ProjCh = *pProjChar_I;

		if ( bCanRender( ProjCh ) )
		{
			// Rewritten by DHM 18/3/98 to use the single character renderer:
			RenderChar_Clipped ( R2Pos_Cursor, R2Rect_Clip,	FixP_Alpha, ProjCh );
		}

		pProjChar_I++;
	}
	
}

// :: RenderString_Unclipped ()
void IndexedFont_Proportional :: RenderString_Unclipped
(
	struct r2pos& R2Pos_Cursor,
	int FixP_Alpha, 
	const SCString& SCStr
) const

{
	ProjChar* pProjChar_I = SCStr . pProjCh();

	while ( *pProjChar_I )
	{
		const ProjChar ProjCh = *pProjChar_I;

		if ( bCanRender( ProjCh ) )
			{
 				// Rewritten by DHM 18/3/98 to use the single character renderer:
				RenderChar_Unclipped (R2Pos_Cursor,FixP_Alpha,ProjCh );
			}

		pProjChar_I++;
	}
	
}

// :: CalcSize ()
r2size IndexedFont_Proportional :: CalcSize (	ProjChar* pProjCh ) const
{
	GLOBALASSERT( pProjCh );

	r2size R2Size_Return ( 0, GetHeight() );

	if (*pProjCh)
	{
		// non-empty strings have one pixel space between characters; the if/do/while
		// construction is to take one off the final result for a non-empty string
		do
		{
			R2Size_Return . w += GetWidth( *pProjCh ) + 1;

			pProjCh++;
		} while (*pProjCh);

		R2Size_Return . w --;
	}

	return R2Size_Return;
}

// :: CalcSize ()
r2size IndexedFont_Proportional :: CalcSize ( 	ProjChar* pProjCh, int MaxChars ) const
{
	GLOBALASSERT( pProjCh );
	GLOBALASSERT( MaxChars >= 0 );

	r2size R2Size_Return ( 0, GetHeight() );

	if ( (*pProjCh)	&& (MaxChars>0)	)
	{
		// non-empty strings have one pixel space between characters; the if/do/while
		// construction is to take one off the final result for a non-empty string
		do
		{
			R2Size_Return . w += GetWidth( *pProjCh ) + 1;
			pProjCh++;
		} while
		( (*pProjCh) &&	(--MaxChars>0) );

		R2Size_Return . w --;
	}

	return R2Size_Return;	
}


// class IndexedFont_Kerned : public IndexedFont
// public:
// :: RenderString_Clipped ()
void IndexedFont_Kerned :: RenderString_Clipped
(
	struct r2pos& R2Pos_Cursor,
	const struct r2rect& R2Rect_Clip,
	int FixP_Alpha,
	const SCString& SCStr
) const
{
	ProjChar* pProjChar_I = SCStr . pProjCh();

	const LPDIRECTDRAWSURFACE image_ptr = GetImagePtr();

	DDSURFACEDESC ddsdimage;
	
	memset(&ddsdimage, 0, sizeof(ddsdimage));
	ddsdimage.dwSize = sizeof(ddsdimage);

	/* lock the image */
	while (image_ptr->Lock(NULL, &ddsdimage, DDLOCK_WAIT, NULL) == DDERR_WASSTILLDRAWING);

	while ( *pProjChar_I )
	{
		const ProjChar ProjCh = *pProjChar_I;

		if ( bCanRender( ProjCh ) )
		{

			if (ProjCh != ' ')
			{
				unsigned int theOffset=ProjCh - 32;
				int width = GetWidth(ProjCh);

				{
					if ( width>0 )
					{
						{
							// This code adapted from DrawGraphicWithAlphaChannel();
							// it assumes you're in a 16-bit mode...

							// okay, now we have the surfaces, we can copy from one to the other,
							// darkening pixels as we go
							{
								long fontimagePitchInShorts = (ddsdimage.lPitch/2); 
								long backbufferPitchInShorts = (BackBufferPitch/2); 

								unsigned short* fontimageRowStartPtr =
								(
									((unsigned short *)ddsdimage.lpSurface)
									+
									(GetHeight()*theOffset*fontimagePitchInShorts)
								);

								unsigned short* backbufferRowStartPtr =
								(
									((unsigned short *)ScreenBuffer)
									+
									(R2Pos_Cursor.y*backbufferPitchInShorts)
									+
									(R2Pos_Cursor.x)
								);
								int screenY = R2Pos_Cursor.y;

								for (int yCount=GetHeight(); yCount>0; yCount--)
								{
									unsigned short* fontimagePtr = fontimageRowStartPtr;
									unsigned short* backbufferPtr = backbufferRowStartPtr;

									if (screenY >= R2Rect_Clip.y0 && screenY <= R2Rect_Clip.y1)
									for (int xCount=width; xCount>0;xCount--)
									{
										int r = CloudTable[(xCount+R2Pos_Cursor.x+CloakingPhase/64)&127][(screenY+CloakingPhase/128)&127];
										r = MUL_FIXED(FixP_Alpha,r);
										if (*fontimagePtr)
										{
											unsigned int backR = (int)(*backbufferPtr) & DisplayPixelFormat.dwRBitMask;
											unsigned int backG = (int)(*backbufferPtr) & DisplayPixelFormat.dwGBitMask;
											unsigned int backB = (int)(*backbufferPtr) & DisplayPixelFormat.dwBBitMask;

											unsigned int fontR = (int)(*fontimagePtr) & DisplayPixelFormat.dwRBitMask;
											unsigned int fontG = (int)(*fontimagePtr) & DisplayPixelFormat.dwGBitMask;
											unsigned int fontB = (int)(*fontimagePtr) & DisplayPixelFormat.dwBBitMask;

											backR += MUL_FIXED(r,fontR);
											if (backR>DisplayPixelFormat.dwRBitMask) backR = DisplayPixelFormat.dwRBitMask;
											else backR &= DisplayPixelFormat.dwRBitMask;

											backG += MUL_FIXED(r,fontG);
											if (backG>DisplayPixelFormat.dwGBitMask) backG = DisplayPixelFormat.dwGBitMask;
											else backG &= DisplayPixelFormat.dwGBitMask;

											backB += MUL_FIXED(r,fontB);
											if (backB>DisplayPixelFormat.dwBBitMask) backB = DisplayPixelFormat.dwBBitMask;
											else backB &= DisplayPixelFormat.dwBBitMask;

											*backbufferPtr = (short)(backR|backG|backB);
										}
										fontimagePtr++;
										backbufferPtr++;
									}
									screenY++;
									fontimageRowStartPtr += fontimagePitchInShorts;
									backbufferRowStartPtr += backbufferPitchInShorts;
								}
							}
						   	
						}
					}
				}
			}

		}

		R2Pos_Cursor . x += 1+GetXInc ( ProjCh,  *(pProjChar_I+1) );
            // this takes responsibility for updating cursor pos,
            // rather than the RenderChar function

		pProjChar_I++;
	}
	image_ptr->Unlock((LPVOID)ddsdimage.lpSurface);
}


// :: RenderString_Unclipped ()
void IndexedFont_Kerned :: RenderString_Unclipped
(
	struct r2pos& R2Pos_Cursor,
	int FixP_Alpha,
	const SCString& SCStr
) const

{
	ProjChar* pProjChar_I = SCStr . pProjCh();

	const LPDIRECTDRAWSURFACE image_ptr = GetImagePtr();

	DDSURFACEDESC ddsdimage;
	
	memset(&ddsdimage, 0, sizeof(ddsdimage));
	ddsdimage.dwSize = sizeof(ddsdimage);

	/* lock the image */
	while (image_ptr->Lock(NULL, &ddsdimage, DDLOCK_WAIT, NULL) == DDERR_WASSTILLDRAWING);

	while ( *pProjChar_I )
	{
		const ProjChar ProjCh = *pProjChar_I;

		if ( bCanRender( ProjCh ) )
		{

			if (ProjCh != ' ')
			{
				unsigned int theOffset=ProjCh - 32;
				int width = GetWidth(ProjCh);

				{
					if ( width>0 )
					{
						{
							// This code adapted from DrawGraphicWithAlphaChannel();
							// it assumes you're in a 16-bit mode...

							// okay, now we have the surfaces, we can copy from one to the other,
							// darkening pixels as we go
							{
								long fontimagePitchInShorts = (ddsdimage.lPitch/2); 
								long backbufferPitchInShorts = (BackBufferPitch/2); 

								unsigned short* fontimageRowStartPtr =
								(
									((unsigned short *)ddsdimage.lpSurface)
									+
									(GetHeight()*theOffset*fontimagePitchInShorts)
								);

								unsigned short* backbufferRowStartPtr =
								(
									((unsigned short *)ScreenBuffer)
									+
									(R2Pos_Cursor.y*backbufferPitchInShorts)
									+
									(R2Pos_Cursor.x)
								);
								int screenY = R2Pos_Cursor.y;

								for (int yCount=GetHeight(); yCount>0; yCount--)
								{
									unsigned short* fontimagePtr = fontimageRowStartPtr;
									unsigned short* backbufferPtr = backbufferRowStartPtr;

									int xIndex = R2Pos_Cursor.x+CloakingPhase/64;
									int yIndex = (screenY+CloakingPhase/128)&127;

									for (int xCount=width; xCount>0;xCount--)
									{
										int r = CloudTable[(xCount+xIndex)&127][yIndex];
										r = MUL_FIXED(FixP_Alpha,r);
										if (*fontimagePtr)
										{
											unsigned int backR = (int)(*backbufferPtr) & DisplayPixelFormat.dwRBitMask;
											unsigned int backG = (int)(*backbufferPtr) & DisplayPixelFormat.dwGBitMask;
											unsigned int backB = (int)(*backbufferPtr) & DisplayPixelFormat.dwBBitMask;

											unsigned int fontR = (int)(*fontimagePtr) & DisplayPixelFormat.dwRBitMask;
											unsigned int fontG = (int)(*fontimagePtr) & DisplayPixelFormat.dwGBitMask;
											unsigned int fontB = (int)(*fontimagePtr) & DisplayPixelFormat.dwBBitMask;

											backR += MUL_FIXED(r,fontR);
											if (backR>DisplayPixelFormat.dwRBitMask) backR = DisplayPixelFormat.dwRBitMask;
											else backR &= DisplayPixelFormat.dwRBitMask;

											backG += MUL_FIXED(r,fontG);
											if (backG>DisplayPixelFormat.dwGBitMask) backG = DisplayPixelFormat.dwGBitMask;
											else backG &= DisplayPixelFormat.dwGBitMask;

											backB += MUL_FIXED(r,fontB);
											if (backB>DisplayPixelFormat.dwBBitMask) backB = DisplayPixelFormat.dwBBitMask;
											else backB &= DisplayPixelFormat.dwBBitMask;

											*backbufferPtr = (short)(backR|backG|backB);
										}
										fontimagePtr++;
										backbufferPtr++;
									}
									screenY++;
									fontimageRowStartPtr += fontimagePitchInShorts;
									backbufferRowStartPtr += backbufferPitchInShorts;
								}
							}
						   	
						}
					}
				}
			}

		}

		R2Pos_Cursor.x  += 1+GetXInc ( ProjCh, *(pProjChar_I+1) );
            // this takes responsibility for updating cursor pos,
            // rather than the RenderChar function

		pProjChar_I++;
	}
	image_ptr->Unlock((LPVOID)ddsdimage.lpSurface);
}


// The string CalcSize() functions are implemented at this level
// For this class, it's not just done by adding together the sizes for
// the characters:
// :: CalcSize ()
r2size IndexedFont_Kerned :: CalcSize (	ProjChar* pProjCh ) const
{
	GLOBALASSERT(pProjCh);

	r2size R2Size_Return	( 0, GetHeight() );

	while (*pProjCh)
	{
		R2Size_Return.w  += 1+GetXInc ( *(pProjCh), *(pProjCh+1) );
			// note how the final non-null character has a call to GetXInc
			// with the null character as its successor

		pProjCh++;
	}

	return R2Size_Return;
}

// :: CalcSize ()
r2size IndexedFont_Kerned :: CalcSize (	ProjChar* pProjCh, int MaxChars ) const
{
	GLOBALASSERT(pProjCh);
	GLOBALASSERT( MaxChars >=0 );

	r2size R2Size_Return ( 0, GetHeight() );

	while	( (*pProjCh) && ((MaxChars--)>0) )
	{
		R2Size_Return.w += 1+GetXInc ( *(pProjCh), *(pProjCh+1) );
			// note how the final non-null character has a call to GetXInc
			// with the null character as its successor

		pProjCh++;
	}

	return R2Size_Return;
}

// :: RenderChar_Clipped ()
void IndexedFont_Proportional_PF :: RenderChar_Clipped
(
	struct r2pos& R2Pos_Cursor,
	const struct r2rect& R2Rect_Clip,
	int, // FixP_Alpha,
	ProjChar ProjCh
) const
{

	if (bCanRender( ProjCh ) )
	{
		// For the moment, only render characters that are fully on-screen:
		if ( r2rect (R2Pos_Cursor, GetWidth(ProjCh), GetHeight() ).bFitsIn( R2Rect_Clip ) ) 
		{
			textprintXY (R2Pos_Cursor.x, R2Pos_Cursor.y, "%c", ProjCh );
			R2Pos_Cursor . x += 1+GetWidth(ProjCh);
							
		}
		else
		{
			R2Pos_Cursor.x += GetWidth( ProjCh );
		}
	}
}



// :: RenderChar_Unclipped ()
void IndexedFont_Proportional_PF :: RenderChar_Unclipped
(
	struct r2pos& R2Pos_Cursor,
/* adj Do you see the comma after int ? */
	int, // FixP_Alpha,
	ProjChar ProjCh
) const
{
	// Check it's already been properly clipped:
	// (using direct calculation of char size)
	GLOBALASSERT ( r2rect (R2Pos_Cursor, CalcSize( ProjCh ) ).bValidPhys() );
	if (bCanRender( ProjCh ) )
	{
		textprintXY ( R2Pos_Cursor.x, R2Pos_Cursor.y, "%c", ProjCh );
		R2Pos_Cursor . x += 1+GetWidth(ProjCh);
	}
	
}



// :: PFLoadHook () 
void IndexedFont_Proportional_PF :: PFLoadHook ( FontIndex I_Font_New, PFFONT *pffont_New )
{
	new IndexedFont_Proportional_PF	( I_Font_New, pffont_New );
	SCString :: UpdateAfterFontChange( I_Font_New );
}



// :: PFUnLoadHook ()
void IndexedFont_Proportional_PF :: PFUnLoadHook ( FontIndex )
{ /* adj stub */
}



// private:
// :: IndexedFont_Proportional_PF ()
IndexedFont_Proportional_PF :: IndexedFont_Proportional_PF ( FontIndex I_Font_New, PFFONT *pffont_New ) : IndexedFont_Proportional (I_Font_New), MaxWidth_Val(0)
{
	GLOBALASSERT( pffont_New );

	pffont_Val = pffont_New;

	// Calculate MaxWidth_Val:
	int i=(pffont_New -> num_chars_in_font);
	int Offset = pffont_New -> GetOffset();
		
	while (i>0)
		{
			i--;
			int ThisWidth = pffont_Val->GetWidth ( (ProjChar) (i + Offset ) );
			if ( MaxWidth_Val < ThisWidth)
			{
				MaxWidth_Val = ThisWidth;
			}
		}
	
}




void INDEXFNT_PFLoadHook ( FontIndex I_Font_New, PFFONT *pffont_New)
{
	GLOBALASSERT( I_Font_New < IndexedFonts_MAX_NUMBER_OF_FONTS );
	GLOBALASSERT( pffont_New );

	IndexedFont_Proportional_PF :: PFLoadHook ( I_Font_New,	pffont_New );

}

// :: RenderString_Clipped ()
void IndexedFont_HUD :: RenderString_Clipped
(
	struct r2pos& R2Pos_Cursor,
	const struct r2rect& R2Rect_Clip,
	int FixP_Alpha,// FixP_Alpha,
	const SCString& SCStr
) const
{
	/* KJL 16:16:26 16/04/98 - if you're completely off-screen, go away */
	if (R2Pos_Cursor . y<=-HUD_FONT_HEIGHT) return;

	{
		ProjChar* pProjChar_I = SCStr . pProjCh();

		if (R2Pos_Cursor . y<=0)
		{
			D3D_RenderHUDString_Clipped(pProjChar_I,R2Pos_Cursor.x,R2Pos_Cursor.y,(255<<24)+(192<<16)+(192<<8)+(192));
		}
		else
		D3D_RenderHUDString(pProjChar_I,R2Pos_Cursor.x,R2Pos_Cursor.y,(255<<24)+(192<<16)+(192<<8)+(192));
	}
}


// :: RenderString_Unclipped ()
void IndexedFont_HUD :: RenderString_Unclipped
(
	struct r2pos& R2Pos_Cursor,
	int FixP_Alpha, //  FixP_Alpha,
	const SCString& SCStr
) const
{
/* adj * stub */
	LOCALASSERT(0);
}



// :: CalcSize ()
r2size IndexedFont_HUD :: CalcSize ( ProjChar* pProjCh ) const
{
	GLOBALASSERT( pProjCh );

	r2size R2Size_Return ( 0, GetHeight() );

	if (*pProjCh)
	{
		// non-empty strings have one pixel space between characters; the if/do/while
		// construction is to take one off the final result for a non-empty string
		do
		{
			R2Size_Return.w += GetWidth( *pProjCh ) + 1;

			pProjCh++;
		} while (*pProjCh);

		R2Size_Return . w --;
	}

	return R2Size_Return;
}
// :: RenderChar_Clipped ()
void IndexedFont_HUD :: RenderChar_Clipped
(
	struct r2pos& R2Pos_Cursor,
	const struct r2rect& R2Rect_Clip,
	int, // FixP_Alpha,
	ProjChar ProjCh
) const
{
	HUDCharDesc charDesc;
	charDesc.Y = R2Pos_Cursor . y;

	charDesc.Red = 255;
	charDesc.Green = 255;
	charDesc.Blue = 255;
	charDesc.Alpha= 255;

	charDesc.Character=ProjCh;
	charDesc.X = R2Pos_Cursor . x;			
	D3D_DrawHUDFontCharacter(&charDesc);
	R2Pos_Cursor . x += GetWidth(ProjCh);
}
// ::RenderChar_Unclipped ()
void IndexedFont_HUD :: RenderChar_Unclipped
(
	struct r2pos& R2Pos_Cursor,
	int, // FixP_Alpha,
	ProjChar ProjCh
) const
{
	// Check it's already been properly clipped:
	// (using direct calculation of char size)
	GLOBALASSERT ( r2rect (	R2Pos_Cursor,	CalcSize( ProjCh )  ).bValidPhys() );

	HUDCharDesc charDesc;
	charDesc.Character=ProjCh;
	charDesc.X = R2Pos_Cursor . x;			
	charDesc.Y = R2Pos_Cursor . y;

	charDesc.Red = FastRandom()&255;
	charDesc.Green = FastRandom()&255;
	charDesc.Blue = FastRandom()&255;
	charDesc.Alpha=255;

	D3D_DrawHUDFontCharacter(&charDesc);
	R2Pos_Cursor . x += 1+GetWidth(ProjCh);
	
}


