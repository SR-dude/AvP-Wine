/*******************************************************************
 *
 *    DESCRIPTION: 	gadget.cpp
 *
 *    AUTHOR: David Malcolm
 *
 *    HISTORY:  Created 13/11/97 
 *
 *******************************************************************/

/* Includes ********************************************************/
#include "3dc.h"
#include "gadget.h"
#include "rootgadg.hpp"
#include "r2base.h"
#include "hudgadg.hpp"
#include "ahudgadg.hpp"
#include "indexfnt.hpp"
#include "trepgadg.hpp"
#define UseLocalAssert Yes
#include "ourasert.h"


/* Exported function definitions ***********************************/

// class Gadget
// public:

/*virtual*/ Gadget :: ~Gadget()
{
	// ensure virtual destructor

	// empty
}

void Gadget :: Render_Report
(
	const struct r2pos& R2Pos,
	const struct r2rect& R2Rect_Clip,
	int FixP_Alpha			
)
{
	// use to textprint useful information about a call to "Render"
	textprint
	(
		"%s::Render at(%i,%i) clip(%i,%i,%i,%i) a=%i\n",
		DebugName,
		R2Pos . x,
		R2Pos . y,
		R2Rect_Clip . x0,
		R2Rect_Clip . y0,
		R2Rect_Clip . x1,
		R2Rect_Clip . y1,
		FixP_Alpha
	);
}
// end of class Gadget


extern void GADGET_Init(void)
{
	/* expects to be called at program boot-up time */

	/* PRECONDITION */
	{
		GLOBALASSERT( RootGadget :: GetRoot() == NULL );
	}

	/* CODE */
	{
		new RootGadget;
	}
}


extern void GADGET_UnInit(void)
{
	/* expects to be called at program shutdown time */

	/* PRECONDITION */
	{
		GLOBALASSERT( RootGadget :: GetRoot() );
	}

	/* CODE */
	{
		delete RootGadget :: GetRoot();
	}
}


extern void GADGET_Render(void)
{
	/* expects to be called within the rendering part of the main loop */


	/* CODE */
	{

		// under construction...
		GLOBALASSERT( RootGadget :: GetRoot() );
		RootGadget :: GetRoot() -> Render
		(
			r2pos :: Origin, // const struct r2pos& R2Pos,
			r2rect :: PhysicalScreen(), // const struct r2rect& R2Rect_Clip,
			ONE_FIXED // int FixP_Alpha
		);

	}
}


extern void GADGET_ScreenModeChange_Setup(void)
{
	/* expects to be called immediately before anything happens to the screen
	mode */

// adj stub
}


extern void GADGET_ScreenModeChange_Cleanup(void)
{
	/* expects to be called immediately after anything happens to the screen
	mode */


	/* PRECONDITION */
	{
		GLOBALASSERT( RootGadget :: GetRoot() );
	}

	/* CODE */
	{
		RootGadget :: GetRoot() -> RefreshHUD();
	}
}

extern void GADGET_NewOnScreenMessage( ProjChar* messagePtr )
{
	/* PRECONDITION */
	{
		GLOBALASSERT( messagePtr );
		GLOBALASSERT( RootGadget :: GetRoot() );
	}

	/* CODE */
	{
		if ( RootGadget :: GetRoot() -> GetHUD() )
		{
			SCString* pSCString_New = new SCString( messagePtr );

			RootGadget :: GetRoot() -> GetHUD() -> AddTextReport
			(
				pSCString_New
			);

			pSCString_New -> R_Release();
		}
	}
}										

extern void RemoveTheConsolePlease(void)
{
	AlienHUDGadget *HUD = (AlienHUDGadget*)RootGadget::GetRoot()->GetHUD();
	HUD->pTextReportGadg->Disappear();
}




void SCString :: SendToScreen(void)
{
	// adds this as a new on-screen message
	/* PRECONDITION */
	{
		GLOBALASSERT( RootGadget :: GetRoot() );
	}

	/* CODE */
	{
		if ( RootGadget :: GetRoot() -> GetHUD() )
		{
			RootGadget :: GetRoot() -> GetHUD() -> AddTextReport
			(
				this
			);
		}
	}

}


