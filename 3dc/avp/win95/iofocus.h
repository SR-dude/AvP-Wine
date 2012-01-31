/*
	
	iofocus.h

	Created 21/11/97 by DHM: is input focus set to normal controls,
	or to typing?
*/

#ifndef _iofocus
#define _iofocus 1

#include "ourbool.h"
#include "gadget.h"

#ifdef __cplusplus
	extern "C" {
#endif


		extern OurBool IOFOCUS_AcceptControls(void);
		extern OurBool IOFOCUS_AcceptTyping(void);

		extern void IOFOCUS_Toggle(void);



#ifdef __cplusplus
	};
#endif

#endif
