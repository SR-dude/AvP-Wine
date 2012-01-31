/*
	
	equiputl.hpp

	Created 8/1/98 by DHM: Utilies for dealing with equipment

*/

#ifndef _equiputl
#define _equiputl 1

#include "equipmnt.h"
#include "langenum.h"


#ifdef __cplusplus
	extern "C" {
#endif

	namespace EquipmentUtil
	{
		// Functions to get the ID for the text string "ROUNDS" and "MAGAZINES"
		// Mostly returns TEXTSTRING_ROUNDS and TEXTSTRING_MAGAZINES,
		// but some weapons have different strings

		enum TEXTSTRING_ID GetTextID_Rounds
		(
			enum WEAPON_ID WeaponID
		);

		enum TEXTSTRING_ID GetTextID_Magazines
		(
			enum WEAPON_ID WeaponID
		);

	};
	

#ifdef __cplusplus
	};
#endif

#endif
