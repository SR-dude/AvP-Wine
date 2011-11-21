#include "3dc.h"
#include <malloc.h>

#define UseLocalAssert  No
#include "ourasert.h"


int alloc_cnt = 0;
int deall_cnt = 0;


void *AllocMem(size_t __size);
void DeallocMem(void *__ptr);

/* Note: Never use AllocMem directly !   */
/* Instead use AllocateMem() which is a  */
/* macro defined in mem3dc.h that allows */
/* for debugging info.                   */

void *AllocMem(size_t __size)
{
	GLOBALASSERT(__size>0);
	alloc_cnt++;	

	return malloc(__size);
};

/* Note: Never use DeallocMem directly !  */
/* Instead use DeallocateMem() which is a */
/* macro defined in mem3dc.h that allows  */
/* for debugging info.                    */

void DeallocMem(void *__ptr)
{
	deall_cnt++;

	if(__ptr) free(__ptr);

	else {

		textprint("ERROR - freeing null ptr\n");
		WaitForReturn();

	}
	
};

