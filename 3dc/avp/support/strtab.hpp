/*
	
	strtab.hpp

*/

#ifndef _strtab
#define _strtab 1

#include "projtext.h"
#include "langenum.h"

#ifdef __cplusplus
#include "scstring.hpp"
#endif

#ifdef __cplusplus
	extern "C" {
#endif


	#ifdef __cplusplus
	class StringTable
	{
	public:
		static SCString& GetSCString
		(
			enum TEXTSTRING_ID stringID
		);

		static void Add( ProjChar* pProjChar );

		static void Unload(void);

	private:
		static unsigned int NumStrings;
		static SCString* pSCString[ MAX_NO_OF_TEXTSTRINGS ];
	};
	#endif // __cplusplus

	extern void AddToTable( ProjChar* pProjChar );
	extern void UnloadTable(void);

#ifdef __cplusplus
	};
#endif

#endif
