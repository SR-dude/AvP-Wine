/*******************************************************************
 *
 *    DESCRIPTION: 	rootgadg.cpp
 *
 *    AUTHOR: David Malcolm
 *
 *    HISTORY:  Created 14/11/97
 *
 *******************************************************************/

/* Includes ********************************************************/
#include "3dc.h"
#include "rootgadg.hpp"
#include "hudgadg.hpp"
#define UseLocalAssert Yes
#include "ourasert.h"



/* Imported data ***************************************************/
#ifdef __cplusplus
	extern "C"
	{
#endif
		extern signed int HUDTranslucencyLevel;
		// ranges from 0 to 255 inclusive ; convert to fixed point...

#ifdef __cplusplus
	};
#endif



/* Exported globals ************************************************/
// private:
RootGadget* RootGadget :: pSingleton = NULL;

/* Exported function definitions ***********************************/
// class RootGadget : public Gadget
// friend extern void GADGET_Init(void);
// friend extern void GADGET_UnInit(void);
	// friend functions: these get permission in order to allow
	// construction/destruction

// public:
void RootGadget :: Render
(
	const struct r2pos& R2Pos,
	const struct r2rect& R2Rect_Clip,
	int FixP_Alpha
)
{

	/* CODE */
	{

		if ( pHUDGadg )
		{
			// HUDTranslucencyLevel ranges from 0 to 255 inclusive ; convert to fixed point...

			GLOBALASSERT( HUDTranslucencyLevel >= 0);
			GLOBALASSERT( HUDTranslucencyLevel <= 255 );

			pHUDGadg -> Render
			(
				R2Pos,
				R2Rect_Clip,
				(HUDTranslucencyLevel << 8) // int FixP_Alpha
			);
		}
	}
}

void RootGadget :: RefreshHUD(void)
{

	/* CODE */
	{
		// For the moment, destroy any HUD:
		if ( pHUDGadg )
		{
			delete pHUDGadg;
			pHUDGadg = NULL;
		}

		GLOBALASSERT( NULL == pHUDGadg );

		// And then recreate if necessary:
		{
			extern AVP_GAME_DESC AvP;		 /* game description */

			if
			(
				AvP.GameMode == I_GM_Playing
			)
			{
				pHUDGadg = HUDGadget :: MakeHUD
				(
				);
			}
		}
	}
}



// private:
RootGadget :: RootGadget
(
) : Gadget
	(
		"RootGadget"
	)		
{
	/* PRECONDITION */
	{
		GLOBALASSERT( pSingleton == NULL );
	}

	/* CODE */
	{
		pHUDGadg = NULL;

		pSingleton = this;
	}
}

RootGadget :: ~RootGadget()
{
	/* PRECONDITION */
	{
		GLOBALASSERT( pSingleton == this );
	}

	/* CODE */
	{
		pSingleton = NULL;

		if ( pHUDGadg )
		{
			delete pHUDGadg;
		}
	}
}



