#include "3dc.h"
#include <conio.h>
#include <sys/stat.h>
#include "inline.h"
#include "chnktexi.h"

#define UseLocalAssert 0
#include "ourasert.h"

#include "awtexld.h"
#include "alt_tab.h"



/*

 externs for commonly used global variables and arrays

*/

	extern SHAPEHEADER **mainshapelist;
	extern (*ShapeLanguageFunctions[])(SHAPEINSTR *shapeinstrptr);
	extern SCREENDESCRIPTORBLOCK ScreenDescriptorBlock;
	extern unsigned char *ScreenBuffer;
	extern char projectsubdirectory[];
	extern int ScanDrawMode;
	extern int VideoModeType;

/*

 Global Variables for PC Functions

*/

	
	int NumImages = 0;								/* # current images */
	IMAGEHEADER *ImageHeaderPtrs[MaxImages];	/* Ptrs to Image Header Blocks */
	IMAGEHEADER ImageHeaderArray[MaxImages];	/* Array of Image Headers */
	
	static IMAGEHEADER *NextFreeImageHeaderPtr;



/*

 Initialise General Texture Data Structures and Variables

*/


int InitialiseTextures(void)

{

	SHAPEHEADER **shlistptr;
	SHAPEHEADER *shptr;
	char **txfiles;
	int TxIndex;
	int LTxIndex;

	/*

	Free up any currently loaded images

	The caller is responsible for any other structures which might refer
	to the currently loaded images

	*/


    DeallocateAllImages();

	/* Initialise Image Header Variables */

	NumImages = 0;
	NextFreeImageHeaderPtr = ImageHeaderArray;



	/* Added 23/3/98 by DHM so that this can be called without loading any
	shapes (to get textprint working in the menus):
	*/
	if ( NULL == mainshapelist )
	{
		return Yes;
			// early exit
	}

	/* Build the Texture List */

	shlistptr = &mainshapelist[0];

	while(*shlistptr) {

		shptr = *shlistptr++;

		/* If the shape has textures */

		if(shptr->sh_localtextures) {


			txfiles = shptr->sh_localtextures;

			LTxIndex = 0;

			while(*txfiles) {

				/* The RIFF Image loaders have changed to support not loading the same image twice - JH 17-2-96 */

				char *src;
				char *dst;
				char fname[ImageNameSize];
				char *txfilesptr;
				
				txfilesptr = *txfiles++;
				
				/*

				"txfilesptr" is in the form "textures\<fname>". We need to
				prefix that text with the name of the current textures path.

				Soon this path may be varied but for now it is just the name of
				the current project subdirectory.

				*/

				src = projectsubdirectory;
				dst = fname;

				while(*src)
					*dst++ = *src++;

				src = txfilesptr;

				while(*src)
					*dst++ = *src++;

				*dst = 0;



				

				/* This function calls GetExistingImageHeader to figure out if the image is already loaded */
				TxIndex = CL_LoadImageOnce(fname,(ScanDrawDirectDraw == ScanDrawMode ? LIO_CHIMAGE : LIO_D3DTEXTURE)|LIO_TRANSPARENT|LIO_RELATIVEPATH|LIO_RESTORABLE);
				GLOBALASSERT(GEI_NOTLOADED != TxIndex);
				


				/*

					The local index for this image in this shape is
					"LTxIndex".

					The global index for the image is "TxIndex".

					We must go through the shape's items and change all the
					local references to global.

				*/


				MakeShapeTexturesGlobal(shptr, TxIndex, LTxIndex);

				LTxIndex++;			/* Next Local Texture */

			}

			/* Is this shape a sprite that requires resizing? */

			if((shptr->shapeflags & ShapeFlag_Sprite) &&
				(shptr->shapeflags & ShapeFlag_SpriteResizing)) {

				SpriteResizing(shptr);

			}

		}

	}



	return Yes;


}





