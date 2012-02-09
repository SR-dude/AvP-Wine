#include "3dc.h"
#include "inline.h"
#include "module.h"
#include "stratdef.h"
#include "gamedef.h"
#include "kzsort.h"
#include "kshape.h"
#include "d3d_render.h"
#define UseLocalAssert Yes
#include "ourasert.h"
#include "avp_userprofile.h"


extern int *ItemPointers[maxpolyptrs];
extern int ItemCount;

extern int ScanDrawMode;
extern int ZBufferMode;
extern int NumVertices;
extern int WireFrameMode;
extern int DrawingAReflection;

static struct KObject VisibleModules[MAX_NUMBER_OF_VISIBLE_MODULES]={0,};
static struct KObject VisibleModules2[MAX_NUMBER_OF_VISIBLE_MODULES]={0,};
static struct KObject *SortedModules;
static struct KObject VisibleObjects[maxobjects]={0,};




static int PointIsInModule(VECTORCH *pointPtr,MODULE *modulePtr);
/* KJL 12:21:51 02/11/97 - This routine is too big and ugly. Split & clean up required! */
void KRenderItems(VIEWDESCRIPTORBLOCK *VDBPtr)
{
	extern int NumOnScreenBlocks;
	extern DISPLAYBLOCK *OnScreenBlockList[];
	int numOfObjects = NumOnScreenBlocks;
	int numVisMods=0;
	int numVisObjs=0;
	while(numOfObjects)
	{
		extern DISPLAYBLOCK *Player;

		DISPLAYBLOCK *objectPtr = OnScreenBlockList[--numOfObjects];
		MODULE *modulePtr = objectPtr->ObMyModule;

		/* if it's a module, which isn't inside another module */
		if (modulePtr && !(modulePtr->m_flags & m_flag_slipped_inside))
		{
 			if(PointIsInModule(&(VDBPtr->VDB_World),modulePtr))
			{
				VisibleModules[numVisMods].DispPtr = objectPtr;
				VisibleModules[numVisMods].SortKey = smallint;
			}
			else
			{
				VECTORCH position;
				VECTORCH dist;

				position.vx = modulePtr->m_world.vx - Player->ObWorld.vx;
				position.vy = modulePtr->m_world.vy - Player->ObWorld.vy;
				position.vz = modulePtr->m_world.vz - Player->ObWorld.vz;

			    {
				    int minX = modulePtr->m_minx + position.vx;
				    int maxX = modulePtr->m_maxx + position.vx;

					if (maxX<0) /* outside maxX side */
					{
						dist.vx = maxX;
					}
					else if (minX>0) /* outside minX faces */
					{
						dist.vx = minX;
					}
					else /* between faces */
					{
						dist.vx = 0;
					}
				}
			    {
				    int minY = modulePtr->m_miny + position.vy;
				    int maxY = modulePtr->m_maxy + position.vy;

					if (maxY<0) /* outside maxY side */
					{
						dist.vy = maxY;
					}
					else if (minY>0) /* outside minY faces */
					{
						dist.vy = minY;
					}
					else /* between faces */
					{
						dist.vy = 0;
					}		  
				}
			    {
				    int minZ = modulePtr->m_minz + position.vz;
				    int maxZ = modulePtr->m_maxz + position.vz;

					if (maxZ<0) /* outside maxZ side */
					{
						dist.vz = maxZ;
					}
					else if (minZ>0) /* outside minZ faces */
					{
						dist.vz = minZ;
					}
					else /* between faces */
					{
						dist.vz = 0;
					}		  
				}

				VisibleModules[numVisMods].DispPtr = objectPtr;
				VisibleModules[numVisMods].SortKey = Magnitude(&dist);
			}

   			if(numVisMods>MAX_NUMBER_OF_VISIBLE_MODULES)
			{
				/* outside the environment! */
				textprint("MAX_NUMBER_OF_VISIBLE_MODULES (%d) exceeded!\n",MAX_NUMBER_OF_VISIBLE_MODULES);
				textprint("Possibly outside the environment!\n");
				return;
			}

			numVisMods++;
   		}
		else /* it's just an object */
		{
			VisibleObjects[numVisObjs].DispPtr = objectPtr;
			/* this sort key defaults to the object not being drawn, ie. a grenade
			behind a closed door (so there is no module behind door) would still be
			in the OnScreenBlockList but need not be drawn. */
			VisibleObjects[numVisObjs].SortKey = 0X7FFFFFFF;
			numVisObjs++;
		}
   	}
	textprint("numvismods %d\n",numVisMods);
	textprint("numvisobjs %d\n",numVisObjs);

	{
		int numMods = numVisMods;
		
		while(numMods)
		{
			int n = numMods;

			int furthestModule=0;
			int furthestDistance=0;

		   	while(n)
			{
				n--;
				if (furthestDistance < VisibleModules[n].SortKey)
				{
					furthestDistance = VisibleModules[n].SortKey;
					furthestModule = n;
				}
			}
		
			numMods--;

			VisibleModules2[numMods] = VisibleModules[furthestModule];
			VisibleModules[furthestModule] = VisibleModules[numMods];
			SortedModules = VisibleModules2;
		}
	}
	{
		int fogDistance = 0x7f000000;

		int o = numVisObjs;
		while(o--)
		{	
			DISPLAYBLOCK *objectPtr = VisibleObjects[o].DispPtr;
			int maxX = objectPtr->ObWorld.vx + objectPtr->ObRadius; 
			int minX = objectPtr->ObWorld.vx - objectPtr->ObRadius; 
			int maxZ = objectPtr->ObWorld.vz + objectPtr->ObRadius; 
			int minZ = objectPtr->ObWorld.vz - objectPtr->ObRadius; 
			int maxY = objectPtr->ObWorld.vy + objectPtr->ObRadius; 
			int minY = objectPtr->ObWorld.vy - objectPtr->ObRadius; 
			
			int numMods = 0;
			while(numMods<numVisMods)
			{
				MODULE *modulePtr = SortedModules[numMods].DispPtr->ObMyModule;

				if (maxX >= modulePtr->m_minx+modulePtr->m_world.vx) 
				if (minX <= modulePtr->m_maxx+modulePtr->m_world.vx) 
			    if (maxZ >= modulePtr->m_minz+modulePtr->m_world.vz) 
			    if (minZ <= modulePtr->m_maxz+modulePtr->m_world.vz)
			    if (maxY >= modulePtr->m_miny+modulePtr->m_world.vy) 
			    if (minY <= modulePtr->m_maxy+modulePtr->m_world.vy) 
				{
					VisibleObjects[o].SortKey=numMods;
					break;
				}
				numMods++;
			}
			if (CurrentVisionMode == VISION_MODE_PRED_SEEALIENS && objectPtr->ObStrategyBlock)
			{
				if (objectPtr->ObStrategyBlock->I_SBtype == I_BehaviourAlien)
					VisibleObjects[o].DrawBeforeEnvironment = 0;
			}
			else
			{
				VisibleObjects[o].DrawBeforeEnvironment = 0;
			}
		}

		if (fogDistance<0) fogDistance=0;
		SetFogDistance(fogDistance);
	}
	DrawingAReflection=0;
	{
		int numMods = numVisMods;
		{
			int o = numVisObjs;
			CheckWireFrameMode(WireFrameMode&2);
			while(o)
			{
				o--;

				if(VisibleObjects[o].DrawBeforeEnvironment)
				{
					DISPLAYBLOCK *dptr = VisibleObjects[o].DispPtr;
					AddShape(VisibleObjects[o].DispPtr,VDBPtr);	
					if (MirroringActive && !dptr->HModelControlBlock)
					{
						ReflectObject(dptr);

						MakeVector(&dptr->ObWorld, &VDBPtr->VDB_World, &dptr->ObView);
						RotateVector(&dptr->ObView, &VDBPtr->VDB_Mat);

				  		DrawingAReflection=1;
				  		AddShape(dptr,VDBPtr);
				  		DrawingAReflection=0;
						ReflectObject(dptr);
					}
				}
			}
		}

				ClearTranslucentPolyList();

		if (MOTIONBLUR_CHEATMODE)
		{
			for (numMods=0; numMods<numVisMods; numMods++)
			{
				MODULE *modulePtr = SortedModules[numMods].DispPtr->ObMyModule;
				
				CheckWireFrameMode(WireFrameMode&1);
		  		AddShape(SortedModules[numMods].DispPtr,VDBPtr);
				if (MirroringActive)
				{
					DISPLAYBLOCK *dptr = SortedModules[numMods].DispPtr;
					{
						ReflectObject(dptr);

						MakeVector(&dptr->ObWorld, &VDBPtr->VDB_World, &dptr->ObView);
						RotateVector(&dptr->ObView, &VDBPtr->VDB_Mat);

					  	DrawingAReflection=1;
				  		AddShape(dptr,VDBPtr);
			  			DrawingAReflection=0;
		 				ReflectObject(dptr);
					}
				}
				CheckWireFrameMode(WireFrameMode&2);
				{
					int o = numVisObjs;
					while(o)
					{
						o--;

						if(VisibleObjects[o].SortKey == numMods && !VisibleObjects[o].DrawBeforeEnvironment)
						{
							DISPLAYBLOCK *dptr = VisibleObjects[o].DispPtr;
							AddShape(VisibleObjects[o].DispPtr,VDBPtr);	
							if (MirroringActive && !dptr->HModelControlBlock)
							{
								ReflectObject(dptr);

								MakeVector(&dptr->ObWorld, &VDBPtr->VDB_World, &dptr->ObView);
								RotateVector(&dptr->ObView, &VDBPtr->VDB_Mat);

					  			DrawingAReflection=1;
						  		AddShape(dptr,VDBPtr);
					  			DrawingAReflection=0;
								ReflectObject(dptr);
							}
						}
					}
				}


				{
		 			D3D_DrawWaterTest(modulePtr);
				}
			}
		}
		else
		{
			while(numMods--)
			{
				MODULE *modulePtr = SortedModules[numMods].DispPtr->ObMyModule;
				
				CheckWireFrameMode(WireFrameMode&1);
		  		AddShape(SortedModules[numMods].DispPtr,VDBPtr);
				if (MirroringActive)
				{
					DISPLAYBLOCK *dptr = SortedModules[numMods].DispPtr;
					{
						ReflectObject(dptr);

						MakeVector(&dptr->ObWorld, &VDBPtr->VDB_World, &dptr->ObView);
						RotateVector(&dptr->ObView, &VDBPtr->VDB_Mat);

					  	DrawingAReflection=1;
				  		AddShape(dptr,VDBPtr);
			  			DrawingAReflection=0;
		 				ReflectObject(dptr);
					}
				}
				CheckWireFrameMode(WireFrameMode&2);
				{
					int o = numVisObjs;
					while(o)
					{
						o--;

						if(VisibleObjects[o].SortKey == numMods && !VisibleObjects[o].DrawBeforeEnvironment)
						{
							DISPLAYBLOCK *dptr = VisibleObjects[o].DispPtr;
							AddShape(VisibleObjects[o].DispPtr,VDBPtr);	
							if (MirroringActive && !dptr->HModelControlBlock)
							{
								ReflectObject(dptr);

								MakeVector(&dptr->ObWorld, &VDBPtr->VDB_World, &dptr->ObView);
								RotateVector(&dptr->ObView, &VDBPtr->VDB_Mat);

					  			DrawingAReflection=1;
						  		AddShape(dptr,VDBPtr);
					  			DrawingAReflection=0;
								ReflectObject(dptr);
							}
						}
					}
				}


				{
		 			D3D_DrawWaterTest(modulePtr);
				}
			}
		}
		/* KJL 12:51:00 13/08/98 - scan for hierarchical objects which aren't going to be drawn,
		and update their timers */
		{
			int o = numVisObjs;
			while(o)
			{
				o--;

				if(VisibleObjects[o].SortKey == 0x7fffffff)
				{
					if(VisibleObjects[o].DispPtr->HModelControlBlock)
					{
						DoHModelTimer(VisibleObjects[o].DispPtr->HModelControlBlock);
					}
				}
			}
		}
		
		if (MirroringActive)
		{
			DrawingAReflection=1;
			RenderPlayersImageInMirror();
			DrawingAReflection=0;
		}


		if (ScanDrawMode == ScanDrawDirectDraw) 
		{
			UnlockSurface();
		}

	}
}


