/*****************************************************************************/
/* DSoundHost.cpp: DirectSound double-buffer classes                         */
/*****************************************************************************/
/*                                                                           */
/* $Archive::                                                              $ */
/* $Revision::                                                             $ */
/* $Date::                                                                 $ */
/* $Author::                                                               $ */
/* $Modtime::                                                              $ */
/* $Workfile::                                                             $ */
/*                                                                           */
/*****************************************************************************/

#include "stdafx.h"
#include "DSoundDev.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*****************************************************************************/
/* GUIDs (potentially) used herein                                           */
/*****************************************************************************/

#include <initguid.h>
                                        /* copied from dsound.h              */
DEFINE_GUID(CLSID_DirectSound, 0x47d4d946, 0x62e8, 0x11cf, 0x93, 0xbc, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0);
DEFINE_GUID(CLSID_DirectSoundCapture, 0xb0210780, 0x89cd, 0x11d0, 0xaf, 0x8, 0x0, 0xa0, 0xc9, 0x25, 0xcd, 0x16);

DEFINE_GUID(IID_IDirectSound, 0x279AFA83, 0x4981, 0x11CE, 0xA5, 0x21, 0x00, 0x20, 0xAF, 0x0B, 0xE5, 0x60);
DEFINE_GUID(IID_IDirectSoundCapture, 0xb0210781, 0x89cd, 0x11d0, 0xaf, 0x8, 0x0, 0xa0, 0xc9, 0x25, 0xcd, 0x16);
DEFINE_GUID(IID_IDirectSoundBuffer, 0x279AFA85, 0x4981, 0x11CE, 0xA5, 0x21, 0x00, 0x20, 0xAF, 0x0B, 0xE5, 0x60);
DEFINE_GUID(IID_IDirectSound3DListener, 0x279AFA84, 0x4981, 0x11CE, 0xA5, 0x21, 0x00, 0x20, 0xAF, 0x0B, 0xE5, 0x60);
DEFINE_GUID(IID_IDirectSound3DBuffer, 0x279AFA86, 0x4981, 0x11CE, 0xA5, 0x21, 0x00, 0x20, 0xAF, 0x0B, 0xE5, 0x60);
DEFINE_GUID(IID_IDirectSoundCaptureBuffer, 0xb0210782, 0x89cd, 0x11d0, 0xaf, 0x8, 0x0, 0xa0, 0xc9, 0x25, 0xcd, 0x16);
DEFINE_GUID(IID_IDirectSoundNotify, 0xb0210783, 0x89cd, 0x11d0, 0xaf, 0x8, 0x0, 0xa0, 0xc9, 0x25, 0xcd, 0x16);
DEFINE_GUID(IID_IKsPropertySet, 0x31efac30, 0x515c, 0x11d0, 0xa9, 0xaa, 0x00, 0xaa, 0x00, 0x61, 0xbe, 0x93);

/*****************************************************************************/
/* CDSoundDll class declaration                                              */
/*****************************************************************************/
/* This encapsulation is necessary since not all systems have the same set   */
/* of functions in DSOUND.dll (or no DSOUND.dll at all!)                     */
/*****************************************************************************/

class CDSoundDll
  {
  public:
    CDSoundDll();
    virtual ~CDSoundDll();

    bool IsLoaded() { return !!hDll; }

    HRESULT CaptureCreate(LPGUID lpGUID, LPDIRECTSOUNDCAPTURE *lplpDSC, LPUNKNOWN pUnkOuter);
    HRESULT Create(LPGUID lpGuid, LPDIRECTSOUND * ppDS, IUnknown FAR * pUnkOuter);
    HRESULT CaptureEnumerate(LPDSENUMCALLBACK lpDSEnumCallback, LPVOID lpContext);
    HRESULT Enumerate(LPDSENUMCALLBACK lpDSEnumCallback, LPVOID lpContext);

  protected:
    enum
      {
      eFuncCaptureCreate,
      eFuncCreate,
      eFuncCaptureEnumerate,
      eFuncEnumerate,
      eFuncMax
      };

    bool LoadDll();
    bool UnloadDll();
    HMODULE hDll;
    FARPROC proc[eFuncMax];
  };

/*****************************************************************************/
/* CDSoundDll : constructor                                                  */
/*****************************************************************************/

CDSoundDll::CDSoundDll()
{
hDll = NULL;
LoadDll();                              /* initialize DLL automatically      */
}

/*****************************************************************************/
/* ~CDSoundDll : destructor                                                  */
/*****************************************************************************/

CDSoundDll::~CDSoundDll()
{
UnloadDll();                            /* make sure to unload the DLL       */
}

/*****************************************************************************/
/* LoadDll : loads the DirectSound DLL and resolves needed entry points      */
/*****************************************************************************/

