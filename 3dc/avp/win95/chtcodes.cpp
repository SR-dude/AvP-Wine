/*******************************************************************
 *
 *    DESCRIPTION: 	chtcodes.cpp
 *
 *    AUTHOR: David Malcolm
 *
 *    HISTORY:  Created 28/11/97 by moving the cheat code processor
 *				from out of SCSTRING.CPP
 *
 *******************************************************************/

/* Includes ********************************************************/
#include "3dc.h"
#include "scstring.hpp"
#include "gadget.h"
#include "strutil.h"

#include "module.h"
#include "stratdef.h"
#include "dynblock.h"
#include "equipmnt.h"
#include "rootgadg.hpp"
#include "hudgadg.hpp"
#include "modcmds.hpp"
#include "bh_types.h"
#include "consvar.hpp"
#include "missions.hpp"
#include "textexp.hpp"
#include "refobj.hpp"
#include "debuglog.hpp"
#include "trepgadg.hpp"
#include "conscmnd.hpp"
#include "consbind.hpp"

extern "C"
{
#include "weapons.h"
#include "avp_menus.h"
};

#define UseLocalAssert Yes
#include "ourasert.h"

#ifdef __cplusplus
	extern "C"
	{
#endif
extern int DebuggingCommandsActive;
extern int bEnableTextprint;
extern SCENE Global_Scene;
#ifdef __cplusplus
	};
#endif



namespace Cheats
{
	void ToggleImmortality(void);
	void CommitSuicide(void);
};

void SCString :: ProcessAnyCheatCodes(void)
{

	// Processing for the "console variable" system:
	if( ConsoleVariable :: Process( pProjCh_Val ) )
		{
			// then this has been processed; stop
			return;
		}

	// Processing for the "console commands" system:
	if ( ConsoleCommand :: Process( pProjCh_Val ) )
		{
			// then this has been processed; stop
			return;
		}
	// Expansion-related commands:
	if ( 0 == _strnicmp ( pProjCh_Val, "EXP+ ", 5 )	)
		{
			TextExpansion :: AddExpansion (	pProjCh_Val+5  );
			// ProjChar* pProjCh_ToParse
			return;
		}
	if ( 0 == _strnicmp ( pProjCh_Val, "EXP- ", 5 )  )
		{
			TextExpansion :: TryToRemoveExpansion (pProjCh_Val+5);
			// ProjChar* pProjCh_ToParse
			return;
		}

	// Key-binding related commands:
	if ( 0 == _strnicmp ( pProjCh_Val, "BIND ", 5 ) )
		{
			KeyBinding :: ParseBindCommand ( pProjCh_Val+5 );
			// ProjChar* pProjCh_ToParse
			return;
		}
	if ( 0 == _strnicmp ( pProjCh_Val, "UNBIND ", 7 ) )
		{
			KeyBinding :: ParseUnbindCommand ( pProjCh_Val+7  );
			// ProjChar* pProjCh_ToParse
			return;
		}
	if (DebuggingCommandsActive)
	{
		if ( STRUTIL_SC_Strequal ( pProjCh_Val,	"GOD" /*ProjChar* pProjCh_2*/ ) )
		{
			Cheats :: ToggleImmortality();
			return;
		}
	}
	// Module commands:
	if ( 0 == _strnicmp ( pProjCh_Val, "MODULE ", 7	) )
		{
			// Then pass the rest of the string as an argument to the module
			// teleport code:

			ModuleCommands :: TryToTeleport ( pProjCh_Val+7 );
			/*char* UpperCasePotentialModuleName*/

			return;
		}
	if ( 0 == _strnicmp ( pProjCh_Val, "MOD ", 4 ) )
		{
			// Then pass the rest of the string as an argument to the module
			// teleport code:

			ModuleCommands :: TryToTeleport ( pProjCh_Val+4 );
			//char* UpperCasePotentialModuleName
			return;
		}
	// End Module Commands



	if ( STRUTIL_SC_Strequal_Insensitive (	pProjCh_Val, "SUICIDE" /*ProjChar* pProjCh_2*/ ) )
	{
		Cheats :: CommitSuicide();
		return;
	}



	if ( STRUTIL_SC_Strequal_Insensitive ( pProjCh_Val, "DONE IT" /*ProjChar* pProjCh_2*/ ) )
	{
		MissionObjective :: TestCompleteNext();
	}


}

/* Internal function definitions ***********************************/
// namespace Cheats
void Cheats :: ToggleImmortality(void)
{
	// immortality cheat
	if ( PlayerStatusPtr->IsImmortal )
	{
		GADGET_NewOnScreenMessage("IMMORTALITY DISABLED");
		PlayerStatusPtr->IsImmortal = 0;
	}
	else
	{
		GADGET_NewOnScreenMessage("IMMORTALITY ENABLED");
		PlayerStatusPtr->IsImmortal = 1;
	}		
}

void Cheats :: CommitSuicide(void)
{
	// First, disable immortality:
	PlayerStatusPtr->IsImmortal = 0;


	// Then apply lots of damage:		

	CauseDamageToObject(Player->ObStrategyBlock, &certainDeath, ONE_FIXED,NULL);
}
