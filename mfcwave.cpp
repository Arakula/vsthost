/*****************************************************************************/
/* MFCWAVE.CPP : class implementation for wave base classes                  */
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

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/

#include "stdafx.h"
#include "mfcwave.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*****************************************************************************/
/* Global Data                                                               */
/*****************************************************************************/

static char szWMapName[] = WMAP_NAME;   /* Wave Mapper device name           */

/*===========================================================================*/
/* CWaveBuffer class members                                                 */
/*===========================================================================*/

/*****************************************************************************/
/* CWaveBuffer : constructor (various styles)                                */
/*****************************************************************************/

CWaveBuffer::CWaveBuffer(DWORD dwLen)
{
Init();
Set(dwLen);
}

CWaveBuffer::CWaveBuffer(LPWAVEHDR lpHdr)
{
Init();
Set(lpHdr);
}

CWaveBuffer::CWaveBuffer(LPVOID lpBuf, DWORD dwLen)
{
Init();
Set(lpBuf, dwLen);
}

/*****************************************************************************/
/* Set : sets Wave buffer in various ways                                    */
/*****************************************************************************/

void CWaveBuffer::Set(DWORD dwLen)
{
Set((LPVOID)NULL, dwLen);
}

void CWaveBuffer::Set(LPWAVEHDR lpHdr)
{
Empty();
this->lpHdr = lpHdr;
}

void CWaveBuffer::Set(LPVOID lpBuf, DWORD dwLen)
{
Empty();

hGlPtr = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE,
    sizeof(WAVEHDR) + dwLen);
if (!hGlPtr)
  return;
lpHdr = (LPWAVEHDR)GlobalLock(hGlPtr);
if (lpHdr)
  {
  bAlloc = TRUE;
  memset((LPVOID)lpHdr, 0, sizeof(WAVEHDR));
  lpHdr->lpData = (LPSTR)(lpHdr+1);
  lpHdr->dwBufferLength = dwLen;
  lpHdr->dwBytesRecorded = (lpBuf) ? dwLen : 0;
  if (lpBuf)
    memcpy(lpHdr->lpData, lpBuf, dwLen);
  else
    memset(lpHdr->lpData, 0, dwLen);
  }
}

/*****************************************************************************/
/* LPWAVEHDR : returns an LPWAVEHDR                                          */
/*****************************************************************************/

CWaveBuffer::operator LPWAVEHDR()
{
return lpHdr;
}

/*****************************************************************************/
/* Length : returns the buffer's data length                                 */
/*****************************************************************************/

DWORD CWaveBuffer::Length()
{
if (!lpHdr)                             /* if empty                          */
  return 0;                             /* return that fact                  */
if (lpHdr->dwBytesRecorded)             /* if something recorded in there    */
  return lpHdr->dwBytesRecorded;        /* return that                       */
else                                    /* otherwise                         */
  return lpHdr->dwBufferLength;         /* return allocated buffer length    */
}

/*****************************************************************************/
/* Init : initialize memory                                                  */
/*****************************************************************************/

void CWaveBuffer::Init()
{
bAlloc = FALSE;
lpHdr = NULL;
hGlPtr = NULL;
dwStamp = 0;
}

/*****************************************************************************/
/* Empty : empy memory                                                       */
/*****************************************************************************/

void CWaveBuffer::Empty()
{
if (bAlloc)                             /* if self-allocated                 */
  {
  GlobalUnlock(hGlPtr);                 /* free buffer                       */
  GlobalFree(hGlPtr);
  }
Init();                                 /* then re-initialize                */
}

/*===========================================================================*/
/* CWaveDevice class members                                                 */
/*===========================================================================*/

/*****************************************************************************/
/* CWaveDevice : constructor                                                 */
/*****************************************************************************/

CWaveDevice::CWaveDevice()
{
szDevName[0] = 0;                       /* reset device name                 */
hEvent = 0;                             /* no event handle allocated         */
pEvtThread = 0;                         /* no event thread started           */
bEvtCancel = FALSE;                     /* reset event thread cancel flag    */
bIsOpen = FALSE;                        /* reset opened flag                 */

SetupWaveformat(&wf);                   /* default to 44.1kHz,16bit,stereo   */
}

/*****************************************************************************/
/* SetupWaveformat : initializes a wave format buffer                        */
/*****************************************************************************/

void CWaveDevice::SetupWaveformat
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
/* ~CWaveDevice : destructor                                                 */
/*****************************************************************************/

CWaveDevice::~CWaveDevice()
{
KillEvt();                              /* kill event thread & handle        */
}

/*****************************************************************************/
/* CWaveDevice message map                                                   */
/*****************************************************************************/

BEGIN_MESSAGE_MAP(CWaveDevice, CWnd)
	//{{AFX_MSG_MAP(CWaveDevice)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*****************************************************************************/
/* Create : creates necessary notification window                            */
/*****************************************************************************/

BOOL CWaveDevice::Create()
{
if (!CreateEx(0,
    AfxRegisterWndClass(0),
    "Wave Wnd",
    WS_POPUP,
    0,
    0,
    0,
    0,
    NULL,
    NULL))
  {
  TRACE("Failed to create wave notification window");
  return FALSE;
  }
return TRUE;
}

/*****************************************************************************/
/* Open :                                                                    */
/*****************************************************************************/

BOOL CWaveDevice::Open
    (
    int iID,
    LPWAVEFORMATEX pwfx,
    DWORD dwCallback,
    DWORD dwCallbackInstance,
    DWORD fdwOpen
    )
{
if ((fdwOpen & CALLBACK_TYPEMASK) ==    /* if window callback                */
    CALLBACK_WINDOW)
  AssureWnd();                          /* make sure window is there         */
bIsOpen = TRUE;
return TRUE;
}

BOOL CWaveDevice::Open
    (
    LPCSTR szName,
    LPWAVEFORMATEX pwfx,
    DWORD dwCallback,
    DWORD dwCallbackInstance,
    DWORD fdwOpen
    )
{
strcpy(szDevName, szName);
return TRUE;
}

