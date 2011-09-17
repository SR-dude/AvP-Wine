#ifndef SYSTEM_INCLUDED

/*   AVP - WIN95

 Project Specific System Equates etc.

*/


#ifdef __cplusplus

extern "C" {

#endif



/********************* SYSTEM, PLATFORM AND GAME************/

#define Yes 1
#define No 0

#ifdef _DEBUG /* standard compiler command line debugging-ON switch */
	#define debug Yes
#elif defined(NDEBUG) /* standard compiler command line debugging-OFF switch */
	#define debug No
#else /* default switch */
	#define debug Yes
#endif

#define SuppressWarnings 	Yes
							
#define SupportWindows95	Yes
#define Saturn						No
#define PSX								No
#define platform_pc 			Yes
#define InterfaceEngine		No

#define Term -1



/********************  General *****************************/

#define SupportFPMathsFunctions				Yes
#define SupportFPSquareRoot					Yes

#define GlobalScale 1



#define one14 16383

#define ONE_FIXED 65536
#define ONE_FIXED_SHIFT 16

#define Analogue	Yes
#define Digital	No


/* Offsets from *** int pointer *** for vectors and vertices */

typedef enum {

	ix,
	iy,
	iz

} PTARRAYINDICES;

#define StopCompilationOnMultipleInclusions No
#define UseProjPlatAssert                   Yes /* assert fired functions are in dxlog.c */


/***************  CAMERA AND VIEW VOL********************/
#define NearZ 	1024
#define FarZ 	ONE_FIXED

#define SupportMultiCamModules	Yes



/************* Timer and Frame Rate Independence *************/

#define TimerFrame		1000	
#define TimerFrame10	10000
#define NormalFrame ONE_FIXED
#define NormalFrameShift ONE_FIXED_SHIFT

#define UseAlarmTimer							No



/******************** Clip Outcodes *****************/
					
#define ClipTerm 		0x1fffffff

#define oc_left			0x00000001
#define oc_right		0x00000002
#define oc_up				0x00000004
#define oc_down			0x00000008
#define oc_z				0x00000010

#define oc_pntrot		0x00000020
#define oc_xrot			0x00000040
#define oc_yrot			0x00000080
#define oc_zrot			0x00000100

#define oc_z_lt_h1	0x00000200
#define oc_z_gte_h2	0x00000400
#define oc_z_haze		0x00000800
#define oc_clampz		0x00001000

/* Outcodes can be cast to this struct */

typedef struct oc_entry {

	int OC_Flags1;
	int OC_Flags2;
	int OC_HazeAlpha;

} OC_ENTRY;

/* now defunct - see next four defines*/
#define Hardware2dTextureClipping No

#define Texture3dClamping  No
#define Texture2dClamping  No
#define Texture3dSubdivide No

#define SaturnHazing No

/***************** Angles  and VALUES ******************/

#define deg10 114
#define deg22pt5 256
#define deg45 512
#define deg90 1024
#define deg180 2048
#define deg270 3072
#define deg315 3584
#define deg337pt5 3840
#define deg350 3980
#define deg360 4096
#define wrap360 4095

#define Cosine45 		46341		/* 46340.95001 cosine(45deg)*/
#define DefaultSlope 	46341

#define bigint 1<<30		/*  max int size*/
#define smallint -(bigint)	/* smallest int size*/

#define BigLowY 100000			/* Must be bigger than any Screen Y */



/****************** BUFFER SIZES **********************/

#define maxvdbs 1
#define maxobjects 750
extern int maxshapes;
#define maxstblocks 1000

#define maxrotpts 10000

//tuning for morph sizes
#define maxmorphPts 1024
#define maxmorphNormals 1024
#define maxmorphVNormals 1024

#define maxpolys 4000
#define maxpolyptrs maxpolys
#define maxpolypts 9			/* Translates as number of vectors */
#define maxvsize 6				/* 3d Phong is x,y,z,nx,ny,nz */

#define pointsarraysize maxpolypts*maxvsize

#define avgpolysize (1 + 1 + 1 + 1 + (6 * 4) + 1) /* 3d guard poly*/

#define maxscansize 8			/* e.g. Tx, U1, V1, U2, V2, X1, X2, Y */
#define vsize 3					/* Scale for polygon vertex indices */
#define numfreebspblocks 1
#define maxbspnodeitems 1

#if 0
#define MaxImages 80 /* Was 400, but now we only have one NPC in each image group */
#define MaxImageGroups 15 /* Right: One for the ENV one for the CHARACTER, one for each NPC (max 4, though only MPS4 ever has as many as 3 at present) */
#else
#define MaxImages 400
#define MaxImageGroups 1
#endif

#define oversample_8bitg_threshold 256



/************** Some Shell and Loading Platform Compiler Options ******************/

#define binary_loading						No
#undef RIFF_SYSTEM
#define RIFF_SYSTEM
#define TestRiffLoaders Yes
#define SupportFarStrategyModel				No
#define LoadingMapsShapesAndTexturesEtc		No

#define pc_backdrops						No

#define flic_player							Yes

#define SaturnCDQueueSystem					No

#define DynamicAdaptationToFrameRate		No


/***************** DRAW SORT *******************/




#define SupportTrackOptimisation			No


#define SupportBSP						 	No

#define SupportZBuffering				Yes
#define ZBufferTest							No





/***************** SHAPE DATA DEFINES************/

#define StandardShapeLanguage						Yes

#define CalcShapeExtents								No
#define SupportModules 									Yes
#define IncludeModuleFunctionPrototypes	Yes
#define SupportDynamicModuleObjects			Yes


#define SupportMorphing									Yes
#define LazyEvaluationForMorphing				No



/* Default Scale for Shape Vertices */

#define pscale 1



/***************** COLLISION DEFINES*************/
#define StandardStrategyAndCollisions		No
#define IntermediateSSACM	No		/* User preference */



/************** TEXTURE DEFINES*******************/


#define LoadPGMPalettesFromAnywhere Yes


#define maxTxAnimblocks	100

/* Texture usage of the colour int */

#define TxDefn 16				/* Shift up for texture definition index */
#define TxLocal 0x8000			/* Set bit 15 to signify a local index */
#define ClrTxIndex 0xffff0000	/* AND with this to clear the low 16-bits */
#define ClrTxDefn 0x0000ffff	/* AND with this to clear the high 16-bits */

#define draw_palette						No

#define AssumeTextures256Wide				Yes

#define remap_table_rgb_bits 4		/* Gives 4,096 entries */

#define Remap24BitBMPFilesInRaw256Mode      No
#define Remap8BitBMPFilesInRaw256Mode       No

#define SupportMIPMapping					No
#define UseMIPMax							No
#define UseMIPMin							No
#define UseMIPAvg							Yes

/* Jake's addition to mip-mapping */
/* improves resolution of mip images chosen for the scandraws */
/* 0 is the worstt resoloution, best anti-aliasing 6 the best*/
#define MIP_INDEX_SUBTRACT 1
/* What to do if the images are not the most suitable size for mip mapping ... */
/* Set this if you have inappropriately sized images and you want */
/* the mip maps' sizes to be rounded up eg. 161x82 -> 81x41 -> 21x11 -> 11x6 -> 6x3 -> 3x2 -> 2x1 */
#define MIP_ROUNDUP        Yes

#if 0
#define num_shadetable_entries	256
#define shadetable_shift		8				/* 65,535 -> 255 */
#endif

#if 1
#define num_shadetable_entries	1024
#define shadetable_shift		6
#endif

/*
 3d textures
 This defines the amount by which we can scale up U/Z, V/Z & 1/Z
 It is defined in terms of the maximum UV size and the size of an int
 A value of 10 gives a scale of 31 - 10 = 21
 1/Z, the critical value, reaches 0 at 2^21 = 2km
 Since we know that z STARTS at no less than 2^8, we can increase this value
 by 8, giving 2^29
 1/Z now reaches 0 at 2^29 = 537km
*/

#define support3dtextures					Yes
#define int3dtextures						No /* there is no D3D Zbuffer support for int 3d textures */
#define SupportGouraud3dTextures  			Yes





/*************************** WINDOWS 95 *********************/

#ifdef _MSC_VER
#define SUPPORT_MMX 0
#else
#define SUPPORT_MMX 1
#endif

#define MaxD3DInstructions 1000 // includes state change instructions!!!
#define MaxD3DVertices     256

#define optimiseflip No /* unstable at present */
#define optimiseblit Yes /* unstable at present */



/******************** PLAYSTATION ********************/





#ifdef __cplusplus
	
	};

#endif

#define SYSTEM_INCLUDED

#endif

