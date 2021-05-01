/*****************************************************************************/
/* MFCMIDI.CPP : class implementation for MIDI base classes                  */
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

#include "stdafx.h"                     /* MFC include                       */
#include "mfcmidi.h"                    /* private definitions               */

#ifdef __WATCOMC__
#pragma library("MMSystem")
#endif
 
#ifdef _DEBUG
#ifdef WIN32
#define new DEBUG_NEW
#endif
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/

#define MIDIINHDRS      64              /* # MIDI SysEx Input headers        */
#define MIDIINBUFSIZE   256L            /* length of MIDI input buffer       */

/*****************************************************************************/
/* Global Data                                                               */
/*****************************************************************************/

/*===========================================================================*/
/* Class CMidiMsg                                                            */
/*===========================================================================*/

LPMIDIHDR CMidiMsg::lpDetached[MAX_DETACHED] = {0};
HMIDIOUT  CMidiMsg::hdDetached[MAX_DETACHED] = {0};

/*****************************************************************************/
/* CMidiMsg : constructor                                                    */
/*****************************************************************************/

CMidiMsg::CMidiMsg
    (
    DWORD smsg
    )
{
Init();
Set(smsg);
}

CMidiMsg::CMidiMsg
    (
    int b1,
    int b2,
    int b3
    )
{
DWORD smsg = 0;

Init();
Set(b1, b2, b3);
}

CMidiMsg::CMidiMsg
    (
    LPMIDIHDR lphdr
    )
{
Init();
Set(lphdr);
}

CMidiMsg::CMidiMsg
    (
    LPSTR data,
    int len
    )
{
Init();
Set(data,len);
}

/*****************************************************************************/
/* Init : initializes CMidiMsg storage                                       */
/*****************************************************************************/

void CMidiMsg::Init ( )
{
bAlloc = FALSE;
sMsg = 0;
lpHdr = NULL;
hGlPtr = NULL;
dwStamp = 0;
#ifdef _DEBUG
wMsg = 0;
#endif
}

/*****************************************************************************/
/* Empty : empties preinitialized CMidiMsg storage                           */
/*****************************************************************************/

void CMidiMsg::Empty ( )
{
if (bAlloc)
  {
  if (lpHdr->dwFlags & MHDR_PREPARED)   /* if not finished yet,              */
    DetachHdr();
  else
    FreeHdr();
  }
Init();
}

/*****************************************************************************/
/* CleanupDetached : clean up dateached headers, return 1st free pos         */
/*****************************************************************************/

int CMidiMsg::CleanupDetached()
{
int i, j;
                                        /* remove all finished ones          */
for (i = 0; i < MAX_DETACHED; i++)
  {
  if (!lpDetached[i])                   /* stop upon first free one          */
    break;
  if (lpDetached[i]->dwFlags & MHDR_DONE)
    {
    midiOutUnprepareHeader(hdDetached[i], lpDetached[i], sizeof(MIDIHDR));
    HGLOBAL hGl = GlobalHandle(lpDetached[i]);
    if (hGl)
      {
      GlobalUnlock(hGl);
      GlobalFree(hGl);
      }
    for (j = i; j < MAX_DETACHED - 1; j++)
      {
      lpDetached[j] = lpDetached[j + 1];
      hdDetached[j] = hdDetached[j + 1];
      }
    lpDetached[j] = NULL;
    hdDetached[j] = 0;
    i--;
    continue;
    }
  }

TRACE1("CMidiMsg::CleanupDetached(): %d detached messages waiting\n", i);
return i;                               /* pass back 1st free position       */
}

/*****************************************************************************/
/* DetachHdr : detaches the LPMIDIHDR from the object                        */
/*****************************************************************************/

void CMidiMsg::DetachHdr()
{
if (lpHdr->dwFlags & MHDR_DONE)         /* if this msg has been output,      */
  FreeHdr();                            /* just free it                      */
else                                    /* otherwise                         */
  {
  int i = CleanupDetached();            /* clean up list of detached msgs    */
  if (i < MAX_DETACHED)                 /* if space for 1 more,              */
    {
    lpDetached[i] = lpHdr;              /* insert this header                */
    hdDetached[i] = (HMIDIOUT)sMsg;     /* and remember the device (tricky..)*/
    }
  TRACE1("CMidiMsg::DetachHdr(): %d detached messages waiting\n", i + 1);
  }
}

/*****************************************************************************/
/* Set : sets to MIDI message of various types                               */
/*****************************************************************************/

void CMidiMsg::Set
    (
    int b1,
    int b2,
    int b3
    )
{
DWORD smsg =
    (DWORD)(b1 & 0xffL) +
    (DWORD)((b2 & 0xffL) << 8L) +
    (DWORD)((b3 & 0xffL) << 16L);
Set(smsg);
}

void CMidiMsg::Set
    (
    DWORD smsg
    )
{
Empty();
sMsg = smsg;
if (((sMsg & 0xf0) == 0x90) &&          /* if NOTE ON that's really NOTE OFF */
    (!(sMsg & 0xff0000L)))
                                        /* make it a NOTE OFF with vel.64    */
  sMsg = (sMsg & (0x00ff0fL)) | 0x400080L;     
}

void CMidiMsg::Set
    (
    LPMIDIHDR lphdr
    )
{
Empty();
lpHdr = lphdr;
sMsg = (DWORD)-3;
}

void CMidiMsg::Set
    (
    LPSTR data,
    int len
    )
{
if (len <= 3)
  {
  Set(*((DWORD *)data));
  return;
  }

Empty();
hGlPtr = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE,
                     sizeof(MIDIHDR) + len);
if (!hGlPtr)
  return;
lpHdr = (LPMIDIHDR)GlobalLock(hGlPtr);
if (lpHdr)
  {
  bAlloc = TRUE;
  memset((LPVOID)lpHdr, 0, sizeof(MIDIHDR));
  lpHdr->lpData = (LPSTR)(lpHdr+1);
  lpHdr->dwBufferLength = (DWORD)len;
  lpHdr->dwBytesRecorded = (DWORD)len;
  memcpy(lpHdr->lpData, data, len);
  sMsg = (DWORD)-4;
  }
}

/*****************************************************************************/
/* operator char * : returns the message as a character pointer              */
/*****************************************************************************/

CMidiMsg::operator char * ( )
{
if (!lpHdr)                             /* if no header                      */
  return (char *)&sMsg;                 /* pass back short MIDI msg address  */
return (char *)lpHdr->lpData;           /* otherwise pass back data pointer  */
}

/*****************************************************************************/
/* operator BYTE * : returns the message as a character pointer              */
/*****************************************************************************/

CMidiMsg::operator BYTE * ( )
{
if (!lpHdr)                             /* if no header                      */
  return (BYTE *)&sMsg;                 /* pass back short MIDI msg address  */
return (BYTE *)lpHdr->lpData;           /* otherwise pass back data pointer  */
}

