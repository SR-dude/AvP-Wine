/* Patrick 5/6/97 -------------------------------------------------------------
  AvP Project sound source
  ----------------------------------------------------------------------------*/
#include "3dc.h"
#include "module.h"
#include "inline.h"
#include "stratdef.h"
#include "gamedef.h"
#include "gameplat.h"
#include "bh_types.h"
#include "inventry.h"
#include "weapons.h"
#include "psnd.h"
#include "psndplat.h"
#include "avp_menus.h"
#include "scream.h"

#define UseLocalAssert Yes
#include "ourasert.h"
#include "ffstdio.h"
#include "db.h"
#include "dxlog.h"



/* Andy 9/6/97 ----------------------------------------------------------------
  Internal globals  
-----------------------------------------------------------------------------*/
int weaponHandle = SOUND_NOACTIVEINDEX;

static int sadarReloadTimer = 0;
static int weaponPitchTimer = 0;
static int playOneShotWS = 1;
static int oldRandomValue = -1;


/* Has the player made a noise? */
int playerNoise;

/* Patrick 5/6/97 -------------------------------------------------------------
  External refernces
  ----------------------------------------------------------------------------*/
extern int NormalFrameTime;
extern ACTIVESOUNDSAMPLE ActiveSounds[];



void DoPlayerSounds(void)
{
	PLAYER_STATUS *playerStatusPtr;
	PLAYER_WEAPON_DATA *weaponPtr;
 
		
	/* do weapon sound */
	    
 	/* access the extra data hanging off the strategy block */
	playerStatusPtr= (PLAYER_STATUS *) (Player->ObStrategyBlock->SBdataptr);
 	GLOBALASSERT(playerStatusPtr);
    	
 	/* init a pointer to the weapon's data */
 	weaponPtr = &(playerStatusPtr->WeaponSlot[playerStatusPtr->SelectedWeaponSlot]);
    
 	if (sadarReloadTimer)
 	{
 		sadarReloadTimer -= NormalFrameTime;
 		if (sadarReloadTimer <= 0)
		{
			sadarReloadTimer = 0;
			playerNoise=1;
		}
 	}
                           


	switch(weaponPtr->WeaponIDNumber)
	{			
		case(WEAPON_PRED_PISTOL):
		{
			if(weaponPtr->CurrentState == WEAPONSTATE_FIRING_PRIMARY)			
			{
		 		if(weaponHandle == SOUND_NOACTIVEINDEX) 
				{
				  Sound_Play(SID_PRED_PISTOL,"h");					
					playerNoise=1;
			  	}
			}
  		break;
   	}

		case(WEAPON_PULSERIFLE):
		{
			if(weaponPtr->CurrentState == WEAPONSTATE_FIRING_PRIMARY)			
			{
		 		if(weaponHandle == SOUND_NOACTIVEINDEX) 
				{
		 		  Sound_Play(SID_PULSE_START,"h");	  
			   	Sound_Play(SID_PULSE_LOOP,"elh",&weaponHandle);					
					playerNoise=1;
			   	weaponPitchTimer=ONE_FIXED>>3;
			  }
				else
				{
					weaponPitchTimer-=NormalFrameTime;
					if (weaponPitchTimer<=0)
					{
						weaponPitchTimer=ONE_FIXED>>3;
						Sound_ChangePitch(weaponHandle,(FastRandom()&63)-32);
						playerNoise=1;
					}
				}
			}
   		else if (weaponPtr->CurrentState == WEAPONSTATE_FIRING_SECONDARY)
   		{
   			if (weaponHandle == SOUND_NOACTIVEINDEX)
   			{
     			Sound_Play(SID_NADEFIRE,"h");
				playerNoise=1;
   			}
	 		}
   		else
			{
				if(weaponHandle != SOUND_NOACTIVEINDEX)
				{
         	Sound_Play(SID_PULSE_END,"h");
				  Sound_Stop(weaponHandle);
			 	}
			}
   		break;
   	}

   	case(WEAPON_FLAMETHROWER):
		{
			if(weaponPtr->CurrentState == WEAPONSTATE_FIRING_PRIMARY)			
			{
	 			if(weaponHandle == SOUND_NOACTIVEINDEX) 
				{
	    		Sound_Play(SID_INCIN_START,"h");	  
	  			Sound_Play(SID_INCIN_LOOP,"elh",&weaponHandle);					
				playerNoise=1;
			}
	  	}
			else
			{
				if(weaponHandle != SOUND_NOACTIVEINDEX)
				{
					Sound_Play(SID_INCIN_END,"h");
					Sound_Stop(weaponHandle);
		 		}
			}
     	break;
		}      

   	case (WEAPON_MINIGUN):
   	{
        if (PlayerStatusPtr->IsAlive==0) {
     		if(weaponHandle != SOUND_NOACTIVEINDEX)
     		{
       		Sound_Play(SID_MINIGUN_END,"h");
       		Sound_Stop(weaponHandle);
     		}
		}
     	break;
   	}  

   	case (WEAPON_AUTOSHOTGUN):
   	{
     	if (weaponPtr->CurrentState == WEAPONSTATE_FIRING_PRIMARY)
     	{
	   		Sound_Play(SID_SHOTGUN,"h");
			playerNoise=1;
	 	}
     	break;
   	}           

   	case (WEAPON_MARINE_PISTOL):
   	case (WEAPON_TWO_PISTOLS):
   	{
     	if ((weaponPtr->CurrentState == WEAPONSTATE_FIRING_PRIMARY)
			||(weaponPtr->CurrentState == WEAPONSTATE_FIRING_SECONDARY))
     	{
	   		Sound_Play(SID_SHOTGUN,"h");
			playerNoise=1;
	 	}

     	break;
   	}           

 	case (WEAPON_SADAR):
   	{
     	if (weaponPtr->CurrentState == WEAPONSTATE_FIRING_PRIMARY)
     	{
  	 		Sound_Play(SID_SADAR_FIRE,"h");
			playerNoise=1;
	 	}
     	break;
   	}      

 	case (WEAPON_FRISBEE_LAUNCHER):
   	{
     	if (weaponPtr->CurrentState == WEAPONSTATE_FIRING_PRIMARY)
     	{
			if (weaponPtr->StateTimeOutCounter == WEAPONSTATE_INITIALTIMEOUTCOUNT) {
				playerNoise=1;
	 			if (weaponHandle == SOUND_NOACTIVEINDEX) {
		  	 		Sound_Play(SID_ED_SKEETERCHARGE,"eh",&weaponHandle);
				}
			} else {
	 			if (weaponHandle == SOUND_NOACTIVEINDEX) {
					playerNoise=0;
				}
			}
	 	} else {
 			if (weaponHandle != SOUND_NOACTIVEINDEX) {
				Sound_Stop(weaponHandle);
			}
		}
     	break;
   	}      


   	case(WEAPON_SMARTGUN):
		{
			if(weaponPtr->CurrentState == WEAPONSTATE_FIRING_PRIMARY)			
			{
				if(weaponHandle == SOUND_NOACTIVEINDEX) 
				{
				 	unsigned int rand=FastRandom() % 3;
         	if (rand == oldRandomValue) rand=(rand + 1) % 3;
         	oldRandomValue = rand;
			playerNoise=1;
         	switch (rand)
         	{
         		case 0:
         		{
          		Sound_Play(SID_SMART1,"ehp",&weaponHandle,(FastRandom()&255)-128);					
          		break;
         		}
        		case 1:
         		{
          		Sound_Play(SID_SMART2,"ehp",&weaponHandle,(FastRandom()&255)-128);					
          		break;
         		}
        		case 2:
         		{
          		Sound_Play(SID_SMART3,"ehp",&weaponHandle,(FastRandom()&255)-128);					
          		break;
         		}
			 		default:
				 		{
				 			break;
				 		}
				 	}
			 	}
     	}
			break;
		}

		case(WEAPON_GRENADELAUNCHER):
		{
			if(weaponPtr->CurrentState == WEAPONSTATE_FIRING_PRIMARY)			
			{
				if(playOneShotWS) 
				{
					Sound_Play(SID_ROCKFIRE,"h");
					playerNoise=1;
					playOneShotWS = 0;
				}
			}
			else playOneShotWS = 1;
			break;
		}

		case(WEAPON_PRED_WRISTBLADE):
		{

			break;
		}
	
		case(WEAPON_ALIEN_CLAW):
		{
			break;
		}

		case(WEAPON_ALIEN_GRAB):
		{
			break;
		}

		case(WEAPON_PRED_RIFLE):
		{
			if(weaponPtr->CurrentState == WEAPONSTATE_FIRING_PRIMARY)			
			{
				if(playOneShotWS) 
				{
					Sound_Play(SID_PRED_LASER,"hp",(FastRandom()&255)-128);
					playerNoise=1;
					playOneShotWS = 0;
				}
			}
			else playOneShotWS = 1;
			break;
		}

		case(WEAPON_PRED_SHOULDERCANNON):
		{
			if(weaponPtr->CurrentState == WEAPONSTATE_FIRING_PRIMARY)			
			{
				if(playOneShotWS) 
				{
					playOneShotWS = 0;
				}
			}
			else playOneShotWS = 1;
			break;
		}	 

		case(WEAPON_PRED_DISC):
		{
			if(weaponPtr->CurrentState == WEAPONSTATE_FIRING_PRIMARY)			
			{
				if(playOneShotWS) 
				{
					Sound_Play(SID_PRED_FRISBEE,"hp",(FastRandom()&255)-128);
					playerNoise=1;
					playOneShotWS = 0;
				}
			}
			else playOneShotWS = 1;
			break;
		}	 
	   
		case(WEAPON_ALIEN_SPIT):
		{
			if(weaponPtr->CurrentState == WEAPONSTATE_FIRING_PRIMARY)			
			{
				if(playOneShotWS) 
				{
				 	Sound_Play(SID_ACID_SPRAY,"hp",(FastRandom()&255)-128);
					playerNoise=1;
			 		playOneShotWS = 0;
				}
			}
			else playOneShotWS = 1;
			break;
		}	
		default:
		{
			break;
		}
	}
}

