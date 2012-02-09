#include "3dc.h"
#include "module.h"
#include "inline.h"

#include "gameplat.h"
#include "gamedef.h"

#include "dynblock.h"
#include "dynamics.h"
#define UseLocalAssert No
#include "ourasert.h"

/* Externs from pc\io.c */
extern int InputMode;
extern unsigned char KeyboardInput[];


void catpathandextension(char* dst, char* src)
{
	int len = lstrlen(dst);

	if ((len > 0 && (dst[len-1] != '\\' && dst[len-1] != '/')) && *src != '.')
		{
			lstrcat(dst,"\\");
		}

    lstrcat(dst,src);

/*
	The second null here is to support the use
	of SHFileOperation, which is a Windows 95
	addition that is uncompilable under Watcom
	with ver 10.5, but will hopefully become
	available later...
*/
    len = lstrlen(dst);
    dst[len+1] = 0;

}


int IDemandTurnLeft(void)
{
	InputMode = Digital;
	if(KeyboardInput[KEY_LEFT])
		return Yes;
	return No;
}



int IDemandTurnRight(void)
{
	InputMode = Digital;
	if(KeyboardInput[KEY_RIGHT])
		return Yes;
	return No;
}



int IDemandGoForward(void)
{
	InputMode = Digital;
	if(KeyboardInput[KEY_UP])
		return Yes;
	return No;
}



int IDemandGoBackward(void)
{
	InputMode = Digital;
	if(KeyboardInput[KEY_DOWN])
		return Yes;
	return No;
}



int IDemandSelect(void)
{
	InputMode = Digital;
    
    if(KeyboardInput[KEY_CR]) return Yes;
    if(KeyboardInput[KEY_SPACE]) return Yes;
	else return No;
}



/* KJL 15:53:52 05/04/97 - 
Loaders/Unloaders for language internationalization code in language.c */

char *LoadTextFile(char *filename)
{
	char *bufferPtr;
	long int save_pos, size_of_file;
	FILE *fp;
	fp = fopen(filename,"rb");
	
	if (!fp) goto error;

	save_pos=ftell(fp);
	fseek(fp,0L,SEEK_END);
	size_of_file=ftell(fp);
	
	bufferPtr = AllocateMem(size_of_file);
	if (!bufferPtr)
	{
		memoryInitialisationFailure = 1;
		goto error;
	}

	fseek(fp,save_pos,SEEK_SET);

	
	if (!fread(bufferPtr, size_of_file,1,fp))
	{
		fclose(fp);
		goto error;
	}
			
	fclose(fp);
	return bufferPtr;
	
error:
	{
		/* error whilst trying to load file */
		textprint("Error! Can not load file %s.\n",filename);
		LOCALASSERT(0);
		return 0;
	}
}


void UnloadTextFile(char *filename, char *bufferPtr)
{
	if (bufferPtr) DeallocateMem(bufferPtr);
}