/*****************************************************************************/
/* operator DWORD : returns short message as DWORD                           */
/*****************************************************************************/

CMidiMsg::operator DWORD ( )
{
if (!lpHdr)                             /* if no header                      */
  return sMsg;                          /* pass back short MIDI msg          */
return *((DWORD *)lpHdr->lpData);       /* otherwise pass back data pointer  */
}

/*****************************************************************************/
/* operator LPMIDIHDR : returns the message as a MIDIHDR pointer             */
/*****************************************************************************/

CMidiMsg::operator LPMIDIHDR ( )
{
return lpHdr;                           /* pass back MIDI header             */
}

/*****************************************************************************/
/* Length : returns the length of the passed message                         */
/*****************************************************************************/

BYTE CMidiMsg::Length(BYTE bStatus)
{
switch (bStatus & 0xf0)                 /* filter all known lengths          */
  {
  case STATUS_PROGRAMCHANGE :           /* 2-byte messages                   */
  case STATUS_CHANNELPRESSURE  :
    return 2;
  case STATUS_NOTEOFF :                 /* 3-byte messages                   */
  case STATUS_NOTEON :
  case STATUS_POLYPHONICKEY :
  case STATUS_CONTROLCHANGE :
  case STATUS_PITCHBEND :
    return 3;
  }

switch (bStatus)                        /* now process the others            */
  {
  case STATUS_TUNEREQUEST :             /* 1-byte codes                      */
  case STATUS_TIMINGCLOCK :
  case STATUS_START :
  case STATUS_CONTINUE :
  case STATUS_STOP :
  case STATUS_ACTIVESENSING :
  case STATUS_SYSTEMRESET :
    return 1;
  case STATUS_SONGSELECT :              /* 2-byte codes                      */
    return 2;
  case STATUS_QFRAME :                  /* 3-byte codes                      */
  case STATUS_SONGPOINTER :
    return 3;
  }

return 0;                               /* return invalid                    */
}

WORD CMidiMsg::Length ( )
{
if (!lpHdr)                             /* if a short message                */
  return Length((BYTE)(sMsg & 0xFF));   /* return its length                 */

return (WORD)                           /* otherwise return length of buffer */
    (lpHdr->dwBytesRecorded ?
        lpHdr->dwBytesRecorded :
        lpHdr->dwBufferLength);
}

/*===========================================================================*/
/* Class CMidiDevice                                                         */
/*===========================================================================*/

/*****************************************************************************/
/* CMidiDevice : constructor                                                 */
/*****************************************************************************/

CMidiDevice::CMidiDevice ( )
{
szDevName[0] = 0;                       /* reset device name                 */
bIsOpen = FALSE;                        /* reset opened flag                 */
}

/*****************************************************************************/
/* ~CMidiDevice : destructor                                                 */
/*****************************************************************************/

CMidiDevice::~CMidiDevice ( )
{
}

/*****************************************************************************/
/* Create : creates necessary notification window                            */
/*****************************************************************************/

BOOL CMidiDevice::Create ( )
{
return TRUE;
}

/*****************************************************************************/
/* Open : opens the device                                                   */
/*****************************************************************************/

BOOL CMidiDevice::Open
    (
    int iID,
    DWORD lp1,
    DWORD lp2,
    DWORD dwFlags
    )
{
bIsOpen = TRUE;
OnOpen();
return TRUE;
}

BOOL CMidiDevice::Open
    (
    LPCSTR szName,
    DWORD lp1,
    DWORD lp2,
    DWORD dwFlags
    )
{
strcpy(szDevName, szName);
return TRUE;
}

/*****************************************************************************/
/* Close : closes the device                                                 */
/*****************************************************************************/

BOOL CMidiDevice::Close ( )
{
*szDevName = 0;
bIsOpen = FALSE;
OnClose();
return TRUE;
}

/*===========================================================================*/
/* Class CMidiInDeviceList                                                   */
/*===========================================================================*/

/*****************************************************************************/
/* ~CMidiInDeviceList : destructor                                           */
/*****************************************************************************/

CMidiInDeviceList::~CMidiInDeviceList()
{
Unload();
}

/*****************************************************************************/
/* Load : (re-)loads the device list                                         */
/*****************************************************************************/

void CMidiInDeviceList::Load()
{
WORD wNumDevices;
int i;
LPMIDIINCAPS pCaps;

Unload();
wNumDevices = midiInGetNumDevs();       /* get # MIDI input devices          */
if (wNumDevices)
  {
  for (i=0; i < (int) wNumDevices; i++)
    {
    pCaps = new MIDIINCAPS;
    if (pCaps)
      {
      CMidiInDevice::GetDevCaps(i, pCaps);
      Add(pCaps->szPname);
      paCaps.Add(pCaps);
      }
    }
  }
}

/*****************************************************************************/
/* Unload : removes list from memory                                         */
/*****************************************************************************/

void CMidiInDeviceList::Unload()
{
for (int i = paCaps.GetUpperBound(); i >= 0; i--)
  {
  LPMIDIINCAPS pCaps = GetCaps(i);
  if (pCaps)
    delete pCaps;
  }
paCaps.RemoveAll();
RemoveAll();
}

/*===========================================================================*/
/* Class CMidiInDevice                                                       */
/*===========================================================================*/

int CMidiInDevice::nInDevs = 0;         /* # loaded in devices               */
HGLOBAL CMidiInDevice::hGlRing = 0;     /* one handle,                       */
LPBYTE CMidiInDevice::pRing = NULL;     /* one ring to chain them all :-)    */
volatile LPBYTE CMidiInDevice::pRingRead = NULL;
volatile LPBYTE CMidiInDevice::pRingWrite = NULL;

HANDLE CMidiInDevice::hEvt = NULL;      /* event handle & thread             */
CWinThread *CMidiInDevice::pEvtThread = NULL;
bool volatile CMidiInDevice::bKillThread = false;

/*****************************************************************************/
/* CMidiInDevice : constructor                                               */
/*****************************************************************************/

CMidiInDevice::CMidiInDevice ( )
{
hDev = NULL;                            /* reset device handle               */

hGlobal = NULL;                         /* MIDI In buffers                   */
pGlobal = NULL;
lpGlHdr = new LPMIDIHDR[MIDIINHDRS];
if (lpGlHdr)
  for (int i = 0; i < MIDIINHDRS; i++)
    lpGlHdr[i] = NULL;
bRecording = FALSE;                     /* reset recording flag              */
dwStamp = 0;                            /* set internal time stamp           */
memset(&mCap, 0, sizeof(mCap));         /* init capabilities                 */
bClosing = FALSE;

if (!hGlRing)                           /* if ring needs to be setup         */
  {
  hGlRing = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, MIDIIN_RINGSIZE);
  if (hGlRing)
    pRingRead = pRingWrite = pRing = (LPBYTE)GlobalLock(hGlRing);
  }

nInDevs++;
}

