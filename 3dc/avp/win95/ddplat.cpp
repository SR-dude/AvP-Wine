/*KJL**************************************************************************************
* ddplat.cpp - this contains all the display code for the HUD, menu screens and so forth. *
*                                                                                         *
**************************************************************************************KJL*/
extern "C" {

#include "3dc.h"
#include "module.h"
#include "inline.h"
#include "dxlog.h"
#include "stratdef.h"
#include "gamedef.h"
#include "gameplat.h"
#include "equipmnt.h"
#include "huddefs.h"
#include "hudgfx.h"
#include "font.h"
#include "kshape.h"
#include "krender.h"
#include "chnktexi.h"
#include "awtexld.h"
#include "ffstdio.h"
#include "d3d_hud.h"
extern "C++" 
{
#include "r2base.h"
#include "indexfnt.hpp"
#include "projload.hpp" // c++ header which ignores class definitions/member functions if __cplusplus is not defined ?
#include "chnkload.hpp" // c++ header which ignores class definitions/member functions if __cplusplus is not defined ?
#include "pcmenus.h"
};

#include "alt_tab.h"

extern int ScanDrawMode;
extern int ZBufferMode;
extern int sine[],cosine[];
extern IMAGEHEADER ImageHeaderArray[];
int BackdropImage;


/* HUD globals */
extern SCREENDESCRIPTORBLOCK ScreenDescriptorBlock;
static int TrackerPolyBuffer[25];
static int ScanlinePolyBuffer[25];
static int MotionTrackerWidth;
static int MotionTrackerTextureSize;
static int MotionTrackerCentreY;
static int MotionTrackerCentreX;
static RECT MT_BarDestRect;    
static int MT_BlipHeight;
static int MT_BlipWidth;
struct LittleMDescTag *MTLittleMPtr;
enum HUD_RES_ID HUDResolution;

/* display co-ords, etc. */
#include "hud_data.h"



static struct DDGraphicTag PauseDDInfo;															
static struct DDGraphicTag E3FontDDInfo;

    
/*KJL****************************************************************************************
*                                    P R O T O T Y P E S	                                *
****************************************************************************************KJL*/
void PlatformSpecificInitMarineHUD(void);
void PlatformSpecificInitPredatorHUD(void);
void PlatformSpecificExitingHUD(void);
void PlatformSpecificEnteringHUD(void);
void BLTMotionTrackerToHUD(int scanLineSize);
void BLTMotionTrackerBlipToHUD(int x, int y, int brightness);
void BLTGunSightToScreen(int screenX, int screenY, enum GUNSIGHT_SHAPE gunsightShape);
void BLTWeaponToHUD(PLAYER_WEAPON_DATA* weaponPtr);
int CueWeaponFrameFromSequence(struct WeaponFrameTag *weaponFramePtr, int timeOutCounter, int weaponIDNumber);

void LoadDDGraphic(struct DDGraphicTag *DDGfxPtr, char *Filename);
static void SetupScanlinePoly(char const *filenamePtr, int width);
extern void D3D_InitialiseMarineHUD(void);
extern void D3D_BLTMotionTrackerToHUD(int scanLineSize);
extern void D3D_BLTMotionTrackerBlipToHUD(int x, int y, int brightness);
extern void D3D_BLTDigitToHUD(char digit, int x, int y, int font);
extern void D3D_BLTGunSightToHUD(int screenX, int screenY, enum GUNSIGHT_SHAPE gunsightShape);
extern void LoadCommonTextures(void);
/*KJL****************************************************************************************
*                                     F U N C T I O N S	                                    *
****************************************************************************************KJL*/


/****************************************
*          SETTING UP THE HUD           *
****************************************/
void PlatformSpecificInitMarineHUD(void)
{
	if ((ScanDrawMode != ScanDrawDirectDraw) && (ZBufferOn==ZBufferMode))
	{
		D3D_InitialiseMarineHUD();
		LoadCommonTextures();
		return;
	}
	

	/* set game mode: different, though for multiplayer game */
	if(AvP.Network==I_No_Network)
		cl_pszGameMode = "marine";
	else
		cl_pszGameMode = "multip";

	
	/* load HUD gfx */
	int gfxID = NO_OF_MARINE_HUD_GFX;
	if (ScreenDescriptorBlock.SDB_Width>=800)
	{
		HUDResolution = HUD_RES_HI;
		/* load Medres gfx */
		while(gfxID--)			      
	    {
			HUDDDInfo[gfxID].LPDDS = 0; // ensure 0 just in case one doesn't load and we try to delete it
			LoadDDGraphic
			(
				&HUDDDInfo[gfxID],
	    		HiresMarineHUDGfxFilenamePtr[gfxID]
			);	
		}
		
		TrackerPolyBuffer[3] = CL_LoadImageOnce("trakHiRz",(ScanDrawDirectDraw == ScanDrawMode ? LIO_CHIMAGE : LIO_D3DTEXTURE)|LIO_TRANSPARENT|LIO_RIFFPATH|LIO_RESTORABLE);
		MotionTrackerWidth = 244;
		MotionTrackerTextureSize = 243<<16;
		MTLittleMPtr = &HiresHUDLittleM;
		
		SetupScanlinePoly("scanhirz",MotionTrackerTextureSize);

		LoadDDGraphic(&E3FontDDInfo,"e3fontMR");	
	}
	else if (ScreenDescriptorBlock.SDB_Width>=640)
	{
		HUDResolution = HUD_RES_MED;
		/* load Medres gfx */
		while(gfxID--)			      
	    {
			HUDDDInfo[gfxID].LPDDS = 0; // ensure 0 just in case one doesn't load and we try to delete it
			LoadDDGraphic
			(
				&HUDDDInfo[gfxID],
	    		MedresMarineHUDGfxFilenamePtr[gfxID]
			);	
		}
		
		TrackerPolyBuffer[3] = CL_LoadImageOnce("trakMdRz",(ScanDrawDirectDraw == ScanDrawMode ? LIO_CHIMAGE : LIO_D3DTEXTURE)|LIO_TRANSPARENT|LIO_RIFFPATH|LIO_RESTORABLE);
		MotionTrackerWidth = 195;
		MotionTrackerTextureSize = 194<<16;
		MTLittleMPtr = &MedresHUDLittleM;
		
		SetupScanlinePoly("scanmdrz",MotionTrackerTextureSize);

		LoadDDGraphic(&E3FontDDInfo,"e3fontMR");	
	}
	else
	{
		HUDResolution = HUD_RES_LO;
	
		/* load lores gfx */
		while(gfxID--)			      
	    {
			HUDDDInfo[gfxID].LPDDS = 0; // ensure 0 just in case one doesn't load and we try to delete it
			LoadDDGraphic
			(
				&HUDDDInfo[gfxID],
	    		LoresMarineHUDGfxFilenamePtr[gfxID]
			);	
		}
		TrackerPolyBuffer[3] = CL_LoadImageOnce("tracker",(ScanDrawDirectDraw == ScanDrawMode ? LIO_CHIMAGE : LIO_D3DTEXTURE)|LIO_TRANSPARENT|LIO_RIFFPATH|LIO_RESTORABLE);
		MotionTrackerWidth = 97;
		MotionTrackerTextureSize = 96<<16;
		MTLittleMPtr = &LoresHUDLittleM;
		
		/* lores scanline slightly smaller than tracker... */
		SetupScanlinePoly("scan",MotionTrackerTextureSize-65536);

		LoadDDGraphic(&E3FontDDInfo,"e3font");	
	}

	TrackerPolyBuffer[0] = I_2dTexturedPolygon;
	TrackerPolyBuffer[2] = iflag_nolight|iflag_ignore0;
	
	ScanlinePolyBuffer[0] = I_2dTexturedPolygon;
	ScanlinePolyBuffer[2] = iflag_nolight|iflag_ignore0;
			
	/* screen dest of blue bar under motion tracker */
	MT_BarDestRect.bottom = ScreenDescriptorBlock.SDB_Height-1;
	MT_BarDestRect.top = MT_BarDestRect.bottom - HUDDDInfo[MARINE_HUD_GFX_BLUEBAR].SrcRect.bottom;
	MT_BarDestRect.left = 0;//MotionTrackerWidth/4;
	MT_BarDestRect.right = MT_BarDestRect.left + HUDDDInfo[MARINE_HUD_GFX_BLUEBAR].SrcRect.right;
		
	/* centre of motion tracker */
	MotionTrackerCentreY = MT_BarDestRect.top+1;
	MotionTrackerCentreX = (MT_BarDestRect.left+MT_BarDestRect.right)/2;
	
	/* motion tracker blips */
	MT_BlipHeight = HUDDDInfo[MARINE_HUD_GFX_MOTIONTRACKERBLIP].SrcRect.bottom/5;
	MT_BlipWidth = HUDDDInfo[MARINE_HUD_GFX_MOTIONTRACKERBLIP].SrcRect.right;


	LoadDDGraphic(&PauseDDInfo,"paused");	
}

void PlatformSpecificInitPredatorHUD(void)
{
	//SelectGenTexDirectory(ITI_TEXTURE);
	/* set game mode: different, though for multiplayer game */
	if(AvP.Network==I_No_Network)
	{
		cl_pszGameMode = "predator";
		/* load in sfx */
		LoadCommonTextures();
	}
	else
	{
		cl_pszGameMode = "multip";
		/* load in sfx */
		LoadCommonTextures();
		//load marine stuff as well
		D3D_InitialiseMarineHUD();
	}
	return;

	int gfxID = NO_OF_PREDATOR_HUD_GFX;
	
	if (ScreenDescriptorBlock.SDB_Width>=640)
	{
		HUDResolution = HUD_RES_MED;
		/* load Medres gfx */
		while(gfxID--)
	    {
			HUDDDInfo[gfxID].LPDDS = 0; // ensure 0 just in case one doesn't load and we try to delete it
			LoadDDGraphic
			(
				&HUDDDInfo[gfxID],
	    		MedresPredatorHUDGfxFilenamePtr[gfxID]
			);
		}
		LoadDDGraphic(&E3FontDDInfo,"e3fontmr");	
	}
	else
	{
		/* load Lores gfx */
		int gfxID = NO_OF_PREDATOR_HUD_GFX;
		while(gfxID--)
	    {
			HUDDDInfo[gfxID].LPDDS = 0; // ensure 0 just in case one doesn't load and we try to delete it
			LoadDDGraphic
			(
				&HUDDDInfo[gfxID],
	    		LoresPredatorHUDGfxFilenamePtr[gfxID]
			);
		}
		LoadDDGraphic(&E3FontDDInfo,"e3font");	
	}
  	LoadDDGraphic(&PauseDDInfo,"paused");	
}


void PlatformSpecificInitAlienHUD(void)
{
	/* set game mode: different, though for multiplayer game */
	if(AvP.Network==I_No_Network)
	{
		cl_pszGameMode = "alien";
		LoadCommonTextures();
	}
	else
	{
		cl_pszGameMode = "multip";
		/* load in sfx */
		LoadCommonTextures();
		//load marine stuff as well
		D3D_InitialiseMarineHUD();
	}

	return;
	
	int gfxID = NO_OF_ALIEN_HUD_GFX;

	if (ScreenDescriptorBlock.SDB_Width==640)
	{
		HUDResolution = HUD_RES_MED;
		/* load Medres gfx */
		while(gfxID--)			      
	    {
			HUDDDInfo[gfxID].LPDDS = 0; // ensure 0 just in case one doesn't load and we try to delete it
			LoadDDGraphic
			(
				&HUDDDInfo[gfxID],
	    		MedresAlienHUDGfxFilenamePtr[gfxID]
			);	
		}
	   	LoadDDGraphic(&E3FontDDInfo,"e3fontmr");	
	}
	else
	{
		HUDResolution = HUD_RES_LO;

		/* load lores gfx */
		while(gfxID--)			      
	    {
			HUDDDInfo[gfxID].LPDDS = 0; // ensure 0 just in case one doesn't load and we try to delete it
			LoadDDGraphic
			(
				&HUDDDInfo[gfxID],
	    		LoresAlienHUDGfxFilenamePtr[gfxID]
			);	
		}
		LoadDDGraphic(&E3FontDDInfo,"e3font");	
	}
	LoadDDGraphic(&PauseDDInfo,"paused");	
}


/*JH 14/5/97*****************************
*            KILLING THE HUD            *
************************************JH**/


void PlatformSpecificKillMarineHUD(void)
{
	int gfxID = NO_OF_MARINE_HUD_GFX;
	
	while(gfxID--)			      
    {
		if (HUDDDInfo[gfxID].hBackup)
		{
			ATRemoveSurface(HUDDDInfo[gfxID].LPDDS);
			AwDestroyBackupTexture( HUDDDInfo[gfxID].hBackup );
		}
		if (HUDDDInfo[gfxID].LPDDS)
			HUDDDInfo[gfxID].LPDDS->Release();
		HUDDDInfo[gfxID].LPDDS = 0;
		HUDDDInfo[gfxID].hBackup = 0;
	}
	
	if (PauseDDInfo.hBackup)
	{
		ATRemoveSurface(PauseDDInfo.LPDDS);
		AwDestroyBackupTexture( PauseDDInfo.hBackup );
	}
	if (PauseDDInfo.LPDDS)
		PauseDDInfo.LPDDS->Release();	
	PauseDDInfo.LPDDS = 0;
	PauseDDInfo.hBackup = 0;
	
	if (E3FontDDInfo.hBackup)
	{
		ATRemoveSurface(E3FontDDInfo.LPDDS);
		AwDestroyBackupTexture( E3FontDDInfo.hBackup );
	}
	if (E3FontDDInfo.LPDDS)
		E3FontDDInfo.LPDDS->Release();	
	E3FontDDInfo.LPDDS = 0;
	E3FontDDInfo.hBackup = 0;
}

void PlatformSpecificKillPredatorHUD(void)
{
	/* load HUD gfx */
	int gfxID = NO_OF_PREDATOR_HUD_GFX;

	while(gfxID--)			      
    {
		if (HUDDDInfo[gfxID].hBackup)
		{
			ATRemoveSurface(HUDDDInfo[gfxID].LPDDS);
			AwDestroyBackupTexture( HUDDDInfo[gfxID].hBackup );
		}
		if (HUDDDInfo[gfxID].LPDDS)
			HUDDDInfo[gfxID].LPDDS->Release();
		HUDDDInfo[gfxID].LPDDS = 0;
		HUDDDInfo[gfxID].hBackup = 0;
	}
	
	if (PauseDDInfo.hBackup)
	{
		ATRemoveSurface(PauseDDInfo.LPDDS);
		AwDestroyBackupTexture( PauseDDInfo.hBackup );
	}
	if (PauseDDInfo.LPDDS)
		PauseDDInfo.LPDDS->Release();	
	PauseDDInfo.LPDDS = 0;
	PauseDDInfo.hBackup = 0;
	
	if (E3FontDDInfo.hBackup)
	{
		ATRemoveSurface(E3FontDDInfo.LPDDS);
		AwDestroyBackupTexture( E3FontDDInfo.hBackup );
	}
	if (E3FontDDInfo.LPDDS)
		E3FontDDInfo.LPDDS->Release();	
	E3FontDDInfo.LPDDS = 0;
	E3FontDDInfo.hBackup = 0;
}


void PlatformSpecificKillAlienHUD(void)
{
	int gfxID = NO_OF_ALIEN_HUD_GFX;
	while(gfxID--)			      
    {
		if (HUDDDInfo[gfxID].hBackup)
		{
			ATRemoveSurface(HUDDDInfo[gfxID].LPDDS);
			AwDestroyBackupTexture( HUDDDInfo[gfxID].hBackup );
		}
		if (HUDDDInfo[gfxID].LPDDS)
			HUDDDInfo[gfxID].LPDDS->Release();
		HUDDDInfo[gfxID].LPDDS = 0;
		HUDDDInfo[gfxID].hBackup = 0;
	}
	
	if (PauseDDInfo.hBackup)
	{
		ATRemoveSurface(PauseDDInfo.LPDDS);
		AwDestroyBackupTexture( PauseDDInfo.hBackup );
	}
	if (PauseDDInfo.LPDDS)
		PauseDDInfo.LPDDS->Release();	
	PauseDDInfo.LPDDS = 0;
	PauseDDInfo.hBackup = 0;
	
	if (E3FontDDInfo.hBackup)
	{
		ATRemoveSurface(E3FontDDInfo.LPDDS);
		AwDestroyBackupTexture( E3FontDDInfo.hBackup );
	}
	if (E3FontDDInfo.LPDDS)
		E3FontDDInfo.LPDDS->Release();	
	E3FontDDInfo.LPDDS = 0;
	E3FontDDInfo.hBackup = 0;
}


/*********************/
/* RUNTIME HUD STUFF */
/*********************/

void PlatformSpecificExitingHUD(void)
{ // adj stub

}

void PlatformSpecificEnteringHUD(void)
{
	/* Flush the ZBuffer so the weapons don't sink into the wall! */
	if ((ScanDrawMode != ScanDrawDirectDraw) && (ZBufferMode != ZBufferOff))
	{
	}

// adj stub

}

/*KJL**********************
* MARINE DRAWING ROUTINES *
**********************KJL*/
void BLTMotionTrackerToHUD(int scanLineSize)
{
	if ((ScanDrawMode != ScanDrawDirectDraw) && (ZBufferOn==ZBufferMode))
	{
		D3D_BLTMotionTrackerToHUD(scanLineSize);
	}
	return;
	
}

void BLTMotionTrackerBlipToHUD(int x, int y, int brightness)
{
	if ((ScanDrawMode != ScanDrawDirectDraw) && (ZBufferOn==ZBufferMode))
	{
		D3D_BLTMotionTrackerBlipToHUD(x,y,brightness);
	}
	return;

}




void BLTGunSightToScreen(int screenX, int screenY, enum GUNSIGHT_SHAPE gunsightShape)
{
	if ((ScanDrawMode != ScanDrawDirectDraw) && (ZBufferOn==ZBufferMode))
	{
		D3D_BLTGunSightToHUD(screenX,screenY,gunsightShape);
		return;
	}
}


  


void LoadDDGraphic(struct DDGraphicTag *DDGfxPtr, char *Filename)
{
	/*
		set up the direct draw surface. we can take the width and height
		from the imageheader image
	*/

	GLOBALASSERT(DDGfxPtr);
    GLOBALASSERT(Filename);
    
	// get the filename that we need
	char szAbsFileName[MAX_PATH];
	char * pszRet = CL_GetImageFileName(szAbsFileName,sizeof szAbsFileName / sizeof szAbsFileName[0], Filename, LIO_DDSURFACE|LIO_SYSMEM|LIO_TRANSPARENT|LIO_CHROMAKEY|LIO_RIFFPATH|LIO_RESTORABLE);
	GLOBALASSERT(pszRet);
	
	// we'll put the width and height in here
	unsigned nWidth, nHeight;
	
	// is it in a fast file?
	unsigned nFastFileLen;
	void const * pFastFileData = ffreadbuf(szAbsFileName,&nFastFileLen);
	
	if (pFastFileData)
	{
		DDGfxPtr->LPDDS =
			AwCreateSurface
			(
				"pxfXYB",
				pFastFileData,
				nFastFileLen,
				AW_TLF_TRANSP|AW_TLF_CHROMAKEY,
				&nWidth,
				&nHeight,
				&DDGfxPtr->hBackup
			);
	}
	else
	{
		DDGfxPtr->LPDDS =
			AwCreateSurface
			(
				"sfXYB",
				&szAbsFileName[0],
				AW_TLF_TRANSP|AW_TLF_CHROMAKEY,
				&nWidth,
				&nHeight,
				&DDGfxPtr->hBackup
			);
	}
	
	GLOBALASSERT(DDGfxPtr->LPDDS);
	GLOBALASSERT(DDGfxPtr->hBackup);
	ATIncludeSurface(DDGfxPtr->LPDDS,DDGfxPtr->hBackup);

 	// set the rectangle size for blitting before padding to 4x4 has been done
 	DDGfxPtr->SrcRect.left = 0;
	DDGfxPtr->SrcRect.right = nWidth;
	DDGfxPtr->SrcRect.top = 0;
	DDGfxPtr->SrcRect.bottom = nHeight;

 	/*move the width and height to four byte bounadries*/

	GLOBALASSERT((DDGfxPtr->SrcRect.right > 0));
	GLOBALASSERT((DDGfxPtr->SrcRect.bottom > 0));
}

	
/* JH 3/6/97 functions to remove dd surfaces from hud graphics
   so that the video mode can be completely changed,
   but then everything can still be restored */
/* perhaps not a final solution since it will be occupying memory */

void MinimizeAllDDGraphics(void)
{
	/* do all in array - don't care how many actually are used
	   because the array is static (hence initially filled with zeros)
	   The release functions should replace with NULL a pointer
	   that is no longer valid */

	int gfxID = sizeof HUDDDInfo / sizeof (DDGraphicTag); // number of DDGraphicTags in array
	
	while(gfxID--)			      
    {
		if (HUDDDInfo[gfxID].LPDDS)
		{
			ATRemoveSurface(HUDDDInfo[gfxID].LPDDS);
			HUDDDInfo[gfxID].LPDDS->Release();
			HUDDDInfo[gfxID].LPDDS = 0;
		}
	}
	
	if (PauseDDInfo.LPDDS)
	{
		ATRemoveSurface(PauseDDInfo.LPDDS);
		PauseDDInfo.LPDDS->Release();	
		PauseDDInfo.LPDDS = 0;
	}
	
	if (E3FontDDInfo.LPDDS)
	{
		ATRemoveSurface(E3FontDDInfo.LPDDS);
		E3FontDDInfo.LPDDS->Release();	
		E3FontDDInfo.LPDDS = 0;
	}
}



/************************** FONTS *************************/
/**********************************************************/
/**********************************************************/
LPDIRECTDRAWSURFACE FontLPDDS[NUM_FONTS];

PFFONT AvpFonts[] =
{
	{
 		FontLPDDS[0],
 	 	"menufont.bmp",
	 	18,			// font height
	 	118,		// num chars
		I_FONT_UCLC_NUMERIC,
		{0}			//flags
		
 	},
	{
 		FontLPDDS[1],
 	 	"Common\\fontdark.RIM",
	 	14,			// font height
	 	59,		// num chars
		I_FONT_UC_NUMERIC
 	},
	{
 		FontLPDDS[2],
 	 	"Common\\fontlite.RIM",
	 	14,			// font height
	 	59,		// num chars
		I_FONT_UC_NUMERIC
 	},
	{
 		FontLPDDS[3],
 	 	"Common\\dbfont.RIM",
	 	11,	 // font height
	 	59,	 // num chars
		I_FONT_UC_NUMERIC
 	},

};

extern int VideoModeColourDepth;

void LoadFont(PFFONT *pffont)
{
	GLOBALASSERT(pffont);
	GLOBALASSERT(pffont->filename);
	
	// get the filename that we need
	char szAbsFileName[MAX_PATH];
	char * pszRet = CL_GetImageFileName(szAbsFileName,sizeof szAbsFileName / sizeof szAbsFileName[0], pffont->filename, LIO_DDSURFACE|LIO_SYSMEM|LIO_CHROMAKEY|LIO_TRANSPARENT
		// hack for the moment so that the menu font is correctly loaded into an 8-bit vid mode
		|(strchr(pffont->filename,'\\') ?  LIO_RELATIVEPATH : LIO_RIFFPATH));
	GLOBALASSERT(pszRet);
	
	
	// we'll put the width and height in here
	unsigned nWidth, nHeight;
	
	// is it in a fast file?
	unsigned nFastFileLen;
	void const * pFastFileData = ffreadbuf(szAbsFileName,&nFastFileLen);
	
	if (pFastFileData)
	{
		pffont->data =
			AwCreateSurface
			(
				"pxfXYB",
				pFastFileData,
				nFastFileLen,
				AW_TLF_TRANSP|AW_TLF_CHROMAKEY,
				&nWidth,
				&nHeight,
				&pffont->hBackup
			);
	}
	else
	{
		pffont->data =
			AwCreateSurface
			(
				"sfXYB",
				&szAbsFileName[0],
				AW_TLF_TRANSP|AW_TLF_CHROMAKEY,
				&nWidth,
				&nHeight,
				&pffont->hBackup
			);
	}
	
	GLOBALASSERT(pffont->data);
	GLOBALASSERT(pffont->hBackup);
	
	ATIncludeSurface(pffont->data,pffont->hBackup);
	
	pffont->fttexBitDepth = VideoModeColourDepth;
	
	pffont->fttexWidth = nWidth;
	pffont->fttexHeight = nHeight;
	
	GLOBALASSERT((nHeight > 0));
	GLOBALASSERT((nWidth > 0));

	pffont->flags.loaded = 1;
}


void * FontLock(PFFONT const * pFont, unsigned * pPitch)
{
	GLOBALASSERT(pFont);
	GLOBALASSERT(pFont->data);
	
	DDSURFACEDESC ddsd;
	memset(&ddsd,0,sizeof ddsd);
	ddsd.dwSize = sizeof ddsd;
	HRESULT hResult = pFont->data->Lock(NULL,&ddsd,DDLOCK_NOSYSLOCK,NULL);
	GLOBALASSERT(DD_OK == hResult);
	
	*pPitch = ddsd.lPitch;
	return ddsd.lpSurface;
}

void FontUnlock(PFFONT const * pFont)
{
	GLOBALASSERT(pFont);
	GLOBALASSERT(pFont->data);
	
	HRESULT hResult = pFont->data->Unlock(NULL);
	GLOBALASSERT(DD_OK == hResult);
}





void FillCharacterSlot(int u, int v,
											int width, int height,
											int charnum, PFFONT* font)

{

	/*
		 simply set the srcRect.top to null in order to tell the drawing easily that
		 this char dosn't exist
	*/

	GLOBALASSERT(width > -1);
	GLOBALASSERT(height > -1);
	GLOBALASSERT(u > -1);
	GLOBALASSERT(v > -1);

	GLOBALASSERT(font);
	GLOBALASSERT(charnum < font->num_chars_in_font);
	font->srcRect[charnum].left = u;
	font->srcRect[charnum].right = u + width;
	font->srcRect[charnum].top = v;
	font->srcRect[charnum].bottom = v + height;
}


void YClipMotionTrackerVertices(struct VertexTag *v1, struct VertexTag *v2)
{
	char vertex1Inside=0,vertex2Inside=0;

	if (v1->Y<0) vertex1Inside = 1;
	if (v2->Y<0) vertex2Inside = 1;

	/* if both vertices inside clip region no clipping required */
	if (vertex1Inside && vertex2Inside) return;

	/* if both vertices outside clip region no action required 
	(the other lines will be clipped) */
	if (!vertex1Inside && !vertex2Inside) return;

	/* okay, let's clip */
	if (vertex1Inside)
	{
		int lambda = DIV_FIXED(v1->Y,v2->Y - v1->Y);

		v2->X = v1->X - MUL_FIXED(v2->X - v1->X,lambda);
		v2->Y=0;

		v2->U = v1->U - MUL_FIXED(v2->U - v1->U,lambda);
		v2->V = v1->V - MUL_FIXED(v2->V - v1->V,lambda);
	}
	else
	{
		int lambda = DIV_FIXED(v2->Y,v1->Y - v2->Y);

		v1->X = v2->X - MUL_FIXED(v1->X - v2->X,lambda);
		v1->Y=0;

		v1->U = v2->U - MUL_FIXED(v1->U - v2->U,lambda);
		v1->V = v2->V - MUL_FIXED(v1->V - v2->V,lambda);
	}
}
				    

static void SetupScanlinePoly(char const *filenamePtr, int width)
{
	int imageNumber;
	int height;

	imageNumber = CL_LoadImageOnce(filenamePtr, (ScanDrawDirectDraw == ScanDrawMode ? LIO_CHIMAGE : LIO_D3DTEXTURE)|LIO_TRANSPARENT|LIO_RIFFPATH|LIO_RESTORABLE);
	height = width/2;
								
	ScanlinePolyBuffer[3] = imageNumber;

	ScanlinePolyBuffer[6] = 0;
	ScanlinePolyBuffer[7] = height;

	ScanlinePolyBuffer[10] = width;
	ScanlinePolyBuffer[11] = height;

	ScanlinePolyBuffer[14] = width;
	ScanlinePolyBuffer[15] = 0;

	ScanlinePolyBuffer[18] = 0;
	ScanlinePolyBuffer[19] = 0;

	ScanlinePolyBuffer[20] = Term;
}


}; // extern 