/*****************************************************************************/
/* Close : closes the device                                                 */
/*****************************************************************************/

BOOL CWaveDevice::Close ( )
{
KillEvt();                              /* kill event thread & handle        */
*szDevName = 0;
bIsOpen = FALSE;
return TRUE;
}

/*****************************************************************************/
/* GetSample : retrieves a sample from a buffer using open format            */
/*****************************************************************************/

short CWaveDevice::GetSample(CWaveBuffer &buf, int nSample, int nChannel)
{
if (!IsOpen())                          /* if not opened                     */
  return 0;                             /* no sample, sonny...               */

if (Channels() < 2)                     /* if mono                           */
  nChannel = 0;                         /* reset requested channel #         */

if ((nSample < 0) ||                    /* sample must be in range!          */
    (nSample * BlockAlignment() > (int)buf.Length()))
  return 0;
                                        /* position on sample                */
LPSTR lpPos = ((LPWAVEHDR)buf)->lpData +
              (nSample * BlockAlignment()) +
              (nChannel * Bits() / 8);
switch (Bits())                         /* process according to sample bits  */
  {
  case 8 :                              /* 8 bits ?                          */
    return ((short)(*(BYTE *)lpPos) * 256) - (short)32768;
  case 16 :                             /* 16 bits ?                         */
    return *((short *)lpPos);
  default :                             /* anything else ?                   */
    return 0;                           /* we don't deal with pro values here*/
  }
}

/*****************************************************************************/
/* SetSample : puts a sample into a buffer using open format                 */
/*****************************************************************************/

void CWaveDevice::SetSample
    (
    CWaveBuffer &buf,
    int nSample,
    int nChannel,
    short sValue
    )
{
if ((!IsOpen()) ||                      /* if not opened                     */
    (nChannel >= Channels()) ||         /* or beyond possible channels       */
    (nSample < 0) ||                    /* sample must be in range!          */
    (nSample * BlockAlignment() > (int)buf.Length()))
  return;                               /* no sample, sonny...               */

                                        /* position on sample                */
LPSTR lpPos = ((LPWAVEHDR)buf)->lpData +
              (nSample * BlockAlignment()) +
              (nChannel * Bits() / 8);
switch (Bits())                         /* process according to sample bits  */
  {
  case 8 :                              /* 8 bits ?                          */
    *((BYTE *)lpPos) =
      (BYTE)((sValue / 256) + 128);
    break;
  case 16 :                             /* 16 bits ?                         */
    *((short *)lpPos) = sValue;
    break;
  default :                             /* anything else ?                   */
    return;                             /* we don't deal with pro values here*/
  }
}

/*****************************************************************************/
/* EvtThreadProc : event thread procedure                                    */
/*****************************************************************************/

UINT CWaveDevice::EvtThreadProc(LPVOID pParam)
{
UINT result;
CWaveDevice *pDev = (CWaveDevice *)pParam;

while (!pDev->bEvtCancel)
  {
  result = WaitForSingleObject(pDev->hEvent,INFINITE);
  if ((result == WAIT_OBJECT_0) &&
      (!pDev->bEvtCancel))
    pDev->OnEvent();
  else
    break;
  }
pDev->bEvtCancel = FALSE;
return 0;
}

/*****************************************************************************/
/* StartEvt : creates an event handle & thread                               */
/*****************************************************************************/

