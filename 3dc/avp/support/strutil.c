/*

	STRUTIL.C

	Created 13/11/97 by David Malcolm from Headhunter code: more carefully specified
	versions of the C string library functions, to act on ProjChars
	rather than chars

*/

#include "3dc.h"
#include "strutil.h"

#define UseLocalAssert Yes
#include "ourasert.h" 



									
void STRUTIL_SC_WriteTerminator(ProjChar* pProjCh)
{
	/* Supplied as a function in case we switch to double-byte character sets */

	*pProjCh='\0';

}



/* Ansi to HHTS conversion ********************************************/
OurBool STRUTIL_ANSI_To_ProjChar
(
	ProjChar* pProjCh_Out,
	unsigned int MaxSize, /* includes NULL-terminator; truncates after this */
	
	LPTSTR lptszANSI_In
)
{
	/* PRECONDITION */
	{
		GLOBALASSERT(pProjCh_Out);
		GLOBALASSERT(lptszANSI_In);
	}

	/* CODE */
	{
		/* For the moment: */
		return STRUTIL_SC_SafeCopy
		(
			pProjCh_Out,
			MaxSize,

			lptszANSI_In
		);
	}
}

OurBool STRUTIL_ProjChar_To_ANSI
(
	LPTSTR lptszANSI_Out,
	unsigned int MaxSize, /* includes NULL-terminator; truncates after this */

ProjChar* pProjCh_In		
)
{
	/* PRECONDITION */
	{
		GLOBALASSERT(lptszANSI_Out);
		GLOBALASSERT(pProjCh_In);
	}

	/* CODE */
	{
		/* For the moment: */
		return STRUTIL_SC_SafeCopy
		(
			lptszANSI_Out,
			MaxSize,

			pProjCh_In
		);
	}
}

unsigned int STRUTIL_SC_Strlen
(
	const ProjChar* String
)
{
	GLOBALASSERT(String);

	return strlen(String);
}



ProjChar* STRUTIL_SC_StrCpy
(
	ProjChar* pProjCh_Dst,
	const ProjChar* pProjCh_Src
)
{
	GLOBALASSERT(pProjCh_Dst);
	GLOBALASSERT(pProjCh_Src);

	return (strcpy(pProjCh_Dst,pProjCh_Src));
}

void STRUTIL_SC_FastCat
(
	ProjChar* pProjCh_Dst,
	const ProjChar* pProjCh_Src_0,
	const ProjChar* pProjCh_Src_1			
)
{
	/* This function assumes the destination area is large enough;
	it copies Src0 followed by Src1 to the dest area.
	*/

	/* PRECONDITION */
	{
		GLOBALASSERT( pProjCh_Dst );
		GLOBALASSERT( pProjCh_Src_0 );
		GLOBALASSERT( pProjCh_Src_1 );
	}

	/* CODE */
	{
		while ( *pProjCh_Src_0 )
		{
			*( pProjCh_Dst++ ) = *( pProjCh_Src_0++ );
		}
		while ( *pProjCh_Src_1 )
		{
			*( pProjCh_Dst++ ) = *( pProjCh_Src_1++ );
		}

		/* Write terminator */
		*pProjCh_Dst = 0;
	}
}


OurBool STRUTIL_SC_Strequal
(
	const ProjChar* String1,
	const ProjChar* String2
)
{

	while 
	(
		(*String1!='\0')
		&&
		(*String2!='\0')
	)
	{
		if
		(
			(*String1)
			!=
			(*String2)
		)
		{
			return No;
		}
		String1++;
		String2++;
	}

	return 
	(
		(*String1)
		==
		(*String2)
	);
}

OurBool STRUTIL_SC_Strequal_Insensitive
(
	const ProjChar* String1,
	const ProjChar* String2
)
{

	while 
	(
		(*String1!='\0')
		&&
		(*String2!='\0')
	)
	{
		if
		(
			(tolower(*String1))
			!=
			(tolower(*String2))
		)
		{
			return No;
		}
		String1++;
		String2++;
	}

	return 
	(
		(tolower(*String1))
		==
		(tolower(*String2))
	);
}


void STRUTIL_SC_SafeCat
(
	ProjChar* pProjCh_Dst,
	unsigned int MaxSize,

	const ProjChar* pProjCh_Add
)
{
	/* PRECONDITION */
	{
		GLOBALASSERT(pProjCh_Dst);
		GLOBALASSERT(pProjCh_Add);
		GLOBALASSERT(MaxSize>0);
	}

	/* CODE */
	{
		unsigned int MaxNonTerminatingCharsToUse = (MaxSize - 1);

		while
		(
			(*pProjCh_Dst)
			&&
			(MaxNonTerminatingCharsToUse>0)
		)
		{
			pProjCh_Dst++;
			MaxNonTerminatingCharsToUse--;
		}

		while
		(
			(*pProjCh_Add)
			&&
			(MaxNonTerminatingCharsToUse>0)
		)
		{
			*pProjCh_Dst = *pProjCh_Add;

			pProjCh_Add++;
			pProjCh_Dst++;

			MaxNonTerminatingCharsToUse--;

			STRUTIL_SC_WriteTerminator(pProjCh_Dst);
		}
	}
}

size_t STRUTIL_SC_NumBytes
(
	const ProjChar* String
)
{
	return
	(
		sizeof(ProjChar)
		*
		(STRUTIL_SC_Strlen(String)+1)
	);
}



OurBool STRUTIL_SC_SafeCopy
(
	ProjChar* pProjCh_Dst,
	unsigned int MaxSize,

	const ProjChar* pProjCh_Src
)
{
	GLOBALASSERT(pProjCh_Dst);
	GLOBALASSERT(MaxSize > 0);
	GLOBALASSERT(pProjCh_Src);

	{
		unsigned int MaxNonTerminatingCharsToCopy = (MaxSize - 1);

		while
		(
			(MaxNonTerminatingCharsToCopy > 0 )
			&&
			(*pProjCh_Src != 0)
		)
		{
			MaxNonTerminatingCharsToCopy--;
			*(pProjCh_Dst++) = *(pProjCh_Src++);
		}

		STRUTIL_SC_WriteTerminator(pProjCh_Dst);

		return ( STRUTIL_SC_fIsTerminator(pProjCh_Src) );
	}

}



OurBool STRUTIL_SC_fIsTerminator
(
	const ProjChar* pProjCh
)
{
	GLOBALASSERT(pProjCh);
	return (*pProjCh == '\0');
}