bool CDSoundDll::LoadDll()
{
UnloadDll();                            /* start clean                       */
                                        /* make sure there's no popup        */
UINT oem = ::SetErrorMode(SEM_FAILCRITICALERRORS |
                          SEM_NOGPFAULTERRORBOX |
                          SEM_NOOPENFILEERRORBOX);
hDll = ::LoadLibrary("DSOUND.dll");     /* load the DLL                      */
::SetErrorMode(oem);                    /* restore previous error mode       */
if (!hDll)                              /* if error loading the DLL          */
  return false;                         /* return with error                 */

static LPCSTR lpszFuncName[eFuncMax] =
  {
  "DirectSoundCaptureCreate",
  "DirectSoundCreate",
  "DirectSoundCaptureEnumerate",
  "DirectSoundEnumerate",
  };
for (int i = 0; i < eFuncMax; i++)      /* now load all necessary functions  */
  {
  CString sName = lpszFuncName[i];
  proc[i] = GetProcAddress(hDll, sName);
  if (!proc[i])
    {
    sName += "A";
    proc[i] = GetProcAddress(hDll, sName);
    }
  }

return true;                            /* pass back OK                      */
}

/*****************************************************************************/
/* UnloadDll : removes the loaded DLL and all entry points                   */
/*****************************************************************************/

bool CDSoundDll::UnloadDll()
{
if (!hDll)
  return true;

if (hDll)
  FreeLibrary(hDll);
hDll = NULL;

for (int i = 0; i < eFuncMax; i++)
  proc[i] = NULL;

return true;
}

/*****************************************************************************/
/* CaptureCreate : creates an input device                                   */
/*****************************************************************************/

HRESULT CDSoundDll::CaptureCreate(LPGUID lpGUID, LPDIRECTSOUNDCAPTURE *lplpDSC, LPUNKNOWN pUnkOuter)
{
if (!proc[eFuncCaptureCreate])
  return DSERR_INVALIDPARAM;
return ((HRESULT (WINAPI *)(LPGUID, LPDIRECTSOUNDCAPTURE *, LPUNKNOWN))
    proc[eFuncCaptureCreate])(lpGUID, lplpDSC, pUnkOuter);
}

/*****************************************************************************/
/* Create : creates an output device                                         */
/*****************************************************************************/

HRESULT CDSoundDll::Create(LPGUID lpGuid, LPDIRECTSOUND * ppDS, IUnknown FAR * pUnkOuter)
{
if (!proc[eFuncCreate])
  return DSERR_INVALIDPARAM;
return ((HRESULT (WINAPI *)(LPGUID, LPDIRECTSOUND *, IUnknown FAR *))
    proc[eFuncCreate])(lpGuid, ppDS, pUnkOuter);
}

/*****************************************************************************/
/* CaptureEnumerate : enumerates all capture devices if possible             */
/*****************************************************************************/

HRESULT CDSoundDll::CaptureEnumerate(LPDSENUMCALLBACK lpDSEnumCallback, LPVOID lpContext)
{
if (!proc[eFuncCaptureEnumerate])
  return DSERR_INVALIDPARAM;
return ((HRESULT (WINAPI *)(LPDSENUMCALLBACK,LPVOID))
    proc[eFuncCaptureEnumerate])(lpDSEnumCallback, lpContext);
}

/*****************************************************************************/
/* Enumerate : enumerates all output devices if possible                     */
/*****************************************************************************/

HRESULT CDSoundDll::Enumerate(LPDSENUMCALLBACK lpDSEnumCallback, LPVOID lpContext)
{
if (!proc[eFuncEnumerate])
  return DSERR_INVALIDPARAM;
return ((HRESULT (WINAPI *)(LPDSENUMCALLBACK,LPVOID))
    proc[eFuncEnumerate])(lpDSEnumCallback, lpContext);
}

/*===========================================================================*/
/* Global Data needed in this module                                         */
/*===========================================================================*/

static CDSoundDll dll;                  /* the one and only DirectSound DLL  */


/*===========================================================================*/
/* CDSoundDeviceList class members                                           */
/*===========================================================================*/

/*****************************************************************************/
/* CDSoundDeviceList : constructor                                           */
/*****************************************************************************/

CDSoundDeviceList::CDSoundDeviceList()
{
}

/*****************************************************************************/
/* ~CDSoundDeviceList : destructor                                           */
/*****************************************************************************/

CDSoundDeviceList::~CDSoundDeviceList()
{
for (int i = 0; i < Guids.GetSize(); i++)
  if (Guids[i])
    {
    LPGUID pGuid = (LPGUID)Guids[i];
    delete pGuid;
    }
}

/*****************************************************************************/
/* EnumDevices : output enumeration                                          */
/*****************************************************************************/