static int PointIsInModule(VECTORCH *pointPtr,MODULE *modulePtr)
{
	VECTORCH position = *pointPtr;
	position.vx -= modulePtr->m_world.vx;
	position.vy -= modulePtr->m_world.vy;
	position.vz -= modulePtr->m_world.vz;

	if (position.vx >= modulePtr->m_minx) 
    	if (position.vx <= modulePtr->m_maxx) 
		    if (position.vz >= modulePtr->m_minz) 
			    if (position.vz <= modulePtr->m_maxz) 
				    if (position.vy >= modulePtr->m_miny) 
					    if (position.vy <= modulePtr->m_maxy)
							return 1;
	return 0;

}



void RenderThisDisplayblock(DISPLAYBLOCK *dbPtr)
{
	extern VIEWDESCRIPTORBLOCK *ActiveVDBList[];
	VIEWDESCRIPTORBLOCK *VDBPtr = ActiveVDBList[0];

  	AddShape(dbPtr,VDBPtr);
}

void RenderThisHierarchicalDisplayblock(DISPLAYBLOCK *dbPtr)
{
	extern VIEWDESCRIPTORBLOCK *ActiveVDBList[];
	VIEWDESCRIPTORBLOCK *VDBPtr = ActiveVDBList[0];

  	AddHierarchicalShape(dbPtr,VDBPtr);
	if (MirroringActive && dbPtr->ObStrategyBlock)
	{
		ReflectObject(dbPtr);

		MakeVector(&dbPtr->ObWorld, &VDBPtr->VDB_World, &dbPtr->ObView);
		RotateVector(&dbPtr->ObView, &VDBPtr->VDB_Mat);

	  	AddHierarchicalShape(dbPtr,VDBPtr);
		ReflectObject(dbPtr);
	}

}






