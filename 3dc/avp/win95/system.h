#ifndef SYSTEM_INCLUDED

/*   AVP - WIN95

 Project Specific System Equates etc.

*/


#ifdef __cplusplus

extern "C" {

#endif

#define Yes 1
#define No 0
#define debug Yes
#define Term -1
/********************  General *****************************/

#define GlobalScale 1
#define one14 16383

#define ONE_FIXED 65536
#define ONE_FIXED_SHIFT 16

#define Digital	No

/* Offsets from *** int pointer *** for vectors and vertices */

typedef enum {

	ix,
	iy,
	iz

} PTARRAYINDICES;


/***************  CAMERA AND VIEW VOL********************/
#define NearZ 	1024
#define FarZ 	ONE_FIXED

/************* Timer and Frame Rate Independence *************/

#define TimerFrame		1000	

/******************** Clip Outcodes *****************/
					
/* Outcodes can be cast to this struct */

typedef struct oc_entry {

	int OC_Flags1;
	int OC_Flags2;
	int OC_HazeAlpha;

} OC_ENTRY;



/***************** Angles  and VALUES ******************/

#define deg22pt5 256
#define deg45 512
#define deg90 1024
#define deg180 2048
#define wrap360 4095

#define Cosine45 		46341		/* 46340.95001 cosine(45deg)*/

#define bigint 1<<30		/*  max int size*/
#define smallint -(bigint)	/* smallest int size*/


/****************** BUFFER SIZES **********************/

#define maxvdbs 1
#define maxobjects 750
extern int maxshapes;
#define maxstblocks 1000

#define maxrotpts 10000

//tuning for morph sizes
#define maxmorphPts 1024

#define maxpolys 4000
#define maxpolyptrs maxpolys
#define maxpolypts 9			/* Translates as number of vectors */
#define vsize 3					/* Scale for polygon vertex indices */


#define MaxImages 400
#define MaxImageGroups 1


/************** TEXTURE DEFINES*******************/
#define maxTxAnimblocks	100

/* Texture usage of the colour int */

#define TxDefn 16				/* Shift up for texture definition index */
#define TxLocal 0x8000			/* Set bit 15 to signify a local index */
#define ClrTxIndex 0xffff0000	/* AND with this to clear the low 16-bits */
#define ClrTxDefn 0x0000ffff	/* AND with this to clear the high 16-bits */



#define MaxD3DInstructions 1000 // includes state change instructions!!!
#define MaxD3DVertices     256


#ifdef __cplusplus
	
	};

#endif

#define SYSTEM_INCLUDED

#endif

