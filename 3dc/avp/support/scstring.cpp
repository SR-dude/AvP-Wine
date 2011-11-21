/*******************************************************************
 *
 *    DESCRIPTION: 	string.cpp
 *
 *    AUTHOR: David Malcolm
 *
 *    HISTORY:  Created ages ago
 *
 *******************************************************************/

/* Includes ********************************************************/
#include "3dc.h"
#include "scstring.hpp"
#include "strutil.h"
#include "indexfnt.hpp"

#define UseLocalAssert Yes
#include "ourasert.h"

#define MAX_BYTES_IN_NUMERIC_STRING	(100)



/* Imported data ***************************************************/
#ifdef __cplusplus
	extern "C"
	{
#endif
		extern DISPLAYBLOCK *Player;

#ifdef __cplusplus
	};
#endif


/* Exported globals ************************************************/
SCString* SCString :: pFirst = NULL;


/* Exported function definitions ***********************************/
// class SCString
// public:

ProjChar* SCString :: pProjCh(void) const
{
	GLOBALASSERT( pProjCh_Val );
	return pProjCh_Val;
}


SCString :: SCString (	const ProjChar* pProjCh_Init )
{
	GLOBALASSERT( pProjCh_Init );
	AllocatedSize = (size_t) STRUTIL_SC_NumBytes (	pProjCh_Init );

	pProjCh_Val = new ProjChar[	AllocatedSize ];
	GLOBALASSERT( pProjCh_Val );

	STRUTIL_SC_StrCpy ( pProjCh_Val, pProjCh_Init );

	NumberOfCharacters = ( AllocatedSize / sizeof(ProjChar ) ) - 1;
	// doesn't include NULL terminator

	{
		FontIndex i = IndexedFonts_MAX_NUMBER_OF_FONTS; 
		
		while ( i>0 )
		{
			i = (FontIndex)(i-1);

			IndexedFont* pFont = IndexedFont :: GetFont( i );
				
			if ( pFont )
			{
				R2Size[ i ] = pFont -> CalcSize ( pProjCh_Val );
				bCanRender[ i ] = pFont -> bCanRenderFully ( pProjCh_Val );
			}
			else
			{
				R2Size[ i ] = r2size(0,0);
				bCanRender[ i ] = No;					
			}
		}
	}

	// Insert at head of list:
	{
		if ( pFirst )	pFirst -> pPrv = this;
			
		pNxt = pFirst;
		pPrv = NULL;

		pFirst = this;
	}
}

SCString :: SCString (	signed int Number )
{
	// forms a new string object that describes the number passed
	// standard decimal representation


	ProjChar pProjCh_Init[ MAX_BYTES_IN_NUMERIC_STRING ];

	sprintf	(pProjCh_Init, "%i", (int)Number );

	AllocatedSize = (size_t) STRUTIL_SC_NumBytes (pProjCh_Init);

	pProjCh_Val = new ProjChar[AllocatedSize ];
	GLOBALASSERT( pProjCh_Val );
		// this is always "owned" by the String
	STRUTIL_SC_StrCpy ( pProjCh_Val, pProjCh_Init );

	NumberOfCharacters = ( AllocatedSize / sizeof(ProjChar ) ) - 1;
			// doesn't include NULL terminator

	FontIndex i = IndexedFonts_MAX_NUMBER_OF_FONTS; 
		
	while ( i>0 )
	{
		i = (FontIndex)(i-1);
		IndexedFont* pFont = IndexedFont :: GetFont( i );
				
		if ( pFont )
		{
			R2Size[ i ] = pFont -> CalcSize (pProjCh_Val);
			bCanRender[ i ] = pFont -> bCanRenderFully ( pProjCh_Val);
		}
		else
		{
			R2Size[ i ] = r2size(0,0);
			bCanRender[ i ] = No;					
		}
	}

	// Insert at head of list:
	if ( pFirst )
	{
		pFirst -> pPrv = this;
	}
			
	pNxt = pFirst;
	pPrv = NULL;

	pFirst = this;
	

	
}

