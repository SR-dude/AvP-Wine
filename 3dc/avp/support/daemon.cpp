/*******************************************************************
 *
 *    DESCRIPTION: Daemon code - things that need to be updated on a
 *		per-frame basis provided they are "active"
 *    AUTHOR: David Malcolm
 *
 *    HISTORY:    
 *
 *******************************************************************/

/* Includes ********************************************************/
#include "3dc.h"
#include "daemon.h"
#include "inline.h"

#define UseLocalAssert Yes
#include "ourasert.h"

/* Exported globals ************************************************/
int Daemon :: DaemonTimeScale = ONE_FIXED;
int Daemon :: FixP_Time = 0;

/* Internal globals ************************************************/
Daemon* Daemon :: p666_FirstActive = NULL;
Daemon* Daemon :: p666_Iteration_Current = NULL;
Daemon* Daemon :: p666_Iteration_Next = NULL;


// class Daemon
Daemon :: Daemon
(
	OurBool fActive
)
{
	
	fIsActive_Val = No;

	if (fActive)
	{
		Start();
	}
	else
	{
		p666_NextActive = NULL;
		p666_PrevActive = NULL;
	}
}

Daemon :: ~Daemon()
{
	if (fIsActive_Val)	
	{
		Stop();
	}

    if ( p666_Iteration_Current == this )
    {
        // Then this daemon is being processed for Activity();
        // Set the static iteration ptr to NULL to signify it has been deleted
        // so that callback hooks don't get called
        p666_Iteration_Current = NULL;
    }

}


void Daemon :: Start(void)
{
	if (!fIsActive_Val)
	{
		// Insert at front of active 666 list
		p666_PrevActive = NULL;
		p666_NextActive = p666_FirstActive;

		if (p666_FirstActive)
		{
			p666_FirstActive -> p666_PrevActive = this;
		}

		p666_FirstActive = this;

		fIsActive_Val = Yes;
	}
}

void Daemon :: Stop(void)
{
	if (fIsActive_Val)
	{
		// Remove from active 666 list
        {
            // Check against the iteration in the Maintain() static function:
            {
                if ( p666_Iteration_Next == this )
                {
                    // then this is due the next daemon to have its Activity() called;
                    // advance the static iteration ptr to this daemon's next
                    p666_Iteration_Next = p666_NextActive;
                }
            }

    		if ( p666_PrevActive )		
    		{
    			GLOBALASSERT( p666_PrevActive -> fIsActive_Val );

    			p666_PrevActive -> p666_NextActive = p666_NextActive;	
    		} 

    		if ( p666_NextActive )		
    		{
    			GLOBALASSERT( p666_NextActive -> fIsActive_Val );

    			p666_NextActive -> p666_PrevActive = p666_PrevActive;
    		}

    		if (p666_FirstActive == this)
    		{
    			p666_FirstActive = p666_NextActive;
    		}
        }

		fIsActive_Val = No;
	}
}

void Daemon :: SetActive(OurBool fActive)
{
	if (fActive)
	{
		Start();
	}
	else
	{
		Stop();
	}
}

OurBool Daemon :: bActive(void) const
{
	return fIsActive_Val;
}


void Daemon :: Maintain(int FixP_Time_ToUse)
{
	GLOBALASSERT( NULL == p666_Iteration_Current );
	GLOBALASSERT( NULL == p666_Iteration_Next );


	p666_Iteration_Current = p666_FirstActive;

	FixP_Time = FixP_Time_ToUse;

	while ( p666_Iteration_Current )
	{
		p666_Iteration_Next = p666_Iteration_Current -> p666_NextActive;

		{

			{
				{
					p666_Iteration_Current -> Activity();
				}
			}


		}
        /* 
            Advance to the next in the iteration.
            This will be either the next ptr of the current as stored above,
            or one further along the list (since the pNext one itself might
            have got deleted during the call to Activity)
        */
		p666_Iteration_Current = p666_Iteration_Next;
	}


}

void DAEMON_Init(void)
{ /* adj */
}
extern "C" {
extern int RealFrameTime;
}
void DAEMON_Maintain(void)
{

	int DaemonFrameTime =
	(
		RealFrameTime
	);

	{
		if (Daemon :: DaemonTimeScale!=ONE_FIXED)
		{
			DaemonFrameTime = MUL_FIXED(DaemonFrameTime,Daemon :: DaemonTimeScale);
		}

	}

	/* cap DaemonFrameTime if frame rate is really low */
	if (DaemonFrameTime>32768) DaemonFrameTime=32768;

	Daemon :: Maintain
	(
		DaemonFrameTime
	);
}


