#include "3dc.h"
#include "module.h"
#include "stratdef.h"
#include "gamedef.h"
#include "system.h"
#include "equates.h"
#include "platform.h"
#include "shape.h"
#include "prototyp.h"
#include "inline.h"
#include "dp_sprh.h"
#include "dplayext.h"
#include "equipmnt.h"
#include "pldnet.h"			   
#include "dp_func.h"

#define UseLocalAssert Yes
#include "ourasert.h"


/* KJL 14:58:18 03/07/98 - AvP's Guid */
// {379CCA80-8BDD-11d0-A078-004095E16EA5}
static const GUID AvPGuid = 
{ 0x379cca80, 0x8bdd, 0x11d0, { 0xa0, 0x78, 0x0, 0x40, 0x95, 0xe1, 0x6e, 0xa5 } };


LPDPLCONNECTION	glpdplConnection;	// connection settings

/* Some important globals */
LPGUID					glpGuid = (LPGUID)&AvPGuid;
LPDIRECTPLAY4			glpDP	= NULL;		// directplay object pointer
LPDPSESSIONDESC2		glpdpSD;			// current session description

DPID AVPDPNetID;
DPNAME AVPDPplayerName;


BOOL				gbUseProtocol=0;		// DirectPlay Protocol messaging
BOOL				gbAsyncSupported=0;	// asynchronous sends supported


/*
 * DPlayClose
 *
 * Wrapper for DirectPlay Close API
 */
HRESULT DPlayClose(void)
{
	HRESULT hr=E_FAIL;

	if (glpDP) 
		hr = IDirectPlayX_Close(glpDP);
	
	return hr;
}

/*
 * DPlayCreate
 *
 * Wrapper for DirectPlay Create API.
 * Retrieves a DirectPlay4/DirectPlay4A interface based on the UNICODE flag
 * 
 */
HRESULT DPlayCreate(LPVOID lpCon)
{
	HRESULT hr=E_FAIL;

	// release if already exists
	if (glpDP) IDirectPlayX_Release(glpDP);
	glpDP=NULL;


	// create a DirectPlay4(A) interface
	hr = CoCreateInstance(&CLSID_DirectPlay, NULL, CLSCTX_INPROC_SERVER,
						  &IID_IDirectPlay4A, (LPVOID *) &glpDP);
	if (FAILED(hr))
		return (hr);

	// initialize w/address
	if (lpCon)
	{
		hr = IDirectPlayX_InitializeConnection(glpDP, lpCon, 0);
		if (FAILED(hr))
			goto FAILURE;
	}
	return hr;

FAILURE:
	IDirectPlayX_Release(glpDP);
	glpDP = NULL;

	return hr;
}



/*
 * DPlayCreateSession
 *
 * Wrapper for DirectPlay CreateSession API.Uses the global application guid (glpGuid).
 */
HRESULT DPlayCreateSession(LPTSTR lptszSessionName,int maxPlayers,int dwUser1,int dwUser2)
{
	HRESULT hr = E_FAIL;
	DPSESSIONDESC2 dpDesc;

	if (!glpDP)
		return DPERR_NOINTERFACE;

	ZeroMemory(&dpDesc, sizeof(dpDesc));
	dpDesc.dwSize = sizeof(dpDesc);
	dpDesc.dwFlags = DPSESSION_MIGRATEHOST | DPSESSION_KEEPALIVE;
	if (gbUseProtocol)
		dpDesc.dwFlags |= DPSESSION_DIRECTPLAYPROTOCOL;

	dpDesc.lpszSessionNameA = lptszSessionName;
	dpDesc.dwMaxPlayers=maxPlayers;

	dpDesc.dwUser1 = dwUser1;
	dpDesc.dwUser2 = dwUser2;

	// set the application guid
	if (glpGuid)
		dpDesc.guidApplication = *glpGuid;

	hr = IDirectPlayX_Open(glpDP, &dpDesc, DPOPEN_CREATE);


	return hr;
}




/*
 * DPlayOpenSession
 *
 * Wrapper for DirectPlay OpenSession API. 
 */
HRESULT DPlayOpenSession(LPGUID lpSessionGuid)
{
	HRESULT hr = E_FAIL;
	DPSESSIONDESC2 dpDesc;

	if (!glpDP)
		return DPERR_NOINTERFACE;

	ZeroMemory(&dpDesc, sizeof(dpDesc));
	dpDesc.dwSize = sizeof(dpDesc);
	if (gbUseProtocol)
		dpDesc.dwFlags = DPSESSION_DIRECTPLAYPROTOCOL;

	// set the session guid
	if (lpSessionGuid)
		dpDesc.guidInstance = *lpSessionGuid;
	// set the application guid
	if (glpGuid)
		dpDesc.guidApplication = *glpGuid;

	// open it
	hr = IDirectPlayX_Open(glpDP, &dpDesc, DPOPEN_JOIN);

	// Check for Async message support
//	if (SUCCEEDED(hr))
//		CheckCaps();

	return hr;
}



/*
 * DPlayRelease
 *
 * Wrapper for DirectPlay Release API.
 */
HRESULT DPlayRelease(void)
{
	HRESULT hr = E_FAIL;

	if (glpDP)
	{
		// free session desc, if any
		if (glpdpSD) 
		{
			free(glpdpSD);
			glpdpSD = NULL;
		}

		// free connection settings structure, if any (lobby stuff)
		if (glpdplConnection)
		{
			free(glpdplConnection);
			glpdplConnection = NULL;
		}
		// release dplay
		hr = IDirectPlayX_Release(glpDP);
		glpDP = NULL;
	}

	return hr;
}




