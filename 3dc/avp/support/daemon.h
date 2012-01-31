/*
	
	daemon.h

*/

#ifndef _daemon
#define _daemon 1

#ifdef __cplusplus

#include "ourbool.h"

	extern "C" {
#endif

#define ACTIVITY_RETURN_TYPE		void

#define ACTIVITY_RVAL_CHANGE		{return;}
#define ACTIVITY_RVAL_NOCHANGE		{return;}
#define ACTIVITY_RVAL_BOOL(ignore)	{return;}

#define ACTIVITY_INPUT			void
/* note that int FixP_Time is still available to the activity
functions, but in the form of a protected member rather than
an actual parameter */

/* Type definitions *****************************************************/
	#ifdef __cplusplus

	class Daemon
	{
		// Constructors etc:
		public:
			Daemon
			(
				OurBool fActive
			);

			virtual ~Daemon();

		// Per object stuff:
		public:
			void Start(void);
			void Stop(void);
			void SetActive(OurBool fActive);
			OurBool bActive(void) const;

			virtual ACTIVITY_RETURN_TYPE Activity(ACTIVITY_INPUT) = 0;
				// the strategy to run when active; returns Yes if linked screen objects/gadgets will
				// need updating
			

		// Static stuff:
		public:
			static Daemon* p666_FirstActive;
			static Daemon* p666_Iteration_Current;
			static Daemon* p666_Iteration_Next;

			static void Maintain(int FixP_Time);

			static int DaemonTimeScale;
						
		// Private stuff:
		private:
			OurBool fIsActive_Val;
			Daemon* p666_NextActive;  // only valid if fIsActive
			Daemon* p666_PrevActive;  // only valid if fIsActive


		protected:
			// if all Daemon activity calls share one timing; this is it:
			static int FixP_Time;

	};
	#endif // ifdef __cplusplus

	extern void DAEMON_Init(void);
	extern void DAEMON_Maintain(void);



#ifdef __cplusplus
	};
#endif

#endif
