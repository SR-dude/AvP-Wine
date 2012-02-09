/*KJL****************************************************************************************
*  							       D I S P L A Y   D A T A 	            				    *
****************************************************************************************KJL*/

/* KJL 16:58:11 05/08/97 - the display data was getting quite long, so I moved it to 
a header file to make things clearer. */
static struct DDGraphicTag HUDDDInfo[NO_OF_MARINE_HUD_GFX];


static char *LoresMarineHUDGfxFilenamePtr[]=
{
    "blips.pg0", 	//MARINE_HUD_GFX_MOTIONTRACKERBLIP,
    "num.pg0",	//MARINE_HUD_GFX_NUMERALS,
	"gunsight.pg0",	//MARINE_HUD_GFX_GUNSIGHTS,
	"trakfont.pg0",
	"bluebar.pg0"
};
static struct HUDFontDescTag LoresHUDFontDesc[] =
{
	//MARINE_HUD_FONT_BLUE,
	{
		0,//XOffset
		12,//Height
		8,//Width
	},
	//MARINE_HUD_FONT_RED,
	{
		8,//XOffset
		12,//Height
		8,//Width
	},
	//MARINE_HUD_FONT_MT_SMALL,
	{
		8,//XOffset
		8,//Height
		5,//Width
	},
	//MARINE_HUD_FONT_MT_BIG,
	{
		0,//XOffset
		12,//Height
		8,//Width
	},						 
	//ALIEN_HUD_FONT,
	{
		0,
		8,
		6,
	},

};
static struct LittleMDescTag LoresHUDLittleM =
{
	80,8,  // source top,left

	5,5,   // width, height

	7,7,   // screen x,y
};


static char *MedresMarineHUDGfxFilenamePtr[]=
{
    "blipsHRz.pg0", 	//MARINE_HUD_GFX_MOTIONTRACKERBLIP,
    "numMR.pg0",	//MARINE_HUD_GFX_NUMERALS,
	"sightsmr.pg0",	//MARINE_HUD_GFX_GUNSIGHTS,
	"trkfntmr.pg0",
	"blubarmr.pg0"
};
static struct HUDFontDescTag MedresHUDFontDesc[] =
{
	//MARINE_HUD_FONT_BLUE,
	{
		0,//XOffset
		24,//Height
		16,//Width
	},
	//MARINE_HUD_FONT_RED,
	{
		16,//XOffset
		24,//Height
		16,//Width
	},
	//MARINE_HUD_FONT_MT_SMALL,
	{
		14,//XOffset
		12,//Height
		8,//Width
	},
	//MARINE_HUD_FONT_MT_BIG,
	{
		0,//XOffset
		24,//Height
		14,//Width
	},
	//ALIEN_HUD_FONT,
	{
		0,
		16,
		12,
	},

};
static struct LittleMDescTag MedresHUDLittleM =
{
	120,14,  // source top,left

	8,11,   // width, height

	10,10,   // screen x,y
};


static char *HiresMarineHUDGfxFilenamePtr[]=
{
    "blipsHRz.pg0", 	//MARINE_HUD_GFX_MOTIONTRACKERBLIP,
    "numhR.pg0",	//MARINE_HUD_GFX_NUMERALS,
	"sightsmr.pg0",	//MARINE_HUD_GFX_GUNSIGHTS,
	"trkfnthr.pg0",
	"blubarhr.pg0"
};
static struct HUDFontDescTag HiresHUDFontDesc[] =
{
	//MARINE_HUD_FONT_BLUE,
	{
		0,//XOffset
		27,//Height
		19,//Width
	},
	//MARINE_HUD_FONT_RED,
	{
		20,//XOffset
		27,//Height
		19,//Width
	},
	//MARINE_HUD_FONT_MT_SMALL,
	{
		18,//XOffset
		15,//Height
		8,//Width
	},
	//MARINE_HUD_FONT_MT_BIG,
	{
		0,//XOffset
		29,//Height
		17,//Width
	},
};
static struct LittleMDescTag HiresHUDLittleM =
{
	150,17,  // source top,left

	9,13,   // width, height

	14,14,   // screen x,y
};


/* PREDATOR */


static char *LoresPredatorHUDGfxFilenamePtr[]=
{
	"topmask.pg0",	//PREDATOR_HUD_GFX_TOP,
	"botmmask.pg0",	//PREDATOR_HUD_GFX_BOTTOM,
   	"prednum.pg0",	//PREDATOR_HUD_GFX_NUMBERS,
    "predsymb.pg0"   //PREDATOR_HUD_GFX_SYMBOLS,
};
static char *MedresPredatorHUDGfxFilenamePtr[]=
{
	"prhdtpMR.pg0",	//PREDATOR_HUD_GFX_TOP,
	"prhdbmMR.pg0",	//PREDATOR_HUD_GFX_BOTTOM,
   	"prednum.pg0",	//PREDATOR_HUD_GFX_NUMBERS,
    "predsymb.pg0"   //PREDATOR_HUD_GFX_SYMBOLS,
};



/* ALIEN */
static char *LoresAlienHUDGfxFilenamePtr[]=
{
	"AlHudBot.pg0", // ALIEN_HUD_GFX_BOTTOM
	"AlHudLft.pg0", // ALIEN_HUD_GFX_LEFT
	"AlHudRgt.pg0", // ALIEN_HUD_GFX_RIGHT
   	"AlHudTop.pg0", // ALIEN_HUD_GFX_TOP
    "AlienNum.pg0" // ALIEN_HUD_GFX_NUMBERS
};
static char *MedresAlienHUDGfxFilenamePtr[]=
{
	"ahMRBtm.pg0", // ALIEN_HUD_GFX_BOTTOM
	"ahMRLft.pg0", // ALIEN_HUD_GFX_LEFT
	"ahMRRgt.pg0", // ALIEN_HUD_GFX_RIGHT
   	"ahMRTop.pg0", // ALIEN_HUD_GFX_TOP
    "ahMRNum.pg0" // ALIEN_HUD_GFX_NUMBERS
};