SCString :: SCString (	unsigned int Number )
{
	// forms a new string object that describes the number passed
	// standard decimal representation

	ProjChar pProjCh_Init[ MAX_BYTES_IN_NUMERIC_STRING ];

	sprintf	(pProjCh_Init, "%u", Number);

	AllocatedSize = (size_t) STRUTIL_SC_NumBytes (pProjCh_Init);

	pProjCh_Val = new ProjChar[AllocatedSize ];
	GLOBALASSERT( pProjCh_Val );
	// this is always "owned" by the String

	STRUTIL_SC_StrCpy (pProjCh_Val,	pProjCh_Init );

	NumberOfCharacters = ( AllocatedSize / sizeof(ProjChar ) ) - 1;
	// doesn't include NULL terminator

	FontIndex i = IndexedFonts_MAX_NUMBER_OF_FONTS; 
	
	while ( i>0 )
		{
			i = (FontIndex)(i-1);

			IndexedFont* pFont = IndexedFont :: GetFont( i );
				
			if ( pFont )
				{
					R2Size[ i ] = pFont -> CalcSize (pProjCh_Val);
					bCanRender[ i ] = pFont -> bCanRenderFully (pProjCh_Val	);
				}
			else
				{
					R2Size[ i ] = r2size(0,0);
					bCanRender[ i ] = No;					
				}
		}

		// Insert at head of list:
	if ( pFirst )	pFirst -> pPrv = this;
			
	pNxt = pFirst;
	pPrv = NULL;
	pFirst = this;


}

SCString :: SCString (	float Number )
{
	ProjChar pProjCh_Init[ MAX_BYTES_IN_NUMERIC_STRING ];

	sprintf	(pProjCh_Init,	"%6f",	Number	);

	AllocatedSize = (size_t) STRUTIL_SC_NumBytes (pProjCh_Init);

	pProjCh_Val = new ProjChar[	AllocatedSize ];
	GLOBALASSERT( pProjCh_Val );
	// this is always "owned" by the String

	STRUTIL_SC_StrCpy(pProjCh_Val,	pProjCh_Init);

	NumberOfCharacters = ( AllocatedSize / sizeof(ProjChar ) ) - 1;
	// doesn't include NULL terminator

	FontIndex i = IndexedFonts_MAX_NUMBER_OF_FONTS; 
		
	while ( i>0 )
		{
		i = (FontIndex)(i-1);

		IndexedFont* pFont = IndexedFont :: GetFont( i );
				
		if ( pFont )
			{
				R2Size[ i ] = pFont -> CalcSize	(pProjCh_Val);
				bCanRender[ i ] = pFont -> bCanRenderFully ( pProjCh_Val );
			}
		else
			{
					R2Size[ i ] = r2size(0,0);
					bCanRender[ i ] = No;					
			}
		}

	// Insert at head of list:
	if ( pFirst )	pFirst -> pPrv = this;
			
	pNxt = pFirst;
	pPrv = NULL;
	pFirst = this;

}



SCString :: SCString (ProjChar* pProjCh_Init, unsigned int Length )
{
	// Forms a string of length at most Length (with 1 extra for NULL-terminator)

	GLOBALASSERT( pProjCh_Init );
	NumberOfCharacters = STRUTIL_SC_Strlen( pProjCh_Init );
	// doesn't include NULL terminator

	if ( NumberOfCharacters > Length ) NumberOfCharacters = Length;

	AllocatedSize = (size_t) STRUTIL_SC_NumBytes (	pProjCh_Init );

	size_t TruncSize = sizeof(ProjChar) * (Length + 1);

	if (AllocatedSize > TruncSize )	AllocatedSize = TruncSize;

	pProjCh_Val = new ProjChar[	AllocatedSize ];
	GLOBALASSERT( pProjCh_Val );
	// this is always "owned" by the String

	STRUTIL_SC_SafeCopy ( pProjCh_Val, (NumberOfCharacters+1), pProjCh_Init	);

	FontIndex i = IndexedFonts_MAX_NUMBER_OF_FONTS; 
	
	while ( i>0 )
		{
			i = (FontIndex)(i-1);

			IndexedFont* pFont = IndexedFont :: GetFont( i );
				
			if ( pFont )
				{
					R2Size[ i ] = pFont -> CalcSize	(pProjCh_Val);
					bCanRender[ i ] = pFont -> bCanRenderFully ( pProjCh_Val );
				}
			else
				{
					R2Size[ i ] = r2size(0,0);
					bCanRender[ i ] = No;					
				}
		}

	// Insert at head of list:
	if ( pFirst )	pFirst -> pPrv = this;
			
	pNxt = pFirst;
	pPrv = NULL;
	pFirst = this;

}


