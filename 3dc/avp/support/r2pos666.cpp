/*******************************************************************
 *
 *    DESCRIPTION: 	r2pos666.cpp - Daemonic r2 positions
 *
 *    AUTHOR: David Malcolm
 *
 *    HISTORY:  Created 1/12/97
 *
 *******************************************************************/

/* Includes ********************************************************/
#include "3dc.h"
#include "r2pos666.hpp"
#include "inline.h"

#define UseLocalAssert Yes
#include "ourasert.h"


// class R2PosDaemon : public Daemon
// public:
// constructor
R2PosDaemon :: R2PosDaemon (r2pos R2Pos_Int_Initial, OurBool bActive) 
// derives from three classes
: Daemon( bActive ),
  R2Pos_Int_Current( R2Pos_Int_Initial ),
  R2Pos_FixP_Current
	(
		OUR_INT_TO_FIXED( R2Pos_Int_Initial.x ),
		OUR_INT_TO_FIXED( R2Pos_Int_Initial.y )		
	)
{

}

void R2PosDaemon :: SetPos_Int(const r2pos R2Pos_Int_New )
{
	R2Pos_Int_Current = R2Pos_Int_New;
	R2Pos_FixP_Current = r2pos
	(
		OUR_INT_TO_FIXED( R2Pos_Int_Current.x ),
		OUR_INT_TO_FIXED( R2Pos_Int_Current.y )
	);
}

void R2PosDaemon :: SetPos_FixP(const r2pos R2Pos_FixP_New )
{
	R2Pos_FixP_Current = R2Pos_FixP_New;
	R2Pos_Int_Current = r2pos
	(
		OUR_FIXED_TO_INT( R2Pos_FixP_Current.x ),
		OUR_FIXED_TO_INT( R2Pos_FixP_Current.y )
	);
}

// Activity remains pure virtual...