BOOL CALLBACK CDSoundDeviceList::EnumDevices
    (
    LPGUID lpGUID,
    LPCTSTR lpszDesc,
    LPCTSTR lpszDrvName,
    LPVOID lpContext
    )
{
CDSoundDeviceList *pList = (CDSoundDeviceList *)lpContext;
CString sOut;
LPGUID pcGuid = NULL;

if (lpGUID)
  {
  pcGuid = new GUID;
  if (!pcGuid)
    return FALSE;
  *pcGuid = *lpGUID;
  }
if (pList->Guids.Add(pcGuid) < 0)
  {
  if (pcGuid)
    delete pcGuid;
  return FALSE;
  }
if (pList->Add(lpszDesc) < 0)
  return FALSE;

if (pList->Names.Add(lpszDrvName))
  return FALSE;

return TRUE;
}

/*****************************************************************************/
/* GetName : retrieves the driver name for a device                          */
/*****************************************************************************/

CString CDSoundDeviceList::GetName(int nDevNum)
{
if ((nDevNum < 0) || (nDevNum >= Guids.GetSize()))
  return "";
return Names[nDevNum];
}

CString CDSoundDeviceList::GetName(LPCSTR lpszDesc)
{
for (int nDevNum = 0; nDevNum < GetSize(); nDevNum++)
  if (GetAt(nDevNum) == lpszDesc)
    return Names[nDevNum];
return "";
}

/*****************************************************************************/
/* GetGUID : returns the GUID for a device                                   */
/*****************************************************************************/

LPGUID CDSoundDeviceList::GetGUID(int nDevNum)
{
if ((nDevNum < 0) || (nDevNum >= Guids.GetSize()))
  return NULL;
return (LPGUID)Guids[nDevNum];
}

LPGUID CDSoundDeviceList::GetGUID(LPCSTR lpszDesc)
{
for (int nDevNum = 0; nDevNum < GetSize(); nDevNum++)
  if (GetAt(nDevNum) == lpszDesc)
    return (LPGUID)Guids[nDevNum];
return NULL;
}


/*===========================================================================*/
/* CDSoundInputList class members                                            */
/*===========================================================================*/

/*****************************************************************************/
/* CDSoundInputList : constructor                                            */
/*****************************************************************************/

CDSoundInputList::CDSoundInputList()
{
if (dll.CaptureEnumerate((LPDSENUMCALLBACK)EnumDevices, this) != DS_OK)
  RemoveAll(); 
}


/*===========================================================================*/
/* CDSoundOutputList class members                                           */
/*===========================================================================*/

/*****************************************************************************/
/* CDSoundOutputList : constructor                                           */
/*****************************************************************************/

CDSoundOutputList::CDSoundOutputList()
{
if (dll.Enumerate((LPDSENUMCALLBACK)EnumDevices, this) != DS_OK)
  RemoveAll(); 
}


/*===========================================================================*/
/* CDSoundDevice class members                                               */
/*===========================================================================*/

/*****************************************************************************/
/* CDSoundDevice : constructor                                               */
/*****************************************************************************/

CDSoundDevice::CDSoundDevice()
{
szDevName[0] = 0;                       /* reset device name                 */
hEvent = 0;                             /* no event handle allocated         */
pEvtThread = 0;                         /* no event thread started           */
bEvtCancel = FALSE;                     /* reset event thread cancel flag    */
bIsOpen = FALSE;                        /* reset opened flag                 */

SetupWaveformat(&wf);                   /* default to 44.1kHz,16bit,stereo   */
SetupSBDesc(&sbd, 0, 0, &wf);           /* no meaningful values yet          */
sbpn[ePosStart].dwOffset = 0;           /* fill predefined positions         */
sbpn[ePosStop].dwOffset = DSBPN_OFFSETSTOP;
}

/*****************************************************************************/
/* ~CDSoundDevice : destructor                                               */
/*****************************************************************************/

CDSoundDevice::~CDSoundDevice()
{
Close();
KillEvt();                              /* kill event thread & handle        */
}

/*****************************************************************************/
/* SetupWaveformat : initializes a wave format buffer                        */
/*****************************************************************************/

void CDSoundDevice::SetupWaveformat
    (
    LPWAVEFORMATEX pwf,
    DWORD dwSamplesPerSec,
    WORD wBitsPerSample,
    WORD nChannels
    )
{
pwf->wFormatTag = WAVE_FORMAT_PCM;
pwf->nChannels = nChannels;
pwf->nSamplesPerSec = dwSamplesPerSec;
pwf->nAvgBytesPerSec = dwSamplesPerSec * nChannels * wBitsPerSample / 8;
pwf->nBlockAlign = nChannels * wBitsPerSample / 8;
pwf->wBitsPerSample = wBitsPerSample;
pwf->cbSize = 0;
}

/*****************************************************************************/
/* SetupSBDesc : set up a secondary buffer description                       */
/*****************************************************************************/