/*****************************************************************************/
/* ~CMidiInDevice : destructor                                               */
/*****************************************************************************/

CMidiInDevice::~CMidiInDevice ( )
{
if (IsOpen())
  Close();

FreeBuf();
delete[] lpGlHdr;

if (--nInDevs <= 0)
  {
  nInDevs = 0;
  if (pRing)
    {
    GlobalUnlock(hGlRing);
    pRingRead = pRingWrite = pRing = NULL;
    }
  if (hGlRing)
    {
    GlobalFree(hGlRing);
    hGlRing = 0;
    }
  StopThread();                         /* make sure the work thread dies    */
  }
}

/*****************************************************************************/
/* MidiInProc : callback procedure                                           */
/*****************************************************************************/

void CALLBACK CMidiInDevice::MidiInProc
    (
    HMIDIIN hMidiIn,
    UINT wMsg,
    DWORD dwInstance,
    DWORD dwParam1,
    DWORD dwParam2
    )
{
#if 0
                                        /* direct processing                 */
ProcessMessage(wMsg, dwInstance, dwParam1, dwParam2);

#else
                                        /* processing in separate thread     */
int nRest = (pRingRead - pRingWrite);
if (nRest <= 0)
  nRest += MIDIIN_RINGSIZE;
if (nRest >= 16)
  {
  ((LPDWORD)pRingWrite)[0] = wMsg;
  ((LPDWORD)pRingWrite)[1] = dwInstance;
  ((LPDWORD)pRingWrite)[2] = dwParam1;
  ((LPDWORD)pRingWrite)[3] = dwParam2;
  pRingWrite += 16;
  if (pRingWrite >= (pRing + MIDIIN_RINGSIZE))
    pRingWrite = pRing;
  SetEvent(hEvt);
  }

#endif
}

/*****************************************************************************/
/* EvtThreadProc : called when MIDI messages come in                         */
/*****************************************************************************/

UINT CMidiInDevice::EvtThreadProc(LPVOID pParam)
{
UINT result;
UINT wMsg;
DWORD dwInstance, dwParam1, dwParam2;

while (!bKillThread)                    /* while looping                     */
  {                                     /* wait for event to be signaled     */
  result = WaitForSingleObject(hEvt,INFINITE);
  if ((result == WAIT_OBJECT_0) &&      /* if so,                            */
      (!bKillThread))
    {
    while (pRingRead != pRingWrite)
      {
      wMsg = ((LPDWORD)pRingRead)[0];
      dwInstance = ((LPDWORD)pRingRead)[1];
      dwParam1 = ((LPDWORD)pRingRead)[2];
      dwParam2 = ((LPDWORD)pRingRead)[3];
      pRingRead += 16;
      if (pRingRead >= (pRing + MIDIIN_RINGSIZE))
        pRingRead = pRing;

      ProcessMessage(wMsg, dwInstance, dwParam1, dwParam2);
      }
    }
  else                                  /* if terminating,                   */
    break;                              /* stop the loop                     */
  }

bKillThread = false;                    /* reset kill flag                   */
return 0;                               /* and end the thread                */
}

/*****************************************************************************/
/* ProcessMessage : processes a MIDI message                                 */
/*****************************************************************************/

void CMidiInDevice::ProcessMessage
    (
    UINT wMsg,
    DWORD dwInstance,
    DWORD dwParam1,
    DWORD dwParam2
    )
{
CMidiInDevice * pInDev = (CMidiInDevice *)dwInstance;
CMidiMsg msg;

#ifdef _DEBUG
DWORD dwDelay = 0;
DWORD dwNow = timeGetTime() - pInDev->dwStamp;
#endif

switch (wMsg)
  {
  case MIM_CLOSE : 
    pInDev->OnMimClose();
    break;
  case MIM_OPEN : 
    pInDev->OnMimOpen();
    break;
  case MIM_ERROR :
    msg.Set(dwParam1);
    msg.SetStamp(dwParam2);
#ifdef _DEBUG
    msg.SetMsg(wMsg);
    dwDelay = dwNow - dwParam2;
#endif
    pInDev->OnMimError(msg);
    break;
  case MIM_LONGERROR : 
    pInDev->OnMimError((LPMIDIHDR)dwParam1, dwParam2);
    break;
  case MIM_MOREDATA :
    msg.Set(dwParam1);
    msg.SetStamp(dwParam2);
#ifdef _DEBUG
    msg.SetMsg(wMsg);
    dwDelay = dwNow - dwParam2;
#endif
    pInDev->OnMimMoreData(msg);
    break;
  case MIM_DATA : 
    msg.Set(dwParam1);
    msg.SetStamp(dwParam2);
#ifdef _DEBUG
    msg.SetMsg(wMsg);
    dwDelay = dwNow - dwParam2;
#endif
    pInDev->Data(msg);
    break;
  case MIM_LONGDATA : 
    pInDev->OnMimLongData((LPMIDIHDR)dwParam1, dwParam2);
    break;
  }

#ifdef _DEBUG
if ((dwDelay >= 10) && pInDev->dwStamp)
  TRACE1("MIDI In Thread Processing Delay: %dms\n", dwDelay);
#endif
}

/*****************************************************************************/
/* StartThread : assures that the work thread is there                       */
/*****************************************************************************/

bool CMidiInDevice::StartThread()
{
if (!hEvt)                              /* if not yet done,                  */
  {
                                        /* then create a new event handle    */
  hEvt = CreateEvent(NULL, FALSE, FALSE, NULL);
  if (!hEvt)                            /* upon error                        */
    return false;                       /* return error                      */
  }

if (!pEvtThread)                        /* then create the thread            */
  {
  pEvtThread = AfxBeginThread(EvtThreadProc, this,
                              THREAD_PRIORITY_TIME_CRITICAL, 0,
                              CREATE_SUSPENDED, NULL);   
  if (!pEvtThread)                      /* upon error                        */
    {
    StopThread();                       /* kill the event handle             */
    return false;                       /* and return error                  */
    }

  pEvtThread->m_bAutoDelete = TRUE;     /* let it go!                        */
  pEvtThread->ResumeThread();
  }
return true;                            /* pass back OK                      */
}

/*****************************************************************************/
/* StopThread: kills an eventually running event thread & event handle       */
/*****************************************************************************/

bool CMidiInDevice::StopThread()
{
if (hEvt)                               /* if event handle allocated         */
  {
  TRACE0("Killing MIDI In work thread\n");
  if (pEvtThread)                       /* if thread still running           */
    {
    bKillThread = true;                 /* set cancel flag                   */
    SetEvent(hEvt);                     /* tell thread to die                */
    int nLoop = 0;
    while (bKillThread)                 /* wait until it does.               */
      {
      TRACE0("Waiting for MIDI In work thread to stop\n");
      Sleep(50);
      if (++nLoop > 20)                 /* if more than a second passed,     */
        {
        TRACE0("Given up waiting for MIDI In work thread to stop\n");
        break;
        }
      }
    pEvtThread = 0;                     /* and thread handle                 */
    }
  CloseHandle(hEvt);                    /* and close the event handle        */
  hEvt = 0;                             /* and reset it...                   */
  }
return true;
}

