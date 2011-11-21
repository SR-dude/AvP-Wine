/*******************************************************************
 *
 *    DESCRIPTION: 	textexp.cpp
 *
 *    AUTHOR: David Malcolm
 *
 *    HISTORY:  Created 18/12/97
 *
 *******************************************************************/

/* Includes ********************************************************/
#include "3dc.h"
#include "textexp.hpp"
#include "strutil.h"
#include "textin.hpp"
#define UseLocalAssert Yes
#include "ourasert.h"





/* Exported globals ************************************************/
List<TextExpansion*> TextExpansion :: List_pTextExp;
int TextExpansion ::  bVerbose = No;



/* Exported function definitions ***********************************/
// class TextExpansion
// public:
TextExpansion :: ~TextExpansion()
{
	pSCString_Short_Val -> R_Release();
	pSCString_Expansion_Val -> R_Release();

	pSCString_Description_Val -> R_Release();

	List_pTextExp . delete_entry( this );
}

void TextExpansion :: Display(void)
{
	// sends info on this expansion to the screen
	pSCString_Description_Val -> SendToScreen();
}

void TextExpansion :: AddExpansion
(
	ProjChar* pProjCh_ToParse
)
{
	/* PRECONDITION */
	{
		GLOBALASSERT( pProjCh_ToParse );
	}

	/* CODE */
	{
		List<SCString*> List_pSCString = SCString :: Parse
		(
			pProjCh_ToParse
		);

		if ( List_pSCString . size () == 2)
		{
			TextExpansion* pTextExp_New = new TextExpansion
			(
				List_pSCString[0], // pSCString_Short
				List_pSCString[1] // pSCString_Expansion				
			);

			{
				SCString* pSCString_Temp = new SCString("EXPANSION DEFINED: ");
					// LOCALISEME();

				SCString* pSCString_Feedback = new SCString
				(
					pSCString_Temp,
					pTextExp_New -> pSCString_Description_Val
				);

				pSCString_Temp -> R_Release();

				pSCString_Feedback -> SendToScreen();
				pSCString_Feedback -> R_Release();
			}
		}
		else
		{
			SCString* pSCString_Feedback = new SCString("EXPECTING TWO ARGUMENTS");
				// LOCALISEME();
			pSCString_Feedback -> SendToScreen();
			pSCString_Feedback -> R_Release();			
		}
		
		for
		(
			LIF<SCString*> oi(&List_pSCString);
			!oi.done();
			oi.next()
		)
		{
			oi() -> R_Release();
		}
	}
}

void TextExpansion :: TryToRemoveExpansion
(
	ProjChar* pProjCh_ToParse
)
{
	/* PRECONDITION */
	{
		GLOBALASSERT( pProjCh_ToParse );
	}

	/* CODE */
	{
		List<SCString*> List_pSCString = SCString :: Parse
		(
			pProjCh_ToParse
		);

		for
		(
			LIF<SCString*> oi(&List_pSCString);
			!oi.done();
			oi.next()
		)
		{
			TryToRemoveExpansion
			(
				oi()
			);
			oi() -> R_Release();
		}
	}
}