void CDSoundDevice::SetupSBDesc
    (
    LPDSCBUFFERDESC pbd,                /* description to fill               */
    DWORD dwFlags,                      /* I/O specific flags                */
    DWORD dwSamples,                    /* buffer size in samples            */
    LPWAVEFORMATEX pwf                  /* buffer format                     */
    )
{
pbd->dwSize = sizeof(DSCBUFFERDESC);    /* set up size                       */
pbd->dwFlags = dwFlags;                 /* set up flags                      */
pbd->dwReserved = 0;
pbd->lpwfxFormat = (pwf) ? pwf : &wf;   /* use external / internal format    */
                                        /* calculate buffer size in bytes    */
DWORD dwCalcBuf = 2 * dwSamples;        /* we use a double-buffering scheme  */
dwCalcBuf *= pbd->lpwfxFormat->nBlockAlign;
pbd->dwBufferBytes = dwCalcBuf;
}

/*****************************************************************************/
/* Open : opens a specific device                                            */
/*****************************************************************************/

BOOL CDSoundDevice::Open
    (
    LPCSTR lpszDesc,
    DWORD dwFlags,
    DWORD dwSamples,                    /* #samples in one buffer            */
    LPWAVEFORMATEX pwfx
    )
{
LPGUID lpGuid = GetGuid(lpszDesc);      /* try to get a GUID for that        */
if (!lpGuid)                            /* if impossible                     */
  return FALSE;                         /* return error                      */
                                        /* otherwise setup buffer description*/
SetupSBDesc(&sbd, dwFlags, dwSamples, pwfx);
if (!Open(lpGuid))                      /* and try to open the device        */
  return FALSE;                         /* upon error return error           */
strcpy(szDevName, lpszDesc);            /* remember the name                 */
OnOpen();                               /* allow subclasses to react         */
return TRUE;                            /* and pass back OK                  */
}

BOOL CDSoundDevice::Open(LPGUID lpGuid)
{
bIsOpen = TRUE;                         /* remember this has been opened     */
return TRUE;                            /* pass back OK                      */
}

/*****************************************************************************/
/* ReadBuffer : reads one halfth of the double-buffer                        */
/*****************************************************************************/

BOOL CDSoundDevice::ReadBuffer(int nBuffer, LPVOID lpBuf, DWORD &rdwSamples)
{
if ((nBuffer < 0) || (nBuffer > 1) ||
    (!IsOpen()) || (!sbd.lpwfxFormat))
  return FALSE;
DWORD dwCopy = rdwSamples * sbd.lpwfxFormat->nBlockAlign;
if (dwCopy > sbpn[ePosMiddle].dwOffset)
  dwCopy = sbpn[ePosMiddle].dwOffset;
rdwSamples = dwCopy / sbd.lpwfxFormat->nBlockAlign;

LPVOID lpBuf1, lpBuf2;
DWORD dwBytes1, dwBytes2;
if (!Lock(sbpn[nBuffer].dwOffset,
          sbpn[ePosMiddle].dwOffset,
          &lpBuf1, &dwBytes1,
          &lpBuf2, &dwBytes2))
  return FALSE;
memcpy(lpBuf, lpBuf1, dwCopy);          /* CAN only be pointer 1             */
return Unlock(lpBuf1, dwBytes1, lpBuf2, dwBytes2);
}

/*****************************************************************************/
/* WriteBuffer : fills one halfth of the double-buffer                       */
/*****************************************************************************/

BOOL CDSoundDevice::WriteBuffer(int nBuffer, LPVOID lpBuf, DWORD &rdwSamples)
{
if ((nBuffer < 0) || (nBuffer > 1) ||
    (!IsOpen()) || (!sbd.lpwfxFormat))
  return FALSE;
DWORD dwCopy = rdwSamples * sbd.lpwfxFormat->nBlockAlign;
if (dwCopy > sbpn[ePosMiddle].dwOffset)
  dwCopy = sbpn[ePosMiddle].dwOffset;
rdwSamples = dwCopy / sbd.lpwfxFormat->nBlockAlign;

LPVOID lpBuf1, lpBuf2;
DWORD dwBytes1, dwBytes2;

if (!Lock(sbpn[nBuffer].dwOffset,
          sbpn[ePosMiddle].dwOffset,
          &lpBuf1, &dwBytes1,
          &lpBuf2, &dwBytes2))
  return FALSE;
memcpy(lpBuf1, lpBuf, dwCopy);          /* CAN only be pointer 1             */
return Unlock(lpBuf1, dwBytes1, lpBuf2, dwBytes2);
}

/*****************************************************************************/
/* Close : kill eventually running event thread                              */
/*****************************************************************************/

BOOL CDSoundDevice::Close()
{
Release();                              /* release allocated objects         */
KillEvt();                              /* kill event thread & handle        */
*szDevName = 0;                         /* reset device name                 */
bIsOpen = FALSE;                        /* remember we're closed             */

return TRUE;
}

