#ifndef INLINE_INCLUDED
#define INLINE_INCLUDED

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif


#ifdef __cplusplus
extern "C" {
#endif

/* 
	Standard macros.  Note that FIXED_TO_INT
	and INT_TO_FIXED are very suboptimal in 
	this version!!!
	Also, MUL_INT and ISR are ONLY intended 
	to be used in Win95 so that Saturn versions
	of the same code can be compiled using calls
	to hand optimised assembler functions, i.e.
	for code that is never intended to be run on
	a Saturn they are unnecessary.
*/

#define OUR_ABS(x)                (((x) < 0) ? -(x) : (x))
#define OUR_SIGN(x)	             (((x) < 0) ? -1 : +1)
#define OUR_INT_TO_FIXED(x)	 	 (int) ((x) * (65536))
#define OUR_FIXED_TO_INT(x)		 (int) ((x) / (65536))
#define OUR_MUL_INT(a, b)	       ((a) * (b))
#define OUR_ISR(a, shift)		    ((a) >> (shift))



/* inline assembly has been moved to mathline.c */
void ADD_LL_PP(LONGLONGCH *c, LONGLONGCH *a);
void MUL_I_WIDE(int a, int b, LONGLONGCH *c);
int CMP_LL(LONGLONGCH *a, LONGLONGCH *b);
void EQUALS_LL(LONGLONGCH *a, LONGLONGCH *b);
void NEG_LL(LONGLONGCH *a);
void ASR_LL(LONGLONGCH *a, int shift);
void IntToLL(LONGLONGCH *a, int *b);
int MUL_FIXED(int a, int b);
int DIV_FIXED(int a, int b);

#define DIV_INT(a, b) ((a) / (b))

int NarrowDivide(LONGLONGCH *a, int b);
int WideMulNarrowDiv(int a, int b, int c);
void RotateVector_ASM(VECTORCH *v, MATRIXCH *m);
void RotateAndCopyVector_ASM(VECTORCH *v1, VECTORCH *v2, MATRIXCH *m);

/*
int FloatToInt(float);
#define f2i(a, b) { a = FloatToInt(b); }
*/

int SqRoot32(int A);
void FloatToInt();
extern float fti_fptmp;
extern int fti_itmp;

#define f2i(a, b) { \
fti_fptmp = (b); \
FloatToInt(); \
a = fti_itmp;}



int WideMul2NarrowDiv(int a, int b, int c, int d, int e);
int _Dot(VECTORCH *vptr1, VECTORCH *vptr2);


void RotVect(VECTORCH *v, MATRIXCH *m);


#define RotateVector(v,m) (_RotateVector((v),(m)))
#define RotateAndCopyVector(v_in,v_out,m) (_RotateAndCopyVector((v_in),(v_out),(m)))
#define Dot(v1,v2) (_Dot((v1),(v2)))
#define DotProduct(v1,v2) (_DotProduct((v1),(v2)))



#ifdef __cplusplus
}
#endif


#endif
