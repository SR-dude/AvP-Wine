#include "3dc.h"
#include <math.h>
#include "inline.h"
#include "module.h"

#include "stratdef.h"
#include "gamedef.h"
#include "dynblock.h"
#include "dynamics.h"

#include "bh_types.h"
#include "bh_alien.h"
#include "pheromon.h"
#include "pfarlocs.h"
#include "bh_gener.h"
#include "pvisible.h"
#include "lighting.h"
#include "bh_pred.h"
#include "bh_lift.h"
#include "avpview.h"
#include "psnd.h"
#include "psndplat.h"
#include "particle.h"
#include "sfx.h"
#include "version.h"
#include "bh_rubberduck.h"
#include "bh_marin.h"
#include "dxlog.h"
#include "avp_menus.h"
#include "avp_userprofile.h"
#include "davehook.h"
#include "cdtrackselection.h"
#include "savegame.h"
	// Added 18/11/97 by DHM: all hooks for my code

#define UseLocalAssert Yes
#include "ourasert.h"

/* KJL 09:47:33 03/19/97 - vision stuff for marine and predator.
Currently PC only because it will probably be implemented in a completely
different way on the consoles, so I won't worry the PSX guys for now.
*/

#include "vision.h"
#include "cheat.h"	
#include "pldnet.h"								 
#include "kshape.h"
#include "krender.h"
#include "avp_menus.h"

/******************
Extern Engine Varibles
******************/

extern void (*UpdateScreen[]) (void);
extern int VideoMode;
extern void (*SetVideoMode[]) (void);
extern int FrameRate;
extern int NormalFrameTime;
extern int FrameRate;
extern int HWAccel;
extern int Resolution;
unsigned char Null_Name[8];
extern int PlaySounds;

/*******************************
EXPORTED GLOBALS
*******************************/


AVP_GAME_DESC AvP;		 /* game description */

char projectsubdirectory[] = {"avp/"};
int SavedFrameRate;
int SavedNormalFrameTime;


/* Andy 13/10/97

   This global is set by any initialisation routine if a call to AllocateMem fails.
   It can be checked during debugging after all game and level initialisation to see
   if we have run out of memory.
*/
int memoryInitialisationFailure = 0;


/* start inits for the game*/

void ProcessSystemObjects();


/*runtime maintainance*/

void FindObjectOfFocus();
void MaintainPlayer(void);


/*********************************************

Init Game and Start Game

*********************************************/


void InitGame(void)
{
	/*
		RWH 
		InitGame is to be used only to set platform independent
		varibles. It will be called ONCE only by the game
	*/

	/***** Set up default game settings*/

	AvP.Language = I_English;
	AvP.GameMode = I_GM_Playing;
	AvP.Network = I_No_Network;
	AvP.Difficulty = I_Medium;

 	AvP.StartingEnv = I_Entrance;
	AvP.CurrentEnv = AvP.StartingEnv;
	AvP.PlayerType = I_Marine;


	AvP.GameVideoRequestMode = VideoMode_DX_320x200x8; /* ignored */
	if(HWAccel)
		AvP.MenuVideoRequestMode = VideoMode_DX_640x480x15;
	else
		AvP.MenuVideoRequestMode = VideoMode_DX_640x480x8;


	AvP.ElapsedSeconds = 0;
	AvP.ElapsedMinutes = 0;
	AvP.ElapsedHours = 0;
	
	AvP.NetworkAIServer = 0;


	// Added by DHM 18/11/97: Hook for my initialisation code:
	DAVEHOOK_Init();

	/* KJL 15:17:35 28/01/98 - Initialise console variables */
	{
		extern void CreateGameSpecificConsoleVariables(void);
		extern void CreateGameSpecificConsoleCommands(void);

		extern void CreateMoreGameSpecificConsoleVariables(void);

		/* KJL 12:03:18 30/01/98 - Init console variables and commands */
		CreateGameSpecificConsoleVariables();
		CreateGameSpecificConsoleCommands();
		
		/* Next one is CDF's */
		CreateMoreGameSpecificConsoleVariables();
	}
	SetToDefaultDetailLevels();
}

extern void create_strategies_from_list ();
extern void AssignAllSBNames();
extern BOOL Current_Level_Requires_Mirror_Image();

