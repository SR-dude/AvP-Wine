#ifndef EQUATES_INCLUDED


/*

 Equates & Enums for AVP

*/

#ifdef __cplusplus
extern "C" {
#endif



#define MaxObjectLights 50	/* Sources attached to the object */
#define maxlightblocks 100	/* This ought to be MORE than enough */
#define MaxLightsPerObject 100	/* Sources lighting the object */



/*
 Scenes and View Types
*/


typedef enum {

	AVP_Scene0,		/* environments*/
	AVP_Scene1,
	AVP_Scene2,
	AVP_Scene3,
	AVP_Scene4,
	AVP_Scene5,
	AVP_Scene6,
	AVP_Scene7,
	AVP_Scene8,
	AVP_Scene9,
	AVP_Scene10

} SCENE;


typedef enum {

	AVP_ViewType0,   /* worlds within env*/

} VIEWTYPE;

/*

 View Handler Function Array Indices

*/

typedef enum {

	VState_Inside,
	VState_RelativeRemote,
	VState_RelativeYRemote,
	VState_FixedRemote,
	VState_FlyBy,
	VState_LagRelRemote,
	VState_TrackingRemote,
	VState_LagRelYRemote,

	VState_Last

} VIEWSTATES;



/*

 View Interior Types

*/

typedef enum {

	IType_Default,
	IType_Body,
	IType_Car,
	IType_Aircraft,

	IType_Last

} ITYPES;




/* Map Types */

typedef enum {

	MapType_Default,
	MapType_Player,
	MapType_PlayerShipCamera,
 	MapType_Sprite,
	MapType_Term

} AVP_MAP_TYPES;


/*  Strategies */

typedef enum {

	StrategyI_Null,
	StrategyI_Camera,
	StrategyI_Player,
	StrategyI_Test,
	StrategyI_NewtonTest,
	StrategyI_HomingTest,
	StrategyI_MissileTest,
	StrategyI_GravityOnly,
	StrategyI_Database,
	StrategyI_DoorPROX,
	StrategyI_Terminal,
	StrategyI_Last		/* Always the last */

} AVP_STRATEGIES;



#ifdef __cplusplus
};
#endif

#define EQUATES_INCLUDED

#endif
