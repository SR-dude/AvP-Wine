/*
	
	hudgadg.hpp

*/

#ifndef _hudgadg
#define _hudgadg 1

#include "gadget.h"

#ifdef __cplusplus
	extern "C" {
#endif


#ifndef GAMEDEF_INCLUDED
#include "module.h"
// irritatingly, GAMEDEF.H assumes MODULE.H has already been included...
#include "gamedef.h"
#endif

#ifdef __cplusplus
extern "C++" {
		// JH 140298 - C++ header can only be included in C++ source and must have C++ linkage
		#include "scstring.hpp"
	}
#endif

#include "statpane.h"

	class TextReportGadget; // fully declared in TREPGADG.HPP

	// HUD Gadget is an abstract base class for 3 types of HUD; one for each species
	// It's abstract because the Render() method remains pure virtual
	class HUDGadget : public Gadget
	{
	public:
		static HUDGadget* GetHUD(void);

		// Factory method:
		static HUDGadget* MakeHUD
		(
			I_PLAYER_TYPE IPlayerType_ToMake
		);

		virtual void AddTextReport
		(
			SCString* pSCString_ToAdd
				// ultimately turn into an MCString
		) = 0;

		virtual void ClearTheTextReportQueue(void) = 0;

		

		virtual void CharTyped
		(
			char Ch
				// note that this _is _ a char
		) = 0;

		virtual void Key_Backspace(void) = 0;
		virtual void Key_End(void) = 0;
		virtual void Key_Home(void) = 0;
		virtual void Key_Left(void) = 0;
		virtual void Key_Up(void) = 0;
		virtual void Key_Right(void) = 0;
		virtual void Key_Down(void) = 0;
		virtual void Key_Delete(void) = 0;
		virtual void Key_Tab(void) = 0;

		virtual void Jitter(int FixP_Magnitude) = 0;


		// Destructor:
		virtual ~HUDGadget();

	protected:
		// Constructor is protected since an abstract class
		HUDGadget
		(
			char* DebugName
		);

	private:
		static HUDGadget* pSingleton;


	};

	// Inline methods:
		inline /*static*/ HUDGadget* HUDGadget::GetHUD(void)
		{
			return pSingleton;
		}



#ifdef __cplusplus
	};
#endif

#endif
