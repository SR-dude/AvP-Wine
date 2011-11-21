#include "3dc.h"
#include <sys/stat.h>
#include <string.h>
#include "inline.h"
#include "module.h"
#include "krender.h"
#include "chnktexi.h"
#include "d3d_hud.h"
#define UseLocalAssert	Yes
#include "ourasert.h"
#include "hud_layout.h"

#undef textprint

/* externs for commonly used global variables and arrays */
extern SCENE				Global_Scene;
extern SHAPEHEADER			**mainshapelist;
extern SHAPEHEADER			*testpaletteshapelist[];
extern SCREENDESCRIPTORBLOCK	ScreenDescriptorBlock;
extern int				sine[];
extern int				cosine[];
extern int				AdaptiveHazingFlag;
extern int				*Global_ShapeNormals;
extern int				*Global_ShapePoints;
extern int				*ItemPointers[];
extern int				ItemData[];
extern char				projectsubdirectory[];

extern int				WinLeftX;
extern int				WinRightX;
extern int				WinTopY;
extern int				WinBotY;

extern int				WindowRequestMode;
extern int				VideoRequestMode;
extern int				ZBufferRequestMode;
extern int				RasterisationRequestMode;
extern int				SoftwareScanDrawRequestMode;
extern int				DXMemoryRequestMode;

extern int				TotalVideoMemory;
extern int				NumAvailableVideoModes;
extern VIDEOMODEINFO		AvailableVideoModes[];

extern int				memoryInitialisationFailure;

extern IMAGEHEADER			ImageHeaderArray[];		/* Array of Image Headers */

/* Global Variables for PC Watcom Functions and Windows 95! */
int						DrawMode = DrawPerVDB;

/*
 * Win95 default ought to be per frame ;
 * Timer
 */
long						lastTickCount;

unsigned char				*ScreenBuffer = 0;		/* Ensure initialised to Null */
unsigned char				*ScreenBuffer2 = 0;

unsigned char				LPTestPalette[1024];	/* to cast to lp */

int						InputMode;

int						VideoMode;
int						VideoModeType;
int						VideoModeTypeScreen;
int						WindowMode;
int						ScanDrawMode;
int						ZBufferMode;
int						DXMemoryMode;
unsigned char				AttemptVideoModeRestart;
VIDEORESTARTMODES			VideoRestartMode;

PROCESSORTYPES				ProcessorType;
BOOL						MMXAvailable;

unsigned char				*TextureLightingTable = 0;

unsigned char				*PaletteRemapTable = 0;

int						**ShadingTableArray = 0;
int						NumShadingTables = 0;

unsigned char				**PaletteShadingTableArray = 0;
int						NumPaletteShadingTables = 0;

int						FrameRate;
int						NormalFrameTime;
int						PrevNormalFrameTime;
extern int				CloakingPhase;

/* These two are dummy values to get the DOS platform to compile */
unsigned char				KeyCode;
unsigned char				KeyASCII;

unsigned char				*palette_tmp;
static VIEWDESCRIPTORBLOCK	*vdb_tmp;
static SCREENDESCRIPTORBLOCK	*sdb_tmp;

/* Keyboard */
unsigned char				KeyboardInput[MAX_NUMBER_OF_INPUT_KEYS];
unsigned char				GotAnyKey;

/*
 * Input communication with Windows Procedure ;
 * Print system
 */

int						textprintPosX;
int						textprintPosY;
IMAGEHEADER				*fontHeader;

/* Added 28/11/97 by DHM: boolean for run-time switching on/off of textprint */
int						bEnableTextprint = No;

/* Added 28/1/98 by DHM: as above, but applies specifically to textprintXY */
int						bEnableTextprintXY = Yes;

/* Palette */
unsigned char				PaletteBuffer[768 + 1];

/* Test Palette */
unsigned char				TestPalette[768];
unsigned char				TestPalette2[768];

/*
 * KJL 11:48:45 28/01/98 - used to scale NormalFrameTime, so the game can be
 * slowed down
 */
int						TimeScale = 65536;