/*****************************************************************************/
/* EvtThreadProc : event thread procedure                                    */
/*****************************************************************************/

UINT CDSoundDevice::EvtThreadProc(LPVOID pParam)
{
UINT result;
CDSoundDevice *pDev = (CDSoundDevice *)pParam;

while (!pDev->bEvtCancel)
  {
  result = WaitForSingleObject(pDev->hEvent, INFINITE);
  if ((result == WAIT_OBJECT_0) &&
      (!pDev->bEvtCancel))
    {
    if (pDev->IsStarted())              /* if driver is currently started    */
      {                                 /* find out where driver really is   */
                                        /* pass back the other buffer        */
      pDev->OnSwitch(1 - pDev->GetDriverBuf());
      }
    else                                /* otherwise                         */
      pDev->OnStop();                   /* this is a Stop event              */
    }
  else
    break;
  }
pDev->bEvtCancel = FALSE;
return 0;
}

/*****************************************************************************/
/* StartEvt : creates an event handle & thread                               */
/*****************************************************************************/

BOOL CDSoundDevice::StartEvt()
{
int i;

KillEvt();                              /* make sure none's there            */

                                        /* then create a new event handle    */
hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
if (!hEvent)                            /* upon error                        */
  return FALSE;                         /* return error                      */

                                        /* put it into notification handler  */
for (i = 0; i < (sizeof(sbpn)/sizeof(sbpn[0])); i++)
  sbpn[i].hEventNotify = hEvent;

                                        /* then create the thread            */
pEvtThread = AfxBeginThread(EvtThreadProc, this,
                            THREAD_PRIORITY_NORMAL, 0,
                            CREATE_SUSPENDED, NULL);   
if (!pEvtThread)                        /* upon error                        */
  {
  KillEvt();                            /* kill event handle                 */
  return FALSE;                         /* and return error                  */
  }

pEvtThread->m_bAutoDelete = TRUE;       /* let it go!                        */
pEvtThread->ResumeThread();

return TRUE;
}

/*****************************************************************************/
/* KillEvt : kills an eventually running event thread & event handle         */
/*****************************************************************************/

void CDSoundDevice::KillEvt()
{
if (hEvent)                             /* if event handle allocated         */
  {
  TRACE0("Killing event thread\n");
  if (pEvtThread)                       /* if thread still running           */
    {
    bEvtCancel = TRUE;                  /* set cancel flag                   */
    SetEvent(hEvent);                   /* tell thread to die                */
    while (bEvtCancel)                  /* wait until it does.               */
      {
      TRACE0("Waiting for thread to stop\n");
      Sleep(50);
      }
    pEvtThread = 0;                     /* and thread handle                 */
    }
  CloseHandle(hEvent);                  /* and close the event handle        */
  hEvent = 0;                           /* and reset it...                   */
  }
}

/*****************************************************************************/
/* BoostPriority : increases the thread's priority                           */
/*****************************************************************************/

BOOL CDSoundDevice::BoostPriority()
{
if (pEvtThread)
  {
  pEvtThread->SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
  return TRUE;
  }
return FALSE;
}

/*===========================================================================*/
/* CDSoundInDevice class members                                             */
/*===========================================================================*/

/*****************************************************************************/
/* CDSoundInDevice : constructor                                             */
/*****************************************************************************/

CDSoundInDevice::CDSoundInDevice()
{
lpDS = NULL;
lpDSB = NULL;
}

/*****************************************************************************/
/* ~CDSoundInDevice : destructor                                             */
/*****************************************************************************/

CDSoundInDevice::~CDSoundInDevice()
{
}

/*****************************************************************************/
/* Open : opens the input device                                             */
/*****************************************************************************/

BOOL CDSoundInDevice::Open(LPGUID lpGuid)
{
if (IsOpen())                           /* if already open                   */
  Close();                              /* close the opened device           */

                                        /* create DirectSoundCapture instance*/
HRESULT hr = dll.CaptureCreate(lpGuid,
                               &lpDS,
                               NULL);
if (FAILED(hr))                         /* if failed                         */
  return FALSE;

                                        /* create secondary buffer           */
hr = lpDS->CreateCaptureBuffer((LPCDSCBUFFERDESC)&sbd, &lpDSB, NULL);
if (FAILED(hr))                         /* upon error                        */
  {
  lpDSB = NULL;                         /* make sure pointer is correct      */
  Release();                            /* release device                    */
  return FALSE;                         /* and return error                  */
  }

sbpn[ePosMiddle].dwOffset =             /* set up notification positions     */
  sbd.dwBufferBytes >> 1;

#if 1
// got to find out whether this works... for now, just do as if it would...
                                        /* set up notifications              */
hr = lpDSB->QueryInterface(IID_IDirectSoundNotify, (LPVOID *)&lpDSN); 
if (FAILED(hr))                         /* upon error                        */
  {
  lpDSN = NULL;                         /* make sure the pointer is correct  */
  Release();                            /* release device                    */
  return FALSE;                         /* and return error                  */
  }
hr = lpDSN->SetNotificationPositions(ePosMax, sbpn);
if (FAILED(hr))                         /* if error                          */
  {
  Release();                            /* release device                    */
  return FALSE;                         /* and return with error             */
  }
#endif

return CDSoundDevice::Open(lpGuid);     /* let base class do its job         */
}

