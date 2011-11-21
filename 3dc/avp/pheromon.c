/*-----------------------Patrick 12/11/96--------------------------
  Source for AVP Pheromone system
  -----------------------------------------------------------------*/
#include "3dc.h"
#include "inline.h"
#include "module.h"
#include "stratdef.h"
#include "gamedef.h"
#include "bh_types.h"

#define UseLocalAssert Yes
#include "ourasert.h"

#include "pheromon.h"
#include "pfarlocs.h"
#include "pvisible.h"
#include "bh_alien.h"
#include "bh_far.h"
#include "bh_gener.h"
#include "showcmds.h"
#include "pldghost.h"


static unsigned int *Pher_Player1;
static unsigned int *Pher_Player2;
static unsigned char *Pher_Ai1;

/* these pointers indicate the current read from and write to
buffers for the player and ai pheromone systems */
unsigned int *PherPl_ReadBuf;
unsigned int *PherPl_WriteBuf;
unsigned char *PherAi_Buf;
static unsigned int *Pher_Aliens1;
static unsigned int *Pher_Aliens2;
unsigned int *PherAls_ReadBuf;
unsigned int *PherAls_WriteBuf;
unsigned int AlienPheromoneScale;

/* Marine pheromones: a pathfinding system only. */
static unsigned int *Pher_Marines1;
static unsigned int *Pher_Marines2;
unsigned int *PherMars_ReadBuf;
unsigned int *PherMars_WriteBuf;


/* This global is used to store	the current player phermone intensity */
unsigned int PlayerSmell = 3;
MODULE *playerPherModule = (MODULE *)0;

/* external globals */
extern int AIModuleArraySize;


/*----------------------Patrick 12/11/96--------------------------- 
Initialises pheromone systems
-------------------------------------------------------------------*/
void InitPheromoneSystem(void)
{
	int i;
	
	/* allocate	the pheromone buffers */
	Pher_Player1 = (unsigned int *)AllocateMem((AIModuleArraySize+1)*sizeof(unsigned int));
	if(!Pher_Player1) 
	{
		memoryInitialisationFailure = 1;
		return;
	}
	Pher_Player2 = (unsigned int *)AllocateMem((AIModuleArraySize+1)*sizeof(unsigned int));
	if(!Pher_Player2) 
	{
		memoryInitialisationFailure = 1;
		return;
	}
	Pher_Ai1 = (unsigned char *)AllocateMem((AIModuleArraySize+1)*sizeof(unsigned char));
	if(!Pher_Ai1) 
	{
		memoryInitialisationFailure = 1;
		return;
	}

	Pher_Aliens1 = (unsigned int *)AllocateMem((AIModuleArraySize+1)*sizeof(unsigned int));
	if(!Pher_Aliens1) 
	{
		memoryInitialisationFailure = 1;
		return;
	}
	
	Pher_Aliens2 = (unsigned int *)AllocateMem((AIModuleArraySize+1)*sizeof(unsigned int));
	if(!Pher_Aliens2) 
	{
		memoryInitialisationFailure = 1;
		return;
	}

	Pher_Marines1 = (unsigned int *)AllocateMem((AIModuleArraySize+1)*sizeof(unsigned int));
	if(!Pher_Marines1) 
	{
		memoryInitialisationFailure = 1;
		return;
	}
	
	Pher_Marines2 = (unsigned int *)AllocateMem((AIModuleArraySize+1)*sizeof(unsigned int));
	if(!Pher_Marines2) 
	{
		memoryInitialisationFailure = 1;
		return;
	}

	/* init the player phermone system */
	for(i=0;i<AIModuleArraySize;i++) 
	{
		Pher_Player1[i] = 1;
		Pher_Player2[i] = 1;
	}	
	PherPl_ReadBuf = &Pher_Player1[0]; 
	PherPl_WriteBuf = &Pher_Player2[0]; 
	PlayerSmell = 3;
	playerPherModule = (MODULE *)0;

	/* init the ai pheromone system */
	for(i=0;i<AIModuleArraySize;i++) 
	{
		Pher_Ai1[i] = 0;
	}		
	PherAi_Buf = &Pher_Ai1[0]; 


	for(i=0;i<AIModuleArraySize;i++) 
	{
		Pher_Aliens1[i] = 0;
		Pher_Aliens2[i] = 0;
	}	
	PherAls_ReadBuf = &Pher_Aliens1[0]; 
	PherAls_WriteBuf = &Pher_Aliens2[0]; 

	AlienPheromoneScale=1;

	for(i=0;i<AIModuleArraySize;i++) 
	{
		Pher_Marines1[i] = 0;
		Pher_Marines2[i] = 0;
	}	
	PherMars_ReadBuf = &Pher_Marines1[0]; 
	PherMars_WriteBuf = &Pher_Marines2[0]; 

}