BOOL CWaveDevice::StartEvt()
{
KillEvt();                              /* make sure none's there            */

                                        /* then create a new event handle    */
hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
if (!hEvent)                            /* upon error                        */
  return FALSE;                         /* return error                      */

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
/* KillEvt : kills an eventually running event thread or window              */
/*****************************************************************************/

void CWaveDevice::KillEvt()
{
if (hEvent)                             /* if event handle allocated         */
  {
  TRACE0("Killing event thread\n");
  if (pEvtThread)                       /* if thread still running           */
    {
    bEvtCancel = TRUE;                  /* set cancel flag                   */
    SetEvent(hEvent);                   /* tell thread to die                */
    int nLoop = 0;
    while (bEvtCancel)                  /* wait until it does.               */
      {
      TRACE0("Waiting for thread to stop\n");
      Sleep(50);
      if (++nLoop > 40)                 /* if 2 seconds have passed,         */
        {
        TRACE0("Timed out waiting for thread to stop!\n");
        break;
        }
      }
    pEvtThread = 0;                     /* and thread handle                 */
    }
  CloseHandle(hEvent);                  /* and close the event handle        */
  hEvent = 0;                           /* and reset it...                   */
  }

if (m_hWnd)                             /* if window handle there,           */
  DestroyWindow();                      /* destroy the window                */
}

/*****************************************************************************/
/* BoostPriority : increases the thread's priority                           */
/*****************************************************************************/

BOOL CWaveDevice::BoostPriority()
{
if (pEvtThread)
  {
  pEvtThread->SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
  return TRUE;
  }
return FALSE;
}

/*===========================================================================*/
/* Class CWaveInDeviceList                                                   */
/*===========================================================================*/

/*****************************************************************************/
/* CWaveInDeviceList : constructor                                           */
/*****************************************************************************/

CWaveInDeviceList::CWaveInDeviceList ( )
{
WORD wNumDevices;
int i;
WAVEINCAPS wcap;

wNumDevices = waveInGetNumDevs();       /* get # Wave input devices          */
if (wNumDevices)
  {
  for (i=0; i < (int) wNumDevices; i++)
    {
    waveInGetDevCaps(i, &wcap, sizeof(wcap));
    Add(wcap.szPname);
    }
  }
}

/*===========================================================================*/
/* Class CWaveInDevice                                                       */
/*===========================================================================*/

/*****************************************************************************/
/* CWaveInDevice message map                                                 */
/*****************************************************************************/

BEGIN_MESSAGE_MAP(CWaveInDevice, CWaveDevice)
    //{{AFX_MSG_MAP(CWaveInDevice)
    ON_MESSAGE(MM_WIM_OPEN, OnWimOpen)
    ON_MESSAGE(MM_WIM_DATA, OnWimData)
    ON_MESSAGE(MM_WIM_CLOSE, OnWimClose)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*****************************************************************************/
/* CWaveInDevice : constructor                                               */
/*****************************************************************************/

CWaveInDevice::CWaveInDevice ( )
{
hDev = NULL;                            /* reset device handle               */

hGlobal = NULL;                         /* Wave In buffers                   */
pGlobal = NULL;
lpGlHdr = NULL;
nAllocated = 0;

nAllocCount = WAVEINHDRS;
nAllocSize = WAVEINBUFSIZE;

bRecording = FALSE;                     /* reset recording flag              */
dwStamp = 0;                            /* set internal time stamp           */
bClosing = FALSE;
}

/*****************************************************************************/
/* ~CWaveInDevice : destructor                                               */
/*****************************************************************************/

CWaveInDevice::~CWaveInDevice ( )
{
if (IsOpen())
  Close();
if (lpGlHdr)
  delete[] lpGlHdr;
}

/*****************************************************************************/
/* WaveInProc : callback procedure                                           */
/*****************************************************************************/

static void CALLBACK WaveInProc
    (
    HWAVEIN hMidiIn,
    UINT wMsg,
    DWORD dwInstance,
    DWORD dwParam1,
    DWORD dwParam2
    )
{
CWaveInDevice * pInDev = (CWaveInDevice *)dwInstance;
switch (wMsg)
  {
  case WIM_CLOSE : 
    pInDev->OnWimClose();
    break;
  case WIM_OPEN : 
    pInDev->OnWimOpen();
    break;
  case WIM_DATA : 
#if 0
    pInDev->OnWimData((LPWAVEHDR)dwParam1);
#else
                                        /* postpone processing since it could*/
                                        /* lead to a deadlock                */
    pInDev->PostMessage(MM_WIM_DATA, 0, dwParam1);
#endif
    break;
  }
}

/*****************************************************************************/
/* Open : opens a Wave In Device with its ID                                 */
/*****************************************************************************/

BOOL CWaveInDevice::Open
    (
    int iID,
    LPWAVEFORMATEX pwfx,
    DWORD dwCallback,
    DWORD dwCallbackInstance,
    DWORD fdwOpen
    )
{
if (IsOpen())                           /* if already open                   */
  Close();                              /* close the opened device           */

if ((fdwOpen & CALLBACK_TYPEMASK) ==    /* if window callback                */
    CALLBACK_WINDOW)
  AssureWnd();                          /* make sure window is there         */

waveInGetDevCaps(iID,                   /* get device capabilities           */
    &wCap, sizeof(wCap));
if (!pwfx)                              /* if no wave format defined         */
  {
  SetupWaveformat(&wf);                 /* default to 16bit stereo 44.1kHz   */
  pwfx = &wf;
  }

if ((fdwOpen & CALLBACK_TYPEMASK) ==    /* if function callback              */
    CALLBACK_FUNCTION)
  {
  if (dwCallback == WAVEOPENDEFAULT)    /* if default value                  */
    dwCallback = (DWORD)WaveInProc;     /* set LP1 to procedure              */
  if (dwCallbackInstance ==             /* if default value                  */
      WAVEOPENDEFAULT)
    dwCallbackInstance = (DWORD)this;   /* set LP2 to object                 */
  }
else if ((fdwOpen&CALLBACK_TYPEMASK)==  /* if event callback                 */
    CALLBACK_EVENT)
  {
  if (dwCallback == WAVEOPENDEFAULT)    /* if default value                  */
    {
    if (!StartEvt())                    /* start event thread                */
      return FALSE;                     /* upon error terminate              */
    dwCallback = (DWORD)hEvent;         /* set LP1 to event handle           */
    }
  if (dwCallbackInstance ==             /* if default value                  */
      WAVEOPENDEFAULT)
    dwCallbackInstance =                /* set LP2 to instance               */
        (UINT)AfxGetInstanceHandle();
  }
                                        /* if thread callback                */
else if ((fdwOpen & CALLBACK_TYPEMASK) ==    
    CALLBACK_THREAD)
  {
  if (dwCallback == WAVEOPENDEFAULT)    /* if default value                  */
    dwCallback = (DWORD)                /* set LP1 to current thread         */
        GetCurrentThread();
  if (dwCallbackInstance ==             /* if default value                  */
      WAVEOPENDEFAULT)
    dwCallbackInstance =                /* set LP2 to instance               */
        (UINT)AfxGetInstanceHandle();
  }
else
  {
  if (dwCallback == WAVEOPENDEFAULT)    /* if default value                  */
    dwCallback = (UINT)m_hWnd;          /* set LP1 to window                 */
  if (dwCallbackInstance ==             /* if default value                  */
      WAVEOPENDEFAULT)
    dwCallbackInstance =                /* set LP2 to instance               */
        (UINT)AfxGetInstanceHandle();
  }

WORD wRtn = waveInOpen(&hDev,           /* open the device                   */
    iID,                                /* with the passed ID                */
    pwfx, dwCallback, dwCallbackInstance, fdwOpen);
if (wRtn)                               /* if open error                     */
  {
#if defined(_DEBUG) || defined(_DEBUGFILE)
  char szTxt[256];
  waveInGetErrorText(wRtn, szTxt, sizeof(szTxt));
  TRACE2("CWaveInDevice::Open(%d): %s\n", wRtn, szTxt);
#endif
  hDev = NULL;                          /* make sure that handle is reset    */
  return FALSE;
  }

if ((!AllocBuf(nAllocCount,             /* allocate Wave Input buffers       */
               nAllocSize * pwfx->nBlockAlign))
#if 0
    || (!PrepareBuf())                  /* and prepare them for usage        */
#endif
    )
  {
  Close();                              /* upon error close device           */
  return FALSE;                         /* and return error                  */
  }
                                        /* call base class                   */
CWaveDevice::Open(iID, pwfx, dwCallback, dwCallbackInstance, fdwOpen);

if (pwfx != &wf)                        /* remember wave format called with  */
  wf = *pwfx;

#if 0

// if you want to have this computing-intensive task running all the time,
// use this method
return Start();                         /* start recording                   */

#else

// if you want to start/stop recording manually to save processing power
// for more important tasks when not recording, use this method

return TRUE;

#endif
}

/*****************************************************************************/
/* Open : opens a device with its name                                       */
/*****************************************************************************/

BOOL CWaveInDevice::Open
    (
    LPCSTR szName,
    LPWAVEFORMATEX pwfx,
    DWORD dwCallback,
    DWORD dwCallbackInstance,
    DWORD fdwOpen
    )
{
WORD wNumDevices;
int i;
WORD wRtn;
WAVEINCAPS wcap;

if (IsOpen())                           /* if already open                   */
  Close();                              /* close the opened device           */

if (!szName)                            /* if no name passed                 */
  szName = szWMapName;                  /* assume wave mapper                */

wNumDevices = waveInGetNumDevs();       /* get # Wave input devices          */
if (wNumDevices)
  {
  for (i=-1; i < (int) wNumDevices; i++)
    {
    if (i == -1)
      lstrcpy(wcap.szPname, szWMapName);
    else
      wRtn = waveInGetDevCaps(i, &wcap, sizeof(wcap));
    if (!lstrcmp(szName, wcap.szPname))
      {
      if (!Open(i, pwfx, dwCallback, dwCallbackInstance, fdwOpen))
        return(FALSE);                  /* error - return NOT OK             */

      return CWaveDevice::Open(szName,  /* opened - return OK                */
          pwfx, dwCallback, dwCallbackInstance, fdwOpen);
      }
    }
  }
return(FALSE);                          /* return NOT OK                     */
}

/*****************************************************************************/
/* Start : start recording from device                                       */
/*****************************************************************************/

BOOL CWaveInDevice::Start ( )
{
if (!IsOpen() || IsRecording())         /* if no device open or recording    */
  return FALSE;                         /* return error                      */

if ((!PrepareBuf()) ||                  /* start recording from device       */
    (waveInStart(hDev)))
  return FALSE;                         /* upon error return error           */
dwStamp = timeGetTime();                /* set internal time stamp           */
bRecording = TRUE;                      /* set recording flag                */
return TRUE;
}

/*****************************************************************************/
/* Stop : stop recording from device                                         */
/*****************************************************************************/

BOOL CWaveInDevice::Stop ( )
{
#if 1
if (!IsOpen() || !IsRecording())        /* if no device open or recording    */
  return FALSE;                         /* return error                      */
#else
if (!IsOpen())                          /* if no device open                 */
  return FALSE;                         /* return error                      */

if (!IsRecording())                     /* if not recording                  */
  {
  if (PreparedBuffers())                /* but buffers prepared              */
    return UnprepareBuf();              /* unprepare buffers                 */
  return FALSE;                         /* otherwise just return error       */
  }
#endif

bRecording = FALSE;                     /* reset recording flag              */
Sleep(10);                              /* allow others to find that out     */
waveInStop(hDev);                       /* stop recording                    */
Sleep(10);                              /* allow others to find that out     */
dwStamp = 0;                            /* set internal time stamp           */
return TRUE;
}

/*****************************************************************************/
/* PrepareHeader : prepares a header for use in mmsystem                     */
/*****************************************************************************/

BOOL CWaveInDevice::PrepareHeader(LPWAVEHDR lpHdr)
{
if (!hDev)
  return FALSE;
return !waveInPrepareHeader(hDev, lpHdr, sizeof(WAVEHDR));
}

/*****************************************************************************/
/* UnprepareHeader : unprepares a used header                                */
/*****************************************************************************/

BOOL CWaveInDevice::UnprepareHeader(LPWAVEHDR lpHdr)
{
if (!hDev)
  return FALSE;
return !waveInUnprepareHeader(hDev, lpHdr, sizeof(WAVEHDR));
}

/*****************************************************************************/
/* Reset : resets the attached Wave In device                                */
/*****************************************************************************/

BOOL CWaveInDevice::Reset ( )
{
if (!IsOpen())                          /* if no device open                 */
  return FALSE;                         /* return error                      */
Stop();                                 /* make sure recording is stopped    */
waveInReset(hDev);                      /* reset the attached device         */
Sleep(10);                              /* allow others to find out          */
int nSleeps = 0;                        /* simple fuse...                    */
while (PreparedBuffers())               /* wait until all buffers unprepared */
  {
  TRACE1("CWaveInDevice::Reset(): waiting for %d buffers to be unprepared\n",
         PreparedBuffers());
  Sleep(50);
  if (++nSleeps > 20)                   /* wait up to a second               */
    {
    TRACE1("CWaveInDevice::Reset(): ABORTED (%d buffers to be unprepared)\n",
           PreparedBuffers());
    return FALSE;                       /* return with error                 */
    }
  }
return TRUE;                            /* and pass back OK                  */
}

/*****************************************************************************/
/* Close : closes the opened Wave In Device                                  */
/*****************************************************************************/

BOOL CWaveInDevice::Close ( )
{
if (!IsOpen())                          /* if no device open                 */
  return FALSE;                         /* return error                      */
bClosing = TRUE;                        /* set flag that we're closing now   */
Stop();                                 /* stop recording from device        */
Reset();                                /* reset the attached device         */
UnprepareBuf();                         /* unprepare allocated data          */

waveInClose(hDev);                      /* close the attached device         */
hDev = NULL;                            /* and reset the handle              */
bClosing = FALSE;
return CWaveDevice::Close();            /* and pass back OK                  */
}

/*****************************************************************************/
/* GetPosition : returns the current Wave In position                        */
/*****************************************************************************/

BOOL CWaveInDevice::GetPosition
    (
    LPMMTIME pmmt,
    UINT cbmmt
    )
{
return !waveInGetPosition(hDev, pmmt, cbmmt);
}

/*****************************************************************************/
/* Message : sends a special message to the device                           */
/*****************************************************************************/

DWORD CWaveInDevice::Message
    (
    UINT msg,
    DWORD dw1,
    DWORD dw2
    )
{
DWORD rc = 0;

if (hDev)
  rc = waveInMessage(hDev, msg, dw1, dw2);
return rc;
}

/*****************************************************************************/
/* AddHeader : adds a Wave Input buffer to the device                        */
/*****************************************************************************/

BOOL CWaveInDevice::AddHeader
    (
    LPWAVEHDR lpHdr
    )
{
if (lpHdr->dwFlags & WHDR_PREPARED)
#if 1
  return TRUE;
#else
  UnprepareHeader(lpHdr);
#endif

lpHdr->dwFlags = 0;
//Hdr->dwBufferLength = wf.nAvgBytesPerSec / (2 * nAllocated);
lpHdr->dwBytesRecorded = 0;

if (!PrepareHeader(lpHdr))
  return FALSE;
if (waveInAddBuffer(hDev, lpHdr, sizeof(WAVEHDR)))
  return FALSE;
return TRUE;
}

/*****************************************************************************/
/* CheckHeader : checks the passed Wave header for validity                  */
/*****************************************************************************/

BOOL CWaveInDevice::CheckHeader
    (
    LPWAVEHDR lpHdr
    )
{
int i;

if ((IsBadReadPtr(lpHdr, sizeof(WAVEHDR))) ||
    (!lpGlHdr))
  goto Ouch;
for (i = 0; i < nAllocated; i++)        /* if it's one we allocated          */
  if (lpHdr == lpGlHdr[i])
    return TRUE;                        /* pass back OK                      */
Ouch:
TRACE0("CWaveInDevice::CheckHeader: invalid header encountered!\n");
return FALSE;
}

/*****************************************************************************/
/* AllocBuf : allocates Wave In buffers                                      */
/*****************************************************************************/

BOOL CWaveInDevice::AllocBuf (int nCount, int nSize)
{
int i;                                  /* loop counter                      */

FreeBuf();                              /* free eventually allocated buffers */

hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE,
    nCount * (sizeof(WAVEHDR) + nSize));
if (hGlobal == NULL)
  return FALSE;

pGlobal = (char *)GlobalLock(hGlobal);  /* lock allocated memory             */
if (pGlobal == NULL)
  {
  GlobalFree(hGlobal);
  hGlobal = NULL;
  return FALSE;
  }

lpGlHdr = new LPWAVEHDR[nCount];
if (!lpGlHdr)
  return FALSE;
for (i = 0; i < nCount; i++)            /* now generate the wave buffers     */
  {
  lpGlHdr[i] = (LPWAVEHDR)
      (pGlobal + i * (sizeof(WAVEHDR) + nSize));
  memset(lpGlHdr[i], 0, sizeof(WAVEHDR) + nSize);
  lpGlHdr[i]->dwBufferLength = nSize;
  lpGlHdr[i]->lpData = (char *)(lpGlHdr[i]+1);
  }
nAllocated = nCount;

return TRUE;
}