/*****************************************************************************/
/* GetDevCaps : retrieves a specific device' capabilities                    */
/*****************************************************************************/

BOOL CMidiInDevice::GetDevCaps(UINT iID, LPMIDIINCAPS pmCap)
{
MMRESULT res = midiInGetDevCaps(iID,    /* get device capabilities           */
                                pmCap,
                                sizeof(*pmCap));
return (res == MMSYSERR_NOERROR);
}

BOOL CMidiInDevice::GetDevCaps(LPMIDIINCAPS pmCap)
{
memcpy(pmCap, &mCap, sizeof(mCap));     /* return known capabilities         */
return TRUE;
}

/*****************************************************************************/
/* Open : opens a MIDI In Device with its ID                                 */
/*****************************************************************************/

BOOL CMidiInDevice::Open
    (
    int iID,
    DWORD lp1,
    DWORD lp2,
    DWORD dwFlags
    )
{
if (IsOpen())                           /* if already open                   */
  Close();                              /* close the opened device           */

if ((dwFlags & CALLBACK_TYPEMASK) ==    /* if function callback              */
    CALLBACK_FUNCTION)
  {
  if (lp1 == OPENDEFAULT)               /* if default value                  */
    lp1 = (DWORD)MidiInProc;            /* set LP1 to procedure              */
  if (lp2 == OPENDEFAULT)               /* if default value                  */
    lp2 = (DWORD)this;                  /* set LP2 to object                 */
#ifdef WIN32
  StartThread();                        /* make sure the thread is there     */
#endif
  }
#ifdef WIN32
                                        /* if thread callback                */
else if ((dwFlags & CALLBACK_TYPEMASK) ==    
    CALLBACK_THREAD)
  {
  if (lp1 == OPENDEFAULT)               /* if default value                  */
    lp1 = (DWORD)GetCurrentThreadId();  /* set LP1 to thread ID              */
  if (lp2 == OPENDEFAULT)               /* if default value                  */
    lp2 = (UINT)AfxGetInstanceHandle(); /* set LP2 to instance               */
  }
#endif
else                                    /* window callback not allowed here  */
  return FALSE;

WORD wRtn = 1;
try
  {
  wRtn = midiInOpen(&hDev,              /* open the device                   */
                    iID,                /* with the passed ID                */
                    lp1, lp2, dwFlags);
  }
catch(...)
  {
  }
if (wRtn)                               /* if open error                     */
  {
  TRACE1("CMidiInDevice::Open:midiInOpen(): %s\n", GetErrorText(wRtn));
  hDev = NULL;                          /* make sure that handle is reset    */
  return FALSE;
  }

if (!AllocBuf())                        /* allocate MIDI Input buffers       */
  {
  Close();                              /* upon error close device           */
  return(FALSE);                        /* and                               */
  }

CMidiDevice::Open(iID, lp1, lp2, dwFlags);/* call ancestor                   */

GetDevCaps(iID, &mCap);                 /* get device capabilities           */

return Start();                         /* start recording                   */
}

/*****************************************************************************/
/* Open : opens a device with its name                                       */
/*****************************************************************************/

BOOL CMidiInDevice::Open
    (
    LPCSTR szName,
    DWORD lp1,
    DWORD lp2,
    DWORD dwFlags
    )
{
WORD wNumDevices;
int i;
WORD wRtn;
MIDIINCAPS mcap = {0};

wNumDevices = midiInGetNumDevs();       /* get # MIDI input devices          */
if (wNumDevices)
  {
  for (i=-1; i < (int) wNumDevices; i++)
    {
    wRtn = GetDevCaps((i == -1) ? MIDI_MAPPER : i, &mcap);
    if (!lstrcmp(szName, mcap.szPname))
      {
      if (!Open(i, lp1, lp2, dwFlags))
        return(FALSE);                  /* error - return NOT OK             */

      return CMidiDevice::Open(szName,  /* opened - return OK                */
          lp1, lp2, dwFlags);
      }
    }
  }
return(FALSE);                          /* return NOT OK                     */
}

/*****************************************************************************/
/* Start : start recording from device                                       */
/*****************************************************************************/

BOOL CMidiInDevice::Start ( )
{
if (!IsOpen() || IsRecording())         /* if no device open or recording    */
  return FALSE;                         /* return error                      */

MMRESULT res;
if ((res = midiInStart(hDev)))          /* start recording from device       */
  {
  TRACE1("CMidiInDevice::Start:midiInStart(): %s\n", GetErrorText(res));
  return FALSE;                         /* upon error return error           */
  }
dwStamp = ::timeGetTime();              /* set internal time stamp           */

bRecording = TRUE;                      /* set recording flag                */
return TRUE;
}

/*****************************************************************************/
/* Stop : stop recording from device                                         */
/*****************************************************************************/

BOOL CMidiInDevice::Stop ( )
{
if (!IsOpen() || !IsRecording())        /* if no device open or recording    */
  return FALSE;                         /* return error                      */

bRecording = FALSE;                     /* reset recording flag              */
MMRESULT res = midiInStop(hDev);        /* stop recording                    */
#if _DEBUG
if (res)
  TRACE1("CMidiInDevice::Stop:midiInStop(): %s\n", GetErrorText(res));
#endif
dwStamp = 0;                            /* set internal time stamp           */
return TRUE;
}

/*****************************************************************************/
/* Reset : resets the attached MIDI In device                                */
/*****************************************************************************/

BOOL CMidiInDevice::Reset ( )
{
if (!IsOpen())                          /* if no device open                 */
  return FALSE;                         /* return error                      */
MMRESULT res = midiInReset(hDev);       /* reset the attached device         */
#ifdef _DEBUG
if (res != MMSYSERR_NOERROR)
  TRACE1("CMidiInDevice::Reset:midiInReset(): %s\n", GetErrorText(res));
#endif
bRecording = FALSE;                     /* reset recording flag              */
return TRUE;                            /* and pass back OK                  */
}

/*****************************************************************************/
/* Close : closes the opened MIDI In Device                                  */
/*****************************************************************************/

BOOL CMidiInDevice::Close ( )
{
if (!IsOpen())                          /* if no device open                 */
  return FALSE;                         /* return error                      */
bClosing = TRUE;                        /* set flag that we're closing now   */
Stop();                                 /* stop recording from device        */
Reset();                                /* reset the attached device         */
UnprepareBuf();                         /* unprepare allocated data          */
MMRESULT res = midiInClose(hDev);       /* close the attached device         */
#ifdef _DEBUG
if (res != MMSYSERR_NOERROR)
  TRACE1("CMidiInDevice::Close: midiInClose()=%d\n", res);
#endif
hDev = NULL;                            /* and reset the handle              */
bClosing = FALSE;
return CMidiDevice::Close();            /* and pass back OK                  */
}

