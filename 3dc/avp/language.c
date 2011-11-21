/*KJL***************************************
*    Language Internationalization Code    *
***************************************KJL*/
#include "3dc.h"
#include "inline.h"
#include "module.h"
#include "gamedef.h"
#include "langenum.h"
#include "language.h"
#include "huffman.hpp"
	// DHM 12 Nov 97: hooks for C++ string handling code:
#include "strtab.hpp"

#define UseLocalAssert Yes
#include "ourasert.h"
#include "avp_menus.h"

static char EmptyString[]="";

static char *TextStringPtr[MAX_NO_OF_TEXTSTRINGS]={&EmptyString,};
static char *TextBufferPtr;

void InitTextStrings(void)
{
	char *textPtr;
	int i;

	/* language select here! */
	GLOBALASSERT(AvP.Language>=0);
	GLOBALASSERT(AvP.Language<I_MAX_NO_OF_LANGUAGES);
	
	TextBufferPtr = LoadTextFile("language.txt");
/* adj */
//	TextBufferPtr = LoadTextFile(LanguageFilename[AvP.Language]);
	
	LOCALASSERT(TextBufferPtr);

	if (!strncmp (TextBufferPtr, "REBCRIF1", 8))
	{
		textPtr = (char*)HuffmanDecompress((HuffmanPackage*)(TextBufferPtr)); 		
		DeallocateMem(TextBufferPtr);
		TextBufferPtr=textPtr;
	}
	else
	{
		textPtr = TextBufferPtr;
	}

	AddToTable( &EmptyString );

	for (i=1; i<MAX_NO_OF_TEXTSTRINGS; i++)
	{	
		/* scan for a quote mark */
		while (*textPtr++ != '"');

		/* now pointing to a text string after quote mark*/
		TextStringPtr[i] = textPtr;

		/* scan for a quote mark */
		while (*textPtr != '"')
		{	
			textPtr++;
		}

		/* change quote mark to zero terminator */
		*textPtr = 0;

		AddToTable( TextStringPtr[i] );
	}
}
void KillTextStrings(void)
{
	UnloadTextFile(LanguageFilename[AvP.Language],TextBufferPtr);

	UnloadTable();
}

char *GetTextString(enum TEXTSTRING_ID stringID)
{
	LOCALASSERT(stringID<MAX_NO_OF_TEXTSTRINGS);

	return TextStringPtr[stringID];
}