static int SpotEffectWeaponHandle = SOUND_NOACTIVEINDEX;
void PlayWeaponClickingNoise(enum WEAPON_ID weaponIDNumber)
{
 	if(SpotEffectWeaponHandle != SOUND_NOACTIVEINDEX)
		return;
	
	switch(weaponIDNumber)
	{
		// Marine weapons
		case WEAPON_PULSERIFLE:
		{
 			Sound_Play(SID_PULSE_RIFLE_FIRING_EMPTY,"eh",&SpotEffectWeaponHandle);
			break;
		}
		case WEAPON_SMARTGUN:
		{
 			Sound_Play(SID_NOAMMO,"eh",&SpotEffectWeaponHandle);
			break;
		}
		case WEAPON_MINIGUN:
		{
			break;
		}
		// Predator weapons
		case WEAPON_PRED_RIFLE:
		{
			Sound_Play(SID_PREDATOR_SPEARGUN_EMPTY,"eh",&SpotEffectWeaponHandle);
			break;
		}

		default:
			break;
	}
}


void MakeRicochetSound(VECTORCH *position)
{
	switch(NormalFrameTime&0x3)
	{
		case(0):
			Sound_Play(SID_RICOCH1,"pd",((FastRandom()&255)-128),position);	 
			break;
		case(1):
			Sound_Play(SID_RICOCH2,"pd",((FastRandom()&255)-128),position);	 
			break;
		case(2):
			Sound_Play(SID_RICOCH3,"pd",((FastRandom()&255)-128),position);	 
			break;
		case(3):
			Sound_Play(SID_RICOCH4,"pd",((FastRandom()&255)-128),position);	 
			break;
		default:
			break;
	}
}