/*

 This function accepts a shape header, a global texture index and a local
 index. It searches through all the items that use textures and converts all
 local references to a texture to global references.

 The index to a texture that refers to the IMAGEHEADER pointer array is in
 the low word of the colour int.

 Bit #15 is set if this index refers to a local texture.

 Here is an example of a textured item:


int CUBE_item3[]={

	I_2dTexturedPolygon,3*vsize,0,

	(3<<TxDefn) + TxLocal + 0,

	7*vsize,5*vsize,1*vsize,3*vsize,
	Term

};


 To test for a local texture use:

	if(ColourInt & TxLocal)



 NOTE

 The procedure is NOT reversible!

 If one wishes to reconstruct the table, all the shapes and textures must be
 reloaded and the initialisation function recalled.

 Actually a function could be written that relates global filenames back to
 local filenames, so in a sense that is not true. This function will only be
 written if it is needed.

*/

void MakeShapeTexturesGlobal(SHAPEHEADER *shptr, int TxIndex, int LTxIndex)

{

	int **ShapeItemArrayPtr;
	POLYHEADER *ShapeItemPtr;


	int i, txi;


	/* Are the items in a pointer array? */

	if(shptr->items) {


		ShapeItemArrayPtr = shptr->items;

		for(i = shptr->numitems; i!=0; i--) {

			ShapeItemPtr = (POLYHEADER *) *ShapeItemArrayPtr++;
			
			if(ShapeItemPtr->PolyItemType == I_2dTexturedPolygon
				|| ShapeItemPtr->PolyItemType == I_ZB_2dTexturedPolygon
				|| ShapeItemPtr->PolyItemType == I_Gouraud2dTexturedPolygon
				|| ShapeItemPtr->PolyItemType == I_ZB_Gouraud2dTexturedPolygon
				|| ShapeItemPtr->PolyItemType == I_Gouraud3dTexturedPolygon
				|| ShapeItemPtr->PolyItemType == I_ZB_Gouraud3dTexturedPolygon
				|| ShapeItemPtr->PolyItemType == I_ScaledSprite
				|| ShapeItemPtr->PolyItemType == I_3dTexturedPolygon
				|| ShapeItemPtr->PolyItemType == I_ZB_3dTexturedPolygon) {

				if(ShapeItemPtr->PolyFlags & iflag_txanim) {

					MakeTxAnimFrameTexturesGlobal(shptr, ShapeItemPtr,
															LTxIndex, TxIndex);

				}

				if(ShapeItemPtr->PolyColour & TxLocal) {

					txi = ShapeItemPtr->PolyColour;
					txi &= ~TxLocal;							/* Clear Flag */
					txi &= ClrTxDefn;							/* Clear UV array index */

					/* Is this the local index? */

					if(txi == LTxIndex) {

						/* Clear low word, OR in global index */

						ShapeItemPtr->PolyColour &= ClrTxIndex;
						ShapeItemPtr->PolyColour |= TxIndex;

					}

				}

			}

		}

	}


	


}


/*

 The animated texture frames each have a local image index in their frame
 header structure. Convert these to global texture list indices in the same
 way that the item function does.

*/

void MakeTxAnimFrameTexturesGlobal(SHAPEHEADER *sptr,
														POLYHEADER *pheader,
														int LTxIndex, int TxIndex)