/*****************************************************************************/
/* FreeBuf : frees allocated Wave In buffers                                 */
/*****************************************************************************/

BOOL CWaveInDevice::FreeBuf ( )
{
int i;

if (!hGlobal)                           /* if nothing allocated              */
  return(TRUE);                         /* pass back OK                      */

UnprepareBuf();                         /* make sure everything's unprepared */

for (i = 0; i < nAllocated; i++)        /* otherwise remove the Wave buffers */
  lpGlHdr[i] = NULL;                    /* reset buffer pointer              */

pGlobal = NULL;                         /* reset global buffer pointer       */
GlobalUnlock(hGlobal);                  /* unlock the global buffer          */
GlobalFree(hGlobal);                    /* free it                           */
hGlobal = NULL;                         /* and reset the handle              */

if (lpGlHdr)
  delete[] lpGlHdr;
lpGlHdr = NULL;

nAllocated = 0;

return(TRUE);
}

/*****************************************************************************/
/* PrepareBuf : prepares Wave In buffers                                     */
/*****************************************************************************/

BOOL CWaveInDevice::PrepareBuf ( )
{
int i;                                  /* loop counter                      */

if ((!hGlobal) || (!pGlobal))           /* if not already allocated          */
  return FALSE;                         /* pass back NOT OK                  */

for (i = 0; i < nAllocated; i++)        /* now prepare the Wave buffers      */
  if (!AddHeader(lpGlHdr[i]))
    return FALSE;

return TRUE;
}

