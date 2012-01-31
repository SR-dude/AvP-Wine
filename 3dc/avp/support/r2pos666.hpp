/*
	
	r2pos666.hpp

*/

#ifndef _r2pos666
#define _r2pos666 1

#include "r2base.h"
#include "daemon.h"
#ifdef __cplusplus
	extern "C" {
#endif

/* Type definitions *****************************************************/
	class R2PosDaemon : public Daemon
	{
	public:
		R2PosDaemon
		(
			r2pos R2Pos_Int_Initial,
			OurBool bActive
		);

		~R2PosDaemon()
		{
			// empty
		}

		r2pos GetPos_Int(void) const;
		r2pos GetPos_FixP(void) const;

		void SetPos_Int(const r2pos R2Pos_Int_New );
		void SetPos_FixP(const r2pos R2Pos_FixP_New );

		// Activity remains pure virtual...

	private:
		r2pos R2Pos_Int_Current;
		r2pos R2Pos_FixP_Current;

	};
	// Inline methods:
		inline r2pos R2PosDaemon::GetPos_Int(void) const
		{
			return R2Pos_Int_Current;
		}
		inline r2pos R2PosDaemon::GetPos_FixP(void) const
		{
			return R2Pos_FixP_Current;
		}		


#ifdef __cplusplus
	};
#endif

#endif