{

	TXANIMHEADER **txah_ptr;
	TXANIMHEADER *txah;
	TXANIMFRAME *txaf;
	int **shape_textures;
	int *txf_imageptr;
	int texture_defn_index;
	int i, txi, image;




	/* Get the animation sequence header */

	shape_textures = sptr->sh_textures;
	texture_defn_index = (pheader->PolyColour >> TxDefn);
	txah_ptr = (TXANIMHEADER **) shape_textures[texture_defn_index];


	/* The first array element is the sequence shadow, which we skip here */

	txah_ptr++;


	/* Process the animation sequences */

	while(*txah_ptr) {

		/* Get the animation header */

		txah = *txah_ptr++;

		/* Process the animation frames */

		if(txah && txah->txa_numframes) {

			txaf = txah->txa_framedata;

			for(i = txah->txa_numframes; i!=0; i--) {

				/* Multi-View Sprite? */

				if(sptr->shapeflags & ShapeFlag_MultiViewSprite) {

					txf_imageptr = (int *) txaf->txf_image;

					for(image = txah->txa_num_mvs_images; image!=0; image--) {

						if(*txf_imageptr & TxLocal) {

							txi = *txf_imageptr;
							txi &= ~TxLocal;					/* Clear Flag */

							if(txi == LTxIndex) {

								*txf_imageptr = TxIndex;

							}

						}

						txf_imageptr++;

					}

				}

				else {

					if(txaf->txf_image & TxLocal) {

						txi = txaf->txf_image;
						txi &= ~TxLocal;					/* Clear Flag */

						if(txi == LTxIndex) {

							txaf->txf_image = TxIndex;

						}

					}

				}

				txaf++;

			}

		}

	}

}



/*

 Sprite Resizing

 or

 An Optimisation For Sprite Images

 This shape is a sprite which has requested the UV rescaling optimisation.
 The UV array is resized to fit the sprite image bounding rectangle, and
 after the UV array, in the space provided (don't forget that!), is a new
 shape local space XY array of points which overwrite the standard values
 when the shape is displayed.

*/

void SpriteResizing(SHAPEHEADER *sptr)