/*****************************************************************************/
/* IsStarted : returns whether the device is currently recording             */
/*****************************************************************************/

BOOL CDSoundInDevice::IsStarted()
{
if (!lpDSB)
  return FALSE;
DWORD dwStatus = 0;
lpDSB->GetStatus(&dwStatus);
return !!(dwStatus & DSCBSTATUS_CAPTURING);
}

/*****************************************************************************/
/* Start : start recording                                                   */
/*****************************************************************************/

BOOL CDSoundInDevice::Start(DWORD dwFlags)
{
if (!lpDSB)
  return FALSE;
if (dwFlags == (DWORD)-1)               /* if default flags                  */
  dwFlags = DSCBSTART_LOOPING;          /* start in loop mode                */
return SUCCEEDED(lpDSB->Start(dwFlags));
}

/*****************************************************************************/
/* Stop : stops recording                                                    */
/*****************************************************************************/

BOOL CDSoundInDevice::Stop()
{
if (!lpDSB)
  return FALSE;
return SUCCEEDED(lpDSB->Stop());
}

/*****************************************************************************/
/* Close : closes the current output device                                  */
/*****************************************************************************/

BOOL CDSoundInDevice::Close()
{
Stop();
return CDSoundDevice::Close();
}

/*****************************************************************************/
/* GetDriverPos : get driver's current position                              */
/*****************************************************************************/

DWORD CDSoundInDevice::GetDriverPos()
{
if (!lpDSB)
  return 0;
DWORD dwCapture, dwRead;
lpDSB->GetCurrentPosition(&dwCapture, &dwRead);
return dwCapture;
}

/*****************************************************************************/
/* GetBufferPos : get our own position                                       */
/*****************************************************************************/

DWORD CDSoundInDevice::GetBufferPos()
{
if (!lpDSB)
  return 0;
DWORD dwCapture, dwRead;
lpDSB->GetCurrentPosition(&dwCapture, &dwRead);
return dwRead;
}

/*****************************************************************************/
/* GetFormat : retrieves the currently configured format                     */
/*****************************************************************************/

BOOL CDSoundInDevice::GetFormat(LPWAVEFORMATEX lpWF)
{
if (!lpDSB)
  return FALSE;
return SUCCEEDED(lpDSB->GetFormat(lpWF, sizeof(WAVEFORMATEX), NULL));
}

/*****************************************************************************/
/* GetGuid : returns the GUID for a device descriptor                        */
/*****************************************************************************/

LPGUID CDSoundInDevice::GetGuid(LPCSTR lpszDesc)
{
CDSoundInputList list;
LPGUID lpGuid = list.GetGUID(lpszDesc);
if (!lpGuid)
  return NULL;
guid = *lpGuid;
return &guid;
}

/*****************************************************************************/
/* GetCaps : retrieves the device capabilities                               */
/*****************************************************************************/

BOOL CDSoundInDevice::GetCaps(LPDSCBCAPS lpDSCBCaps)
{
if (!lpDSB)
  return FALSE;
lpDSCBCaps->dwSize = sizeof(DSCBCAPS);
return !FAILED(lpDSB->GetCaps(lpDSCBCaps));
}

/*****************************************************************************/
/* Lock : locks a range of bytes                                             */
/*****************************************************************************/

BOOL CDSoundInDevice::Lock(DWORD dwCursor, DWORD dwBytes, LPVOID *lplpvPtr1, LPDWORD lpdwBytes1, LPVOID *lplpvPtr2, LPDWORD lpdwBytes2, DWORD dwFlags)
{
if (!lpDSB)
  return FALSE;
return SUCCEEDED(lpDSB->Lock(dwCursor, dwBytes, lplpvPtr1, lpdwBytes1, lplpvPtr2, lpdwBytes2, dwFlags));
}

/*****************************************************************************/
/* Unlock : remnoves current lock                                            */
/*****************************************************************************/

BOOL CDSoundInDevice::Unlock(LPVOID lpPtr1, DWORD dwBytes1, LPVOID lpPtr2, DWORD dwBytes2)
{
if (!lpDSB)
  return FALSE;
return SUCCEEDED(lpDSB->Unlock(lpPtr1, dwBytes1, lpPtr2, dwBytes2));
}

/*****************************************************************************/
/* Release : releases the allocated                                          */
/*****************************************************************************/