/*----------------------Patrick 14/3/96--------------------------- 
End of level clean up for pheromone system
-------------------------------------------------------------------*/
void CleanUpPheromoneSystem(void)
{
	if (Pher_Player1) DeallocateMem(Pher_Player1);
	if (Pher_Player2) DeallocateMem(Pher_Player2);
	if (Pher_Ai1) DeallocateMem(Pher_Ai1);

	if (Pher_Aliens1) DeallocateMem(Pher_Aliens1);
	if (Pher_Aliens2) DeallocateMem(Pher_Aliens2);
	if (Pher_Marines1) DeallocateMem(Pher_Marines1);
	if (Pher_Marines2) DeallocateMem(Pher_Marines2);
}


int AIModuleAdmitsPheromones(AIMODULE *targetModule) {

	/* Check state. */

	MODULEDOORTYPE doorStatus;

	doorStatus = (AIModuleIsADoor(targetModule));

	switch(doorStatus)
	{
		case(MDT_ProxDoor):
		{	
			/* Go thru UNLOCKED proxdoors... */
			MODULE *renderModule;
			PROXDOOR_BEHAV_BLOCK *pdbblk;
			
			renderModule=*(targetModule->m_module_ptrs);
			pdbblk=((PROXDOOR_BEHAV_BLOCK *)renderModule->m_sbptr->SBdataptr);

			if (pdbblk->lockable_door) {
				if (pdbblk->door_locked) {
					return(0);
				} else {
					return(1);
				}
			} else {
				if (pdbblk->door_locked) {
					return(0);
				} else {
					return(1);
				}
			}
		}

		case(MDT_LiftDoor):
		{	
 			GLOBALASSERT(targetModule->m_module_ptrs);
 			GLOBALASSERT(*(targetModule->m_module_ptrs));
 			if(GetState((*(targetModule->m_module_ptrs))->m_sbptr)) {
				/* Open. */
 				return (1);
			} else {
				/* Closed. */
				return (0);
			}
			break;
		}

		case(MDT_SecurityDoor):
		{	
 			GLOBALASSERT(targetModule->m_module_ptrs);
 			GLOBALASSERT(*(targetModule->m_module_ptrs));
 			if(GetState((*(targetModule->m_module_ptrs))->m_sbptr)) {
				/* Open. */
 				return (1);
			} else {
				/* Closed. */
				return (0);
			}
			break;
		}

		default:
		{
			LOCALASSERT(doorStatus==MDT_NotADoor);
			return(1);
		}

	}

}

void AddMarinePheromones(AIMODULE *targetModule) {

	int ThisModuleIndex;	

	ThisModuleIndex = targetModule->m_index;

	PherAls_WriteBuf[ThisModuleIndex] += 3;

}

void MaintainMarineTargetZone(AIMODULE *targetModule) {

	int ThisModuleIndex;	

	ThisModuleIndex = targetModule->m_index;

	PherMars_WriteBuf[ThisModuleIndex] += 3;

}