{

	TXANIMHEADER **txah_ptr;
	TXANIMHEADER *txah;
	TXANIMFRAME *txaf;
	int **shape_textures;
	IMAGEHEADER *ihdr;
	IMAGEEXTENTS e;
	IMAGEEXTENTS e_curr;
	IMAGEPOLYEXTENTS e_poly;
	int *uvptr;
	int texture_defn_index;
	int **item_array_ptr;
	int *item_ptr;
	POLYHEADER *pheader;
	int i, f;
	int polypts[4 * vsize];
	int *iptr;
	int *iptr2;
	int *mypolystart;
	int *ShapePoints = *(sptr->points);
	VECTOR2D cen_poly;
	VECTOR2D size_poly;
	VECTOR2D cen_uv_curr;
	VECTOR2D size_uv_curr;
	VECTOR2D cen_uv;
	VECTOR2D size_uv;
	VECTOR2D tv;
	int *txf_imageptr;
	int **txf_uvarrayptr;
	int *txf_uvarray;
	int image;
	int num_images;




	/* Get the animation sequence header */

	shape_textures = sptr->sh_textures;

	item_array_ptr = sptr->items;		/* Assume item array */
	item_ptr = item_array_ptr[0];		/* Assume only one polygon */
	pheader = (POLYHEADER *) item_ptr;


	/* Get the polygon points, and at the same time the extents, assuming an XY plane polygon */

	e_poly.x_low = bigint;
	e_poly.y_low = bigint;

	e_poly.x_high = smallint;
	e_poly.y_high = smallint;

	iptr = polypts;
	mypolystart = &pheader->Poly1stPt;

	for(i = 4; i!=0; i--) {

		iptr[ix] = ((VECTORCH*)ShapePoints)[*mypolystart].vx;
		iptr[iy] = ((VECTORCH*)ShapePoints)[*mypolystart].vy;
		iptr[iz] = ((VECTORCH*)ShapePoints)[*mypolystart].vz;

		if(iptr[ix] < e_poly.x_low) e_poly.x_low = iptr[ix];
		if(iptr[iy] < e_poly.y_low) e_poly.y_low = iptr[iy];
		if(iptr[ix] > e_poly.x_high) e_poly.x_high = iptr[ix];
		if(iptr[iy] > e_poly.y_high) e_poly.y_high = iptr[iy];

		iptr += vsize;
		mypolystart++;

	}


	/* TEST */
	/*trip_up;*/


	texture_defn_index = (pheader->PolyColour >> TxDefn);
	txah_ptr = (TXANIMHEADER **) shape_textures[texture_defn_index];


	/* The first array element is the sequence shadow, which we skip here */

	txah_ptr++;


	/* Process the animation sequences */

	while(*txah_ptr) {

		/* Get the animation header */

		txah = *txah_ptr++;

		/* Process the animation frames */

		if(txah && txah->txa_numframes) {

			txaf = txah->txa_framedata;

			for(f = txah->txa_numframes; f!=0; f--) {


				/* Multi-View Sprite? */

				if(sptr->shapeflags & ShapeFlag_MultiViewSprite) {

					txf_imageptr = (int *) txaf->txf_image;
					num_images = txah->txa_num_mvs_images;

					txf_uvarrayptr = (int **) txaf->txf_uvdata;

				}

				/* A standard "Single View" Sprite has just one image */

				else {

					txf_imageptr = &txaf->txf_image;
					num_images = 1;

					txf_uvarrayptr = &txaf->txf_uvdata;

				}


				for(image = 0; image < num_images; image++) {




					/* Get the image */

					ihdr = ImageHeaderPtrs[txf_imageptr[image]];

					/* Get the uv array ptr */

					txf_uvarray = txf_uvarrayptr[image];

					/* Find the extents of the image, assuming transparency */

					FindImageExtents(ihdr, txaf->txf_numuvs, txf_uvarray, &e, &e_curr);

					/* Convert the image extents to fixed point */


					e.u_low  <<= 16;
					e.v_low  <<= 16;
					e.u_high <<= 16;
					e.v_high <<= 16;

					/*

					We now have all the information needed to create a NEW UV array and a NEW
					polygon points XY array.

					*/

					/* Centre of the polygon */

					cen_poly.vx = (e_poly.x_low + e_poly.x_high) / 2;
					cen_poly.vy = (e_poly.y_low + e_poly.y_high) / 2;

					/* Size of the polygon */

					size_poly.vx = e_poly.x_high - e_poly.x_low;
					size_poly.vy = e_poly.y_high - e_poly.y_low;


					/* Centre of the current cookie */

					cen_uv_curr.vx = (e_curr.u_low + e_curr.u_high) / 2;
					cen_uv_curr.vy = (e_curr.v_low + e_curr.v_high) / 2;

					/* Size of the current cookie */

					size_uv_curr.vx = e_curr.u_high - e_curr.u_low;
					size_uv_curr.vy = e_curr.v_high - e_curr.v_low;


					/* Centre of the new cookie */

					cen_uv.vx = (e.u_low + e.u_high) / 2;
					cen_uv.vy = (e.v_low + e.v_high) / 2;

					/* Size of the new cookie */

					size_uv.vx = e.u_high - e.u_low;
					size_uv.vy = e.v_high - e.v_low;


					/* Write out the new UV data */

					uvptr = txf_uvarray;


					/*

					Convert the duplicate UV array to this new scale
					ASSUME that the format is "TL, BL, BR, TR"

					*/

					uvptr[0] = e.u_low;
					uvptr[1] = e.v_low;

					uvptr[2] = e.u_low;
					uvptr[3] = e.v_high;

					uvptr[4] = e.u_high;
					uvptr[5] = e.v_high;

					uvptr[6] = e.u_high;
					uvptr[7] = e.v_low;


					/*

					Create the new polygon XY array

					*/

					uvptr += (txaf->txf_numuvs * 2);	/* Advance the pointer past the UV array */


					/* Copy the polygon points (XY only) to the UV array space */

					iptr  = polypts;
					iptr2 = uvptr;

					for(i = 4; i!=0; i--) {

						iptr2[0] = iptr[ix];
						iptr2[1] = iptr[iy];

						iptr  += vsize;
						iptr2 += 2;

					}


					/* Scale the polygon points */

					iptr = uvptr;

					for(i = 4; i!=0; i--) {

						iptr[0] = WideMulNarrowDiv(iptr[0], size_uv.vx, size_uv_curr.vx);
						iptr[1] = WideMulNarrowDiv(iptr[1], size_uv.vy, size_uv_curr.vy);

						iptr += 2;

					}


					/* The translation vector in UV space */

					tv.vx = cen_uv.vx - cen_uv_curr.vx;
					tv.vy = cen_uv.vy - cen_uv_curr.vy;


					/* And now in world space */

					tv.vx = WideMulNarrowDiv(tv.vx, size_poly.vx, size_uv_curr.vx);
					tv.vy = WideMulNarrowDiv(tv.vy, size_poly.vy, size_uv_curr.vy);


					/* Translate the polygon points */

					iptr = uvptr;

					for(i = 4; i!=0; i--) {

						iptr[0] += tv.vx;
						iptr[1] += tv.vy;

						iptr += 2;

					}




				}




				/* Next Texture Animation Frame */

				txaf++;

			}

		}

	}



}


