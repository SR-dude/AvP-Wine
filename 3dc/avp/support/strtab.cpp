/*******************************************************************
 *
 *    DESCRIPTION: 	strtab.cpp
 *
 *    AUTHOR: David Malcolm
 *
 *    HISTORY:  Created 12 Nov 97
 *
 *******************************************************************/

/* Includes ********************************************************/
#include "3dc.h"
#include "strtab.hpp"
#include "ourasert.h"


/* Exported globals ************************************************/

SCString* StringTable :: pSCString[ MAX_NO_OF_TEXTSTRINGS ];
unsigned int StringTable :: NumStrings = 0;



/* Exported function definitions ***********************************/
// class StringTable
// public:
 SCString& StringTable :: GetSCString (	enum TEXTSTRING_ID stringID )
{
	GLOBALASSERT( stringID < MAX_NO_OF_TEXTSTRINGS );
	GLOBALASSERT( pSCString[ stringID ] );

	pSCString[ stringID ] -> R_AddRef();
	return *( pSCString[ stringID ] );

}


void StringTable :: Add( ProjChar* pProjChar )
{
	GLOBALASSERT( NumStrings < MAX_NO_OF_TEXTSTRINGS );
	GLOBALASSERT( pSCString[ NumStrings ] == 0 );

	pSCString[ NumStrings++ ] = new SCString( pProjChar );

}


void StringTable :: Unload(void)
{
	while ( NumStrings )
		pSCString[ --NumStrings ] -> R_Release();

}



// C-callable functions:

extern void AddToTable( ProjChar* pProjChar )
{
	GLOBALASSERT( pProjChar );
	
	StringTable :: Add( pProjChar );

}

extern void UnloadTable(void)
{

	StringTable :: Unload();

}


