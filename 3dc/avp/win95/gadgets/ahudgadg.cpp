/*******************************************************************
 *
 *    DESCRIPTION: 	ahudgadg.cpp
 *
 *    AUTHOR: David Malcolm
 *
 *    HISTORY:  Created 14/11/97
 *
 *******************************************************************/

/* Includes ********************************************************/
#include "3dc.h"
#include "ahudgadg.hpp"
#include "trepgadg.hpp"
#include "t_ingadg.hpp"
#include "iofocus.h"
#define UseLocalAssert Yes
#include "ourasert.h"


/* Exported function definitions ***********************************/

// class AlienHUDGadget : public HUDGadget
// public:


// :: Render ()
void AlienHUDGadget :: Render (/*adj unused*/const struct r2pos& R2Pos , const struct r2rect& R2Rect_Clip, int FixP_Alpha )
{

	pTextReportGadg -> UpdateLineTimes();

	struct r2pos R2Pos_TextReport = pTextReportGadg->GetPos_Rel( R2Rect_Clip );

	GLOBALASSERT( pTextReportGadg );
	pTextReportGadg->Render ( R2Pos_TextReport, R2Rect_Clip, FixP_Alpha );

	// Render the text entry line iff input focus is set to text entry:
	GLOBALASSERT( pTextEntryGadg );
	if  ( IOFOCUS_AcceptTyping()	)
	{
		// Force the text report gadget onto the screen
	   	pTextReportGadg	-> ForceOnScreen();

		// Render the text entry gadget:
		pTextEntryGadg->Render(r2pos (	R2Pos_TextReport.x, R2Pos_TextReport.y + pTextReportGadg->GetSize (R2Rect_Clip).h), R2Rect_Clip, FixP_Alpha );
	}
	else
	{
		// Tell the text entry gadget it's not being rendered
		// (so it can fade out internally; however nothing will appear on-screen)
		pTextEntryGadg -> DontRender();
	}

}


// :: AlienHUDGadget ()
AlienHUDGadget :: AlienHUDGadget ( ) : HUDGadget("AlienHUDGadget")
{
	pTextReportGadg = new TextReportGadget();
	pTextEntryGadg = new TextEntryGadget();
}


AlienHUDGadget :: ~AlienHUDGadget()
{
	delete pTextEntryGadg;
	delete pTextReportGadg;
}


void AlienHUDGadget :: AddTextReport (SCString* pSCString_ToAdd )
// ultimately turn into an MCString
{
	GLOBALASSERT( pSCString_ToAdd );
	pTextReportGadg -> AddTextReport ( pSCString_ToAdd );

}

void AlienHUDGadget :: ClearTheTextReportQueue(void)
{
	GLOBALASSERT( pTextReportGadg );
	pTextReportGadg -> ClearQueue();
}


void AlienHUDGadget :: CharTyped(char Ch )
{
	GLOBALASSERT( pTextEntryGadg );
	pTextEntryGadg -> CharTyped (Ch	);
}

void AlienHUDGadget :: Key_Backspace(void)
{
	GLOBALASSERT( pTextEntryGadg );
	pTextEntryGadg -> Key_Backspace();
}
void AlienHUDGadget :: Key_End(void)
{
	GLOBALASSERT( pTextEntryGadg );
	pTextEntryGadg -> Key_End();
}
void AlienHUDGadget :: Key_Home(void)
{
	GLOBALASSERT( pTextEntryGadg );
	pTextEntryGadg -> Key_Home();
}
void AlienHUDGadget :: Key_Left(void)
{
	GLOBALASSERT( pTextEntryGadg );
	pTextEntryGadg -> Key_Left();
}
void AlienHUDGadget :: Key_Up(void)
{
	GLOBALASSERT( pTextEntryGadg );
	pTextEntryGadg -> Key_Up();
}
void AlienHUDGadget :: Key_Right(void)
{
	GLOBALASSERT( pTextEntryGadg );
	pTextEntryGadg -> Key_Right();
}
void AlienHUDGadget :: Key_Down(void)
{
	GLOBALASSERT( pTextEntryGadg );
	pTextEntryGadg -> Key_Down();
}
void AlienHUDGadget :: Key_Delete(void)
{
	GLOBALASSERT( pTextEntryGadg );
	pTextEntryGadg -> Key_Delete();
}
void AlienHUDGadget :: Key_Tab(void)
{
	GLOBALASSERT( pTextEntryGadg );
	pTextEntryGadg -> Key_Tab();
}

void AlienHUDGadget :: Jitter(int)
{
// adj
	// empty for now
}

void AlienHUDGadget :: SetString(const char* text)
{
	SCString* string = new SCString(text);
	pTextEntryGadg -> SetString(*string);
	string->R_Release();
}


extern "C"
{
void BringDownConsoleWithSayTypedIn()
{
	//bring down console if it isn't already down
	if(!IOFOCUS_AcceptTyping()) IOFOCUS_Toggle();
		
	//put "SAY " in the console
	((AlienHUDGadget*)HUDGadget :: GetHUD())->SetString("SAY ");
}

void BringDownConsoleWithSaySpeciesTypedIn()
{
	//bring down console if it isn't already down
	if(!IOFOCUS_AcceptTyping()) IOFOCUS_Toggle();
		
	//put "SAY_SPECIES " in the console
	((AlienHUDGadget*)HUDGadget :: GetHUD())->SetString("SAY_SPECIES ");
}
};