/*

 This function is for animated sprites, although it has been given an interface that would
 allow anyone to call it for their own purposes. It assumes that colour 0 is transparent and
 then calculates the actual maximum extents of the image.

 NOTE:

 The image scanned is that given by the CURRENT UV ARRAY and may therefore be smaller than
 the actual image. This allows for animation sequences with frames on the same image.

*/

void FindImageExtents(IMAGEHEADER *ihdr, int numuvs, int *uvdata, IMAGEEXTENTS *e, IMAGEEXTENTS *e_curr)

{

	int i;
	int *uvptr;
	int u, v;
	int startu, endu;
	int startv, endv;


	/* Find the current UV extents */

	e_curr->u_low = bigint;
	e_curr->v_low = bigint;

	e_curr->u_high = smallint;
	e_curr->v_high = smallint;

	uvptr = uvdata;

	for(i = numuvs; i!=0; i--) {

		if(uvptr[0] < e_curr->u_low) e_curr->u_low = uvptr[0];
		if(uvptr[1] < e_curr->v_low) e_curr->v_low = uvptr[1];

		if(uvptr[0] > e_curr->u_high) e_curr->u_high = uvptr[0];
		if(uvptr[1] > e_curr->v_high) e_curr->v_high = uvptr[1];

		uvptr += 2;

	}


	/* Look for the actual UV extents, assuming that colour 0 is transparent */

	switch(VideoModeType) {

		case VideoModeType_8:

			{

				TEXTURE *tptr = ihdr->ImagePtr;
				TEXTURE texel;


				/* Search for u_low and v_low */

				e->u_low = bigint;
				e->v_low = bigint;

				startv = e_curr->v_low  >> 16;
				endv   = e_curr->v_high >> 16;
				startu = e_curr->u_low  >> 16;
				endu   = e_curr->u_high >> 16;

				for(v = startv; v <= endv; v++) {

					for(u = startu; u <= endu; u++) {

						texel = tptr[(v * ihdr->ImageWidth) + u];

						if(texel) {

							if(u < e->u_low) e->u_low = u;
							if(v < e->v_low) e->v_low = v;

						}

					}

				}

				if(e->u_low == bigint) e->u_low = e_curr->u_low;
				if(e->v_low == bigint) e->v_low = e_curr->v_low;


				/* Search for u_high and v_high */

				e->u_high = smallint;
				e->v_high = smallint;

				for(v = endv; v >= startv; v--) {

					for(u = endu; u >= startu;  u--) {

						texel = tptr[(v * ihdr->ImageWidth) + u];

						if(texel) {

							if(u > e->u_high) e->u_high = u;
							if(v > e->v_high) e->v_high = v;

						}

					}

				}

				if(e->u_high == smallint) e->u_high = e_curr->u_high;
				if(e->v_high == smallint) e->v_high = e_curr->v_high;


				/* TEST */
				/*trip_up;*/

			}

			break;

		case VideoModeType_15:
			break;

		case VideoModeType_24:
			break;

		case VideoModeType_8T:
			break;

	}

}