/* KJL 16:00:11 28/01/98 - unscaled frame time */
int						RealFrameTime;
int						GlobalFrameCounter;
int						RouteFinder_CallsThisFrame;

/* KJL 15:08:43 29/03/98 - added to give extra flexibility to debugging text */
int						PrintDebuggingText(const char *t, ...);
int						ReleasePrintDebuggingText(const char *t, ...);

/*
 =======================================================================================================================
     IO and Other Functions for the PC ;
     Get Shape Data Function returns a pointer to the Shape Header Block
 =======================================================================================================================
 */
SHAPEHEADER *GetShapeData(int shapenum)
{
	if(shapenum >= 0 && shapenum < maxshapes)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		SHAPEHEADER	*sptr = mainshapelist[shapenum];
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		return sptr;
	}

	return NULL;
}

/*
 =======================================================================================================================
     Platform specific VDB functions for Initialisation and ShowView()
 =======================================================================================================================
 */
void PlatformSpecificVDBInit(VIEWDESCRIPTORBLOCK *vdb)
{
	vdb_tmp = vdb;

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PlatformSpecificShowViewEntry(VIEWDESCRIPTORBLOCK *vdb, SCREENDESCRIPTORBLOCK *sdb)
{

	vdb_tmp = vdb;
	sdb_tmp = sdb;

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void PlatformSpecificShowViewExit(VIEWDESCRIPTORBLOCK *vdb, SCREENDESCRIPTORBLOCK *sdb)
{

	vdb_tmp = vdb;
	sdb_tmp = sdb;

}

/*
 =======================================================================================================================
     Convert UNIX to MS-DOS
 =======================================================================================================================
 */
void GetDOSFilename(char *fnameptr)
{
	while(*fnameptr)
	{
		if(*fnameptr == 0x2f) *fnameptr = 0x5c;
		fnameptr++;
	}
}

/*
 =======================================================================================================================
     Compare two strings. Return Yes if they're the same, else No. ;
     IMPORTANT!!! This function is not ideal!!! It is used here because this is only an initialisation stage, but if
     you want to compare strings elsewhere you should either use the C library function or (if there's a problem with
     that) write your own.
 =======================================================================================================================
 */
int CompareStringCH(char *string1, char *string2)
{
	/*~~~~~~~~~~~*/
	char *srtmp;
	char *srtmp2;
	int	slen1 = 0;
	int	slen2 = 0;
	int	i;
	/*~~~~~~~~~~~*/

	srtmp = string1;

	while(*srtmp++ != 0) slen1++;

	srtmp = string2;

	while(*srtmp++ != 0) slen2++;

	if(slen1 != slen2)
		return No;
	else
	{
		srtmp = string1;
		srtmp2 = string2;

		for(i = slen1; i != 0; i--)
			if(*srtmp++ != *srtmp2++) return No;

		return Yes;
	}
}

/*
 =======================================================================================================================
     Compare two filenames. The first filename is assumed to be raw i.e. has no project subdirectory appended. The
     second is assumed to be ready for use. Make a copy of both strings, prefix the copy of the first with the project
     subdirectory and convert them to DOS format before the comparison.
 =======================================================================================================================
 */
int CompareFilenameCH(char *string1, char *string2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	char *srtmp1;
	char *srtmp2;
	int	slen1 = 0;
	int	slen2 = 0;
	int	i;
	char fname1[ImageNameSize];
	char fname2[ImageNameSize];
	/*~~~~~~~~~~~~~~~~~~~~~~~*/


	/* Make a copy of string 1, adding the project subdirectory */
	srtmp1 = projectsubdirectory;
	srtmp2 = fname1;
	while(*srtmp1) *srtmp2++ = *srtmp1++;
	srtmp1 = string1;
	while(*srtmp1) *srtmp2++ = *srtmp1++;
	*srtmp2 = 0;

	/* Make a copy of string 2 */
	srtmp1 = string2;
	srtmp2 = fname2;
	while(*srtmp1) *srtmp2++ = *srtmp1++;
	*srtmp2 = 0;

	/* How long are they? */
	srtmp1 = fname1;
	while(*srtmp1++ != 0) slen1++;

	srtmp2 = fname2;
	while(*srtmp2++ != 0) slen2++;

	fname1[slen1] = 0;	/* Term */
	fname2[slen2] = 0;


	GetDOSFilename(fname1);
	GetDOSFilename(fname2);

	if(slen1 != slen2)
	{
		return No;
	}

	srtmp1 = fname1;
	srtmp2 = fname2;


	for(i = slen1; i != 0; i--)
	{
		if(*srtmp1++ != *srtmp2++)
		{
			return No;
		}
	}

	return Yes;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */

int NearestColour(int rs, int gs, int bs, unsigned char *palette)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	int		i;
	VECTORCH	p0;
	VECTORCH	p1;
	int		nearest_index;
	int		nearest_delta;
	int		d;
	/*~~~~~~~~~~~~~~~~~~~~*/

	p0.vx = rs;
	p0.vy = gs;
	p0.vz = bs;

	nearest_index = 0;
	nearest_delta = bigint;

	for(i = 0; i < 256; i++)
	{
		p1.vx = palette[0];
		p1.vy = palette[1];
		p1.vz = palette[2];

		d = FandVD_Distance_3d(&p0, &p1);

		if(d < nearest_delta)
		{
			nearest_delta = d;
			nearest_index = i;
		}

		palette += 3;
	}

	return nearest_index;
}




/* PC Video Mode Function Arrays */
void (*SetVideoMode[]) (void) = { 0 };

/*
 =======================================================================================================================
     Initialise System and System Variables
 =======================================================================================================================
 */
void InitialiseSystem(HINSTANCE hInstance, int nCmdShow)
{
	/*~~~~*/
	BOOL rc;
	/*~~~~*/

	/* Pick up processor type */
	ProcessorType = ReadProcessorType();

	if
	(
		(ProcessorType == PType_PentiumMMX)
	||	(ProcessorType == PType_Klamath)
	||	(ProcessorType == PType_OffTopOfScale)
	) MMXAvailable = TRUE;
	else
		MMXAvailable = FALSE;

	/* Copy initial requests to current variables, subject to later modification. */
	VideoMode = VideoRequestMode;
	WindowMode = WindowRequestMode;

	/* Initialise dubious restart system for ModeX emulation and other problems */
	AttemptVideoModeRestart = No;

	VideoRestartMode = NoRestartRequired;

	/*
	 * Potentially a whole suite of caps functions could be sensibly called from here,
	 * to determine available sound hardware, network links, 3D hardware acceleration
	 * etc ;
	 * Initialise the basic Direct Draw object, find a hardware 3D capable driver if
	 * possible and appropriate, and determine what display modes, available video
	 * memory etc exist.
	 */
	if(InitialiseDirectDrawObject() == FALSE)
	/*
	 * If we cannot get a video mode, fail. No point in a non debugging option for
	 * this.
	 */
	{
		ReleaseDirect3D();
		exit(0x997799);
	}

	/*
	 * Initialise global to say whether we think there is an onboard 3D acceleration
	 * card / motherboard built-in
	 */
	TestInitD3DObject();

	/*
	 * This is (HOPEFULLY!!) now the right place to put this call. Note that it is not
	 * absolutely certain that we can do test blits from DirectDraw without setting a
	 * cooperative level, however... And note also that MMX works better with the back
	 * buffer in system memory...
	 */
	TestMemoryAccess();

	/* Initialise main window, windows procedure etc */
	rc = InitialiseWindowsSystem(hInstance, nCmdShow, WinInitFull);

	/* Initialise input interface */
	memset((void *) KeyboardInput, No, MAX_NUMBER_OF_INPUT_KEYS);
	GotAnyKey = No;

	/* launch Direct Input */
	InitialiseDirectInput();
	InitialiseDirectKeyboard();
	InitialiseDirectMouse();
	InitJoysticks();

	/* Initialise textprint system */
	textprintPosX = 0;
	textprintPosY = 0;

	InitPrintQueue();

	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		/* CDF 4/2/97 */
		extern void	ConstructOneOverSinTable(void);
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ConstructOneOverSinTable();
	}
}

/*
 =======================================================================================================================
     Exit the system
 =======================================================================================================================
 */
void ExitSystem(void)
{
	/* Game specific exit functions */
	ExitGame();

	/* Added by Mark so that Direct Sound exits cleanly */


	/*
	 * Shaft DirectDraw and hit Direct3D with a blunt Bill. Note that ReleaseDirect3D
	 * is currently responsible for whacking DirectDraw and DirectInput as well;
	 * I should probably rename it ReleaseDirectX sometime...
	 */
	ReleaseDirect3D();

	/* Kill windows procedures */
	ExitWindowsSystem();
}

/*
 =======================================================================================================================
     Timer functions are based on Windows timer giving number of millisecond ticks since Windows was last booted. Note
     this will wrap round after Windows has been up continuously for approximately 49.7 days. This is not considered to
     be too significant a limitation...
 =======================================================================================================================
 */
void ResetFrameCounter(void)
{
	lastTickCount = timeGetTime();

	/*
	 * KJL 15:03:33 12/16/96 - I'm setting NormalFrameTime too, rather than checking
	 * that it's non-zero everytime I have to divide by it, since it usually is zero
	 * on the first frame.
	 */
	NormalFrameTime = 65536 >> 4;
	PrevNormalFrameTime = NormalFrameTime;

	RealFrameTime = NormalFrameTime;
	FrameRate = 16;
	GlobalFrameCounter = 0;
	CloakingPhase = 0;

	RouteFinder_CallsThisFrame = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void FrameCounterHandler(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int	newTickCount = timeGetTime();
	int	fcnt;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	fcnt = newTickCount - lastTickCount;
	lastTickCount = newTickCount;

	if(fcnt == 0) fcnt = 1;	/* for safety */

	FrameRate = TimerFrame / fcnt;

	PrevNormalFrameTime = NormalFrameTime;
	NormalFrameTime = DIV_FIXED(fcnt, TimerFrame);

	RealFrameTime = NormalFrameTime;
	{
		if(TimeScale != ONE_FIXED)
		{
			NormalFrameTime = MUL_FIXED(NormalFrameTime, TimeScale);
		}
	}

	/* cap NormalFrameTime if frame rate is really low */
	if(NormalFrameTime > 16384) NormalFrameTime = 16384;
	GlobalFrameCounter++;
	CloakingPhase += NormalFrameTime >> 5;

	RouteFinder_CallsThisFrame = 0;
}

/*
 * This jump table has been provided solely to ensure compatibility with DOS and
 * other versions.
 */
void (*UpdateScreen[]) (void) =
{
	FlipBuffers,
	FlipBuffers,
	FlipBuffers,
	FlipBuffers,
	FlipBuffers,
	FlipBuffers,
	FlipBuffers,
	FlipBuffers,
	FlipBuffers,
	FlipBuffers,
	FlipBuffers,
	FlipBuffers,
	FlipBuffers,
	FlipBuffers,
	FlipBuffers,
	FlipBuffers,
	FlipBuffers,
	FlipBuffers,
	FlipBuffers,
	FlipBuffers,
	FlipBuffers,
	FlipBuffers,
	FlipBuffers,
	FlipBuffers
};

/*
 =======================================================================================================================
     Not supported in Windows 95 !!!
 =======================================================================================================================
 */
void PlotPixelTest(int x, int y, unsigned char col)
{ /* adj stub */
}

/*
 =======================================================================================================================
     Wait for Return Key Such a function may not be defined on some platforms On Windows 95 the description of this
     function has been changed, so that it calls FlushTextprintBuffer and FlipBuffers before going into the actual
     WaitForReturn code. This is necessary if it is to behave in the same way after a textprint call as it does on the
     DOS platform.
 =======================================================================================================================
 */
void WaitForReturn(void)
{
	/*~~~~~~~~~~~~~~~~*/
	/* Crude but probably serviceable for now */
	long SavedTickCount;
	/*~~~~~~~~~~~~~~~~*/

	SavedTickCount = lastTickCount;

	/* Display any lingering text */
	FlushTextprintBuffer();
	FlipBuffers();

	while(!(KeyboardInput[KEY_CR])) DirectReadKeyboard();

	lastTickCount = SavedTickCount;
}

/*
 =======================================================================================================================
     By copying the globals here we guarantee that game functions will receive a set of input values updated at a
     defined time
 =======================================================================================================================
 */
void ReadUserInput(void)
{
	DirectReadMouse();
	ReadJoysticks();
	DirectReadKeyboard();
}

/*
 =======================================================================================================================
     At present all keyboard and mouse input is handled through project specific functionality in win_func, and all
     these functions are therefore empty. Later we may port to DirectInput, at which point we may reactivate these.
 =======================================================================================================================
 */
void ReadKeyboard(void)
{ /* adj stub */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ReadMouse(void)
{/* adj stub */
}

/*
 =======================================================================================================================
     Not NECESSARILY the standard functionality, but it seems good enough to me...
 =======================================================================================================================
 */
void CursorHome(void)
{
	/* Reset positions for textprint system */
	textprintPosX = 0;
	textprintPosY = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GetProjectFilename(char *fname, char *image)
{
	/*~~~~~~*/
	char *src;
	char *dst;
	/*~~~~~~*/

	src = projectsubdirectory;
	dst = fname;

	while(*src) *dst++ = *src++;

	src = image;

	while(*src) *dst++ = *src++;

	*dst = 0;
}

/*
 =======================================================================================================================
     Attempts to load the image file. Returns a pointer to the image if successful, else zero. Image Header is filled
     out if successful, else ignore it. NOTE The pointer to the image data is also stored in the image header.
 =======================================================================================================================
 */
TEXTURE *LoadImageCH(char *fname, IMAGEHEADER *iheader)
{
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ConvertToDDPalette(unsigned char *src, unsigned char *dst, int length, int flags)
{
	/*~~~*/
	int	i;
	/*~~~*/

	/* Copy palette, introducing flags and shifting up to 8 bit triple */
	for(i = 0; i < length; i++)
	{
		*dst++ = (*src++) << 2;
		*dst++ = (*src++) << 2;
		*dst++ = (*src++) << 2;
		*dst++ = flags;
	}
}

/*
 * Platform specific version of "printf()" Not all platforms support, or indeed
 * are ABLE to support printf() in its general form. For this reasons calls to
 * textprint() are made through this function. ;
 * If debug or textprintOn are not defined, these function defintions are
 * collapsed to a simple return value, which should collapse to no object code
 * under optimisation. The whole issue of turning on or off textprint beyond this
 * point is hereby left to Kevin and Chris H to fight to the death about...
 */

/*
 * Dave Malcolm 21/11/96: I have rewritten the Win95 textprint routines below. It
 * should now support: - carriage returns are no longer automatic at the end of lines;
 * there is a #define if you want this behaviour back - carriage return characters
 * cause a carriage return - wraparound at the right-hand edge of the screen, with
 * textprint() wrapping to the left-hand edge, and textprintXY() wrapping back to
 * the X coordinate - clipping at the bottom edge of the screen - a warning
 * message if text has been lost due to clipping or buffer overflows etc. - a
 * y-offset that can be used to scroll up and down the text overlay output from
 * textprint ;
 * VERSION SETTINGS:
 */



/*
 * We extract arguments into a buffer, with a dodgy hack to increase it in size to
 * give more defence against buffer overflows;
 * there seems to be no easy & robust way to give vsprintf() a buffer size... This
 * buffer is reset once per string per frame
 */
#define PARANOIA_BYTES		(1024)
#define TEXTPRINT_BUFFER_SIZE (MaxMsgChars + PARANOIA_BYTES + 1)
static char	TextprintBuffer[TEXTPRINT_BUFFER_SIZE] = "";

/*
 -----------------------------------------------------------------------------------------------------------------------
     The PRINTQUEUEITEM structure from PLATFORM.H is not used by my system;
     instead of queueing strings to be displayed we do it on a character by character basis, with a limit on the total
     number of chars per frame. This limit is set to be (MaxMsgChars*MaxMessages), which gives the same power and more
     flexibility than the old system. When the queue is full, additional characters get ignored. This is queue is reset
     once per frame.
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct daveprintchar
{
	char CharToPrint;
	int	x, y;
} DAVEPRINTCHAR;

#define DHM_PRINT_QUEUE_SIZE	(MaxMsgChars * MaxMessages)

static DAVEPRINTCHAR	DHM_PrintQueue[DHM_PRINT_QUEUE_SIZE];
static int			DHM_NumCharsInQueue = 0;

static int			fTextLost = No;
static char			TextLostMessage[] = "textprint warning:TEXT LOST";

volatile int			textprint_Y_offset = 0;

/*
 =======================================================================================================================
     Dave's version of initialising the print queue
 =======================================================================================================================
 */
void InitPrintQueue(void)
{
	DHM_NumCharsInQueue = 0;
	fTextLost = No;
}

/*
 =======================================================================================================================
     Old systems comment: Write all messages in buffer to screen (to be called at end of frame, after surface / execute
     buffer unlock in DrawItemListContents, so that text appears at the front of the back buffer immediately before the
     flip). This is Dave's version of the same:
 =======================================================================================================================
 */
void FlushTextprintBuffer(void)
{

	/* CODE: */
	{
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			int			i;
			DAVEPRINTCHAR	*pDPR = &DHM_PrintQueue[0];
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			for(i = 0; i < DHM_NumCharsInQueue; i++)
			{
				D3D_BlitWhiteChar(pDPR->x, pDPR->y, pDPR->CharToPrint);
				pDPR++;
			}

			if(fTextLost)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				/*
				 * Display error message in case test has been lost due to clipping of Y edge, or
				 * buffer overflow
				 */
				int	i;
				int	NumChars = strlen(TextLostMessage);
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				for(i = 0; i < NumChars; i++)
				{
					/*
					 * BlitWin95Char(TEXT_LOST_X+(i*CharWidth),TEXT_LOST_Y,TextLostMessage[i]);
					 */
				}

				fTextLost = No;
			}
		}

		DHM_NumCharsInQueue = 0;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int LastDisplayableXForChars(void)
{
	return ScreenDescriptorBlock.SDB_Width - CharWidth;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int LastDisplayableYForChars(void)
{
	return ScreenDescriptorBlock.SDB_Height - CharHeight;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void DHM_AddToQueue(int x, int y, char Ch)
{
	if((y >= 0) && (y <= LastDisplayableYForChars()))
	{
		if(DHM_NumCharsInQueue < DHM_PRINT_QUEUE_SIZE)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			DAVEPRINTCHAR	*pDPR = &DHM_PrintQueue[DHM_NumCharsInQueue++];
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			/* We insert into the queue at this position, updating the length of the queue */
			pDPR->x = x;
			pDPR->y = y;
			pDPR->CharToPrint = Ch;
		}
		else
		{
			/*
			 * Otherwise the queue if full, we will have to ignore this char;
			 * set an error flag so we get a message
			 */
			fTextLost = Yes;
		}
	}
	else
	{
		/*
		 * Otherwise the text is off the top or bottom of the screen;
		 * set an error flag to get a message up
		 */
		fTextLost = Yes;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int DHM_MoveBufferToQueue(int *pPosX, int *pPosY, int fZeroLeftMargin)
{
	/*
	 * Function takes two integers by reference (using pointers), and outputs whatever
	 * is in the string buffer into the character queue, so that code can be shared by
	 * textprint() and textprintXY() Returns "number of lines": any carriage returns
	 * or word wraps ;
	 */

	/* CODE */
	{
		/*~~~~~~~~~~~~~~*/
		int	NumLines = 0;
		int	LeftMarginX;
		/*~~~~~~~~~~~~~~*/

		if(fZeroLeftMargin)
		{
			LeftMarginX = 0;
		}
		else
		{
			LeftMarginX = *pPosX;
		}

		/*
		 * Iterate through the string in the buffer, adding the individual characters to
		 * the queue
		 */
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			char *pCh = &TextprintBuffer[0];
			int	SafetyCount = 0;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			while(((*pCh) != '\0') && ((SafetyCount++) < MaxMsgChars))
			{
				switch(*pCh)
				{
				case '\n':
					{
						/* Wrap around to next line.,. */
						(*pPosY) += HUD_FONT_HEIGHT;
						(*pPosX) = LeftMarginX;
						NumLines++;
					}
					break;

				default:
					{
						/* It is a standard character or a space */
						DHM_AddToQueue(*pPosX, (*pPosY) + textprint_Y_offset, *pCh);
						(*pPosX) += AAFontWidths[*pCh];	/* CharWidthInPixels(*pCh);
													 * */

						if((*pPosX) > LastDisplayableXForChars())
						{
							/* Wrap around to next line.,. */
							(*pPosY) += HUD_FONT_HEIGHT;
							(*pPosX) = LeftMarginX;
							NumLines++;
						}
					}
				}

				/* ...and on to the next character */
				pCh++;
			}
		}

		/* Clear the string buffer */
		{
			TextprintBuffer[0] = '\0';
		}

		return NumLines;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int textprint(const char *t, ...)
{
	if(bEnableTextprint)
	{
		/* Get message string from arguments into buffer... */
		{
			/*~~~~~~~~~*/
			va_list	ap;
			/*~~~~~~~~~*/

			va_start(ap, t);
			vsprintf(&TextprintBuffer[0], t, ap);
			va_end(ap);
		}


		return DHM_MoveBufferToQueue(&textprintPosX, &textprintPosY, Yes);
	}
	else
	{
		/* Run-time disabling of textprint() */
		return 0;
	}


}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int PrintDebuggingText(const char *t, ...)
{
	/* Get message string from arguments into buffer... */
	{
		/*~~~~~~~~~*/
		va_list	ap;
		/*~~~~~~~~~*/

		va_start(ap, t);
		vsprintf(&TextprintBuffer[0], t, ap);
		va_end(ap);
	}


	return DHM_MoveBufferToQueue(&textprintPosX, &textprintPosY, Yes);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ReleasePrintDebuggingText(const char *t, ...)
{
	/* Get message string from arguments into buffer... */
	{
		/*~~~~~~~~~*/
		va_list	ap;
		/*~~~~~~~~~*/

		va_start(ap, t);
		vsprintf(&TextprintBuffer[0], t, ap);
		va_end(ap);
	}


	return DHM_MoveBufferToQueue(&textprintPosX, &textprintPosY, Yes);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int textprintXY(int x, int y, const char *t, ...)
{
	if(bEnableTextprintXY)
	{
		/* Get message string from arguments into buffer... */
		{
			/*~~~~~~~~~*/
			va_list	ap;
			/*~~~~~~~~~*/

			va_start(ap, t);
			vsprintf(&TextprintBuffer[0], t, ap);
			va_end(ap);
		}

		{
			/*~~~~~~~~~~~~*/
			int	localX = x;
			int	localY = y;
			/*~~~~~~~~~~~~*/

			return DHM_MoveBufferToQueue(&localX, &localY, No);
		}
	}
	else
	{
		/* Run-time disabling of textprint() */
		return 0;
	}


}

/*
 * Load main, 8 bit paletted, font (assumed to be on hard drive at present) and
 * create hi and true colour mode fonts from it. Note that for this system to work
 * properly all bits on must be white or similar in 8 bit mode 222 and Raw256
 * palettes as well as mode 8T. ;
 * MUST be called after GenerateDirectDrawSurface, i.e. AFTER SetVideoMode. AND
 * ONLY ONCE!!!!
 */

extern LPDIRECTDRAWSURFACE	lpDDDbgFont;


/*
 =======================================================================================================================
     This function is intended to allow YOU, the user, to obtain your heart's fondest desires by one simple call.
     Money? Love? A better job? It's all here, you have only to ask... No, I was lying actually. In fact, this should
     allow you to change display modes cleanly. Pass your request modes (as normally set up in system.c). For all
     entries which you do not want to change, simply pass the current global value (e.g. ZBufferRequestMode in the
     NewZBufferMode entry). Note that the function must always be passed the HINSTANCE and nCmdShow from winmain. ;
     Note that this function will NOT reinitialise the DirectDraw object or switch to or from a hardware DD device, but
     it will release and rebuild all the Direct3D objects. ;
     Note that you MUST be in the right directory for a texture reload before you call this, and normal operations CAN
     change the directory... ;
     NOTE!!! If you start in DirectDraw mode and go to Direct3D mode, this function CANNOT POSSIBLY WORK WITHOUT A FULL
     SHAPE RELOAD, since the shape data is overwritten during DirectDraw initialisation!!!! NOTE ALSO: TEXTURE RELOAD
     MAY BE DODGY WITHOUT A SHAPE RELOAD!!!
 =======================================================================================================================
 */
int ChangeDisplayModes
(
	HINSTANCE hInst,
	int		nCmd,
	int		NewVideoMode,
	int		NewWindowMode,
	int		NewZBufferMode,
	int		NewRasterisationMode,
	int		NewSoftwareScanDrawMode,
	int		NewDXMemoryMode
)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	BOOL rc;
	BOOL ChangeWindow = No;
	/*~~~~~~~~~~~~~~~~~~~*/

	/* Shut down DirectX objects and destroy the current window, if necessary. */
	if(NewWindowMode != WindowMode) ChangeWindow = Yes;

	DeallocateAllImages();

	ReleaseDirect3DNotDD();

	finiObjectsExceptDD();

	if(ChangeWindow) ExitWindowsSystem();

	/*
	 * Test!! ;
	 * Set the request modes and actual modes according to the passed values.
	 */
	VideoRequestMode = NewVideoMode;
	WindowRequestMode = NewWindowMode;
	ZBufferRequestMode = NewZBufferMode;
	RasterisationRequestMode = NewRasterisationMode;
	SoftwareScanDrawRequestMode = NewSoftwareScanDrawMode;
	DXMemoryRequestMode = NewDXMemoryMode;

	VideoMode = VideoRequestMode;
	WindowMode = WindowRequestMode;

	/*
	 * this may reconstruct the dd object depending on the rasterisation request mode
	 * and whether a hardware dd driver is selected or could be available
	 */
	ChangeDirectDrawObject();

	/*
	 * Check that our new video mode exists, and pick a valid option if it doesn't and
	 * we can find one.
	 */

	/* Recreate the window, allowing for possible change in WindowMode. */
	if(ChangeWindow)
	{
		rc = InitialiseWindowsSystem(hInst, nCmd, WinInitChange);

		if(rc == FALSE) return rc;
	}

	/*
	 * Set the video mode again. This will handle all changes to DirectDraw objects,
	 * all Direct3D initialisation, and other request modes such as zbuffering. ;
	 * Err... shutting down and restarting on a hardware driver appears to screw up
	 * file handling somehow... umm... but not for Microsoft demos, obviously...
	 * FIXME!!! ;
	 * test only!!!
	 */
	SetVideoMode[VideoMode]();

	/* Lose all the textures and reload the debugging font */
	InitialiseTextures();

	/* Well, we HOPE it's okay... */
	return TRUE;
}

/*
 =======================================================================================================================
     Reverse of ConvertToDDPalette, introduced to maintain internal interfaces only...
 =======================================================================================================================
 */
void ConvertDDToInternalPalette(unsigned char *src, unsigned char *dst, int length)
{
	/*~~~*/
	int	i;
	/*~~~*/

	/* Copy palette, shifting down to 5 bit triple */
	for(i = 0; i < length; i++)
	{
		*dst++ = (*src++) >> 2;
		*dst++ = (*src++) >> 2;
		*dst++ = (*src++) >> 2;
	}
}
