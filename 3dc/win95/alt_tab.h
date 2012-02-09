/*
JH - 18/02/98
Deal with lost surfaces and textures - restore them when the application is re-activated
*/

#ifndef _INCLUDED_ALT_TAB_H_
#define _INCLUDED_ALT_TAB_H_

#include "aw.h"

#ifdef __cplusplus
	extern "C" {
#endif

typedef void (* AT_PFN_RESTORETEXTURE) (D3DTexture * pTexture, void * pUser);
typedef void (* AT_PFN_RESTORESURFACE) (DDSurface * pSurface, void * pUser);

	extern void _ATIncludeTexture(D3DTexture * pTexture, AW_BACKUPTEXTUREHANDLE hBackup, char const * pszFile, unsigned nLine, char const * pszDebugString);
	extern void _ATIncludeSurface(DDSurface * pSurface, AW_BACKUPTEXTUREHANDLE hBackup, char const * pszFile, unsigned nLine, char const * pszDebugString);
	#define ATIncludeTexture(p,h) _ATIncludeTexture(p,h,__FILE__,__LINE__,NULL)
	#define ATIncludeSurface(p,h) _ATIncludeSurface(p,h,__FILE__,__LINE__,NULL)
	#define ATIncludeTextureDb(p,h,d) _ATIncludeTexture(p,h,__FILE__,__LINE__,d)
	#define ATIncludeSurfaceDb(p,h,d) _ATIncludeSurface(p,h,__FILE__,__LINE__,d)


extern void ATRemoveTexture(D3DTexture * pTexture);
extern void ATRemoveSurface(DDSurface * pSurface);

extern void ATOnAppReactivate();

#ifdef __cplusplus
	}
#endif

#endif /* ! _INCLUDED_ALT_TAB_H_ */