void TextExpansion :: TestForExpansions
(
	TextInputState* pTextInputState_In
)
{
	// Called by the typing code whenever a word is completed
	// This function is a friend to the class TextEntryGadget

	/* PRECONDITION */
	{
		GLOBALASSERT( pTextInputState_In );
	}

	/* CODE */
	{
		{
			// Find the word that's just been typed:
			int StartOfWordPos = pTextInputState_In -> CursorPos;
			int EndOfWordPos = pTextInputState_In -> CursorPos;

			do 
			{
				if ( StartOfWordPos > 0)
				{
					StartOfWordPos--;
				}
				else
				{
					// Reached beginning of line:
					break;
				}

				if ( pTextInputState_In -> ProjCh[ StartOfWordPos ] == ' ' )
				{
					// Reached start of word:
					StartOfWordPos++;
					break;
				}

			} while ( 1 );

			GLOBALASSERT( StartOfWordPos <= EndOfWordPos );

			if ( StartOfWordPos == EndOfWordPos )
			{
				// Empty string
				return;
			}

			// See if it matches an expansion; if so, replace as much as you can:
			// (watch out for the string getting too long)

			SCString* pSCString_Compare = new SCString
			(
				&( pTextInputState_In -> ProjCh[ StartOfWordPos ]),
				( EndOfWordPos - StartOfWordPos )
			);

			TextExpansion* pTextExp_Found = NULL;

			for
			(
				LIF<TextExpansion*> oi(&List_pTextExp);
				(
					!( oi.done() || pTextExp_Found )
				);
				oi.next()
			)
			{
				if
				(
					STRUTIL_SC_Strequal
					(
						oi() -> pSCString_Short_Val -> pProjCh(),
						pSCString_Compare -> pProjCh()
					)
				)
				{
					pTextExp_Found = oi();				
				}
			}

			pSCString_Compare -> R_Release();

			if ( !pTextExp_Found )
			{
				// No matches found:
				return;
			}			

			// Got a match; now try to actually replace the short form
			// with the long form:
			{
				if ( bVerbose )
				{
					SCString* pSCString_Temp = new SCString
					(
						"EXPANDING: "
					);
						// LOCALISEME();

					SCString* pSCString_Feedback = new SCString
					(
						pSCString_Temp,
						pTextExp_Found -> pSCString_Expansion_Val
					);

					pSCString_Temp -> R_Release();

					pSCString_Feedback -> SendToScreen();
					pSCString_Feedback -> R_Release();
				}
					// for now

				// Delete the shorthand version:
				{
					int NumToDelete = EndOfWordPos - StartOfWordPos;

					GLOBALASSERT( NumToDelete > 0 );

					while ( NumToDelete-- )
					{
						pTextInputState_In -> DeleteAt
						(
							StartOfWordPos
						);
					}
				}
				
				// Try to insert the longer version:
				{
					pTextInputState_In -> TryToInsertAt
					(
						pTextExp_Found -> pSCString_Expansion_Val,
						StartOfWordPos
					);
				}
			}

		}
	}
}

void TextExpansion :: ListAll(void)
{
	for
	(
		LIF<TextExpansion*> oi(&List_pTextExp);
		!oi.done();
		oi.next()
	)
	{
		oi() -> Display();
	}	
	
}

// private:
TextExpansion :: TextExpansion
(
	SCString* pSCString_Short,
	SCString* pSCString_Expansion
) : pSCString_Short_Val( pSCString_Short ),
	pSCString_Expansion_Val( pSCString_Expansion )
{
	pSCString_Short_Val -> R_AddRef();
	pSCString_Expansion_Val -> R_AddRef();

	// Build description string:
	{
		SCString* pSCString_Temp1 = new SCString
		(
			"\""
		);

		SCString* pSCString_Temp2 = new SCString
		(
			"\" -> \""
		);

		pSCString_Description_Val = new SCString
		(
			pSCString_Temp1,
			pSCString_Short_Val,
			pSCString_Temp2,
			pSCString_Expansion_Val,
			pSCString_Temp1
		);

		pSCString_Temp2 -> R_Release();
		pSCString_Temp1 -> R_Release();
	}


	List_pTextExp . add_entry( this );
}

void TextExpansion :: TryToRemoveExpansion
(
	SCString* pSCString_Word
)
{
	// assumed the word is already parsed/doesn't contain whitespace

	/* PRECONDITION */
	{
		GLOBALASSERT( pSCString_Word );
	}

	/* CODE */
	{
		TextExpansion* pTextExp_ToKill = NULL;

		// Find the one to be removed; removed later to avoid confusing the
		// list iteration code
		for
		(
			LIF<TextExpansion*> oi(&List_pTextExp);
			(
				!( oi.done() || pTextExp_ToKill )
			);
			oi.next()
		)
		{
			if
			(
				STRUTIL_SC_Strequal
				(
					oi() -> pSCString_Short_Val -> pProjCh(),
					pSCString_Word -> pProjCh()
				)
			)
			{
				pTextExp_ToKill = oi();
				continue;
			}
			if
			(
				STRUTIL_SC_Strequal
				(
					oi() -> pSCString_Expansion_Val -> pProjCh(),
					pSCString_Word -> pProjCh()
				)
			)
			{
				pTextExp_ToKill = oi();				
				continue;
			}
		}

		if ( pTextExp_ToKill )
		{
			{
				SCString* pSCString_Temp = new SCString("EXPANSION REMOVED: ");

				SCString* pSCString_Feedback = new SCString
				(
					pSCString_Temp,
					pTextExp_ToKill -> pSCString_Description_Val
				);

				pSCString_Temp -> R_Release();

				pSCString_Feedback -> SendToScreen();
				pSCString_Feedback -> R_Release();

			}

			delete pTextExp_ToKill;
		}
		else
		{
		}
	}
}


