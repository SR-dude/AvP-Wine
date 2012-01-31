/*
	
	modcmds.hpp

*/

#ifndef _modcmds
#define _modcmds 1

#include "module.h"

#ifdef __cplusplus
	extern "C" {
#endif

	namespace ModuleCommands
	{
		void ListModules(void);
		
		void TryToTeleport(char* UpperCasePotentialModuleName);

		MODULE* FindModule(char* UpperCasePotentialModuleName);
			// allowed to return NULL if no match

		void TeleportPlayerToModule(MODULE* pModule_Dst);
	};
	



#ifdef __cplusplus
	};
#endif

#endif

