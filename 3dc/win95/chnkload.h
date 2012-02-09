#ifndef _chnkload_h_
#define _chnkload_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"
#include "equates.h"
#include "platform.h"
#include "shape.h"
#include "prototyp.h"
#include "module.h"

extern SCREENDESCRIPTORBLOCK ScreenDescriptorBlock;

#define GLS_NOTINLIST (-1)

///////////////////
// RIF Loading, etc
///////////////////

typedef struct _RifHandle * RIFFHANDLE;
#define INVALID_RIFFHANDLE 0

// flags - project specific ones start at lsb
// - generic ones to start from msb

#define CCF_NOMORPH 0x80000000

typedef enum UVCoordType
{
	UVC_SPRITE_U,
	UVC_SPRITE_V,
	UVC_POLY_U,
	UVC_POLY_V,
	
} UVCOORDTYPE;

// Note: for aesthetic reasons, macros enable one to have all one's fuctions in lower case or captialized, to suit one's style!

// For clarity, functions which are to be defined in project specific files
// are here declared with extern

/////////////////////////////////////////
// Functions which operate on RIFFHANDLEs
/////////////////////////////////////////

// load a rif file into memory
RIFFHANDLE load_rif (const char * fname);
RIFFHANDLE load_rif_non_env (const char * fname);

// deallocate the shapes, unload the rif, close the handle
void undo_rif_load (RIFFHANDLE);

// deallocate the shapes copied from the rif
void deallocate_loaded_shapes (RIFFHANDLE);

// unloads the rif but keeps the handle and associated copied shapes
void unload_rif (RIFFHANDLE);

// close the handle - performs tidying up and memory deallocation
void close_rif_handle (RIFFHANDLE);

// load textures for environment
BOOL load_rif_bitmaps (RIFFHANDLE, int flags);

// set the quantization event depending on CL_RIFFImage::game_mode
BOOL set_quantization_event(RIFFHANDLE, int flags);

// copy palette
BOOL copy_rif_palette (RIFFHANDLE, int flags);

// copy texture lighting table
BOOL copy_rif_tlt (RIFFHANDLE, int flags);

// copy palette remap table (15-bit) - post_process_shape may use it
BOOL get_rif_palette_remap_table (RIFFHANDLE, int flags);

// copy one named shape or sprite; does not put in main shape list, needs deallocating
SHAPEHEADER * CopyNamedShapePtr (RIFFHANDLE, char const * shapename);


////////////////////////////////////////////////////////////////////////
// Functions which do not operate on RIFFHANDLEs and may become obsolete
////////////////////////////////////////////////////////////////////////

// these functions work on the current rif; they only remain for historical reasons
extern RIFFHANDLE current_rif_handle;
// returns NULL on fail; does not put it in the mainshapelist
SHAPEHEADER * CopyNamedShape (char const * shapename);

/////////////////////////////////////////////
// Functions for handling the main shape list
/////////////////////////////////////////////

// reserves the next avaialbe position in the main shape list and returns it
extern int GetMSLPos(void);

// frees a position in the main shape list
extern void FreeMSLPos(int);

////////////////////////////////////////////////
// Functions retrieving data about loaded shapes
////////////////////////////////////////////////

// gets the main shape list position of a shape loaded into the msl
int GetLoadedShapeMSL(char const * shapename);


//////////////////////////////////////////////////////////////////////////////
// Initializing, deallocating of shapes, mainly hooks for project specific fns
//////////////////////////////////////////////////////////////////////////////

// perform initial post processing on shape just after loading
// note that the copy named shape functions will not call this
extern void post_process_shape (SHAPEHEADER *);


// hook to perhaps scale the uv coordinates - should return new value
extern int ProcessUVCoord(RIFFHANDLE,UVCOORDTYPE,int uv_value,int image_num);

// your function could perform any extra tidying up you need
extern void DeallocateLoadedShapeheader(SHAPEHEADER *);

///////
// Misc
///////

// return TRUE if the poly item type corresponds to a textured polygon
BOOL is_textured(int);

/////////////////////
// Rif loader globals
/////////////////////

extern unsigned char const * PaletteMapTable;

/////////////////
// Engine globals
/////////////////

extern int start_of_loaded_shapes;

extern unsigned char *TextureLightingTable;


extern SHAPEHEADER ** mainshapelist;


extern MAPHEADER Map[];

extern MAPBLOCK8 Player_and_Camera_Type8[];
extern MAPBLOCK6 Empty_Landscape_Type6;
extern MAPBLOCK6 Empty_Object_Type6;


extern unsigned char TestPalette[];

extern unsigned char LPTestPalette[]; /* to cast to lp*/



extern SCENEMODULE MainScene;
extern MODULE Empty_Module;
extern MODULE Term_Module;

extern MODULEMAPBLOCK Empty_Module_Map;


#ifdef __cplusplus

}

#endif

#endif
