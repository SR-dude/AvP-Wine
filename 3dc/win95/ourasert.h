 /* This is our assert file for the Win95 platform, with Dave's global/local assert distinctions. */

/*
 * Note that WaitForReturn now calls FlushTextprintBuffer and FlipBuffers
 * implicitly. ;
 * Modified 10th December 1996 by Dave Malcolm. Now can be set so that functions
 * are supplied by the project/platform to fire when an assertion fires. Also is
 * set so that the compiler will generate an error message if you manage to
 * include the file more than once (with confusing definitons of UseLocalAssert);
 * this can be disabled.
 */

#ifdef __cplusplus
extern "C"
{
#endif
int		GlobalAssertFired(char *Filename, int LineNum, char *Condition);
int		LocalAssertFired(char *Filename, int LineNum, char *Condition);
void	ExitFired(char *Filename, int LineNum, int ExitCode);
#ifdef __cplusplus
};
#endif


#define GLOBALASSERT(x) (void) ((x) ? 1 : (GlobalAssertFired(__FILE__, __LINE__, #x)))
#if UseLocalAssert
#define LOCALASSERT(x)	(void) ((x) ? 1 : (LocalAssertFired(__FILE__, __LINE__, #x)))
#else
#define LOCALASSERT(ignore)
#endif

#define exit(x) ExitFired(__FILE__, __LINE__, x)
