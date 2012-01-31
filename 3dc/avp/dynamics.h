#ifndef _dynamics_h_ /* KJL 17:23:01 11/05/96 - is this your first time? */
#define _dynamics_h_ 1
#include "particle.h"


struct ColPolyTag
{
	int NumberOfVertices;
	VECTORCH PolyPoint[4];
    VECTORCH PolyNormal;
	DISPLAYBLOCK *ParentObject;
};


#define GRAVITY_STRENGTH 25000
#define TIME_BEFORE_GRAVITY_KICKS_IN 16384
#define MAXIMUM_STEP_HEIGHT 450

extern void ObjectDynamics(void);
extern void DynamicallyRotateObject(DYNAMICSBLOCK *dynPtr);


/* externs to shape access fns (platform specific) */
extern int SetupPolygonAccess(DISPLAYBLOCK *objectPtr);
extern void AccessNextPolygon(void);
extern void GetPolygonVertices(struct ColPolyTag *polyPtr);
extern void GetPolygonNormal(struct ColPolyTag *polyPtr);



/* extra camera movement */
extern EULER HeadOrientation;

extern int ParticleDynamics(PARTICLE *particlePtr, VECTORCH *obstacleNormalPtr, int *moduleIndexPtr);

#endif /* end of preprocessor condition for file wrapping */