/*****************************************************************************/
/* Message : sends a special message to the device                           */
/*****************************************************************************/

DWORD CMidiInDevice::Message
    (
    UINT msg,
    DWORD dw1,
    DWORD dw2
    )
{
DWORD rc = 0;

if (hDev)
  rc = midiInMessage(hDev, msg, dw1, dw2);
return rc;
}

/*****************************************************************************/
/* IsRecording : returns whether currently recording                         */
/*****************************************************************************/

BOOL CMidiInDevice::IsRecording ( )
{
return bRecording;
}

/*****************************************************************************/
/* AddHeader : adds a MIDI Input buffer to the device                        */
/*****************************************************************************/

BOOL CMidiInDevice::AddHeader
    (
    LPMIDIHDR lpHdr
    )
{
MMRESULT res;
if (lpHdr->dwFlags & MHDR_PREPARED)
  {
  res = midiInUnprepareHeader(hDev, lpHdr, sizeof(MIDIHDR));
#ifdef _DEBUG
  if (res != MMSYSERR_NOERROR)
    TRACE1("CMidiInDevice::AddHeader:midiInUnprepareHeader()=%d\n", res);
#endif
  lpHdr->dwBufferLength = MIDIINBUFSIZE;
  }
lpHdr->dwBytesRecorded = 0;
lpHdr->dwFlags &= ~MHDR_DONE;
if ((res = midiInPrepareHeader(hDev, lpHdr, sizeof(MIDIHDR))))
  {
  TRACE1("CMidiInDevice::AddHeader:midiInPrepareHeader()=%d\n", res);
  return(FALSE);
  }
if ((res = midiInAddBuffer(hDev, lpHdr, sizeof(MIDIHDR))))
  {
  TRACE1("CMidiInDevice::AddHeader:midiInAddBuffer()=%d\n", res);
  return(FALSE);
  }
return(TRUE);
}

/*****************************************************************************/
/* CheckHeader : checks the passed MIDI header for validity                  */
/*****************************************************************************/

BOOL CMidiInDevice::CheckHeader
    (
    LPMIDIHDR lpHdr
    )
{
int i;

if ((IsBadReadPtr(lpHdr, sizeof(MIDIHDR))) ||
    (!lpGlHdr))
  return FALSE;
for (i=0; i < MIDIINHDRS; i++)          /* if it's one we allocated          */
  if (lpHdr == lpGlHdr[i])
    return TRUE;                        /* pass back OK                      */
return FALSE;
}

/*****************************************************************************/
/* AllocBuf : allocates MIDI In buffers                                      */
/*****************************************************************************/

BOOL CMidiInDevice::AllocBuf ( )
{
int i;                                  /* loop counter                      */

if (hGlobal)                            /* if already allocated              */
  return TRUE;                          /* pass back OK                      */

hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE,
    MIDIINHDRS * (sizeof(MIDIHDR) + MIDIINBUFSIZE));
if (hGlobal == NULL)
  return FALSE;

pGlobal = (char *)GlobalLock(hGlobal);  /* lock allocated memory             */
if ((pGlobal == NULL) ||
    (lpGlHdr == NULL))
  {
  GlobalFree(hGlobal);
  hGlobal = NULL;
  return FALSE;
  }

for (i=0; i < MIDIINHDRS; i++)          /* now generate the MIDI buffers     */
  {
  lpGlHdr[i] = (LPMIDIHDR)
      (pGlobal + i * (sizeof(MIDIHDR) + MIDIINBUFSIZE));
  memset(lpGlHdr[i], 0, sizeof(MIDIHDR) + MIDIINBUFSIZE);
  lpGlHdr[i]->dwBufferLength = MIDIINBUFSIZE;
  lpGlHdr[i]->lpData = (char *)(lpGlHdr[i]+1);
  }

PrepareBuf();                           /* and prepare them for usage        */
return TRUE;
}

/*****************************************************************************/
/* FreeBuf : frees allocated MIDI In buffers                                 */
/*****************************************************************************/

BOOL CMidiInDevice::FreeBuf ( )
{
int i;

if (!hGlobal)                           /* if nothing allocated              */
  return(TRUE);                         /* pass back OK                      */

UnprepareBuf();                         /* make sure everything's unprepared */

for (i = 0; i < MIDIINHDRS; i++)        /* otherwise remove the MIDI buffers */
  lpGlHdr[i] = NULL;                    /* reset buffer pointer              */

pGlobal = NULL;                         /* reset global buffer pointer       */
GlobalUnlock(hGlobal);                  /* unlock the global buffer          */
GlobalFree(hGlobal);                    /* free it                           */
hGlobal = NULL;                         /* and reset the handle              */
return(TRUE);
}

/*****************************************************************************/
/* PrepareBuf : prepares MIDI In buffers                                     */
/*****************************************************************************/

BOOL CMidiInDevice::PrepareBuf ( )
{
int i;                                  /* loop counter                      */

if ((!hGlobal) || (!pGlobal))           /* if not already allocated          */
  return FALSE;                         /* pass back NOT OK                  */

for (i = 0; i < MIDIINHDRS; i++)        /* now prepare the MIDI buffers      */
  if (!AddHeader(lpGlHdr[i]))
    return FALSE;

return TRUE;
}

/*****************************************************************************/
/* UnprepareBuf : unprepares MIDI In buffers                                 */
/*****************************************************************************/

BOOL CMidiInDevice::UnprepareBuf ( )
{
int i;

if ((!hGlobal)||(!pGlobal))             /* if nothing allocated              */
  return(TRUE);                         /* pass back OK                      */

for (i = 0; i < MIDIINHDRS; i++)        /* otherwise remove the MIDI buffers */
  {
  if ((lpGlHdr[i]) &&
      (lpGlHdr[i]->dwFlags & MHDR_PREPARED))
    {
    MMRESULT res = 
        midiInUnprepareHeader(hDev, lpGlHdr[i], sizeof(MIDIHDR));
#ifdef _DEBUG
    if (res != MMSYSERR_NOERROR)
      TRACE1("CMidiInDevice::UnprepareBuf:midiInUnprepareHeader(): %s\n", GetErrorText(res));
#endif
    }
  }

return(TRUE);
}

/*****************************************************************************/
/* GetErrorText : retrieves the error text for a specific return code        */
/*****************************************************************************/

LPCSTR CMidiInDevice::GetErrorText(MMRESULT wError)
{
szErrorText[0] = '\0';
midiInGetErrorText(wError, szErrorText, sizeof(szErrorText));
return szErrorText;
}

/*****************************************************************************/
/* PreparedBuffers : returns # buffers that still are prepared               */
/*****************************************************************************/

