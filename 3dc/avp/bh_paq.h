/* Patrick 18/2/97 ------------------------------------------------
  Header file for alien queen and predator-alien support functions
  -----------------------------------------------------------------*/

#ifndef _bhpaq_h_
#define _bhpaq_h_ 1

#ifdef __cplusplus
extern "C" {
#endif

#include "bh_pred.h"
 
/* Patrick 18/2/97 ------------------------------------------------
  Some enumerations
-----------------------------------------------------------------*/
typedef enum PaqSequence
{
	PaqSQ_Run,
	PaqSQ_Attack,
	PaqSQ_Stand,
	PaqSQ_Dying,
	PaqSQ_Dead,
	PaqSQ_Attack2,
}PAQ_SEQUENCE;

typedef enum paq_far_bhstate
{
	PAQFS_Wait,
	PAQFS_Hunt,
	PAQFS_Wander,
	PAQFS_Dying,
}PAQ_FAR_BHSTATE;

typedef enum paq_near_bhstate
{
	PAQNS_Wait,
	PAQNS_Approach,
	PAQNS_Attack,
	PAQNS_Wander,
	PAQNS_Avoidance,
	PAQNS_Dying,
}PAQ_NEAR_BHSTATE;

/* Patrick 18/2/97 ------------------------------------------------
  Some structures
-----------------------------------------------------------------*/
typedef struct paqStatusBlock
{
	signed int health;
	signed int nearSpeed;
	signed int damageInflicted;
	PAQ_FAR_BHSTATE FarBehaviourState;	
	PAQ_NEAR_BHSTATE NearBehaviourState;	
	int stateTimer;  		
	SHAPEANIMATIONCONTROLLER ShpAnimCtrl;
	HMODELCONTROLLER HModelController;
	NPC_MOVEMENTDATA moveData;
	NPC_WANDERDATA wanderData;
}PAQ_STATUS_BLOCK;

typedef struct tools_data_paq
{
	struct vectorch position;
	int shapeIndex;
	char nameID[SB_NAME_LENGTH];
}TOOLS_DATA_PAQ;

 	   	
/* Patrick 18/2/97 ------------------------------------------------
  Some prototypes
-----------------------------------------------------------------*/
void InitPredAlBehaviour(void* bhdata, STRATEGYBLOCK *sbPtr);
void InitQueenBehaviour(void* bhdata, STRATEGYBLOCK *sbPtr);
void PAQBehaviour(STRATEGYBLOCK *sbPtr);
void MakePAQNear(STRATEGYBLOCK *sbPtr);
void MakePAQFar(STRATEGYBLOCK *sbPtr);
void PAQIsDamaged(STRATEGYBLOCK *sbPtr, DAMAGE_PROFILE *damage, int multiple);
		

#ifdef __cplusplus
}
#endif

#endif