void CDSoundInDevice::Release()
{
if (lpDSN)
  lpDSN->Release();
lpDSN = NULL;
if (lpDSB)
  lpDSB->Release();
lpDSB = NULL;
if (lpDS)
  lpDS->Release();
lpDS = NULL;
}


/*===========================================================================*/
/* CDSoundOutDevice class members                                            */
/*===========================================================================*/

/*****************************************************************************/
/* CDSoundOutDevice : constructor                                            */
/*****************************************************************************/

CDSoundOutDevice::CDSoundOutDevice()
{
lpDS = NULL;
lpDSB = NULL;
lpDSN = NULL;
}

/*****************************************************************************/
/* ~CDSoundOutDevice : destructor                                            */
/*****************************************************************************/

CDSoundOutDevice::~CDSoundOutDevice()
{

}

/*****************************************************************************/
/* Open : opens the output device                                            */
/*****************************************************************************/

BOOL CDSoundOutDevice::Open(LPGUID lpGuid)
{
if (IsOpen())                           /* if already open                   */
  Close();                              /* close the opened device           */

HRESULT hr = dll.Create(lpGuid,         /* create DirectSound instance       */
                        &lpDS,
                        NULL);
if (FAILED(hr))                         /* upon error                        */
  {
  lpDS = NULL;                          /* reset DirectSound pointer         */
  return FALSE;                         /* and pass back error               */
  }
                                        /* set cooperative level to normal   */
hr = lpDS->SetCooperativeLevel(AfxGetMainWnd()->GetSafeHwnd(),
                               DSSCL_NORMAL);
if (FAILED(hr))                         /* upon error                        */
  {
  lpDSB = NULL;                         /* make sure pointer is correct      */
  Release();                            /* release device                    */
  return FALSE;                         /* and return error                  */
  }

                                        /* create secondary buffer           */
hr = lpDS->CreateSoundBuffer((LPCDSBUFFERDESC)&sbd, &lpDSB, NULL);
if (FAILED(hr))                         /* upon error                        */
  {
  lpDSB = NULL;                         /* make sure pointer is correct      */
  Release();                            /* release device                    */
  return FALSE;                         /* and return error                  */
  }

sbpn[ePosMiddle].dwOffset =             /* set up notification positions     */
  sbd.dwBufferBytes >> 1;

if (sbd.dwFlags & DSBCAPS_CTRLPOSITIONNOTIFY)
  {
#if 0
  // this simply doesn't work. No idea why.
  // Ah well, set up a timer for that...
                                        /* set up notifications              */
  hr = lpDSB->QueryInterface(IID_IDirectSoundNotify, (LPVOID *)&lpDSN); 
  if (FAILED(hr))                       /* upon error                        */
    {
    lpDSN = NULL;                       /* make sure the pointer is correct  */
    Release();                          /* release device                    */
    return FALSE;                       /* and return error                  */
    }

  hr = lpDSN->SetNotificationPositions(ePosMax, sbpn);
  if (FAILED(hr))
    {
    Release();
    return FALSE;
    }
#else
  // set up timer-based double-buffer handling?
  // nah, we let input handle that
#endif
  }

return CDSoundDevice::Open(lpGuid);     /* let base class do its job         */
}

/*****************************************************************************/
/* GetDriverPos : get driver's current position                              */
/*****************************************************************************/

DWORD CDSoundOutDevice::GetDriverPos()
{
if (!lpDSB)
  return 0;
DWORD dwPlay, dwWrite;
lpDSB->GetCurrentPosition(&dwPlay, &dwWrite);
TRACE2("DSoundOutDevice::GetCurrentPosition(%d,%d)\n", dwPlay, dwWrite);
return dwPlay;
}

/*****************************************************************************/
/* SetDriverPos : sets the driver's current position                         */
/*****************************************************************************/

BOOL CDSoundOutDevice::SetDriverPos(DWORD dwPos)
{
if (!lpDSB)
  return FALSE;
return SUCCEEDED(lpDSB->SetCurrentPosition(dwPos));
}

/*****************************************************************************/
/* GetBufferPos : get our own position                                       */
/*****************************************************************************/

DWORD CDSoundOutDevice::GetBufferPos()
{
if (!lpDSB)
  return 0;
DWORD dwPlay, dwWrite;
lpDSB->GetCurrentPosition(&dwPlay, &dwWrite);
TRACE2("DSoundOutDevice::GetCurrentPosition(%d,%d)\n", dwPlay, dwWrite);
return dwWrite;
}

/*****************************************************************************/
/* IsStarted : returns whether the device is currently playing               */
/*****************************************************************************/

BOOL CDSoundOutDevice::IsStarted()
{
if (!lpDSB)
  return FALSE;
DWORD dwStatus = 0;
lpDSB->GetStatus(&dwStatus);
return !!(dwStatus & DSBSTATUS_PLAYING);
}

/*****************************************************************************/
/* Start : start playing                                                     */
/*****************************************************************************/

