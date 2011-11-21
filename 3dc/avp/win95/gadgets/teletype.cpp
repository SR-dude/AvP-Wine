/*******************************************************************
 *
 *    DESCRIPTION: 	teletype.cpp
 *
 *    AUTHOR: David Malcolm
 *
 *    HISTORY:  Created 17/11/97
 *
 *******************************************************************/

/* Includes ********************************************************/
#include "3dc.h"
#include "teletype.hpp"	
#include "daemon.h"
#include "inline.h"
#include "trepgadg.hpp"
#define UseLocalAssert Yes
#include "ourasert.h"
#include "indexfnt.hpp"
#include "psnd.h"
#include "psndproj.h"


/* Constants *******************************************************/
#define FIXP_PIXELS_PER_SECOND	(ONE_FIXED * 768 * 16)


/* Internal type definitions ***************************************/

class TeletypeDaemon : public Daemon
{
public:
	TeletypeDaemon
	(
		TeletypeGadget* pTeletypeGadg
	);
	~TeletypeDaemon();

	ACTIVITY_RETURN_TYPE Activity(ACTIVITY_INPUT);

	OurBool HasFinishedPrinting(void);
		// so it can trigger next line to print...

	int CursorXOffset(void);

private:
	TeletypeGadget* pTeletypeGadg_Val;
	OurBool fFinished_Val;
	int FixP_TotalPixels;
		// total pixels within the string to be drawn

	int FixP_PixelsCovered;
		// pixels covered so far; also equals the x-offset of the cursor.

    int SoundHandle;


};
// Inline functions:
	inline OurBool TeletypeDaemon::HasFinishedPrinting(void)
	{
		// so it can trigger next line to print...
		return fFinished_Val;
	}
	inline int TeletypeDaemon::CursorXOffset(void)
	{
		return OUR_FIXED_TO_INT( FixP_PixelsCovered );
	}

namespace TeletypeCursor
{
	void Render
	(
		const struct r2pos& R2Pos,
		const struct r2rect& R2Rect_Clip,
		int FixP_Alpha
	);
};



/* Exported function definitions ***********************************/
// class TeletypeGadget : public Gadget
// public:
void TeletypeGadget :: Render
(
	const struct r2pos& R2Pos,
	const struct r2rect& R2Rect_Clip,
	int FixP_Alpha
)
{

    GLOBALASSERT( p666 );
    int Int_CursorXOffset = p666 -> CursorXOffset();

	IndexedFont* pLetterFont = IndexedFont :: GetFont( I_Font_TeletypeLettering );
	GLOBALASSERT( pLetterFont );

	r2pos R2Pos_Temp_Cursor = R2Pos;

	r2rect R2Rect_TeletypeClip = R2Rect_Clip;

	if
	(
		R2Rect_TeletypeClip . Width() > Int_CursorXOffset
	)
	{
		R2Rect_TeletypeClip . SetWidth( Int_CursorXOffset );
	}

	pLetterFont -> RenderString_Clipped
	(
		R2Pos_Temp_Cursor,
		R2Rect_TeletypeClip,
		FixP_Alpha,
		*pSCString_Val
	);

	if
	(
		!p666 -> HasFinishedPrinting()
	)
	{
		// then render cursor:
		struct r2pos R2Pos_Cursor = R2Pos;
		R2Pos_Cursor . x += Int_CursorXOffset;

		TeletypeCursor :: Render
		(
			R2Pos_Cursor,
			R2Rect_Clip,
			FixP_Alpha
		);
	}
}

TeletypeGadget :: TeletypeGadget
(
	TextReportGadget* pTextReportGadg,
	// parent
	SCString* pSCString
) : Gadget
	(
		"TeletypeGadget"

	)
{
	/* PRECONDITION */
	{
		GLOBALASSERT( pTextReportGadg );
		GLOBALASSERT( pSCString );
	}

	/* CODE */
	{
		pTextReportGadg_Val = pTextReportGadg;
		pSCString_Val = pSCString;
		pSCString_Val -> R_AddRef();

		p666 = new TeletypeDaemon
		(
			this
		);
		GLOBALASSERT( p666 );
	}
}

TeletypeGadget :: ~TeletypeGadget()
{
	pSCString_Val -> R_Release();

	GLOBALASSERT( p666 );

	delete p666;
}