void StartGame(void)
{
	/* called whenever we start a game (NOT when we change */
	/* environments - destroy anything from a previous game*/

	/*
	Temporarily disable sounds while loading. Largely to avoid 
	some irritating teletext sounds starting up
	*/
	int playSoundsStore=PlaySounds;
	PlaySounds=0;

	//get the cd to start again at the beginning of the play list.
	ResetCDPlayForLevel();
	
	
	ProcessSystemObjects();
	
	create_strategies_from_list ();
	AssignAllSBNames();
	
	SetupVision();
	/*-------------- Patrick 11/1/97 ----------------
	  Initialise visibility system and NPC behaviour 
	  systems for new level.	  
	  -----------------------------------------------*/
	
	InitObjectVisibilities();
	InitPheromoneSystem();
	BuildFarModuleLocs();
	InitHive();
	InitSquad();

	InitialiseParticleSystem();
	InitialiseSfxBlocks();
	InitialiseLightElementSystem();

	AvP.DestructTimer=-1;

	// DHM 18/11/97: I've put hooks for screen mode changes here for the moment:
	DAVEHOOK_ScreenModeChange_Setup();
	DAVEHOOK_ScreenModeChange_Cleanup();

	/* KJL 11:46:42 30/03/98 - I thought it'd be nice to display the version details
	when you start a game */

	if(Current_Level_Requires_Mirror_Image())
	{
		CreatePlayersImageInMirror();
	}

	/* KJL 16:13:30 01/05/98 - rubber ducks! */
	CreateRubberDucks();
	
	/* adj stub */
	/*CheckCDStatus();*/

	{
		extern int LeanScale;
		if (AvP.PlayerType==I_Alien)
		{
			LeanScale=ONE_FIXED*3;
		}
		else
		{
			LeanScale=ONE_FIXED;
		}
	}
	{
		extern SCREENDESCRIPTORBLOCK ScreenDescriptorBlock;
		extern int MotionTrackerScale;
		MotionTrackerScale = DIV_FIXED(ScreenDescriptorBlock.SDB_Width,640);
	}

	InitialiseTriggeredFMVs();
	CreateStarArray();

	{
		//check the containing modules for preplaced decals
		void check_preplaced_decal_modules();
		check_preplaced_decal_modules();
	}

	CurrentGameStats_Initialise();
	MessageHistory_Initialise();

	if (DISCOINFERNO_CHEATMODE || TRIPTASTIC_CHEATMODE)
	{
		MakeLightElement(&Player->ObWorld,LIGHTELEMENT_ROTATING);
	}

	//restore the play sounds setting
	PlaySounds=playSoundsStore;

  	//make sure the visibilities are up to date
  	Global_VDB_Ptr->VDB_World = Player->ObWorld;
  	AllNewModuleHandler();
  	DoObjectVisibilities();
}


#define FIXED_MINUTE ONE_FIXED*60

void DealWithElapsedTime()
{
	AvP.ElapsedSeconds += NormalFrameTime;
	
	if(AvP.ElapsedSeconds  >= FIXED_MINUTE)
	{
		AvP.ElapsedSeconds -= FIXED_MINUTE;
		AvP.ElapsedMinutes ++;
	}
	
	if(AvP.ElapsedMinutes >= 60)
	{
		AvP.ElapsedMinutes -= 60;
		AvP.ElapsedHours ++;
	}		
}
	


/**********************************************

 Main Loop Game Functions

**********************************************/
void UpdateGame(void)
{
	/* Read Keyboard, Keypad, Joystick etc. */
	ReadUserInput();

	/* DHM 18/11/97: hook for my code */
	DAVEHOOK_Maintain();

	/*-------------- Patrick 14/11/96 ----------------
	 call the pheronome system maintainence functions
	-------------------------------------------------*/
	PlayerPheromoneSystem();
	AiPheromoneSystem();

	/*-------------- Patrick 11/1/97 ----------------
	Call the alien hive management function
	-------------------------------------------------*/
	DoHive();
	DoSquad();
	

 	ObjectBehaviours();
	 
	/* KJL 10:32:55 09/24/96 - update player */
	MaintainPlayer();

	/* KJL 12:54:08 21/04/98 - make sure the player's matrix is always normalised */
	MNormalise(&(Player->ObStrategyBlock->DynPtr->OrientMat));

	/* netgame support: it seems necessary to collect all our messages here, as some
	things depend on the player's behaviour running before anything else... 
	including firing the player's weapon */
	if(AvP.Network != I_No_Network)	NetCollectMessages();

	RemoveDestroyedStrategyBlocks();

	{

		if(SaveGameRequest != SAVELOAD_REQUEST_NONE)
		{
			SaveGame();
		}
		else if(LoadGameRequest != SAVELOAD_REQUEST_NONE)
		{
			LoadSavedGame();
		}
	}
	
	ObjectDynamics();

	// now for the env teleports
	
	if(RequestEnvChangeViaLift)
	{
		CleanUpLiftControl();								
 	}	

	/* netgame support */
	if(AvP.Network != I_No_Network)	NetSendMessages();

	/* KJL 11:50:18 03/21/97 - cheat modes */
	HandleCheatModes();
	

	/*------------Patrick 1/6/97---------------
	New sound system 
	-------------------------------------------*/
	
	if(playerPherModule)
	{
		PlatSetEnviroment(playerPherModule->m_sound_env_index,playerPherModule->m_sound_reverb);		
	}
	SoundSys_Management();
	DoPlayerSounds();
 
	MessageHistory_Maintain();

	if(AvP.LevelCompleted)
	{
		/*
		If player is dead and has also completed level , then cancel
		level completion.
		*/
		PLAYER_STATUS* PlayerStatusPtr = (PLAYER_STATUS*) Player->ObStrategyBlock->SBdataptr;
		if(!PlayerStatusPtr->IsAlive)
		{
			AvP.LevelCompleted=0;
		}
	}

	if (TRIPTASTIC_CHEATMODE)
	{
		extern int TripTasticPhase;
		extern int CloakingPhase;
	 	int a = GetSin(CloakingPhase&4095);
	 	TripTasticPhase = MUL_FIXED(MUL_FIXED(a,a),128)+64;
	}
	if (JOHNWOO_CHEATMODE)
	{
		extern int LeanScale;
		extern int TimeScale;
		TimeScaleThingy();
		
		//in john woo mode leanscale is dependent on the TimeScale
		if (AvP.PlayerType==I_Alien)
		{
			LeanScale=ONE_FIXED*3;
		}
		else
		{
			LeanScale=ONE_FIXED;
		}

		LeanScale+=(ONE_FIXED-TimeScale)*5;
		
	}
	

}