/*

 Return a pointer to the next image header

*/


IMAGEHEADER* GetImageHeader(void)
{

	IMAGEHEADER *iheader;


	iheader = NextFreeImageHeaderPtr++;
	GLOBALASSERT(NumImages < MaxImages);

	
	/* ensure flags are zero */
	memset(iheader,0,sizeof(IMAGEHEADER));

	ImageHeaderPtrs[NumImages] = iheader;
	NumImages++;
	
	return iheader;

}




static void DeallocateImageHeader(IMAGEHEADER * ihptr)
{
	if (ihptr->hBackup)
	{
		if (ihptr->DDSurface)
		{
			GLOBALASSERT(!ihptr->D3DTexture);
			ATRemoveSurface(ihptr->DDSurface);
		}
		else if (ihptr->D3DTexture)
		{
			GLOBALASSERT(!ihptr->DDSurface);
			ATRemoveTexture(ihptr->D3DTexture);
		}
		
		AwDestroyBackupTexture(ihptr->hBackup);
		ihptr->hBackup = 0;
	}
	
	if (ihptr->ImagePtr) 
	{
		DeallocateMem(ihptr->ImagePtr);
		ihptr->ImagePtr = 0;
	}

	if (ihptr->DDSurface)
	{
		ReleaseDDSurface(ihptr->DDSurface);
		ihptr->DDSurface = (void*) 0;
	}

	if (ihptr->D3DTexture)
	{
		ReleaseD3DTexture(ihptr->D3DTexture);
		ihptr->D3DTexture = (void*) 0;
		ihptr->D3DHandle = (void*) 0;
	}
}

static void MinimizeImageHeader(IMAGEHEADER * ihptr)
{
	if (ihptr->DDSurface)
	{
		ReleaseDDSurface(ihptr->DDSurface);
		ihptr->DDSurface = (void*) 0;
	}

	if (ihptr->D3DTexture)
	{
		ReleaseD3DTexture(ihptr->D3DTexture);
		ihptr->D3DTexture = (void*) 0;
		ihptr->D3DHandle = (void*) 0;
	}
}



int DeallocateAllImages(void)
{
	int i;
	IMAGEHEADER *ihptr;

	if (NumImages)
	{
		ihptr = ImageHeaderArray;
		for (i = NumImages; i!=0; i--)
		{
			DeallocateImageHeader(ihptr++);
		}		
		NumImages = 0;
		NextFreeImageHeaderPtr = ImageHeaderArray;
	}

	return Yes; /* ok for the moment */
}

int MinimizeAllImages(void)
{
	int i;
	IMAGEHEADER *ihptr;

	if (NumImages)
	{
		ihptr = ImageHeaderArray;
		for (i = NumImages; i!=0; i--)
		{
			MinimizeImageHeader(ihptr++);
		}		
	}

	return Yes; /* ok for the moment */
}




/*
The RIFF_SYSTEM uses this function to return an image number
for an image which might be already loaded. The argument
passed points to the full pathname of the image that the
system wants to load, so an explicit stricmp on the
image names of already loaded images will suffice. To
avoid loading images more than once, it ensures that the
path generated for two identical images will always be
the same. It also fills in the ImageName field with the
fill path of images it loads.

Currently I am assuming that users of ImageGroups will
no longer need images in group n+1 when images in group
n are deleted, so I can check in all groups from 0...Current
to see if the image is already loaded.

Jake.
*/

int GetExistingImageNum(char const * fname)
{
	int i;
	IMAGEHEADER * iharrayptr;
	
	
	for (i=0, iharrayptr = ImageHeaderArray; i<NumImages; ++i, ++iharrayptr)
	{
		if (!stricmp(iharrayptr->ImageName,fname)) return i;
	}

	
	return GEI_NOTLOADED;
}