SCString :: SCString (	SCString* pStringObj_0,	SCString* pStringObj_1 )
{
	// forms a new string object by concatenating the strings in 0
	// and 1

	GLOBALASSERT( pStringObj_0 );
	GLOBALASSERT( pStringObj_1 );

	ProjChar* pProjCh_Init_0 = pStringObj_0 -> pProjCh();
	GLOBALASSERT( pProjCh_Init_0 );
		
	ProjChar* pProjCh_Init_1 = pStringObj_1 -> pProjCh();
	GLOBALASSERT( pProjCh_Init_1 );
		
	AllocatedSize = (size_t) ( STRUTIL_SC_NumBytes (pProjCh_Init_0	) + STRUTIL_SC_NumBytes ( pProjCh_Init_1 )- sizeof(ProjChar));
	// only one null terminator needed				

	pProjCh_Val = new ProjChar[	AllocatedSize ];
	GLOBALASSERT( pProjCh_Val );

	// this is always "owned" by the String
	STRUTIL_SC_FastCat ( pProjCh_Val, pProjCh_Init_0, pProjCh_Init_1 );

	NumberOfCharacters = ( AllocatedSize / sizeof(ProjChar ) ) - 1;
	// doesn't include NULL terminator

	FontIndex i = IndexedFonts_MAX_NUMBER_OF_FONTS; 
		
	while ( i>0 )
		{
			i = (FontIndex)(i-1);

			R2Size[ i ] = pStringObj_0 -> CalcSize( i );
			R2Size[ i ] . w += pStringObj_1 -> CalcSize( i ) . w;

			bCanRender[ i ] = ( pStringObj_0 -> bCanRenderFully( i ) && pStringObj_1 -> bCanRenderFully( i ) );				
		}

		// Insert at head of list:
	if ( pFirst ) pFirst -> pPrv = this;
			
	pNxt = pFirst;
	pPrv = NULL;
	pFirst = this;
	
}


SCString :: SCString ( SCString* pStringObj_0,	SCString* pStringObj_1,	SCString* pStringObj_2 )
{
	// forms a new string object by concatenating the strings in 0, 1 and 2

	GLOBALASSERT( pStringObj_0 );
	GLOBALASSERT( pStringObj_1 );
	GLOBALASSERT( pStringObj_2 );

	// Insert at head of list:
	if ( pFirst ) pFirst -> pPrv = this;

	pNxt = pFirst;
	pPrv = NULL;
	pFirst = this;

	SCString* pStringObj_Intermediate = new SCString ( pStringObj_1, pStringObj_2 );
		

	ProjChar* pProjCh_Init_0 = pStringObj_0 -> pProjCh();
	GLOBALASSERT( pProjCh_Init_0 );
			
	ProjChar* pProjCh_Intermediate = pStringObj_Intermediate -> pProjCh();
	GLOBALASSERT( pProjCh_Intermediate );
			
	AllocatedSize = (size_t) ( STRUTIL_SC_NumBytes(	pProjCh_Init_0) + STRUTIL_SC_NumBytes(pProjCh_Intermediate) - sizeof(ProjChar) );
	// only one null terminator needed				
			

	pProjCh_Val = new ProjChar[	AllocatedSize ];
	GLOBALASSERT( pProjCh_Val );
	// this is always "owned" by the String
	STRUTIL_SC_FastCat (pProjCh_Val, pProjCh_Init_0, pProjCh_Intermediate );

	NumberOfCharacters = ( AllocatedSize / sizeof(ProjChar ) ) - 1;
	// doesn't include NULL terminator

			
	FontIndex i = IndexedFonts_MAX_NUMBER_OF_FONTS; 
				
	while ( i>0 )
		{
			i = (FontIndex)(i-1);
			R2Size[ i ] = pStringObj_0 -> CalcSize( i );
			R2Size[ i ].w += ( pStringObj_1 -> CalcSize(i).w + pStringObj_2 -> CalcSize(i).w );

			bCanRender[ i ] = ( pStringObj_0->bCanRenderFully(i) && pStringObj_1 -> bCanRenderFully(i) && pStringObj_2 -> bCanRenderFully(i) );
		}
			
	pStringObj_Intermediate -> R_Release();
	
}