/*----------------------Patrick 12/11/96--------------------------- 
Updates the player pheromone system:
this is used by the NPC far behaviour for hunting the player.
-------------------------------------------------------------------*/
void PlayerPheromoneSystem(void)
{
	int moduleCounter;
	AIMODULE *ModuleListPointer;	
	AIMODULE *ThisModulePtr;
	int ThisModuleIndex;	
	AIMODULE **AdjModuleRefPtr;
	int AdjModuleIndex;

		
	
	/* get a pointer to the global array of pointers to the modules
	in the environment (interfaces'r'us).  
	First check if  Global_ModulePtr is set. If not we're buggered, 
	so leave everything as it is and try again next frame*/	
	{
		extern AIMODULE *AIModuleArray;

		ModuleListPointer = AIModuleArray;
	}
	

	/* go through each module in the environment  */	
	for(moduleCounter = 0; moduleCounter < AIModuleArraySize; moduleCounter++)
	{

		/* get a pointer to the next current module */
		ThisModulePtr = &(ModuleListPointer[moduleCounter]); 
		LOCALASSERT(ThisModulePtr);
		
		/* get it's index */
		ThisModuleIndex = ThisModulePtr->m_index;
				
		LOCALASSERT(ThisModuleIndex >= 0);
		LOCALASSERT(ThisModuleIndex < AIModuleArraySize);
		
			
		/* !!!!!!!!!!!!!!!!!!!!!
		check for closed non-traversable door module here if detected, do not update its smell.
		
		Actually, no: allow smell to pass thro' non-openable doors. Otherwise AIs that can open
		doors will choose not to 
		!!!!!!!!!!!!!!!!!!!!!!!!*/

		/* CDF 4/12/97: Actually, yes.  AIs CAN'T open security doors, fool! */
		
		/* check for universal module: don't want to update this! */
		if(AIModuleIsPhysical(ThisModulePtr))
		{

			if (AIModuleAdmitsPheromones(ThisModulePtr)) {
				/* get a pointer to the list of physically adjacent modules
				and traverse them */
				
				AdjModuleRefPtr = ThisModulePtr->m_link_ptrs;
				
				if(AdjModuleRefPtr)	/* check that there is a list of adjacent modules */
				{
					while(*AdjModuleRefPtr != 0)
					{
						/* get the index */
						AdjModuleIndex = (*AdjModuleRefPtr)->m_index;
				
						/* if adjacent module's previous smell is greater than
						the current module's new smell (so far), then update
						the current module's newq smell */
						if(PherPl_ReadBuf[AdjModuleIndex] > PherPl_WriteBuf[ThisModuleIndex])
							PherPl_WriteBuf[ThisModuleIndex] = (PherPl_ReadBuf[AdjModuleIndex] - 1);
				
						if(PherAls_ReadBuf[AdjModuleIndex] > PherAls_WriteBuf[ThisModuleIndex]) {
							PherAls_WriteBuf[ThisModuleIndex] = (PherAls_ReadBuf[AdjModuleIndex] - 1);
						}

						if (CheckAdjacencyValidity((*AdjModuleRefPtr),ThisModulePtr,0)) {
							if(PherMars_ReadBuf[AdjModuleIndex] > PherMars_WriteBuf[ThisModuleIndex]) {
								PherMars_WriteBuf[ThisModuleIndex] = (PherMars_ReadBuf[AdjModuleIndex] - 1);
							}
						}

						/* next adjacent module reference pointer */
						AdjModuleRefPtr++;
					}
				}
			}
			/* Decay pheromones. */
			if (PherAls_WriteBuf[ThisModuleIndex]>0) {
				PherAls_WriteBuf[ThisModuleIndex]--;
			}

			if (PherMars_WriteBuf[ThisModuleIndex]>0) {
				PherMars_WriteBuf[ThisModuleIndex]--;
			}
		}
	}

	/*If in a network game add pheromon's for other players*/
	if(AvP.Network!=I_No_Network && AvP.NetworkAIServer)
	{
		/* go through the strategy blocks looking for players*/
		int sbIndex;
		for(sbIndex=0;sbIndex<NumActiveStBlocks;sbIndex++)
		{
			STRATEGYBLOCK *playerSbPtr = ActiveStBlockList[sbIndex];
			NETGHOSTDATABLOCK *ghostData;
			if(playerSbPtr->I_SBtype!=I_BehaviourNetGhost) continue;
			ghostData = (NETGHOSTDATABLOCK *)playerSbPtr->SBdataptr;

			if(ghostData->type==I_BehaviourMarinePlayer ||
			   ghostData->type==I_BehaviourPredatorPlayer)
			{
				/*this is another player*/
				if(playerSbPtr->containingModule)
				{
		   			PherPl_WriteBuf[playerSbPtr->containingModule->m_aimodule->m_index] = PlayerSmell;
					AddMarinePheromones(playerSbPtr->containingModule->m_aimodule);
				}
			}
		}
	}
	
	/* That completed, find which module the player is in, set it's smell to the
	current player smell value, and update the player smell for the next frame */
	{
		extern DISPLAYBLOCK* Player;
		VECTORCH playerPosition = Player->ObWorld;		
		PLAYER_STATUS *playerStatusPtr= (PLAYER_STATUS *) (Player->ObStrategyBlock->SBdataptr);
   		
		playerPherModule = (ModuleFromPosition(&playerPosition, playerPherModule));
		if(playerPherModule)
		{
   			//the player must be alive to leave pheromones 
			//(mainly relevant in coop games)
   			if(playerStatusPtr->IsAlive)
			{
   				PherPl_WriteBuf[playerPherModule->m_aimodule->m_index] = PlayerSmell;
				if(playerPherModule->name)
				{
					if (ShowDebuggingText.Module)
					{
						ReleasePrintDebuggingText("Player Module: %d '%s'\n", playerPherModule->m_index,playerPherModule->name);
						ReleasePrintDebuggingText("Player Module Coords: %d %d %d\n",playerPherModule->m_world.vx,playerPherModule->m_world.vy,playerPherModule->m_world.vz);
					}
					AlienPheromoneScale+=3;
					if (AlienPheromoneScale==0) AlienPheromoneScale=1;
					{
						unsigned int prop=DIV_FIXED(PherAls_WriteBuf[playerPherModule->m_aimodule->m_index],AlienPheromoneScale);
						textprint("Alien readable pheromones in Player Module: %d\n",prop);
					}
					/* No scale for 'marine' pheromones, the player will never see it. */
				}
			}
		}
	}

	PlayerSmell++;

	/* Note that marines should add pheromones at the AI level... */
	{
		unsigned int *tempBufPointer = PherAls_ReadBuf;
		PherAls_ReadBuf = PherAls_WriteBuf;
		PherAls_WriteBuf= tempBufPointer;
  	}
	/* As should the pathfinding system. */
	{
		unsigned int *tempBufPointer = PherMars_ReadBuf;
		PherMars_ReadBuf = PherMars_WriteBuf;
		PherMars_WriteBuf= tempBufPointer;
  	}
	
	/* swap the read and write buffers:
	   behaviours access most recent data thro' the read buffer */
	{
		unsigned int *tempBufPointer = PherPl_ReadBuf;
		PherPl_ReadBuf = PherPl_WriteBuf;
		PherPl_WriteBuf	= tempBufPointer;
  	}


}


/*----------------------Patrick 14/11/96--------------------------- 
Ai Pheromone system.

This system just keeps track of how many aliens are in each module,
and is calculated from scratch at the start of each frame.
Also, the numactivealiens bit of the hive data block is calculated
for this frame.
-------------------------------------------------------------------*/
void AiPheromoneSystem(void)
{
	extern int NumActiveStBlocks;
	extern STRATEGYBLOCK *ActiveStBlockList[];	

	int sbIndex = 0;
	STRATEGYBLOCK *sbPtr;
	int i;
			
	/* first, zero the buffer, and hive counter */
	for(i=0;i<AIModuleArraySize;i++) PherAi_Buf[i] = 0;

	/* next, have a look at the sb list */ 
	while(sbIndex < NumActiveStBlocks)
	{	
		sbPtr = ActiveStBlockList[sbIndex++];
		if((sbPtr->I_SBtype == I_BehaviourAlien)||(sbPtr->I_SBtype == I_BehaviourMarine))
		{
			if(sbPtr->containingModule)
			{
				PherAi_Buf[(sbPtr->containingModule->m_aimodule->m_index)]++;						
			}
		}							
	}
}




