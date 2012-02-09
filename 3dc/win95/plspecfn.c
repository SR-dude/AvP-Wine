#include "3dc.h"
#include <math.h>
#include "inline.h"
#include "module.h"
#include "stratdef.h"
#include "gamedef.h"
#include "bh_types.h"
#include "pvisible.h"

#include "krender.h"
#include "kzsort.h"
#include "kshape.h"



/*

 Platform Specific Functions

 These functions have been written specifically for a given platform.

 They are not necessarily IO or inline functions; these have their own files.

*/


/*

 externs for commonly used global variables and arrays

*/

	extern VECTORCH RotatedPts[];
	extern unsigned int Outcodes[];
	extern DISPLAYBLOCK *Global_ODB_Ptr;
	extern VIEWDESCRIPTORBLOCK *Global_VDB_Ptr;
	extern SHAPEHEADER *Global_ShapeHeaderPtr;
	extern MATRIXCH LToVMat;
	extern VECTORCH MorphedPts[];
	extern MORPHDISPLAY MorphDisplay;

	extern DISPLAYBLOCK *OnScreenBlockList[];
	extern int NumOnScreenBlocks;
	extern int NumActiveBlocks;
	extern DISPLAYBLOCK *ActiveBlockList[];
	extern char *ModuleLocalVisArray;



/*

 Global Variables

*/

	LONGLONGCH ll_one14 = {one14, 0};
	LONGLONGCH ll_zero = {0, 0};
// adj formerly an extern declared in kzsort.c
	static int *MorphedObjectPointsPtr;


/*

 WideMul2NarrowDiv

 This function takes two pairs of integers, adds their 64-bit products
 together, divides the summed product with another integer and then returns
 the result of that divide, which is also an integer.

 It is not inlined for Watcom C, although the functions it calls ARE.

*/

int WideMul2NarrowDiv(int a, int b, int c, int d, int e)

{

	LONGLONGCH f;
	LONGLONGCH g;


	MUL_I_WIDE(a, b, &f);
	MUL_I_WIDE(c, d, &g);
	ADD_LL_PP(&f, &g);

	return NarrowDivide(&f, e);

}





/*

 Square Root

 Returns the Square Root of a 32-bit number

*/


#if 0  // adj
// adj   There is a version of this in mathline.c but it is assembly
// adj  I may want this

int SqRoot32(int A)

{

	unsigned int edx = A;
	unsigned int ecx;

	unsigned int ax = 0;
	unsigned int bx = 0;
	unsigned int di = 0;


	for(ecx = 15; ecx!=0; ecx--) {

		bx <<= 1;
		if(edx & 0x80000000) bx |= 1;
		edx <<= 1;

		bx <<= 1;
		if(edx & 0x80000000) bx |= 1;
		edx <<= 1;

		ax += ax;
		di =  ax;
		di += di;

		if(bx > di) {

			di++;
			ax++;

			bx -= di;

		}

	}

	bx <<= 1;
	if(edx & 0x80000000) bx |= 1;
	edx <<= 1;

	bx <<= 1;
	if(edx & 0x80000000) bx |= 1;
	edx <<= 1;

	ax += ax;
	di =  ax;
	di += di;

	if(bx > di) {

		ax++;

	}

	return ((int)ax);

}
#endif



/*

 Calculate Plane Normal from three POP's

 The three input vectors are treated as POP's and used to make two vectors.
 These are then crossed to create the normal.

 Make two vectors; (2-1) & (3-1)
 Cross them
 Normalise the vector
  Find the magnitude of the vector
  Divide each component by the magnitude

*/

void MakeNormal(VECTORCH *v1, VECTORCH *v2, VECTORCH *v3, VECTORCH *v4)

{



	VECTORCHF vect0;
	VECTORCHF vect1;
	VECTORCHF n;


	/* vect0 = v2 - v1 */

	vect0.vx = v2->vx - v1->vx;
	vect0.vy = v2->vy - v1->vy;
	vect0.vz = v2->vz - v1->vz;

	/* vect1 = v3 - v1 */

	vect1.vx = v3->vx - v1->vx;
	vect1.vy = v3->vy - v1->vy;
	vect1.vz = v3->vz - v1->vz;


	/* nx = v0y.v1z - v0z.v1y */

	n.vx = (vect0.vy * vect1.vz) - (vect0.vz * vect1.vy);

	/* ny = v0z.v1x - v0x.v1z */

	n.vy = (vect0.vz * vect1.vx) - (vect0.vx * vect1.vz);

	/* nz = v0x.v1y - v0y.v1x */

	n.vz = (vect0.vx * vect1.vy) - (vect0.vy * vect1.vx);


	FNormalise(&n);

	f2i(v4->vx, n.vx * ONE_FIXED);
	f2i(v4->vy, n.vy * ONE_FIXED);
	f2i(v4->vz, n.vz * ONE_FIXED);



}


/*

 Normalise a vector.

 The returned vector is a fixed point unit vector.

 WARNING!

 The vector must be no larger than 2<<14 because of the square root.
 Because this is an integer function, small components produce errors.

 e.g.

 (100,100,0)

 m=141 (141.42)

 nx = 100 * ONE_FIXED / m = 46,479
 ny = 100 * ONE_FIXED / m = 46,479
 nz = 0

 New m ought to be 65,536 but in fact is 65,731 i.e. 0.29% too large.

*/

void Normalise(VECTORCH *nvector)

{

	VECTORCHF n;
	float m;


	n.vx = nvector->vx;
	n.vy = nvector->vy;
	n.vz = nvector->vz;

	m = 65536.0/sqrt((n.vx * n.vx) + (n.vy * n.vy) + (n.vz * n.vz));

	f2i(nvector->vx, (n.vx * m) );
	f2i(nvector->vy, (n.vy * m) );
	f2i(nvector->vz, (n.vz * m) );


}

void FNormalise(VECTORCHF *n)

{

	float m;


	m = sqrt((n->vx * n->vx) + (n->vy * n->vy) + (n->vz * n->vz));

	n->vx /= m;
	n->vy /= m;
	n->vz /= m;

}

/*

 Return the magnitude of a vector

*/

int Magnitude(VECTORCH *v)

{

	VECTORCHF n;
	int m;


	n.vx = v->vx;
	n.vy = v->vy;
	n.vz = v->vz;

	f2i(m, sqrt((n.vx * n.vx) + (n.vy * n.vy) + (n.vz * n.vz)));

	return m;


}



/*

 Dot Product Function - Inline Version

 It accepts two pointers to vectors and returns an int result

*/

int _Dot(VECTORCH *vptr1, VECTORCH *vptr2)

{

	int dp;

	dp  = MUL_FIXED(vptr1->vx, vptr2->vx);
	dp += MUL_FIXED(vptr1->vy, vptr2->vy);
	dp += MUL_FIXED(vptr1->vz, vptr2->vz);

	return(dp);

}





