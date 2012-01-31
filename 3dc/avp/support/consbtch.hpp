/*
	
	consbtch.hpp

	Console batch file support

*/

#ifndef _consbtch
#define _consbtch 1

#include "scstring.hpp"


#ifdef __cplusplus
	extern "C" {
#endif

	class BatchFileProcessing
	{
	public:
		static OurBool Run(char* Filename);
			// Tries to find the file, if it finds it it reads it,
			// adds the non-comment lines to the pending list, and returns Yes
			// If it can't find the file, it returns No

	public:
		static int bEcho;
	};

#ifdef __cplusplus
	};
#endif

#endif