SCString :: SCString( SCString* pStringObj_0, SCString* pStringObj_1, SCString* pStringObj_2, SCString* pStringObj_3)
{
	GLOBALASSERT( pStringObj_0 );
	GLOBALASSERT( pStringObj_1 );
	GLOBALASSERT( pStringObj_2 );
	GLOBALASSERT( pStringObj_3 );

	// Insert at head of list:
	if ( pFirst ) pFirst -> pPrv = this;
	
	pNxt = pFirst;
	pPrv = NULL;
	pFirst = this;

	SCString* pStringObj_Intermediate = new SCString( pStringObj_1,	pStringObj_2, pStringObj_3 );

	ProjChar* pProjCh_Init_0 = pStringObj_0 -> pProjCh();
	GLOBALASSERT( pProjCh_Init_0 );
	ProjChar* pProjCh_Intermediate = pStringObj_Intermediate -> pProjCh();
	GLOBALASSERT( pProjCh_Intermediate );
			
	AllocatedSize = (size_t) ( STRUTIL_SC_NumBytes (pProjCh_Init_0)+ STRUTIL_SC_NumBytes(pProjCh_Intermediate)- sizeof(ProjChar) );
	// only one null terminator needed				
			
	pProjCh_Val = new ProjChar[	AllocatedSize ];
	GLOBALASSERT( pProjCh_Val );
	// this is always "owned" by the String
	STRUTIL_SC_FastCat ( pProjCh_Val, pProjCh_Init_0, pProjCh_Intermediate );

	NumberOfCharacters = ( AllocatedSize / sizeof(ProjChar ) ) - 1;
	// doesn't include NULL terminator
			
	FontIndex i = IndexedFonts_MAX_NUMBER_OF_FONTS; 
			
	while ( i>0 )
		{
			i = (FontIndex)(i-1);

			R2Size[i] = pStringObj_0 -> CalcSize(i);
			R2Size[i].w +=(pStringObj_1->CalcSize(i).w + pStringObj_2->CalcSize(i).w + pStringObj_3->CalcSize(i).w);

			bCanRender[i] =	(pStringObj_0->bCanRenderFully(i) && pStringObj_1->bCanRenderFully(i) && pStringObj_2->bCanRenderFully(i) && pStringObj_3->bCanRenderFully(i));
		}
		
	pStringObj_Intermediate -> R_Release();

}


SCString :: SCString ( SCString* pStringObj_0, SCString* pStringObj_1, SCString* pStringObj_2, SCString* pStringObj_3, SCString* pStringObj_4 )

{
	GLOBALASSERT( pStringObj_0 );
	GLOBALASSERT( pStringObj_1 );
	GLOBALASSERT( pStringObj_2 );
	GLOBALASSERT( pStringObj_3 );
	GLOBALASSERT( pStringObj_4 );

	if ( pFirst ) pFirst -> pPrv = this;
			
	pNxt = pFirst;
	pPrv = NULL;
	pFirst = this;
		
	SCString* pStringObj_Intermediate = new SCString ( pStringObj_1, pStringObj_2, pStringObj_3, pStringObj_4 );

	ProjChar* pProjCh_Init_0 = pStringObj_0 -> pProjCh();
	GLOBALASSERT( pProjCh_Init_0 );
			
	ProjChar* pProjCh_Intermediate = pStringObj_Intermediate -> pProjCh();
	GLOBALASSERT( pProjCh_Intermediate );
			
	AllocatedSize = (size_t) ( STRUTIL_SC_NumBytes ( pProjCh_Init_0 ) + STRUTIL_SC_NumBytes	(pProjCh_Intermediate )	- sizeof(ProjChar) );
	// only one null terminator needed				
			

	pProjCh_Val = new ProjChar[ AllocatedSize ];
	GLOBALASSERT( pProjCh_Val );
	// this is always "owned" by the String
	STRUTIL_SC_FastCat ( pProjCh_Val, pProjCh_Init_0, pProjCh_Intermediate );

	NumberOfCharacters = ( AllocatedSize / sizeof(ProjChar ) ) - 1;
	// doesn't include NULL terminator

			
	FontIndex i = IndexedFonts_MAX_NUMBER_OF_FONTS; 
		
	while ( i>0 )
		{
			i = (FontIndex)(i-1);

			R2Size[i] = pStringObj_0->CalcSize(i);
			R2Size[i].w += (pStringObj_1->CalcSize(i).w + pStringObj_2->CalcSize(i).w + pStringObj_3->CalcSize(i).w + pStringObj_4->CalcSize(i).w);

			bCanRender[i] = (pStringObj_0->bCanRenderFully(i) && pStringObj_1->bCanRenderFully(i) && pStringObj_2->bCanRenderFully(i) && pStringObj_3->bCanRenderFully(i) && pStringObj_4->bCanRenderFully(i));
		}
		
		pStringObj_Intermediate -> R_Release();
	
}


