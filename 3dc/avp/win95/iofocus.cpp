/*******************************************************************
 *
 *    DESCRIPTION: 	iofocus.cpp
 *
 *    AUTHOR: David Malcolm
 *
 *    HISTORY:  Created 21/11/97
 *
 *******************************************************************/

/* Includes ********************************************************/
#include "3dc.h"
#include "iofocus.h"
#include "gadget.h"
#include "avp_menus.h"
#include "psnd.h"
/* adj moved this */
#define UseLocalAssert Yes
#include "ourasert.h"

extern "C"
{
	
extern int InGameMenusAreRunning(void);
static OurBool iofocus_AcceptTyping = No;


OurBool IOFOCUS_AcceptControls(void)
{
	return !iofocus_AcceptTyping;
}

OurBool IOFOCUS_AcceptTyping(void)
{
	return iofocus_AcceptTyping;
}

void IOFOCUS_Toggle(void)
{
	if(InGameMenusAreRunning()) return;;

	iofocus_AcceptTyping = !iofocus_AcceptTyping;
	if (iofocus_AcceptTyping)
	{
		Sound_Play(SID_CONSOLE_ACTIVATES,NULL);
	}
	else
	{
		Sound_Play(SID_CONSOLE_DEACTIVATES,NULL);
		RemoveTheConsolePlease();
	}
}



};