void PlayAlienSwipeSound(void) {

	PlayAlienSound(0,ASC_Swipe,((FastRandom()&255)-128),
		&weaponHandle,NULL);
}

void PlayAlienTailSound(void) {

	PlayAlienSound(0,ASC_TailSound,((FastRandom()&255)-128),
		&weaponHandle,NULL);

}
	
void PlayPredSlashSound(void) {

	PlayPredatorSound(0,PSC_Swipe,((FastRandom()&255)-128),
		&weaponHandle,NULL);

}

void PlayCudgelSound(void) {

	unsigned int rand=FastRandom() % 4;
	if (rand == oldRandomValue) rand = (rand + 1) % 4;
	oldRandomValue = rand;
	switch (rand)
	{
		case 0:
		{
	  		Sound_Play(SID_PULSE_SWIPE01,"ehp",&weaponHandle,(FastRandom()&255)-128);					
	  		break;
	  	}
	  	case 1:
	  	{
	  		Sound_Play(SID_PULSE_SWIPE02,"ehp",&weaponHandle,(FastRandom()&255)-128);					
	  		break;
	  	}
	  	case 2:
	  	{
	   		Sound_Play(SID_PULSE_SWIPE03,"ehp",&weaponHandle,(FastRandom()&255)-128);					
	     		break;
	   	}
		case 3:
	  	{
	  		Sound_Play(SID_PULSE_SWIPE04,"ehp",&weaponHandle,(FastRandom()&255)-128);					
	 		break;
	 	}
	 	default:
	 	{
	 		break;
	 	}
	}

}


