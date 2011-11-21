/*******************************************************************
 *
 *    DESCRIPTION: 	davehook.cpp
 *
 *    AUTHOR: David Malcolm
 *
 *    HISTORY:  Created 18/11/97
 *
 *******************************************************************/

/* Includes ********************************************************/
#include "3dc.h"
#include "davehook.h"
#include "r2base.h"
	// hooks to R2 code
#include "gadget.h"
	// hooks to gadgets code
#include "daemon.h"
	// hooks to daemon code
#include "rentrntq.h"
#include "iofocus.h"
#include "font.h"
#include "hudgadg.hpp"
#include "consvar.hpp"
#include "conscmnd.hpp"
#include "missions.hpp"
#include "rebmenus.hpp"
#include "indexfnt.hpp"
	// Includes for console variables:
#include "textexp.hpp"
	// Includes for console commands:
#include "consvar.hpp"
#include "modcmds.hpp"
#include "trepgadg.hpp"
#include "consbind.hpp"
#include "consbtch.hpp"

#define UseLocalAssert Yes
#include "ourasert.h"
#include "frontend/avp_menus.h"
/* Version settings ************************************************/

/* Constants *******************************************************/
	#define DEFAULT_KEY_STATUS_PANEL_WEAPONS ( KEY_TAB )
	#define DEFAULT_KEY_STATUS_PANEL_INVENTORY ( KEY_V )
	#define DEFAULT_KEY_STATUS_PANEL_OBJECTIVES ( KEY_O )
	#define DEFAULT_KEY_STATUS_PANEL_GAMESTATS ( KEY_G )

/* Macros **********************************************************/

/* Imported function prototypes ************************************/

/* Imported data ***************************************************/
#ifdef __cplusplus
	extern "C"
	{
#endif

		extern unsigned char KeyboardInput[];

		extern SCREENDESCRIPTORBLOCK ScreenDescriptorBlock;
		extern int VideoModeColourDepth;

		extern int bEnableTextprint;
		extern int bEnableTextprintXY;
		extern signed int HUDTranslucencyLevel;


#ifdef __cplusplus
	};
#endif



/* Internal function prototypes ************************************/
	namespace Testing
	{
		void VVTest(void);
		void VITest(int);
		int IVTest(void);
		int IITest(int);

		void DumpRefCounts(void);
		void DumpVideoMode(void);
	};

	static void davehook_HandleStatusPanelControls(void);

	static int bFirstFrame = No;


/* Exported function definitions ***********************************/
void ConsoleVariable :: CreateAll(void)
{
	// hook to create all the console variables
	// (to make it easy to add new ones)


	MakeSimpleConsoleVariable_Int
	(
		TextExpansion ::  bVerbose, // int& Value_ToUse,
		"EXPV", // ProjChar* pProjCh_ToUse,
		"(VERBOSE REPORTS OF TEXT EXPANSIONS)", // ProjChar* pProjCh_Description_ToUse
		0, // int MinVal_New,
		1  // int MaxVal_New
	);



}

void ConsoleCommand :: CreateAll(void)
{
	Make
	(
		"LISTCMD",
		"LIST ALL CONSOLE COMMANDS",
		ListAll
	);

	Make
	(
		"LISTEXP",
		"LIST ALL TEXT EXPANSIONS",
		TextExpansion :: ListAll
	);

	Make
	(
		"LISTVAR",
		"LIST ALL CONSOLE VARIABLES",
		ConsoleVariable :: ListAllVariables
	);



	Make
	(
		"LISTBIND",
		"LIST ALL KEY BINDINGS",
		KeyBinding::ListAllBindings
	);
	Make
	(
		"UNBIND-ALL",
		"GET RID OF ALL KEY BINDINGS",
		KeyBinding::UnbindAll
	);


}

void DAVEHOOK_Init(void)
{
	SCString* pSCString_TestLeak = new SCString("this is a test memory leak");

	MissionHacks :: TestInit();

	{
		DAEMON_Init();

	}

	new IndexedFont_HUD(DATABASE_MESSAGE_FONT);
	
	GADGET_Init();

	ConsoleVariable :: CreateAll();
	ConsoleCommand :: CreateAll();


}

void DAVEHOOK_UnInit(void)
{
	IndexedFont :: UnloadFont(DATABASE_MESSAGE_FONT);

	GADGET_UnInit();
}

void DAVEHOOK_Maintain(void)
{
	{
		DAEMON_Maintain();
	}

	{
		KeyBinding :: Maintain();
	}

	// Hacked in input support:
	{


	}

	if ( bFirstFrame )
	{
		RE_ENTRANT_QUEUE_WinMain_FlushMessagesWithoutProcessing();
		// this is a hack to ensure that none of the keypresses used
		// in the menu get through to the first frame of the game and 
		// for example, switch to typing mode (for CR presses)
		
		bFirstFrame = No;
	}
	else
	{
		// Flush the WinProc messages:
		RE_ENTRANT_QUEUE_WinMain_FlushMessages();
	}

	/* KJL 20:14:23 28/03/98 - for now I've disabled the calls to the menus while in-game */
}

void DAVEHOOK_ScreenModeChange_Setup(void)
{
}

void DAVEHOOK_ScreenModeChange_Cleanup(void)
{
	R2BASE_ScreenModeChange_Cleanup();
	GADGET_ScreenModeChange_Cleanup();


	bFirstFrame = Yes;
		// to ensure a flush without processing of messages in first frame, so as to
		// avoid carriage returns/enter from menu selections triggering typing mode

	// Run program-generated batch file:
	#if !(PREDATOR_DEMO|MARINE_DEMO||ALIEN_DEMO||DEATHMATCH_DEMO)
	BatchFileProcessing :: Run("CONFIG.CFG");

	// Run user-generated batch file:
	BatchFileProcessing :: Run("STARTUP.CFG");
	#endif
}


/* Internal function definitions ***********************************/
void Testing :: VVTest(void)
{
	textprint("Testing :: VVTest()\n");
}
void Testing :: VITest(int i)
{
	textprint("Testing :: VITest(%i)\n",i);
}
int Testing :: IVTest(void)
{
	textprint("Testing :: IVTest()\n");

	return 180;
}
int Testing :: IITest(int i)
{
	textprint("Testing :: IITest(%i)\n",i);

	return (i*2);
}


// Diagnostic hook for reference counting system:
void Testing :: DumpRefCounts(void)
{
	{
		SCString* pSCString_Feedback = new SCString("REFCOUNT INFO DISABLED AT COMPILE-TIME");
		pSCString_Feedback -> SendToScreen();
		pSCString_Feedback -> R_Release();
	}
}

void Testing :: DumpVideoMode(void)
{
	char msg[256];
	sprintf
	(
		msg,
		"VIDEO MODE:%iX%iX%i",
		ScreenDescriptorBlock . SDB_Width,
		ScreenDescriptorBlock . SDB_Height,
		VideoModeColourDepth
	);
	
	SCString* pSCString_Feedback = new SCString(msg);
	pSCString_Feedback -> SendToScreen();
	pSCString_Feedback -> R_Release();
}

