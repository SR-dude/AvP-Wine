/*******************************************************************
 *
 *    DESCRIPTION: 	conssym.cpp
 *
 *    AUTHOR: David Malcolm
 *
 *    HISTORY:  Created 26/1/98
 *
 *******************************************************************/

/* Includes ********************************************************/
#include "3dc.h"
#include "conssym.hpp"
#define UseLocalAssert Yes
#include "ourasert.h"


/* Exported globals ************************************************/
	/*static*/ List <ConsoleSymbol*> ConsoleSymbol :: List_pConsoleSym;


/* Exported function definitions ***********************************/
// class ConsoleSymbol
// public:

// protected:
ConsoleSymbol :: ConsoleSymbol
(
	ProjChar* pProjCh_ToUse
) :	pSCString_Symbol
	(
		new SCString( pProjCh_ToUse )
			// constructor for the SCString adds the required reference
	)
{
    List_pConsoleSym . add_entry
    (
    	this
    );
}

ConsoleSymbol :: ~ConsoleSymbol()
{
	pSCString_Symbol ->R_Release();

	// remove from the list
    List_pConsoleSym . delete_entry
    (
    	this
    );
}


