/****

Windows functionality that is definitely
not project specific.

****/

// To link code to main C functions 

extern "C" {

#include "3dc.h"
#include "inline.h"


// Globals

static HANDLE RasterThread;

// Externs

extern BOOL bActive;

// These function are here solely to provide a clean
// interface layer, since Win32 include files are fully
// available in both C and C++.
// All functions linking to standard windows code are
// in win_func.cpp or win_proj.cpp, and all DirectX 
// interface functions
// should be in dd_func.cpp (in the Win95 directory)
// or d3_func.cpp, dp_func.cpp, ds_func.cpp etc.
// Project specific platfrom functionality for Win95
// should be in project/win95, in files called 
// dd_proj.cpp etc.



// This function is set up using a PeekMessage check,
// with a return on a failure of GetMessage, on the
// grounds that it might be more stable than just
// GetMessage.  But then again, maybe not.  
// PM_NOREMOVE means do not take this message out of
// the queue.  The while loop is designed to ensure
// that all messages are sent through to the Windows
// Procedure are associated with a maximum of one frame's
// delay in the main engine cycle, ensuring that e.g.
// keydown messages do not build up in the queue.

// if necessary, one could extern this flag
// to determine if a task-switch has occurred which might
// have trashed a static display, to decide whether to
// redraw the screen. After doing so, one should reset
// the flag

BOOL g_bMustRedrawScreen = FALSE;

void CheckForWindowsMessages(void)
{
	MSG         msg;
	extern signed int MouseWheelStatus;
	
	MouseWheelStatus = 0;

	// Initialisation for the current embarassingly primitive mouse 
	// handler...

	do
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage(&msg, NULL, 0, 0))
				return;

			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		
		// JH 13/2/98 - if the app is not active we should not return from the message lopp
		// until the app is re-activated
		
		if (!bActive)
		{
			ResetFrameCounter();
			Sleep(0);
			g_bMustRedrawScreen = TRUE;
		}
	}
		while (!bActive);
}


/*
  Pick up processor types,
  either from assembler test (note
  I have asm to do this, but it must 
  be converted from as / Motorola format
  to masm / Intel), or (more likely) from
  a text file left by the launcher, which
  can use GetProcessorType from the 
  mssetup api
*/


static unsigned int GetCPUId(void)
{

#if 0 // adj stub
	unsigned int retval;
	_asm
	{
		mov eax,1
		_emit 0x0f   ; CPUID (00001111 10100010) - This is a Pentium
		             ; specific instruction which gets information on the
		_emit 0xa2   ; processor. A Pentium family processor should set
		             ; bits 11-8 of eax to 5.
		mov retval,edx
	}
	return retval;
#endif
}



PROCESSORTYPES ReadProcessorType(void)
{
/* adj */


	SYSTEM_INFO SystemInfo;
	int ProcessorType;
	PROCESSORTYPES RetVal;

    GetSystemInfo(&SystemInfo);

    ProcessorType = SystemInfo.dwProcessorType;

    switch (ProcessorType)
	  {
	   case PROCESSOR_INTEL_386:
		 RetVal = PType_OffBottomOfScale;
		 break;

	   case PROCESSOR_INTEL_486:
		 RetVal = PType_486;
		 break;

	   case PROCESSOR_INTEL_PENTIUM:
		 if (GetCPUId() & 0x00800000)
		 	RetVal = PType_PentiumMMX;
		 else
		 	RetVal = PType_Pentium;
		 break;

	   default:
	     RetVal = PType_OffTopOfScale;
		 break;
	  }

	return RetVal;
}



// End of extern C declaration 

};