int CMidiInDevice::PreparedBuffers()
{
int i, prep = 0;

if ((!hGlobal) || (!pGlobal))           /* if nothing allocated              */
  return 0;                             /* pass back NONE                    */

for (i = 0; i < MIDIINHDRS; i++)        /* otherwise remove the MIDI buffers */
  {
  if ((lpGlHdr[i]) &&
      (lpGlHdr[i]->dwFlags & MHDR_PREPARED))
    prep++;
  }

return prep;
}

/*****************************************************************************/
/* OnMimLongData : reacts on incoming (MM_)MIM_LONGDATA message              */
/*****************************************************************************/

void CMidiInDevice::OnMimLongData(LPMIDIHDR lphdr, DWORD dwTS)
{
if ((!lphdr) ||                         /* if erroneous or invalid pointer   */
    (!CheckHeader((LPMIDIHDR)lphdr)) ||
    (bClosing))                         /* or in closing state               */
  return;                               /* treat as if no error happened     */

if (lphdr->dwBytesRecorded)
  {
  CMidiMsg iMsg(lphdr);                 /* generate MIDI message             */
  iMsg.SetStamp(dwTS);
#ifdef _DEBUG
  iMsg.SetMsg(MIM_LONGDATA);
#endif
  Data(iMsg);                           /* process the message               */
  }

AddHeader(lphdr);                       /* then re-enable the buffer         */
}

/*****************************************************************************/
/* OnMimError : called upon (MM_)MIM_(LONG)ERROR messages                    */
/*****************************************************************************/

void CMidiInDevice::OnMimError(LPMIDIHDR lphdr, DWORD dwTS)
{
if ((!lphdr) ||                         /* if erroneous or invalid pointer   */
    (!CheckHeader((LPMIDIHDR)lphdr)) ||
    (bClosing))                         /* or in closing state               */
  return;                               /* treat as if no error happened     */

if (lphdr->dwBytesRecorded)
  {
  CMidiMsg iMsg(lphdr);                 /* generate MIDI message             */
  iMsg.SetStamp(dwTS ? dwTS : GetStamp());
#ifdef _DEBUG
  iMsg.SetMsg(MIM_LONGERROR);
#endif
  OnMimError(iMsg);
  }
AddHeader(lphdr);                       /* then re-enable the buffer         */
}

/*****************************************************************************/
/* Data : processes incoming MIDI data                                       */
/*****************************************************************************/

void CMidiInDevice::Data
    (
    CMidiMsg & Msg
    )
{
switch (Msg[0] & 0xf0)                  /* look what kind of message :       */
  {
  case STATUS_NOTEOFF :
    NoteOff(Msg[0] & 0x0f, Msg[1]);
    break;
  case STATUS_NOTEON :
    NoteOn(Msg[0] & 0x0f, Msg[1], Msg[2]);
    break;
  case STATUS_POLYPHONICKEY :
    KeyAftertouch(Msg[0] & 0x0f, Msg[1], Msg[2]);
    break;
  case STATUS_CONTROLCHANGE :
    Controller(Msg[0] & 0x0f, Msg[1], Msg[2]);
    break;
  case STATUS_PROGRAMCHANGE :
    Patch(Msg[0] & 0x0f, Msg[1]);
    break;
  case STATUS_CHANNELPRESSURE :
    ChannelAftertouch(Msg[0] & 0x0f, Msg[1]);
    break;
  case STATUS_PITCHBEND :
    PitchBend(Msg[0] & 0x0f, (int)Msg[1] | ((int)Msg[2] << 7));
    break;
  case STATUS_SYSTEMRESET :
    SystemReset();
    break;
  }
}

/*===========================================================================*/
/* Class CMidiOutDeviceList                                                  */
/*===========================================================================*/

/*****************************************************************************/
/* ~CMidiOutDeviceList : destructor                                          */
/*****************************************************************************/

CMidiOutDeviceList::~CMidiOutDeviceList()
{
Unload();
}

/*****************************************************************************/
/* Load : (re-)loads the device list                                         */
/*****************************************************************************/

void CMidiOutDeviceList::Load()
{
WORD wNumDevices;
int i;
LPMIDIOUTCAPS pCaps;

Unload();                               /* drop previous contents            */

pCaps = new MIDIOUTCAPS;
if (pCaps)
  {
  CMidiOutDevice::GetDevCaps(MIDI_MAPPER, pCaps);
  Add(pCaps->szPname);
  paCaps.Add(pCaps);
  }
wNumDevices = midiOutGetNumDevs();      /* get # MIDI devices                */
if (wNumDevices)
  {
  for (i=0; i < (int) wNumDevices; i++)
    {
    pCaps = new MIDIOUTCAPS;
    if (pCaps)
      {
      CMidiOutDevice::GetDevCaps(i, pCaps);
      Add(pCaps->szPname);
      paCaps.Add(pCaps);
      }
    }
  }
}

/*****************************************************************************/
/* Unload : removes list from memory                                         */
/*****************************************************************************/

void CMidiOutDeviceList::Unload()
{
for (int i = paCaps.GetUpperBound(); i >= 0; i--)
  {
  LPMIDIOUTCAPS pCaps = GetCaps(i);
  if (pCaps)
    delete pCaps;
  }
paCaps.RemoveAll();
RemoveAll();
}


/*===========================================================================*/
/* Class CMidiOutDevice                                                      */
/*===========================================================================*/

/*****************************************************************************/
/* CMidiOutDevice : constructor                                              */
/*****************************************************************************/

CMidiOutDevice::CMidiOutDevice ( )
{
hDev = NULL;                            /* reset device handle               */
RunningStatus = 0;                      /* reset running status              */
memset(&mCap, 0, sizeof(mCap));         /* init capabilities                 */
}

/*****************************************************************************/
/* ~CMidiOutDevice : destructor                                               */
/*****************************************************************************/

CMidiOutDevice::~CMidiOutDevice ( )
{
if (IsOpen())
  Close();
}

/*****************************************************************************/
/* MidiOutProc : callback procedure                                          */
/*****************************************************************************/

void CALLBACK MidiOutProc
    (
    HMIDIOUT hMidiOut,
    UINT wMsg,
    DWORD dwInstance,
    DWORD dwParam1,
    DWORD dwParam2
    )
{
CMidiOutDevice * pOutDev = (CMidiOutDevice *)dwInstance;
switch (wMsg)
  {
  case MOM_OPEN : 
    pOutDev->OnMomOpen();
    break;
  case MOM_DONE : 
    pOutDev->OnMomDone((LPMIDIHDR)dwParam1);
    break;
  case MOM_CLOSE : 
    pOutDev->OnMomClose();
    break;
  }
}

/*****************************************************************************/
/* GetDevCaps : retrieves a specific device' capabilities                    */
/*****************************************************************************/

BOOL CMidiOutDevice::GetDevCaps(UINT iID, LPMIDIOUTCAPS pmCap)
{
return (midiOutGetDevCaps(iID,          /* get device capabilities           */
                          pmCap,
                          sizeof(*pmCap)) == MMSYSERR_NOERROR);
}

