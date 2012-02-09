/*******************************************************************
 *
 *    DESCRIPTION: 	hudgadg.cpp
 *
 *    AUTHOR: David Malcolm
 *
 *    HISTORY:  Created 14/11/97
 *
 *******************************************************************/

/* Includes ********************************************************/
#include "3dc.h"
#include "hudgadg.hpp"
#include "ahudgadg.hpp"
#include "trepgadg.hpp"
#define UseLocalAssert Yes
#include "ourasert.h"


/* Exported globals ************************************************/
HUDGadget* HUDGadget :: pSingleton;


/* Exported function definitions ***********************************/

// HUD Gadget is an abstract base class for 3 types of HUD; one for each species
// It's abstract because the Render() method remains pure virtual
// class HUDGadget : public Gadget
// public:

// Factory method:
HUDGadget* HUDGadget :: MakeHUD
(
)
{

	/* CODE */
	{
		return new AlienHUDGadget();
	}
}


// Destructor:
/*virtual*/ HUDGadget :: ~HUDGadget()
{
	/* PRECONDITION */
	{
		GLOBALASSERT( this == pSingleton );
	}

	/* CODE */
	{
		pSingleton = NULL;
	}
}


// protected:
// Constructor is protected since an abstract class
HUDGadget :: HUDGadget
(
	char* DebugName

) : Gadget
	(
		DebugName
	)
{
	/* PRECONDITION */
	{
		GLOBALASSERT( NULL == pSingleton );
	}

	/* CODE */
	{

		pSingleton = this;
	}
}

