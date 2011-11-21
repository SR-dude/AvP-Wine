/* mem3dc.h */
#ifndef MEM3DC_H_INCLUDED
#define MEM3DC_H_INCLUDED

#ifdef __cplusplus

	extern "C" {

#endif

#include "system.h"
#include <stddef.h>

/* defines */

#define DBGMALLOC 0




/* JH - 30.5.97
I noticed that the MALLOC_RECORD structure has char[40]
for the filename. Since I know that on the PC, the __FILE__
macro results in a string compiled into the executable, and
evaulates to a pointer to that string, we do not need to make
a separate copy of the string for each malloc - just store the
pointer.
So, on PC this reduces the data size for the malloc records from 1.04Mb to 320K ! */

/*  #define AllocateMem(x) record_malloc(x,__FILE__, __LINE__) */


/* platform specific memory allocation and deallocation declarations */
extern void *AllocMem(size_t __size);
extern void DeallocMem(void *__ptr);

/* mem.c public functions */

extern void record_free(void *ptr, char const * string, unsigned long lineno);
extern void *record_malloc(long size, char const * string, unsigned long lineno);

#define AllocateMem(x) AllocMem(x) 
#define DeallocateMem(x) DeallocMem(x) 

#ifdef __cplusplus

	};

#endif


#endif