BOOL CMidiOutDevice::GetDevCaps(LPMIDIOUTCAPS pmCap)
{
memcpy(pmCap, &mCap, sizeof(mCap));     /* return known capabilities         */
return TRUE;
}

/*****************************************************************************/
/* Open : opens a MIDI In Device with its ID                                 */
/*****************************************************************************/

BOOL CMidiOutDevice::Open
    (
    int iID,
    DWORD lp1,
    DWORD lp2,
    DWORD dwFlags
    )
{
if (IsOpen())                           /* if already open                   */
  Close();                              /* close the opened device           */

if ((dwFlags & CALLBACK_TYPEMASK) ==    /* if function callback              */
    CALLBACK_FUNCTION)
  {
  if (lp1 == OPENDEFAULT)               /* if default value                  */
    lp1 = (DWORD)MidiOutProc;           /* set LP1 to procedure              */
  if (lp2 == OPENDEFAULT)               /* if default value                  */
    lp2 = (DWORD)this;                  /* set LP2 to object                 */
  }
else                                    /* window callback not allowed here  */
  return FALSE;

WORD wRtn = 1;
try
  {
  wRtn = midiOutOpen(&hDev,             /* open the device                   */
                     iID,               /* with the passed ID                */
                     lp1, lp2, dwFlags);
  }
catch(...)
  {
  }
if (wRtn)                               /* if open error                     */
  {
  TRACE1("CMidiOutDevice::Open:midiOutOpen(): %s\n", GetErrorText(wRtn));
  hDev = NULL;                          /* make sure that handle is reset    */
  return FALSE;
  }

GetDevCaps(iID, &mCap);                 /* get device capabilities           */

return CMidiDevice::Open(iID,           /* pass back OK                      */
    lp1, lp2, dwFlags);
}

/*****************************************************************************/
/* Open : opens a device with its name                                       */
/*****************************************************************************/

BOOL CMidiOutDevice::Open
    (
    LPCSTR szName,
    DWORD lp1,
    DWORD lp2,
    DWORD dwFlags
    )
{
WORD wNumDevices;
int i;
WORD wRtn;
MIDIOUTCAPS mcap = {0};

wNumDevices = midiOutGetNumDevs();      /* get # MIDI input devices          */
if (wNumDevices)
  {
  for (i=-1; i < (int) wNumDevices; i++)
    {
    wRtn = GetDevCaps((i == -1) ? MIDI_MAPPER : i, &mcap);
    if (!lstrcmp(szName, mcap.szPname))
      {
      if (!Open(i, lp1, lp2, dwFlags))
        return(FALSE);                  /* error - return NOT OK             */

      return CMidiDevice::Open(szName,  /* opened - return OK                */
          lp1, lp2, dwFlags );
      }
    }
  }
return(FALSE);                          /* return NOT OK                     */
}

/*****************************************************************************/
/* Reset : resets the attached MIDI In device                                */
/*****************************************************************************/

BOOL CMidiOutDevice::Reset ( )
{
if (!IsOpen())                          /* if no device open                 */
  return FALSE;                         /* return error                      */
MMRESULT res = midiOutReset(hDev);      /* reset the attached device         */
#ifdef _DEBUG
if (res != MMSYSERR_NOERROR)
  TRACE1("CMidiOutDevice::Reset:midiOutReset(): %s\n", GetErrorText(res));
#endif
return TRUE;                            /* and pass back OK                  */
}

/*****************************************************************************/
/* Close : closes the opened MIDI In Device                                  */
/*****************************************************************************/

BOOL CMidiOutDevice::Close ( )
{
if (!IsOpen())                          /* if no device open                 */
  return FALSE;                         /* return error                      */
Reset();                                /* reset the attached device         */
MMRESULT res = midiOutClose(hDev);      /* close the attached device         */
#ifdef _DEBUG
if (res != MMSYSERR_NOERROR)
  TRACE1("CMidiOutDevice::Close:midiOutClose(): %s\n", GetErrorText(res));
#endif
hDev = NULL;                            /* and reset the handle              */
return CMidiDevice::Close();            /* and pass back OK                  */
}

/*****************************************************************************/
/* Message : sends a special message to the device                           */
/*****************************************************************************/

DWORD CMidiOutDevice::Message
    (
    UINT msg,
    DWORD dw1,
    DWORD dw2
    )
{
DWORD rc = 0;

if (hDev)
  rc = midiOutMessage(hDev, msg, dw1, dw2);
return rc;
}

/*****************************************************************************/
/* SendingDone : returns whether buffer has been sent                        */
/*****************************************************************************/

BOOL CMidiOutDevice::SendingDone
    (
    LPMIDIHDR lpHdr
    )
{
return((lpHdr->dwFlags & MHDR_DONE) ? TRUE : FALSE);
}

/*****************************************************************************/
/* CachePatches : caches MIDI device patches                                 */
/*****************************************************************************/

BOOL CMidiOutDevice::CachePatches
    (
    UINT uBank,
    WORD FAR* lpwPatchArray,
    UINT uFlags
    )
{
if ((!IsOpen()) ||                      /* if not opened / supported         */
    (!(mCap.dwSupport & MIDICAPS_CACHE)))
  return FALSE;                         /* pass back error                   */

MMRESULT res = midiOutCachePatches(hDev,
                                   uBank,
                                   lpwPatchArray,
                                   uFlags);
#ifdef _DEBUG
if (res != MMSYSERR_NOERROR)
  TRACE1("CMidiOutDevice::CachePatches:midiOutCachePatches(): %s\n", GetErrorText(res));
#endif

return !res;
}

/*****************************************************************************/
/* CacheDrumPatches : caches drum patches                                    */
/*****************************************************************************/

BOOL CMidiOutDevice::CacheDrumPatches
    (
    UINT uPatch,
    WORD FAR* lpwKeyArray,
    UINT uFlags
    )
{
if ((!IsOpen()) ||                      /* if not opened / supported         */
    (!(mCap.dwSupport & MIDICAPS_CACHE)))
  return FALSE;                         /* pass back error                   */

MMRESULT res = midiOutCacheDrumPatches(hDev,
                                       uPatch,
                                       lpwKeyArray,
                                       uFlags);
#ifdef _DEBUG
if (res != MMSYSERR_NOERROR)
  TRACE1("CMidiOutDevice::CacheDrumPatches:midiOutCacheDrumPatches(): %s\n", GetErrorText(res));
#endif

return !res;
}

/*****************************************************************************/
/* Output : sends MIDI data to output                                        */
/*****************************************************************************/