/*****************************************************************************/
/* UnprepareBuf : unprepares Wave In buffers                                 */
/*****************************************************************************/

BOOL CWaveInDevice::UnprepareBuf ( )
{
int i;

if ((!hGlobal)||(!pGlobal))             /* if nothing allocated              */
  return(TRUE);                         /* pass back OK                      */

for (i = 0; i < nAllocated; i++)        /* otherwise remove the MIDI buffers */
  {
  if ((lpGlHdr[i]) &&
      (lpGlHdr[i]->dwFlags & WHDR_PREPARED))
    UnprepareHeader(lpGlHdr[i]);
  }

return(TRUE);
}

/*****************************************************************************/
/* PreparedBuffers : returns how many buffers are still prepared             */
/*****************************************************************************/

int CWaveInDevice::PreparedBuffers()
{
int i, prep = 0;

if ((!IsOpen()) ||                      /* if device not opened              */
    (!hGlobal) || (!pGlobal))           /* or nothing allocated              */
  return 0;                             /* pass back NONE                    */

for (i = 0; i < nAllocated; i++)        /* otherwise remove the MIDI buffers */
  {
  if ((lpGlHdr[i]) &&
      (lpGlHdr[i]->dwFlags & WHDR_PREPARED))
    {
//  TRACE1("CWaveInDevice::PreparedBuffers: Buffer %d is prepared\n", i);
    prep++;
    }
  }

return prep;
}

/*****************************************************************************/
/* OnEvent : called when the event thread gets kicked                        */
/*****************************************************************************/

