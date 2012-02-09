/*
	
	gadget.h

	Base header file for Dave Malcolm's user interface "gadget" code.

	Note to "C" programmers: look at the bottom of this file

*/

#ifndef _gadget
#define _gadget 1


#define UseGadgets Yes
#include "projtext.h"

#ifdef __cplusplus
#include "r2base.h"

	class Gadget
	{
	public:
		// Pure virtual render method:
		virtual void Render
		(
			const struct r2pos& R2Pos,
			const struct r2rect& R2Rect_Clip,
			int FixP_Alpha
		) = 0;
			// Render yourself at the coordinates given, clipped by the clipping rectangle
			// Note that the position need not be at all related to the clipping rectangle;
			// it's up to the implementation to behave for these cases.
			// Both the coordinate and the clipping rectangle are in absolute screen coordinates
			// The alpha value to use is "absolute"

		virtual ~Gadget();
			// ensure virtual destructor

		char* GetDebugName(void);
		void Render_Report
		(
			const struct r2pos& R2Pos,
			const struct r2rect& R2Rect_Clip,
			int FixP_Alpha			
		);
			// use to textprint useful information about a call to "Render"

	protected:
		// Protected constructor since abstract base class
		Gadget
		(
			char* DebugName_New
		) : DebugName( DebugName_New )
		{
			// empty
		}

	private:
		char* DebugName;

	}; // end of class Gadget

	// Inline methods:
	inline char* Gadget::GetDebugName(void)
	{
		return DebugName;
	}
		


	#endif /* __cplusplus */

#ifdef __cplusplus
	extern "C" {
#endif

	extern void GADGET_Init(void);
		/* expects to be called at program boot-up time */

	extern void GADGET_UnInit(void);
		/* expects to be called at program shutdown time */

	extern void GADGET_Render(void);
		/* expects to be called within the rendering part of the main loop */


	extern void GADGET_ScreenModeChange_Cleanup(void);
		/* expects to be called immediately after anything happens to the screen
		mode */

	extern void GADGET_NewOnScreenMessage( ProjChar* messagePtr );

	extern void RemoveTheConsolePlease(void);


#ifdef __cplusplus
	};
#endif

#endif
