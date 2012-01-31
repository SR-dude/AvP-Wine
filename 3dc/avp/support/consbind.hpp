/*
	
	consbind.hpp

	Created 6/4/98 by DHM:
	
	Ability to bind strings to a key.  When the key is pressed, the string
	is passed to the console as if it had been typed.
*/

#ifndef _consbind_hpp
#define _consbind_hpp 1

#ifdef __cplusplus
#include "scstring.hpp"
#include "reflist.hpp"
#endif 

/* Type definitions *****************************************************/
#ifdef __cplusplus
	typedef enum KEY_ID BindableKey;


	class KeyBinding
	{
	public:
		static void ParseBindCommand
		(
			ProjChar* pProjCh_ToParse
		);
		static void ParseUnbindCommand
		(
			ProjChar* pProjCh_ToParse
		);

		static void UnbindAll(void);

		static void ListAllBindings(void);

		static void WriteToConfigFile(char* Filename);
			// overwrites the file with a batch file that'll
			// restore current bindings
			// Also destroys all current bindings, so that
			// next time into the game you don't get a second
			// lot when the config file is processed


		static void Maintain(void);

	public:
		static int bEcho;
		
	private:
		// Private ctor/dtor; to be called only by static fns of the class:
		KeyBinding
		(
			BindableKey theKey_ToUse,
			SCString* pSCString_ToBind
		);
		~KeyBinding();


		static void ErrorDontRecogniseKey( SCString* pSCString_Key );

		void ListThis(void) const;
			// used by ListAllBindings()

		static SCString* MakeStringForKey
		(
			BindableKey theKey
		);

		static OurBool ParseBindCommand
		(
			BindableKey& theKey_Out,
			ProjChar** ppProjCh_Out,
				// returns where in the input string to continue processing

			ProjChar* pProjCh_In
		);	// returns Yes if it understands the binding and fills out the output


	private:
		BindableKey theKey;
		SCString* pSCString_ToOutput;

		// Maintain a static list of all of objects of the class:
		static List<KeyBinding*> List_pKeyBindings;

		// A list that ought to be local to Process_WM_KEYDOWN()
		// and the Maintain() functions
		// but has been made static to the class as an optimisation:
		static RefList<SCString> PendingList;
		

	};
		// Should the binding be debounced/undebounced?
		// Should the string appear to be typed?

		// Algorithm for processing?

		// Perhaps should just look at Windows messages for key down,
		// like we should have been doing all along...
#endif /* __cplusplus */


#ifdef __cplusplus
	extern "C" {
#endif
		void CONSBIND_WriteKeyBindingsToConfigFile(void);
#ifdef __cplusplus
	};
#endif


#endif