void CWaveInDevice::OnEvent()
{
for (int i = 0; i < nAllocated; i++)    /* examine headers for a DONE one    */
  {
  if ((lpGlHdr[i]) &&
      (lpGlHdr[i]->dwFlags & WHDR_DONE))
    OnWimData(lpGlHdr[i]);              /* if found, process it              */
  }
}

/*****************************************************************************/
/* OnWimData : reacts on incoming WIM_DATA or MM_WIM_DATA message            */
/*****************************************************************************/

void CWaveInDevice::OnWimData(LPWAVEHDR lphdr)
{
if ((!lphdr) ||                         /* if erroneous or invalid pointer   */
    (!CheckHeader(lphdr)))
  return;                               /* treat as if no error happened     */

UnprepareHeader(lphdr);                 /* unprepare the header              */

if (!bRecording)                        /* if not recording any more         */
  return;                               /* that's all.                       */

if (lphdr->dwBytesRecorded)             /* if bytes in there                 */
  {
                                        /* calculate timestamp when the      */
                                        /* buffer started                    */
  DWORD dwOff = 1000 * lphdr->dwBytesRecorded / wf.nAvgBytesPerSec;
  DWORD dwTime = timeGetTime() - dwStamp;
  DWORD dwCStamp = dwTime - dwOff;
  TRACE2("Wave buffer at %d - %d\n", dwTime, dwOff);

  CWaveBuffer iBuf(lphdr);              /* generate wave buffer              */
  iBuf.SetStamp(dwCStamp);              /* set timestamp                     */
  Data(iBuf);                           /* process the buffer                */
  }

AddHeader(lphdr);                       /* then re-enable the buffer         */
}

LRESULT CWaveInDevice::OnWimData
    (
    WPARAM wParam,
    LPARAM lParam
    )
{
OnWimData((LPWAVEHDR)lParam);
return 0;
}

/*****************************************************************************/
/* OnWimOpen : called when the device is opened                              */
/*****************************************************************************/

LRESULT CWaveInDevice::OnWimOpen
    (
    WPARAM wParam,
    LPARAM lParam
    )
{
OnWimOpen();
return 0;
}

/*****************************************************************************/
/* OnWimClose : called when the device is closed                             */
/*****************************************************************************/

LRESULT CWaveInDevice::OnWimClose
    (
    WPARAM wParam,
    LPARAM lParam
    )
{
OnWimClose();
return 0;
}

/*===========================================================================*/
/* Class CWaveOutDeviceList                                                  */
/*===========================================================================*/

/*****************************************************************************/
/* CWaveOutDeviceList : constructor                                          */
/*****************************************************************************/

CWaveOutDeviceList::CWaveOutDeviceList ( )
{
WORD wNumDevices;
int i;
WAVEOUTCAPS mcap;

Add(szWMapName);
wNumDevices = waveOutGetNumDevs();      /* get # MIDI devices                */
if (wNumDevices)
  {
  for (i=0; i < (int) wNumDevices; i++)
    {
    waveOutGetDevCaps(i,
        &mcap,
        sizeof(mcap));
    Add(mcap.szPname);
    }
  }
}

/*===========================================================================*/
/* Class CWaveOutDevice                                                      */
/*===========================================================================*/

BEGIN_MESSAGE_MAP(CWaveOutDevice, CWaveDevice)
    //{{AFX_MSG_MAP(CWaveOutDevice)
    ON_MESSAGE(MM_WOM_OPEN, OnWomOpen)
    ON_MESSAGE(MM_WOM_DONE, OnWomDone)
    ON_MESSAGE(MM_WOM_CLOSE, OnWomClose)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*****************************************************************************/
/* CWaveOutDevice : constructor                                               */
/*****************************************************************************/

CWaveOutDevice::CWaveOutDevice ( )
{
hDev = NULL;                            /* reset device handle               */
}

/*****************************************************************************/
/* ~CWaveOutDevice : destructor                                               */
/*****************************************************************************/

CWaveOutDevice::~CWaveOutDevice ( )
{
if (IsOpen())
  Close();
}

/*****************************************************************************/
/* WaveOutProc : callback procedure                                          */
/*****************************************************************************/

void CALLBACK CWaveOutDevice::WaveOutProc
    (
    HMIDIOUT hWaveOut,
    UINT wMsg,
    DWORD dwInstance,
    DWORD dwParam1,
    DWORD dwParam2
    )
{
CWaveOutDevice * pOutDev = (CWaveOutDevice *)dwInstance;
switch (wMsg)
  {
  case WOM_OPEN : 
    pOutDev->OnWomOpen();
    break;
  case WOM_DONE : 
    pOutDev->OnWomDone((LPWAVEHDR)dwParam1);
    break;
  case WOM_CLOSE : 
    pOutDev->OnWomClose();
    break;
  }
}

/*****************************************************************************/
/* Open : opens a Wave Out Device with its ID                                */
/*****************************************************************************/

