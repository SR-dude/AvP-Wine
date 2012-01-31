/*
	
	trepgadg.hpp

	TextReportGadget : a class for the text report area of the HUD
*/

#ifndef _trepgadg
#define _trepgadg 1
#include "gadget.h"
#include "list_tem.hpp"
#include "scstring.hpp"
#include "reflist.hpp"
#define TEXT_REPORT_MAX_W (300)



#ifdef __cplusplus
	extern "C" {
#endif


	class TeletypeGadget; // fully declared in TELETYPE.HPP

	class TextReportDaemon_Scroll; // fully declared in MHUDGADG.HPP
	class TextReportDaemon_Disappear; // fully declared in MHUDGADG.HPP

	class CheesyDaemon_Flash; // fully declared in TREPGADG.CPP
	class CheesyDaemon_Lifetime; // fully declared in TREPGADG.CPP

	class TextReportGadget : public Gadget
	{
	public:
		void Render
		(
			const struct r2pos& R2Pos,
			const struct r2rect& R2Rect_Clip,
			int FixP_Alpha
		);

		struct r2pos TextReportGadget :: GetPos_Rel
		(
			const struct r2rect& R2Rect_Parent
		) const;

		r2size TextReportGadget :: GetSize
		(
			const struct r2rect& R2Rect_Parent
		) const;

		TextReportGadget();
		~TextReportGadget();

		void AddTextReport
		(
			SCString* pSCString_ToAdd
		);

		void ClearQueue(void);
			// clears the queue of buffered messages; could be handy if you've
			// started a listing of 300 module names

		static void ClearTheQueue(void);
			// tries to find the (singleton) queue and clears it

		void TeletypeCompletionHook(void);
			// called by the teletyp object when it has finished displaying itself

		int GetFullyOnScreenScrollCoord(void);			
		int GetPartiallyOnScreenScrollCoord(void);			
		int GetOffScreenScrollCoord(void);
			// coordinates for scroll daemons for putting the text report either on the screen
			// or off it ( in integer rather than not fixed point)

		void Disappear(void);
			// to be called only by TextReportDaemon_Disappear

		void ForceOnScreen(void);
			// called by the marine HUD gadget if input focus set to typing
			// to stop the object scrolling away

		void UpdateLineTimes();
	private:
		void AddTeletypeLine
		(
			SCString* pSCString_ToAdd
		);

		void DirectAddTeletypeLine
		(
			SCString* pSCString_ToAdd
		);

		void PostprocessForAddingTeletypeLine(void);

		// need a daemon for handling scrolling, disposal of old messages, etc.

		int MinYDisplacement(void);
		int MaxYDisplacement(void);

		// Queue of messages waiting to appear:
		RefList<SCString> RefList_SCString_ToAppear;

		// Queue of messages that are being displayed:
		List<TeletypeGadget*> List_pTeletypeGadg_Displaying;

		//time left to display each teletype line thingy
		List<int> LineTimes;

		CheesyDaemon_Flash* p666_CheeseFlash;
		CheesyDaemon_Lifetime* p666_CheeseLifetime;


		TextReportDaemon_Scroll* p666_Scroll;
			// scrolling the text box, making it appear and disappear

		TextReportDaemon_Disappear* p666_Disappear;
			// countdown for making the text box disappear

	};


#ifdef __cplusplus
	};
#endif

#endif
