#include "3dc.h"

#include "inline.h"



extern MORPHDISPLAY MorphDisplay;
extern int NormalFrameTime;



// Update Morphing Animation Control Block

void UpdateMorphing(MORPHCTRL *mcptr)

{

	MORPHHEADER *mhdr = mcptr->ObMorphHeader;
	int UpdateRate;


	if(mcptr->ObMorphFlags & mph_flag_play) {

		/* How fast? */

		if(mcptr->ObMorphSpeed == ONE_FIXED) {

			UpdateRate = NormalFrameTime;

		}

		else {

			UpdateRate = MUL_FIXED(NormalFrameTime, mcptr->ObMorphSpeed);

		}


		/* Update the current frame */

		if(mcptr->ObMorphFlags & mph_flag_reverse) {

			mcptr->ObMorphCurrFrame -= UpdateRate;

			if(mcptr->ObMorphCurrFrame < 0) {

				if(mcptr->ObMorphFlags & mph_flag_noloop) {

					mcptr->ObMorphCurrFrame = 0;

					/* The sequence has finished and we are at the start */

					mcptr->ObMorphFlags |= (mph_flag_finished | mph_flag_start);

				}

				else {

					mcptr->ObMorphCurrFrame += mhdr->mph_maxframes;

					/* The sequence has looped and we are back at the end */

					mcptr->ObMorphFlags |= (mph_flag_looped | mph_flag_end);

				}

			}

		}

		else {

			mcptr->ObMorphCurrFrame += UpdateRate;

			if(mcptr->ObMorphCurrFrame >= mhdr->mph_maxframes) {

				if(mcptr->ObMorphFlags & mph_flag_noloop) {

					/* The sequence has finished and we are at the end */

					mcptr->ObMorphFlags |= (mph_flag_finished | mph_flag_end);

					mcptr->ObMorphCurrFrame = mhdr->mph_maxframes - 1;

				}

				else {

					mcptr->ObMorphCurrFrame -= mhdr->mph_maxframes;

					/* The sequence has looped and we are back at the start */

					mcptr->ObMorphFlags |= (mph_flag_looped | mph_flag_start);

				}

			}

		}

	}

}




/*

 Using the current frame, calculate the lerp values and find out which two
 shapes to interpolate between.

 Write this information back to a MORPHDISPLAY structure.

*/

void GetMorphDisplay(MORPHDISPLAY *md, DISPLAYBLOCK *dptr)

{

	MORPHFRAME *mdata;
	MORPHCTRL *mc = dptr->ObMorphCtrl;
	MORPHHEADER *mhdr = mc->ObMorphHeader;


	md->md_lerp = mc->ObMorphCurrFrame & 0xffff;
	md->md_one_minus_lerp = ONE_FIXED - md->md_lerp;

	mdata = mhdr->mph_frames;
	mdata = &mdata[mc->ObMorphCurrFrame >> 16];

	md->md_shape1 = mdata->mf_shape1;
	md->md_shape2 = mdata->mf_shape2;

	md->md_sptr1 = GetShapeData(md->md_shape1);
	md->md_sptr2 = GetShapeData(md->md_shape2);

}




 
