/*------------------------------Patrick 18/3/97------------------------
Header for setting up and handling direct play objects
(This is mostly nicked from DHM's headhunter stuff)
-----------------------------------------------------------------------*/
#ifndef dpfunc_h_included
#define dpfunc_h_included
#ifdef __cplusplus
extern "C" {
#endif

/* globals */
extern LPDIRECTPLAY4 glpDP;
extern LPGUID glpGuid;

extern DPNAME AVPDPplayerName;
extern DPID AVPDPNetID;


/* Constants */ 
#define	MAX_SIZE_FORMAL_NAME	128+1
#define	MAX_SIZE_FRIENDLY_NAME	128+1


HRESULT DPlayClose(void);
HRESULT DPlayCreate(LPVOID lpCon);
HRESULT DPlayCreateSession(LPTSTR lptszSessionName,int maxPlayers,int dwUser1,int dwUser2);
HRESULT DPlayClose(void);
HRESULT DPlayCreate(LPVOID lpCon);

HRESULT DPlayOpenSession(LPGUID lpSessionGuid);
HRESULT DPlayRelease(void);



#ifdef __cplusplus
}
#endif
#endif