BOOL CWaveOutDevice::Open
    (
    int iID,
    LPWAVEFORMATEX pwfx,
    DWORD dwCb,
    DWORD dwCbInst,
    DWORD fdwOpen
    )
{
if (IsOpen())                           /* if already open                   */
  Close();                              /* close the opened device           */

                                        /* try to get device capabilities    */
if (waveOutGetDevCaps(iID, &wc, sizeof(wc)))
  return FALSE;

if ((fdwOpen & CALLBACK_TYPEMASK) ==    /* if window callback                */
    CALLBACK_WINDOW)
  AssureWnd();                          /* make sure hWnd is there           */

if (!pwfx)                              /* if no wave format defined         */
  {
  SetupWaveformat(&wf);                 /* default to 16bit stereo 44.1kHz   */
  pwfx = &wf;
  }

if ((fdwOpen & CALLBACK_TYPEMASK) ==    /* if function callback              */
    CALLBACK_FUNCTION)
  {
  if (dwCb == WAVEOPENDEFAULT)          /* if default value                  */
    dwCb = (DWORD)WaveOutProc;          /* set LP1 to procedure              */
  if (dwCbInst == WAVEOPENDEFAULT)      /* if default value                  */
    dwCbInst = (DWORD)this;             /* set LP2 to object                 */
  }
else if ((fdwOpen&CALLBACK_TYPEMASK)==  /* if event callback                 */
    CALLBACK_EVENT)
  {
  if (dwCb == WAVEOPENDEFAULT)          /* if default value                  */
    {
    if (!StartEvt())                    /* start event thread                */
      return FALSE;                     /* upon error terminate              */
    dwCb = (DWORD)hEvent;               /* set LP1 to event handle           */
    }
  if (dwCbInst == WAVEOPENDEFAULT)      /* if default value                  */
    dwCbInst =                          /* set LP2 to instance handle        */
        (UINT)AfxGetInstanceHandle();
  }
else if ((fdwOpen&CALLBACK_TYPEMASK)==  /* if thread callback                */
    CALLBACK_THREAD)
  {
  if (dwCb == WAVEOPENDEFAULT)          /* if default value                  */
    dwCb = (DWORD)GetCurrentThread();   /* use current thread                */
  if (dwCbInst == WAVEOPENDEFAULT)      /* if default value                  */
    dwCbInst =                          /* set LP2 to instance handle        */
        (UINT)AfxGetInstanceHandle();
  }
else
  {
  if (dwCb == WAVEOPENDEFAULT)          /* if default value                  */
    dwCb = (UINT)m_hWnd;                /* set LP1 to window                 */
  if (dwCbInst == WAVEOPENDEFAULT)      /* if default value                  */
    dwCbInst =                          /* set LP2 to instance handle        */
        (UINT)AfxGetInstanceHandle();
  }

WORD wRtn = waveOutOpen(&hDev,          /* open the device                   */
    iID,                                /* with the passed ID                */
    pwfx, dwCb, dwCbInst, fdwOpen);
if (wRtn)                               /* if open error                     */
  {
#if defined(_DEBUG) || defined(_DEBUGFILE)
  char szTxt[256];
  waveOutGetErrorText(wRtn, szTxt, sizeof(szTxt));
  TRACE2("CWaveOutDevice::Open(%d): %s\n", wRtn, szTxt);
#endif
  hDev = NULL;                          /* make sure that handle is reset    */
  return FALSE;
  }

waveOutGetDevCaps(iID,                  /* get device capabilities           */
    &wCap, sizeof(wCap));

return CWaveDevice::Open(iID,           /* pass back OK                      */
    pwfx, dwCb, dwCbInst, fdwOpen);
}

/*****************************************************************************/
/* Open : opens a device with its name                                       */
/*****************************************************************************/

BOOL CWaveOutDevice::Open
    (
    LPCSTR szName,
    LPWAVEFORMATEX pwfx,
    DWORD dwCb,
    DWORD dwCbInst,
    DWORD fdwOpen
    )
{
WORD wNumDevices;
int i;
WORD wRtn;
WAVEOUTCAPS wcap;

if (IsOpen())                           /* if already open                   */
  Close();                              /* close the opened device           */

if (!szName)                            /* if no name passed                 */
  szName = szWMapName;                  /* assume wave mapper                */

wNumDevices = waveOutGetNumDevs();      /* get # Wave output devices         */
if (wNumDevices)
  {
  for (i=-1; i < (int) wNumDevices; i++)
    {
    if (i == -1)
      lstrcpy(wcap.szPname, szWMapName);
    else
      wRtn = waveOutGetDevCaps(i,
          &wcap,
          sizeof(wcap));
    if (!lstrcmp(szName, wcap.szPname))
      {
      if (!Open(i, pwfx, dwCb, dwCbInst, fdwOpen))
        return(FALSE);                  /* error - return NOT OK             */

      return CWaveDevice::Open(szName,  /* opened - return OK                */
          pwfx, dwCb, dwCbInst, fdwOpen);
      }
    }
  }
return(FALSE);                          /* return NOT OK                     */
}

/*****************************************************************************/
/* Reset : resets the attached Wave Out device                               */
/*****************************************************************************/

BOOL CWaveOutDevice::Reset ( )
{
if (!IsOpen())                          /* if no device open                 */
  return FALSE;                         /* return error                      */
waveOutReset(hDev);                     /* reset the attached device         */
return TRUE;                            /* and pass back OK                  */
}

/*****************************************************************************/
/* Pause : pauses the attached Wave Out device                               */
/*****************************************************************************/

BOOL CWaveOutDevice::Pause ( )
{
if (!IsOpen())                          /* if no device open                 */
  return FALSE;                         /* return error                      */
waveOutPause(hDev);                     /* pause the attached device         */
return TRUE;                            /* and pass back OK                  */
}

/*****************************************************************************/
/* Restart : restarts the attached Wave Out device after a Pause             */
/*****************************************************************************/

BOOL CWaveOutDevice::Restart ( )
{
if (!IsOpen())                          /* if no device open                 */
  return FALSE;                         /* return error                      */
waveOutRestart(hDev);                   /* restart the attached device       */
return TRUE;                            /* and pass back OK                  */
}

/*****************************************************************************/
/* BreakLoop : breaks current loop and allows driver to continue             */
/*****************************************************************************/

BOOL CWaveOutDevice::BreakLoop ( )
{
if (!IsOpen())                          /* if no device open                 */
  return FALSE;                         /* return error                      */
waveOutBreakLoop(hDev);                 /* break current loop, if any        */
return TRUE;                            /* and pass back OK                  */
}

/*****************************************************************************/
/* Close : closes the opened Wave Out Device                                 */
/*****************************************************************************/

BOOL CWaveOutDevice::Close ( )
{
if (!IsOpen())                          /* if no device open                 */
  return FALSE;                         /* return error                      */
Reset();                                /* reset the attached device         */
waveOutClose(hDev);                     /* close the attached device         */
hDev = NULL;                            /* and reset the handle              */
return CWaveDevice::Close();            /* and pass back OK                  */
}

/*****************************************************************************/
/* Output : sends a buffer to the Wave Out Device                            */
/*****************************************************************************/