char * SecondSoundDir = 0;
static const char * FirstSoundDir = "SOUND\\";
static char *CommonSoundDirectory = ".\\SOUND\\COMMON\\";

int FindAndLoadWavFile(int soundNum,char* wavFileName)
{
	static char sound_name[200];
	sprintf (sound_name, "%s%s", FirstSoundDir,wavFileName);

	//first look in fast file
	{
		unsigned nLen;
		if(ffreadbuf(sound_name,&nLen))
		{
			return LoadWavFromFastFile(soundNum,sound_name);
		}
	}

	//look for sound locally
	{
	
		{
			//check to see if file exists locally first
			FILE* wavFile=fopen(sound_name,"rb");
	
			if(!wavFile && SecondSoundDir)
			{
				//look for sound over network
				sprintf (sound_name, "%s%s", SecondSoundDir,wavFileName);
	
				wavFile=fopen(sound_name,"rb");
				if(!wavFile)
				{
					LOGDXFMT(("Failed to find %s\n",wavFileName));	
					return 0;
				}

			}
			fclose(wavFile);
		}

		return LoadWavFile(soundNum,sound_name) ;
	}

}

// adj deleted fastfile sound loading code

/* Patrick 5/6/97 -------------------------------------------------------------
  Sound data loaders 
  ----------------------------------------------------------------------------*/

void LoadSounds(char *soundDirectory)
{
	char soundFileName[48];
	char fileLine[128];
	FILE *myFile;
	int soundNum;
	int pitchOffset;
	int ok;

	LOCALASSERT(soundDirectory);

	/* first check that sound has initialised and is turned on */
	if(!SoundSys_IsOn()) return;	
	
	/* construct the sound list file name, and load it */
	strcpy((char*)&soundFileName, CommonSoundDirectory);
	strcat((char*)&soundFileName, soundDirectory);
	strcat((char*)&soundFileName, ".SL");
	myFile = fopen(soundFileName,"rt");
	LOCALASSERT(myFile!=NULL);
	
	/* just return if we can't find the file */
	if(!myFile)	return;

	/* Process the file */
	while(fgets((char*)fileLine,128,myFile))
	{
		char wavFileName[128];
/*adj*/
		if(!strncmp((char*)fileLine,"//",2)) continue; /* comment */
		if(strlen((char*)fileLine) < 4) continue; /* blank line, or something */

		
		/* Assume the string is a valid wav file reference */		
		soundNum = atoi(strtok(fileLine,", \n"));
		strcpy((char*)&wavFileName,"Common\\");		
		strcat((char*)&wavFileName,strtok(NULL,", \n")); 
		
		/* pitch offset is in semitones: need to convert to 1/128ths */
		pitchOffset = PITCH_DEFAULTPLAT + (atoi(strtok(NULL,", \n"))*128); 

		if((soundNum<0)||(soundNum>=SID_MAXIMUM)) continue; /* invalid sound number */
		if(GameSounds[soundNum].loaded)	continue; /* Duplicate game sound loaded */
		ok = FindAndLoadWavFile(soundNum, wavFileName);
		
		/* Fill in the GameSound: the pointer to the ds buffer is filled in by 
		the wav file loader, if everthing went ok.  If the load failed, do not
		fill in the game sound data: it should remain initialised */
		if(ok)
		{
	  	GameSounds[soundNum].loaded = 1;
			GameSounds[soundNum].activeInstances = 0;;	 
			GameSounds[soundNum].volume = VOLUME_DEFAULT;		
			GameSounds[soundNum].pitch = pitchOffset;				
			InitialiseBaseFrequency(soundNum);
		}
	}
	fclose(myFile);

	db_log1("loaded all the sounds.");
}