OurBool TeletypeGadget :: HasFinishedPrinting(void)
{
	// so that the next line knows when to begin
	GLOBALASSERT( p666 );
	return p666 -> HasFinishedPrinting();
}

void TeletypeGadget :: InformParentOfTeletypeCompletion(void)
{
	pTextReportGadg_Val -> TeletypeCompletionHook();
}

void TeletypeGadget :: DirectRenderCursor
(
	const struct r2pos& R2Pos,
	const struct r2rect& R2Rect_Clip,
	int FixP_Alpha
)
{
	// called by parent so that it can render its cursor even if 
	// it's finished printing - so that the last message can have
	// a flashing cursor

	GLOBALASSERT( HasFinishedPrinting() );

	GLOBALASSERT( p666 );

    int Int_CursorXOffset = p666 -> CursorXOffset();

	struct r2pos R2Pos_Cursor = R2Pos;
	R2Pos_Cursor . x += Int_CursorXOffset;

	TeletypeCursor :: Render
	(
		R2Pos_Cursor,
		R2Rect_Clip,
		FixP_Alpha
	);	
}


/* Internal function definitions ***********************************/
// class TeletypeDaemon : public CoordinateWithStrategy
// public:
TeletypeDaemon :: TeletypeDaemon
(
	TeletypeGadget* pTeletypeGadg
) : Daemon
	(
		Yes // OurBool fActive
	)
{
	GLOBALASSERT( pTeletypeGadg );

	pTeletypeGadg_Val = pTeletypeGadg;

	fFinished_Val = No;

	FixP_TotalPixels = 
	OUR_INT_TO_FIXED
	(
		pTeletypeGadg -> GetStringWithoutReference() -> CalcSize
		(
			I_Font_TeletypeLettering
		) . w
	);

	FixP_PixelsCovered = 0;

    // Try to start looping teletype sound:
    Sound_Play
    (
        SID_TELETEXT,
        "el",
        &SoundHandle
    );
        // SOUND_NOACTIVEINDEX used as error value

}

TeletypeDaemon :: ~TeletypeDaemon()
{

    if ( SoundHandle != SOUND_NOACTIVEINDEX )
    {
        Sound_Stop
        (
            SoundHandle
        );

        SoundHandle = SOUND_NOACTIVEINDEX;
    }

}

ACTIVITY_RETURN_TYPE TeletypeDaemon :: Activity(ACTIVITY_INPUT)
{

	int FixP_PixelsThisFrame = MUL_FIXED(FIXP_PIXELS_PER_SECOND,FixP_Time);


	FixP_PixelsCovered += FixP_PixelsThisFrame;


	
	if
	(
		FixP_PixelsCovered >= FixP_TotalPixels  
	)
	{
		// Teletype has finished:
		FixP_PixelsCovered = FixP_TotalPixels;

		fFinished_Val = Yes;		
		
		Stop();


        if ( SoundHandle != SOUND_NOACTIVEINDEX )
        {
            Sound_Stop
            (
                SoundHandle
            );
            SoundHandle = SOUND_NOACTIVEINDEX;
        }

		// Tell text report line that it can trigger next string in the queue
		// (if there is one):
		pTeletypeGadg_Val -> InformParentOfTeletypeCompletion();

	}

	ACTIVITY_RVAL_CHANGE
}
// private:


// namespace TeletypeCursor
void TeletypeCursor :: Render
(
	const struct r2pos& R2Pos,
	const struct r2rect& R2Rect_Clip,
	int FixP_Alpha
)
{
		#define TELETYPE_CURSOR_WIDTH (10)
	IndexedFont* pLetterFont = IndexedFont :: GetFont( I_Font_TeletypeLettering );
	GLOBALASSERT( pLetterFont );	

	r2rect R2Rect_Area = r2rect
	(
		R2Pos,
		TELETYPE_CURSOR_WIDTH,
		pLetterFont -> GetHeight()
	);

	R2Rect_Clip . Clip
	(
		R2Rect_Area
	);

	if
	(
		R2Rect_Area . bHasArea()
	)
	{

		R2Rect_Area . AlphaFill
		(
			255, // unsigned char R,
			255, // unsigned char G,
			255, // unsigned char B,
			(FixP_Alpha/256) // unsigned char translucency
		);
	}

}