BOOL CWaveOutDevice::Output(CWaveBuffer &Buf, BOOL bSync)
{
if (!IsOpen())                          /* if not opened                     */
  return FALSE;                         /* pass back error                   */

BOOL rc = PrepareHeader(Buf);
if (rc)
  {
  rc = !waveOutWrite(hDev, Buf, sizeof(WAVEHDR));
  if (bSync)
    {
    while ((rc) && (!SendingDone(Buf)))
      ;
    UnprepareHeader(Buf);
    }
  }
return rc;
}

/*****************************************************************************/
/* GetPosition : returns the current Wave Out position                       */
/*****************************************************************************/

BOOL CWaveOutDevice::GetPosition
    (
    LPMMTIME pmmt,
    UINT cbmmt
    )
{
if (!IsOpen())                          /* if no device open                 */
  return FALSE;                         /* return error                      */
return !waveOutGetPosition(hDev, pmmt, cbmmt);
}

/*****************************************************************************/
/* GetPitch : gets device's pitch                                            */
/*****************************************************************************/

BOOL CWaveOutDevice::GetPitch(LPDWORD pdwPitch)
{
if ((!IsOpen()) ||                      /* if no device open                 */
    (!(wc.dwSupport & WAVECAPS_PITCH))) /* or unsupported                    */
  return FALSE;                         /* return error                      */
return !waveOutGetPitch(hDev, pdwPitch);
}

/*****************************************************************************/
/* SetPitch : adjust device's pitch                                          */
/*****************************************************************************/

BOOL CWaveOutDevice::SetPitch(DWORD dwPitch)
{
if ((!IsOpen()) ||                      /* if no device open                 */
    (!(wc.dwSupport & WAVECAPS_PITCH))) /* or unsupported                    */
  return FALSE;                         /* return error                      */
return !waveOutSetPitch(hDev, dwPitch);
}

/*****************************************************************************/
/* GetPlaybackRate : gets device's Playback Rate                             */
/*****************************************************************************/

BOOL CWaveOutDevice::GetPlaybackRate(LPDWORD pdwPlaybackRate)
{
if ((!IsOpen()) ||                      /* if no device open or unsupported  */
    (!(wc.dwSupport & WAVECAPS_PLAYBACKRATE)))
  return FALSE;                         /* return error                      */
return !waveOutGetPlaybackRate(hDev, pdwPlaybackRate);
}

/*****************************************************************************/
/* SetPlaybackRate : adjust device's Playback Rate                           */
/*****************************************************************************/

BOOL CWaveOutDevice::SetPlaybackRate(DWORD dwPlaybackRate)
{
if ((!IsOpen()) ||                      /* if no device open or unsupported  */
    (!(wc.dwSupport & WAVECAPS_PLAYBACKRATE)))
  return FALSE;                         /* return error                      */
return !waveOutSetPlaybackRate(hDev, dwPlaybackRate);
}

/*****************************************************************************/
/* GetVolume : gets device's Volume                                          */
/*****************************************************************************/

BOOL CWaveOutDevice::GetVolume(LPDWORD pdwVolume)
{
if ((!IsOpen()) ||                      /* if no device open or unsupported  */
    (!(wc.dwSupport & WAVECAPS_VOLUME)))
  return FALSE;                         /* return error                      */
return !waveOutGetVolume(hDev, pdwVolume);
}

/*****************************************************************************/
/* SetVolume : adjust device's Volume                                        */
/*****************************************************************************/

BOOL CWaveOutDevice::SetVolume(DWORD dwVolume)
{
if ((!IsOpen()) ||                      /* if no device open or unsupported  */
    (!(wc.dwSupport & WAVECAPS_VOLUME)))
  return FALSE;                         /* return error                      */
return !waveOutSetVolume(hDev, dwVolume);
}

/*****************************************************************************/
/* OnWomOpen : reacts on incoming MM_WOM_OPEN message                        */
/*****************************************************************************/

LRESULT CWaveOutDevice::OnWomOpen
    (
    WPARAM wParam,
    LPARAM lParam
    )
{
OnWomOpen();
return 0;
}

/*****************************************************************************/
/* OnWomDone : reacts on incoming MM_WOM_DONE message                        */
/*****************************************************************************/

LRESULT CWaveOutDevice::OnWomDone
    (
    WPARAM wParam,
    LPARAM lParam
    )
{
OnWomDone((LPWAVEHDR)lParam);
return 0;
}

/*****************************************************************************/
/* OnWomClose : reacts on incoming MM_WOM_CLOSE message                      */
/*****************************************************************************/

LRESULT CWaveOutDevice::OnWomClose
    (
    WPARAM wParam,
    LPARAM lParam
    )
{
OnWomClose();
return 0;
}

/*****************************************************************************/
/* Message : sends a special message to the device                           */
/*****************************************************************************/

DWORD CWaveOutDevice::Message
    (
    UINT msg,
    DWORD dw1,
    DWORD dw2
    )
{
DWORD rc = 0;

if (hDev)
  rc = waveOutMessage(hDev, msg, dw1, dw2);
return rc;
}

/*****************************************************************************/
/* SendingDone : returns whether buffer has been sent                        */
/*****************************************************************************/

BOOL CWaveOutDevice::SendingDone
    (
    LPWAVEHDR lpHdr
    )
{
return((lpHdr->dwFlags & WHDR_DONE) ? TRUE : FALSE);
}

/*****************************************************************************/
/* PrepareHeader : prepares a header for use in mmsystem                     */
/*****************************************************************************/

BOOL CWaveOutDevice::PrepareHeader(LPWAVEHDR lpHdr)
{
if (!hDev)
  return FALSE;
return !waveOutPrepareHeader(hDev, lpHdr, sizeof(WAVEHDR));
}

/*****************************************************************************/
/* UnprepareHeader : unprepares a used header                                */
/*****************************************************************************/

BOOL CWaveOutDevice::UnprepareHeader(LPWAVEHDR lpHdr)
{
if (!hDev)
  return FALSE;
return !waveOutUnprepareHeader(hDev, lpHdr, sizeof(WAVEHDR));
}