BOOL CMidiOutDevice::Output
    (
    CMidiMsg &Msg,
    BOOL bSync
    )
{
if (!IsOpen())                          /* if not opened                     */
  return FALSE;                         /* pass back error                   */

MMRESULT res;
switch (Msg.Length())                   /* which length has the message ?    */
  {
  case 0 :                              /* none ?                            */
    return FALSE;                       /* pass back error                   */
  case 1 :                              /* short message ?                   */
  case 2 :
  case 3 :
    {
    BYTE bRS = Msg[0] & 0xf0;           /* get running status                */
    BYTE bCh = Msg[0] & 0x0f;
    if ((bRS == 0x80) &&                /* if sending NOTE OFF on running chn*/
        (RunningStatus == (0x90|bCh)))  /* and running status is NOTE ON     */
      {
      Msg[0] = 0x90 | bCh;              /* make it a NOTE ON                 */
      Msg[2] = 0;                       /* with velocity 0                   */
      }
    if (bRS < 0xf0)                     /* if no realtime / system message   */
      RunningStatus = Msg[0];           /* remember running status           */
    res = midiOutShortMsg(hDev, Msg);
#ifdef _DEBUG
    if (res != MMSYSERR_NOERROR)
      TRACE1("CMidiOutDevice::Output:midiOutShortMsg(): %s\n", GetErrorText(res));
#endif
    return !res;
    }
  default :
    {
                                        /* calculate maximum send time in ms */
    DWORD dwMaxTime = Msg.Length() + 2000; /* (rather gracious :-)           */
    UINT rc = midiOutPrepareHeader(hDev, Msg, sizeof(MIDIHDR));
    if (!rc)
      {
      rc = midiOutLongMsg(hDev, Msg, sizeof(MIDIHDR));
#ifdef _DEBUG
      if (rc != MMSYSERR_NOERROR)
        TRACE1("CMidiOutDevice::Output:midiOutLongMsg(): %s\n", GetErrorText(rc));
#endif
      if (bSync)
        {
        DWORD dwStamp = timeGetTime();
        while ((!rc) && (!SendingDone(Msg)))
          {
          Sleep(0);
          if ((timeGetTime() - dwStamp) > dwMaxTime)
            {
            TRACE1("CMidiOutDevice::Output: timeout in midiOutLongMsg(%d)!\n", Msg.Length());
            Reset();
            rc = MIDIERR_NOTREADY;
            }
          }
        res = midiOutUnprepareHeader(hDev, Msg, sizeof(MIDIHDR));
#ifdef _DEBUG
        if (res != MMSYSERR_NOERROR)
          TRACE1("CMidiOutDevice::Output:midiOutUnprepareHeader(): %s\n", GetErrorText(res));
#endif
        }
      else                              /* if asynchronous output            */
        Msg.Output(hDev);               /* just remember the device handle   */
      }
#ifdef _DEBUG
    else
      TRACE1("CMidiOutDevice::Output:midiOutPrepareHeader(): %s\n", GetErrorText(rc));
#endif

    return !rc;
    }
  }
}

/*****************************************************************************/
/* GetErrorText : retrieves the error text for a specific return code        */
/*****************************************************************************/

LPCSTR CMidiOutDevice::GetErrorText(MMRESULT wError)
{
szErrorText[0] = '\0';
midiOutGetErrorText(wError, szErrorText, sizeof(szErrorText));
return szErrorText;
}

/*****************************************************************************/
/* NoteOn : sends a NOTE ON to output                                        */
/*****************************************************************************/

BOOL CMidiOutDevice::NoteOn
    (
    int channel,
    int note,
    int vel
    )
{
CMidiMsg msg(STATUS_NOTEON | (channel & 0x0f), note, vel);
return Output(msg);
}

/*****************************************************************************/
/* NoteOff : sends a NOTE OFF to output                                      */
/*****************************************************************************/

BOOL CMidiOutDevice::NoteOff
    (
    int channel,
    int note,
    int vel
    )
{
CMidiMsg msg(STATUS_NOTEOFF | (channel & 0x0f), note, vel);
return Output(msg);
}

/*****************************************************************************/
/* KeyAftertouch : sends a Key Aftertouch to output                          */
/*****************************************************************************/

BOOL CMidiOutDevice::KeyAftertouch
    (
    int channel,
    int note,
    int vel
    )
{
CMidiMsg msg(STATUS_POLYPHONICKEY | (channel & 0x0f), note, vel);
return Output(msg);
}

/*****************************************************************************/
/* ChannelAftertouch : sends a channel aftertouch to output                  */
/*****************************************************************************/

BOOL CMidiOutDevice::ChannelAftertouch
    (
    int channel,
    int vel
    )
{
CMidiMsg msg(STATUS_CHANNELPRESSURE | (channel & 0x0f), vel);
return Output(msg);
}

/*****************************************************************************/
/* Controller : sends a 7-bit controller to output                           */
/*****************************************************************************/

BOOL CMidiOutDevice::Controller
    (
    int channel,
    int ctrl,
    int vel
    )
{
CMidiMsg msg(STATUS_CONTROLCHANGE | (channel & 0x0f), ctrl, vel);
return Output(msg);
}

/*****************************************************************************/
/* Controller14 : sends a 14-bit controller to output                        */
/*****************************************************************************/

BOOL CMidiOutDevice::Controller14
    (
    int channel,
    int ctrl,
    int vel
    )
{
CMidiMsg msg(STATUS_CONTROLCHANGE | (channel & 0x0f), ctrl, (vel >> 7));
CMidiMsg msg2(STATUS_CONTROLCHANGE | (channel & 0x0f), ctrl+0x20, (vel & 0x7f));
if (Output(msg))
  return Output(msg2);
return FALSE;
}

/*****************************************************************************/
/* Patch : sends a patch change to output                                    */
/*****************************************************************************/

BOOL CMidiOutDevice::Patch
    (
    int channel,
    int patch
    )
{
CMidiMsg msg(STATUS_PROGRAMCHANGE | (channel & 0x0f), patch);
return Output(msg);
}

/*****************************************************************************/
/* PitchBend : sends pitch bend data to output                               */
/*****************************************************************************/

BOOL CMidiOutDevice::PitchBend
    (
    int channel,
    int val
    )
{
CMidiMsg msg(STATUS_PITCHBEND | (channel & 0x0f), (val & 0x7f), (val >> 7));
return Output(msg);
}

/*****************************************************************************/
/* SystemReset : sends system reset message to output                        */
/*****************************************************************************/

BOOL CMidiOutDevice::SystemReset ( void )
{
CMidiMsg msg((DWORD)STATUS_SYSTEMRESET);
return Output(msg);
}

/*****************************************************************************/
/* Bank : activates another bank on the device                               */
/*****************************************************************************/

BOOL CMidiOutDevice::Bank
    (
    int channel, 
    int bank
    )
{ 
return Controller(channel, 0, bank);
};

/*****************************************************************************/
/* Bank14 : activates another bank on the device with 14-bit resolution      */
/*****************************************************************************/

BOOL CMidiOutDevice::Bank14
    (
    int channel, 
    int bank
    )
{
return Controller14(channel, 0, bank);
};