SCString :: SCString (	List<ProjChar> List_ProjChar )
{
	AllocatedSize = (size_t) (List_ProjChar . size() + 1) * sizeof(ProjChar);

	pProjCh_Val = new ProjChar[	AllocatedSize ];
	GLOBALASSERT( pProjCh_Val );
		
	ProjChar* pDst = pProjCh_Val;

	for ( LIF<ProjChar> oi(&(List_ProjChar)); !oi.done(); 	oi.next() )
		{
			*(pDst++) = oi();
		}

	// Write terminator:
	*pDst = 0;
		
	NumberOfCharacters = ( AllocatedSize / sizeof(ProjChar ) ) - 1;
	// doesn't include NULL terminator

		
	FontIndex i = IndexedFonts_MAX_NUMBER_OF_FONTS; 
			
	while ( i>0 )
		{
			i = (FontIndex)(i-1);

			IndexedFont* pFont = IndexedFont :: GetFont( i );
				
			if ( pFont )
				{
					R2Size[i] = pFont->CalcSize ( pProjCh_Val );
					bCanRender[i] = pFont->bCanRenderFully ( pProjCh_Val );
				}
			else
				{
					R2Size[ i ] = r2size(0,0);
					bCanRender[ i ] = No;					
				}
		}
		

		// Insert at head of list:
		
	if ( pFirst )	pFirst -> pPrv = this;
			
	pNxt = pFirst;
	pPrv = NULL;
	pFirst = this;
		
	
}



 void SCString :: UpdateAfterFontChange( FontIndex I_Font_Changed )
{
	// called by the font code whenever fonts are loaded/unloaded

	GLOBALASSERT( I_Font_Changed < IndexedFonts_MAX_NUMBER_OF_FONTS );
	IndexedFont* pFont = IndexedFont :: GetFont( I_Font_Changed );

	SCString* pSCString = pFirst;

	while ( pSCString )
		{
			if ( pFont )
			{
				pSCString->R2Size[I_Font_Changed] = pFont->CalcSize ( pSCString->pProjCh_Val );
				pSCString->bCanRender[I_Font_Changed] = pFont->bCanRenderFully ( pSCString->pProjCh_Val);
			}
			else
			{
				pSCString->R2Size[I_Font_Changed] = r2size(0,0);
				pSCString->bCanRender[I_Font_Changed] = No;					
			}
			
			pSCString = pSCString->pNxt;
		}

}

List<SCString*> SCString :: Parse ( ProjChar* pProjChar_Start )
{
	// takes a string and builds a list of new SCStrings, in which
	// each string in the list consists of non-whitespace characters from
	// the input, and the whitespace is used to separate individual
	// strings

	// I call the strings "words"

	GLOBALASSERT( pProjChar_Start );

	List<SCString*> List_Return;

	ProjChar* pProjChar_Iterate = pProjChar_Start;
	int NumCharsNonWhitespace = 0;

	while (	*pProjChar_Iterate )
		{
			if  ( *pProjChar_Iterate == ' ' )
			{
				// Whitespace:
				if ( NumCharsNonWhitespace > 0 )
				{
					// End of a word; add the string to the list:
					List_Return.add_entry (	new SCString ( pProjChar_Start,	NumCharsNonWhitespace ) );
					NumCharsNonWhitespace = 0;
				}
				else
				{
					// Already processing a block of whitespace; do nothing
				}
			}
			else
			{
				// Non-whitespace:
				if ( NumCharsNonWhitespace > 0 )
				{
					// In the middle of a word:
				}
				else
				{
					// Start of a word:
					pProjChar_Start = pProjChar_Iterate;
				}

				NumCharsNonWhitespace++;

			}

			pProjChar_Iterate++;
		}

		// End of the string; flush any remaining whitespace:
	if ( NumCharsNonWhitespace > 0 )
			List_Return.add_entry (new SCString ( pProjChar_Start, NumCharsNonWhitespace ) );

	return List_Return;
	
}


//private:
SCString :: ~SCString()
{
	GLOBALASSERT( pProjCh_Val );

	delete[] pProjCh_Val;

	// Remove from list:
	if ( pFirst == this )
		{
			pFirst = pNxt;
		}
	else
		{
			pPrv -> pNxt = pNxt;
		}

	if (pNxt)
		{
			pNxt -> pPrv = pPrv;
		}			
		

	
}