BOOL CDSoundOutDevice::Start(DWORD dwFlags)
{
if (!lpDSB)
  return FALSE;
if (dwFlags == (DWORD)-1)               /* if default flags                  */
  dwFlags = DSBPLAY_LOOPING;            /* start in loop mode                */
return SUCCEEDED(lpDSB->Play(0, 0, dwFlags));
}

/*****************************************************************************/
/* Stop : stops playing                                                      */
/*****************************************************************************/

BOOL CDSoundOutDevice::Stop()
{
if (!lpDSB)
  return FALSE;
// doc is not clear on the effect if buffer doesn't play, so inhibit that
if (!IsStarted())
  return TRUE;
return SUCCEEDED(lpDSB->Stop());
}

/*****************************************************************************/
/* GetFormat : retrieves the currently configured format                     */
/*****************************************************************************/

BOOL CDSoundOutDevice::GetFormat(LPWAVEFORMATEX lpWF)
{
if (!lpDSB)
  return FALSE;
return SUCCEEDED(lpDSB->GetFormat(lpWF, sizeof(WAVEFORMATEX), NULL));
}

/*****************************************************************************/
/* Close : closes the current output device                                  */
/*****************************************************************************/

BOOL CDSoundOutDevice::Close()
{
Stop();
return CDSoundDevice::Close();          /* let base class handle the rest    */
}

/*****************************************************************************/
/* GetGuid : returns the GUID for a device descriptor                        */
/*****************************************************************************/

LPGUID CDSoundOutDevice::GetGuid(LPCSTR lpszDesc)
{
CDSoundOutputList list;
LPGUID lpGuid = list.GetGUID(lpszDesc);
if (!lpGuid)
  return NULL;
guid = *lpGuid;
return &guid;
}

/*****************************************************************************/
/* GetCaps : retrieves the device capabilities                               */
/*****************************************************************************/

BOOL CDSoundOutDevice::GetCaps(LPDSBCAPS lpDSBufferCaps)
{
if (!lpDSB)
  return FALSE;
lpDSBufferCaps->dwSize = sizeof(DSBCAPS);
return !FAILED(lpDSB->GetCaps(lpDSBufferCaps));
}

/*****************************************************************************/
/* GetVolume : gets the mixer volume level                                   */
/*****************************************************************************/

BOOL CDSoundOutDevice::GetVolume(LPLONG lpVolume)
{
if (!lpDSB)
  return FALSE;
return SUCCEEDED(lpDSB->GetVolume(lpVolume));
}

/*****************************************************************************/
/* SetVolume : sets the mixer volume level                                   */
/*****************************************************************************/

BOOL CDSoundOutDevice::SetVolume(LONG lVolume)
{
if (!lpDSB)
  return FALSE;
return SUCCEEDED(lpDSB->SetVolume(lVolume));
}

/*****************************************************************************/
/* GetPan : gets the mixer Panorama position                                 */
/*****************************************************************************/

BOOL CDSoundOutDevice::GetPan(LPLONG lpPan)
{
if (!lpDSB)
  return FALSE;
return SUCCEEDED(lpDSB->GetPan(lpPan));
}

/*****************************************************************************/
/* SetPan : sets the mixer Panorama position                                 */
/*****************************************************************************/

BOOL CDSoundOutDevice::SetPan(LONG lPan)
{
if (!lpDSB)
  return FALSE;
return SUCCEEDED(lpDSB->SetPan(lPan));
}

/*****************************************************************************/
/* Lock : locks a range of bytes                                             */
/*****************************************************************************/

BOOL CDSoundOutDevice::Lock(DWORD dwCursor, DWORD dwBytes, LPVOID *lplpvPtr1, LPDWORD lpdwBytes1, LPVOID *lplpvPtr2, LPDWORD lpdwBytes2, DWORD dwFlags)
{
if (!lpDSB)
  return FALSE;
return SUCCEEDED(lpDSB->Lock(dwCursor, dwBytes, lplpvPtr1, lpdwBytes1, lplpvPtr2, lpdwBytes2, dwFlags));
}

/*****************************************************************************/
/* Unlock : removes current lock                                             */
/*****************************************************************************/

BOOL CDSoundOutDevice::Unlock(LPVOID lpPtr1, DWORD dwBytes1, LPVOID lpPtr2, DWORD dwBytes2)
{
if (!lpDSB)
  return FALSE;
return SUCCEEDED(lpDSB->Unlock(lpPtr1, dwBytes1, lpPtr2, dwBytes2));
}

/*****************************************************************************/
/* Release : releases the allocated                                          */
/*****************************************************************************/

void CDSoundOutDevice::Release()
{
if (lpDSN)
  lpDSN->Release();
lpDSN = NULL;
if (lpDSB)
  lpDSB->Release();
lpDSB = NULL;
if (lpDS)
  lpDS->Release();
lpDS = NULL;
}
