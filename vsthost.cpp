/*****************************************************************************/
/* vsthost.cpp : Defines the class behaviors for the application.            */
/*****************************************************************************/

#include "stdafx.h"
#include "vsthost.h"

#include "MainFrm.h"
#include "ChildFrm.h"

#include "specmidi.h"
#include "mididev.h"
#include "specwave.h"
#include "wavedev.h"
#include "WorkThread.h"
#include "SpecDSound.h"
#include "midikeybdlg.h"

#ifdef __ASIO_H
#include "SpecAsioHost.h"
#include "AsioChannelSelectDialog.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*****************************************************************************/
/* CAboutDlg dialog used for App About                                       */
/*****************************************************************************/

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*===========================================================================*/
/* CMyCommandLineInfo : command line information class                       */
/*===========================================================================*/

class CMyCommandLineInfo : public CCommandLineInfo
{
public:
    CMyCommandLineInfo()
      {
      bNoLoad = FALSE;
      bNoSave = FALSE;
      }
	virtual void ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast);
    BOOL bNoLoad;
    BOOL bNoSave;
};

/*****************************************************************************/
/* ParseParam : called to parse parameters                                   */
/*****************************************************************************/

void CMyCommandLineInfo::ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast)
{
if (bFlag)
  {
  CString sParam(lpszParam);
  if (!sParam.CompareNoCase("noload"))
    bNoLoad = TRUE;
  else if (!sParam.CompareNoCase("nosave"))
    bNoSave = TRUE;
  }
CCommandLineInfo::ParseParam(lpszParam, bFlag, bLast);
}

/*===========================================================================*/
/* CVsthostApp class members                                                 */
/*===========================================================================*/

/*****************************************************************************/
/* CVsthostApp message map                                                   */
/*****************************************************************************/

BEGIN_MESSAGE_MAP(CVsthostApp, CWinApp)
	//{{AFX_MSG_MAP(CVsthostApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(IDM_MIDIDEV, OnMididev)
	ON_COMMAND(IDM_WAVEDEV, OnWavedev)
	ON_COMMAND(IDM_ENG_START, OnEngStart)
	ON_UPDATE_COMMAND_UI(IDM_ENG_START, OnUpdateEngStart)
	ON_COMMAND(IDM_ENGINE_RESTART, OnEngineRestart)
	ON_UPDATE_COMMAND_UI(IDM_ENGINE_RESTART, OnUpdateEngineRestart)
	ON_COMMAND(IDM_ASIO_CPL, OnAsioCpl)
	ON_UPDATE_COMMAND_UI(IDM_ASIO_CPL, OnUpdateAsioCpl)
	ON_COMMAND(IDM_MIDIKEYB, OnMidikeyb)
	ON_UPDATE_COMMAND_UI(IDM_MIDIKEYB, OnUpdateMidikeyb)
	ON_COMMAND(IDM_MIDIKEYB_PROPERTIES, OnMidikeybProperties)
	ON_UPDATE_COMMAND_UI(IDM_MIDIKEYB_PROPERTIES, OnUpdateMidikeybProperties)
	ON_COMMAND(IDM_ASIO_CHN, OnAsioChn)
	ON_UPDATE_COMMAND_UI(IDM_ASIO_CHN, OnUpdateAsioChn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*****************************************************************************/
/* CVsthostApp : constructor                                                 */
/*****************************************************************************/

CVsthostApp::CVsthostApp()
{
bEngRunning = FALSE;
nTypeWIn = nTypeWOut = -1;
pMidiKeyb = 0;
}

/*****************************************************************************/
/* The one and only CVsthostApp object                                       */
/*****************************************************************************/

CVsthostApp theApp;

/*****************************************************************************/
/* InitInstance : application initialization                                 */
/*****************************************************************************/

BOOL CVsthostApp::InitInstance()
{
// Allow XP Look & Feel
InitCommonControls();
CWinApp::InitInstance();

if (FAILED(CoInitialize(NULL)))
  return FALSE;
AfxEnableControlContainer();
 
#if 0
SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#endif

// Standard initialization
// If you are not using these features and wish to reduce the size
//  of your final executable, you should remove from the following
//  the specific initialization routines you do not need.

#ifdef _AFXDLL
Enable3dControls();			// Call this when using MFC in a shared DLL
#else
Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

// Change the registry key under which our settings are stored.
SetRegistryKey(_T("Seib"));

#if 0                                   /* change for dynamic profiles!      */
// dynamic profile based on application name
char szAppName[_MAX_PATH];
::GetModuleFileName(NULL,szAppName,sizeof(szAppName));
char *absl = strrchr(szAppName, '\\');
char *extn = strrchr(szAppName, '.');
if (extn > absl)
  *extn = '\0';
free((void *)m_pszProfileName);
m_pszProfileName = strdup(absl ? absl + 1 : szAppName);
#endif

CMyCommandLineInfo cmdInfo;
ParseCommandLine(cmdInfo);
bNoSave = cmdInfo.bNoSave;

#ifdef __ASIO_H
pAsioHost = new CSpecAsioHost;
#else
pAsioHost = 0;
#endif

// To create the main window, this code creates a new frame window
// object and then sets it as the application's main window object.

CMDIFrameWnd* pFrame = new CMainFrame;
m_pMainWnd = pFrame;

// create main MDI frame window
if (!pFrame->LoadFrame(IDR_MAINFRAME))
  return FALSE;

// try to load shared MDI menus and accelerator table
//TODO: add additional member variables and load calls for
//	additional menu types your application may need. 

HINSTANCE hInst = AfxGetResourceHandle();
m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_VSTHOSTYPE));
m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_VSTHOSTYPE));

// The main window has been initialized, so show and update it.
pFrame->ShowWindow(m_nCmdShow);
pFrame->UpdateWindow();

wTID = 0;                               /* reset HR timer ID                 */
timeGetDevCaps(&tc, sizeof(tc));        /* get timer capabilities            */
for (int i = 0; i < 2; i++)             /* prepare empty buffers             */
  {
  emptyBuf[i] = new float[44100];
  if (emptyBuf[i])
    for (int j = 0; j < 44100; j++)
      emptyBuf[i][j] = 0.f;
  }

CWaveOutDeviceList ol;
sWaveMapper = CString("MME: ") + ol[0];

                                        /* create work thread                */
pWorkThread = new CWorkThread(&vstHost);

int nBufSz = GetProfileInt("Settings", "WaveOutBufSize", 4410);
                                        /* open outputs device               */
MidiOut.Open(GetProfileString("Settings", "MidiOut"));
LoadWaveOutDevice(GetProfileString("Settings", "WaveOut", sWaveMapper),
                  nBufSz);

#ifdef __ASIO_H
if (pAsioHost)                          /* if ASIO host there, pass thread   */
  {
  pAsioHost->SetWorkThread(pWorkThread);
  if (pAsioHost->IsLoaded())
    SetAsioChannels(GetProfileString("Settings", "AsioChnIn"),
                    GetProfileString("Settings", "AsioChnOut"));
  }
#endif
                                        /* open input devices                */
MidiIn.Open(GetProfileString("Settings", "MidiIn"),
            OPENDEFAULT,
            OPENDEFAULT,
            CALLBACK_FUNCTION);
MidiIn.SetMidiThru(GetProfileInt("Settings", "MidiThru", FALSE));

if (GetProfileString("Settings", "WaveOut", sWaveMapper).
        Left(6).CompareNoCase("ASIO: "))
  LoadWaveInDevice(GetProfileString("Settings", "WaveIn", sWaveMapper),
                   nBufSz);

if (!cmdInfo.bNoLoad)                   /* if not disabled,                  */
  LoadSetup();                          /* load setup                        */

                                        /* if called with effect name, open  */
if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
  LoadEffect(cmdInfo.m_strFileName);

if (GetProfileInt("Settings", "Engine", 0))
  EngineStart();

return TRUE;
}

/*****************************************************************************/
/* ExitInstance : called when the application terminates                     */
/*****************************************************************************/

int CVsthostApp::ExitInstance() 
{
FullStop();                             /* stop engine and close devices     */

if (!bNoSave)                           /* if not disabled,                  */
  SaveSetup();                          /* save current setup                */

vstHost.RemoveAll();                    /* remove all loaded effects         */

if (pMidiKeyb)                          /* if MIDI keyboard there            */
  delete pMidiKeyb;                     /* delete it                         */

for (int i = 0; i < 2; i++)             /* delete empty buffers              */
  if (emptyBuf[i])
    delete[] emptyBuf[i];

#ifdef __ASIO_H
if (pAsioHost)
  delete pAsioHost;
#endif

if (pWorkThread)
  delete pWorkThread;

if (m_hMDIMenu != NULL)
  FreeResource(m_hMDIMenu);
if (m_hMDIAccel != NULL)
  FreeResource(m_hMDIAccel);

TRACE0("VSTHost ended.\n");
return CWinApp::ExitInstance();
}

/*****************************************************************************/
/* OnFileNew : called when File / New is selected                            */
/*****************************************************************************/

void CVsthostApp::OnFileNew() 
{
CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);

CString sStartAt;
sStartAt = GetProfileString("Load", "Path");
if (sStartAt.GetLength())
  sStartAt += "\\*.*";

CFileDialog dlg(TRUE, NULL, sStartAt);

if (dlg.DoModal() == IDOK)
  {
  sStartAt = dlg.GetPathName();
  sStartAt = sStartAt.Left(sStartAt.ReverseFind('\\'));
  WriteProfileString("Load", "Path", sStartAt);
  LoadEffect(dlg.GetPathName());
  }
}

/*****************************************************************************/
/* OnAppAbout : called to display the About... dialog                        */
/*****************************************************************************/

void CVsthostApp::OnAppAbout()
{
CAboutDlg aboutDlg;
aboutDlg.DoModal();
}

/*****************************************************************************/
/* FillPopup : fills a popup menu with an effect's programs                  */
/*****************************************************************************/

void CVsthostApp::FillPopup(CMenu* pPopupMenu, int nEffect)
{
int i, j;

CEffect *pEffect = vstHost.GetAt(nEffect);
                                        /* remove all old items              */
for (i = pPopupMenu->GetMenuItemCount() - 1; i >= 2; i--)
  {
  if ((pPopupMenu->GetSubMenu(i)) ||
      (pPopupMenu->GetMenuItemID(i) ==
           IDM_EFF_PROGRAM_0))
    pPopupMenu->RemoveMenu(i, MF_BYPOSITION);
  }

if ((pEffect) &&
    (pEffect->pEffect->numPrograms > 1))
  {
  long lProg = pEffect->EffGetProgram();
  bool bProgSet = false;

  int nClass = (pEffect->pEffect->flags & effFlagsIsSynth) ?
      kPlugCategSynth :
      kPlugCategUnknown;
  for (i = 0; i < pEffect->pEffect->numPrograms; i += 16)
    {
    CMenu popup;
    popup.CreatePopupMenu();
    for (j = i; (j < i + 16) && (j < pEffect->pEffect->numPrograms); j++)
      {
      CString sProg;
      char szPgName[256] = "";
      if (!pEffect->EffGetProgramNameIndexed(nClass, j, szPgName))
        {
        bProgSet = true;
        pEffect->EffSetProgram(j);
        pEffect->EffGetProgramName(szPgName);
        if (!*szPgName)
         sprintf(szPgName, "Program %d", j);
        }
      sProg.Format("%d. %s", j, szPgName);
      popup.AppendMenu(MF_STRING, IDM_EFF_PROGRAM_0 + j, sProg);
      }
    CString sSub;
    sSub.Format("Programs %d-%d", i, j-1);
    pPopupMenu->AppendMenu(MF_POPUP | MF_STRING,
                           (UINT)popup.Detach(),
                           sSub);
    }
  if (bProgSet)
    pEffect->EffSetProgram(lProg);
  pPopupMenu->CheckMenuItem(IDM_EFF_PROGRAM_0 + lProg,
                            MF_CHECKED | MF_BYCOMMAND);
  }
}

/*****************************************************************************/
/* LoadEffect : loads a new effect                                           */
/*****************************************************************************/

BOOL CVsthostApp::LoadEffect(LPCSTR lpszFileName, int *pnEffect)
{
BOOL bWasRunning = bEngRunning;         /* get current engine state          */

EngineStop();                           /* stop engine                       */

if (pnEffect)                           /* initialize effect ID if necessary */
  *pnEffect = -1;
int nEffect = vstHost.LoadPlugin(lpszFileName);

if (bWasRunning)                        /* if it was running before,         */
  EngineStart();                        /* restart engine                    */

if (nEffect >= 0)
  {
  // create a new MDI child window
  CChildFrame *pChild = (CChildFrame *)CreateChild(RUNTIME_CLASS(CChildFrame),
                                                   IDR_VSTHOSTYPE);
  if (pChild)
    {
    pChild->SetEffect(nEffect);
    if (pnEffect)
      *pnEffect = nEffect;
    return TRUE;
    }
  }

return FALSE;
}

/*****************************************************************************/
/* CreateChild : creats a new MDI child window                               */
/*****************************************************************************/

CMDIChildWnd * CVsthostApp::CreateChild
    (
    CRuntimeClass *pClass,
    UINT nResource,
    HMENU hMenu,
    HACCEL hAccel
    )
{
if (!hMenu)
  hMenu = m_hMDIMenu;
if (!hAccel)
  hAccel = m_hMDIAccel;
CMDIFrameWnd *pMain = (CMDIFrameWnd*)m_pMainWnd;
CMDIChildWnd *pChild = pMain->CreateNewChild(pClass,
      nResource,
      hMenu,
      hAccel);
if (pChild)
  pMain->MDIActivate(pChild);
return pChild;
}

/*****************************************************************************/
/* OnMididev : called to determine the used MIDI devices                     */
/*****************************************************************************/

void CVsthostApp::OnMididev() 
{
CMidiDeviceDialog dlg(AfxGetMainWnd());

dlg.InName = GetProfileString("Settings", "MidiIn");
dlg.OutName = GetProfileString("Settings", "MidiOut");
dlg.bPassThru = GetProfileInt("Settings", "MidiThru", FALSE);
if (dlg.DoModal() == IDOK)
  {
  CWaitCursor wc;                       /* display wait cursor               */
  BOOL bWasRunning = bEngRunning;       /* remember old setting              */
  EngineStop();                         /* stop input engine                 */

  if (!MidiIn.Open(dlg.InName,          /* open input device                 */
                  OPENDEFAULT,
                  OPENDEFAULT,
                  CALLBACK_FUNCTION))
    dlg.InName = "";
  if (!MidiOut.Open(dlg.OutName))       /* open output device                */
    dlg.OutName = "";
  MidiIn.SetMidiThru(dlg.bPassThru);    /* set midi thru                     */
  WriteProfileString("Settings", "MidiIn", dlg.InName);
  WriteProfileString("Settings", "MidiOut", dlg.OutName);
  WriteProfileInt("Settings", "MidiThru", dlg.bPassThru);

  if (bWasRunning)                      /* if it was running before,         */
    EngineStart();                      /* start input engine                */
  }
}

/*****************************************************************************/
/* TimerCallback : high-resolution timer entry point                         */
/*****************************************************************************/

void CALLBACK CVsthostApp::TimerCallback
    (
    UINT uID,
    UINT uMsg,
    DWORD dwUser,
    DWORD dw1,
    DWORD dw2
    )
{
if (dwUser)                             /* pass it on to real app. object    */
  ((CVsthostApp *)dwUser)->HRTimer();
}

/*****************************************************************************/
/* SetTimer : starts / stops a high-resolution timer if no Wave In device    */
/*****************************************************************************/

BOOL CVsthostApp::SetTimer(UINT nMSecs)
{
if ((!nMSecs) && (wTID))                /* if killing a running timer        */
  {
  timeKillEvent(wTID);
  wTID = 0;
  timeEndPeriod(tc.wPeriodMin);
  nTimerMSecs = 0;
  return TRUE;
  }
else                                    /* if starting a HR timer            */
  {
  if (nMSecs < tc.wPeriodMin)           /* look whether possible             */
    return FALSE;

  nTimerMSecs = nMSecs;                 /* remember requested period         */
  nMSecs /= 10;                         /* set HR timer to 1/10 of that      */
  if (nMSecs < tc.wPeriodMin)           /* prohibit going TOO low            */
    nMSecs = tc.wPeriodMin;

  timeBeginPeriod(nMSecs);              /* start using timer services        */
  dwLastTime = timeGetTime();           /* setup for correct start           */
  dwRest = 0;
  wTID = timeSetEvent(                  /* start the timer procedure         */
      nMSecs,          
      (nMSecs > 1) ? nMSecs / 2 : 1,
      TimerCallback,
      (DWORD)this,
      TIME_PERIODIC);
  return !!wTID;                        /* return whether started            */
  }
}

/*****************************************************************************/
/* HRTimer : called when the high-res timer ticks                            */
/*****************************************************************************/

void CVsthostApp::HRTimer()
{
static DWORD dwCurTime;                 /* current time                      */
static DWORD dwOffset;                  /* time offset                       */
static WORD  wClocks;                   /* # clock ticks to send             */
static WORD  i;                         /* clock tick counter                */

dwCurTime = timeGetTime();              /* get current time                  */
dwOffset = dwCurTime -                  /* calculate #msecs since last msg   */
           dwLastTime;
if (dwOffset > dwCurTime)               /* if overflow                       */
  {
  dwLastTime = 0;                       /* reset start tick                  */
  dwOffset = dwCurTime;                 /* this may cause one hiccup every   */
  }                                     /* 49 days... horrible...            */

dwOffset += dwRest;                     /* add eventual rest                 */
if (dwOffset > nTimerMSecs)             /* if another timer tick's due       */
  {
  dwLastTime = dwCurTime;               /* remember this tick                */
  dwRest = dwOffset - nTimerMSecs;      /* calculate new rest                */
  pWorkThread->Process(emptyBuf,        /* process empty samples             */
                       (44100 * nTimerMSecs / 1000),
                       2,
                       dwCurTime - dwStartStamp);
  }
}

/*****************************************************************************/
/* LoadWaveInDevice : loads a Wave In device                                 */
/*****************************************************************************/

BOOL CVsthostApp::LoadWaveInDevice(CString sDevice, int nBufSz)
{
BOOL bRC = FALSE;
int nDevType = 0;

TRACE2("LoadWaveInDevice(\"%s\",%d)\n", (LPCSTR)sDevice, nBufSz);

                                        /* either run with Wave In           */
if (!sDevice.Left(5).CompareNoCase("MME: "))
  sDevice = sDevice.Mid(5);
else if (!sDevice.Left(4).CompareNoCase("DS: "))
  {
  nDevType = 1;
  sDevice = sDevice.Mid(4);
  }
else if (!sDevice.Left(6).CompareNoCase("ASIO: "))
  {
  nDevType = 2;
  sDevice = sDevice.Mid(6);
  }
nTypeWIn = nDevType;
switch (nDevType)
  {
  case 0 :                              /* MME ?                             */
    WaveIn.SetupAllocSize(44100 / nBufSz, nBufSz);
    bRC = WaveIn.Open(sDevice,
                      NULL,
                      WAVEOPENDEFAULT,
                      WAVEOPENDEFAULT,
                      CALLBACK_EVENT);
    WaveIn.SetWorkThread(pWorkThread);
    break;
  case 1 :                              /* DirectSound ?                     */
#pragma message("DirectSound Input Device not fully coded!")    
    bRC = DSoundIn.Open((LPCSTR)sDevice, DSCBCAPS_WAVEMAPPED, nBufSz, NULL);
    DSoundIn.SetWorkThread(pWorkThread);
    break;
  case 2 :                              /* ASIO ?                            */
    // done in LoadWaveOutDevice()
    break;
  }

TRACE1("    -> %sOK\n", bRC?"":"NOT ");

return bRC;
}

/*****************************************************************************/
/* LoadWaveOutDevice : loads a Wave Out device                               */
/*****************************************************************************/

BOOL CVsthostApp::LoadWaveOutDevice
    (
    CString sDevice,
    int &nBufSz
    )
{
BOOL bRC = FALSE;
int nDevType = 0;

TRACE2("LoadWaveOutDevice(\"%s\",%d)\n", (LPCSTR)sDevice, nBufSz);

if (!sDevice.Left(5).CompareNoCase("MME: "))
  sDevice = sDevice.Mid(5);
else if (!sDevice.Left(4).CompareNoCase("DS: "))
  {
  nDevType = 1;
  sDevice = sDevice.Mid(4);
  }
else if (!sDevice.Left(6).CompareNoCase("ASIO: "))
  {
  nDevType = 2;
  sDevice = sDevice.Mid(6);
  }
nTypeWOut = nDevType;
switch (nDevType)
  {
  case 0 :                              /* MME ?                             */
    DSoundOut.Close();
#ifdef __ASIO_H
    if ((pAsioHost) && (pAsioHost->IsLoaded()))
      pAsioHost->UnloadDriver();
#endif
    bRC = WaveOut.Open(sDevice,
                 NULL,
                 WAVEOPENDEFAULT,
                 WAVEOPENDEFAULT,
                 CALLBACK_EVENT);
    WaveOut.AllocateBuffers(44100 / nBufSz);
    break;
  case 1 :                              /* DirectSound ?                     */
    WaveOut.Close();
#ifdef __ASIO_H
    if ((pAsioHost) && (pAsioHost->IsLoaded()))
      pAsioHost->UnloadDriver();
#endif
    bRC = DSoundOut.Open(sDevice,
                         DSBCAPS_CTRLDEFAULT |
                             DSBCAPS_CTRLPOSITIONNOTIFY |
                             DSBCAPS_GETCURRENTPOSITION2 |
                             DSBCAPS_GLOBALFOCUS,
                         nBufSz, NULL);
    break;
  case 2 :                              /* ASIO ?                            */
    WaveOut.Close();
    DSoundOut.Close();
#ifdef __ASIO_H
    if (pAsioHost)
      bRC = pAsioHost->LoadDriver((char *)(LPCSTR)sDevice, nBufSz);
    if (bRC)                            /* if load went OK,                  */
      {                                 /* make sure to use the correct      */
      ASIOSampleRate cRate;             /* sample rate...                    */
      pAsioHost->GetSampleRate(&cRate);
      vstHost.SetSampleRate((float)cRate);
      }
#endif
    break;
  }

TRACE2("    -> %sOK, buffer size=%d\n", bRC?"":"NOT ", nBufSz);

vstHost.SetBlockSize(nBufSz);
return bRC;
}

/*****************************************************************************/
/* SetAsioChannels : sets stereo channels                                    */
/*****************************************************************************/

bool CVsthostApp::SetAsioChannels(LPCSTR lpszChnIn, LPCSTR lpszChnOut)
{
#ifdef __ASIO_H
if ((pAsioHost) && (pAsioHost->IsLoaded()))
  {
  CString sIn(lpszChnIn), sOut(lpszChnOut);
  long i, lIn = 0, lOut = 0, lCur = -1;
  pAsioHost->GetChannels(&lIn, &lOut);
  ASIOChannelInfo info = {0};
  long lCurIn = -1, lCurOut = -1;

  info.isInput = ASIOTrue;
  for (i = 0; i < lIn; i++)
    {
    info.channel = i;
    pAsioHost->GetChannelInfo(&info);
    if (lCurIn < 0)                     /* if not started yet                */
      {
      if (sIn.IsEmpty())                /* if simply going for default       */
        pAsioHost->SetActiveChannel(i, true);
      else if (!sIn.CompareNoCase(info.name))
        {
        pAsioHost->SetActiveChannel(i, true);
        lCurIn++;
        }
      else
        pAsioHost->SetActiveChannel(i, false);
      }
    else if (lCurIn >= 1)               /* if already found all              */
      pAsioHost->SetActiveChannel(i, false);
    else                                /* if one of the selected channels   */
      {
      pAsioHost->SetActiveChannel(i, true);
      lCurIn++;
      }
    }

  info.isInput = ASIOFalse;
  for (i = 0; i < lOut; i++)
    {
    info.channel = i;
    pAsioHost->GetChannelInfo(&info);
    if (lCurOut < 0)                    /* if not started yet                */
      {
      if (sOut.IsEmpty())               /* if simply going for default       */
        pAsioHost->SetActiveChannel(lIn + i, true);
      else if (!sOut.CompareNoCase(info.name))
        {
        pAsioHost->SetActiveChannel(lIn + i, true);
        lCurOut++;
        }
      else
        pAsioHost->SetActiveChannel(lIn + i, false);
      }
    else if (lCurOut >= 1)              /* if already found all              */
      pAsioHost->SetActiveChannel(lIn + i, false);
    else                                /* if one of the selected channels   */
      {
      pAsioHost->SetActiveChannel(lIn + i, true);
      lCurOut++;
      }
    }
  return true;
  }
return false;
#endif
}

/*****************************************************************************/
/* OnWavedev : called to configure the wave devices                          */
/*****************************************************************************/

void CVsthostApp::OnWavedev()
{
CWaveDeviceDialog dlg(AfxGetMainWnd());

dlg.InName = GetProfileString("Settings", "WaveIn", sWaveMapper);
dlg.OutName = GetProfileString("Settings", "WaveOut", sWaveMapper);
dlg.nBufSize = GetProfileInt("Settings", "WaveOutBufSize", 4410);
if (dlg.DoModal() == IDOK)
  {
  CWaitCursor wc;                       /* display wait cursor               */
  BOOL bWasRunning = bEngRunning;       /* remember old setting              */
  EngineStop();                         /* stop input engine                 */

  if (dlg.OutName.Left(6).CompareNoCase("ASIO: "))
    {
    TRACE0("#(re-)Open Wave Input Device\n");
                                        /* open input device                 */
    if (!LoadWaveInDevice(dlg.InName, dlg.nBufSize))
      dlg.InName = "";                  /* upon error reset name             */
    }
  else
    {
    WaveIn.Close();
    DSoundIn.Close();
    }
  WriteProfileString("Settings", "WaveIn", dlg.InName);

  TRACE0("#(re-)Open Wave Output Device\n");
                                        /* open output device                */
  if (!LoadWaveOutDevice(dlg.OutName, dlg.nBufSize))
    dlg.OutName = "";
  WriteProfileString("Settings", "WaveOut", dlg.OutName);
  WriteProfileInt("Settings", "WaveOutBufSize", dlg.nBufSize);

  if (bWasRunning)                      /* if it was running before,         */
    EngineStart();                      /* start input engine                */
  }
}

/*****************************************************************************/
/* SetupLatency : we need some latency to make the system work without hiccup*/
/*****************************************************************************/

void CVsthostApp::SetupLatency()
{
if (!WaveOut.IsOpen())                  /* only do if output open            */
  return;

#if 0
// works without that as well.
for (int i = 0; i < 2; i++)             /* prepare a bit                     */
  pWorkThread->Process(emptyBuf, 2205); /* of empty samples                  */
#endif
}

/*****************************************************************************/
/* EngineStop : stops eventual running input engine                          */
/*****************************************************************************/

void CVsthostApp::EngineStop()
{
if (!bEngRunning)
  return;

TRACE0("Stopping VSTHost engine\n");

SetTimer(0);                            /* close eventually opened HR timer  */
WaveIn.Stop();                          /* stop eventual record mode         */
DSoundIn.Stop();
MidiIn.Stop();

TRACE0("#  Resetting Wave input device\n");
WaveIn.Reset();                         /* and make sure it doesn't pump on  */
TRACE0("#  Resetting MIDI input device\n");
MidiIn.Reset();
TRACE0("#    Both devices reset.\n");

#ifdef __ASIO_H
if (pAsioHost)
  {
  TRACE0("#  Stopping ASIO Host\n");
  pAsioHost->Stop();
  }
#endif

int nSleeps = 0;
int nWaitIn = WaveIn.WaitingBuffers();
int nWaitOut = WaveOut.WaitingBuffers();
while (nWaitIn || nWaitOut)             /* wait until all work done          */
  {
  TRACE2("#  Waiting for buffer completion (%d in, %d out)\n",
         WaveIn.WaitingBuffers(),
         WaveOut.WaitingBuffers());
  Sleep (50);
  nWaitIn = WaveIn.WaitingBuffers();
  nWaitOut = WaveOut.WaitingBuffers();
  if (++nSleeps > 20)
    {
    TRACE("#  Waited over a second; wait aborted.\n");
    break;
    }
  }
TRACE0("#VSTHost Engine stopped.\n");
bEngRunning = FALSE;
}

/*****************************************************************************/
/* EngineStart : restarts input engine                                       */
/*****************************************************************************/

void CVsthostApp::EngineStart()
{
if (bEngRunning)
  return;

TRACE0("Starting VSTHost engine\n");

int nTimer = WaveOut.AllocatedBuffers();
if (nTimer)
  nTimer = 1000 / nTimer;
else
  nTimer = 44100 / GetProfileInt("Settings", "WaveOutBufSize", 4410);

#ifdef __ASIO_H
if ((nTypeWOut == 2) &&                 /* if ASIO Host Output               */
    pAsioHost &&
    pAsioHost->IsLoaded())
  {
  TRACE0("  Starting ASIO Host\n");
  pAsioHost->Start();
  dwStartStamp = timeGetTime();         /* put common timestamp reference    */
  }
else
  {
#endif
  
  SetupLatency();                       /* setup latency                     */

  switch (nTypeWIn)                     /* which kind of input device ?      */
    {
    case 0 :                            /* MME Wave Input device ?           */
      TRACE0("  Starting MME Wave Input Device\n");
      if (!WaveIn.Start())              /* start recording (again)           */
        SetTimer(nTimer);               /* upon error start HR timer         */
      dwStartStamp = timeGetTime();     /* put common timestamp reference    */
      WaveIn.SetStartStamp(dwStartStamp);
      break;
    case 1 :                            /* DirectSound Input Capture device? */
      TRACE0("  Starting DirectSound Capture Device\n");
      if (!DSoundIn.Start())            /* start recording (again)           */
        SetTimer(nTimer);               /* upon error start HR timer         */
      break;
    }

  switch (nTypeWOut)                    /* which kind of output device ?     */
    {
    case 1 :                            /* DirectSound Output Device ?       */
      TRACE0("  Starting DirectSound Output Device\n");
      DSoundOut.Start();                /* start playback (again)            */
      break;
    }
#ifdef __ASIO_H
  }
#endif

TRACE0("  Starting MIDI Input Device\n");
MidiIn.Start();
#if 0
// only works with message-based MIDI input!
MidiIn.SetStartStamp(dwStartStamp);     /* into all input devices            */
#endif

TRACE0("VSTHost engine up again\n");
bEngRunning = TRUE;
}

/*****************************************************************************/
/* FullStop : stops engine and closes devices                                */
/*****************************************************************************/

void CVsthostApp::FullStop()
{
TRACE0("Full stop start.\n");
EngineStop();                           /* stop input engine                 */

TRACE0("  Closing MIDI Devices\n");
MidiIn.Close();                         /* close eventual opened MIDI devices*/
MidiOut.Close();
TRACE0("  Closing Wave Devices\n");
WaveIn.Close();                         /* close eventual opened Wave devices*/
WaveOut.Close();
DSoundIn.Close();
DSoundOut.Close();

#ifdef __ASIO_H
if (pAsioHost)
  {
  TRACE0("  Closing ASIO Host\n");
  pAsioHost->Exit();
  }
#endif

TRACE0("Full stop ended.\n");
}

/*****************************************************************************/
/* OnEngStart : called to toggle the engine run state                        */
/*****************************************************************************/

void CVsthostApp::OnEngStart() 
{
if (bEngRunning)
  EngineStop();
else
  EngineStart();
WriteProfileInt("Settings", "Engine", bEngRunning);
}

/*****************************************************************************/
/* OnUpdateEngStart : called to update the visual appearance                 */
/*****************************************************************************/

void CVsthostApp::OnUpdateEngStart(CCmdUI* pCmdUI) 
{
pCmdUI->SetCheck(bEngRunning);
}

/*****************************************************************************/
/* OnEngineRestart : called when the engine needs a restart                  */
/*****************************************************************************/

void CVsthostApp::OnEngineRestart() 
{
if (bEngRunning)
  {
  EngineStop();
  AfxGetMainWnd()->PostMessage(WM_COMMAND, IDM_ENG_START);
  }
}

/*****************************************************************************/
/* OnUpdateEngineRestart : called to update the visual appearance            */
/*****************************************************************************/

void CVsthostApp::OnUpdateEngineRestart(CCmdUI* pCmdUI) 
{
pCmdUI->Enable(bEngRunning);
}

/*****************************************************************************/
/* OnAsioCpl : displays the ASIO Control Panel, if ASIO driver loaded        */
/*****************************************************************************/

void CVsthostApp::OnAsioCpl() 
{
#ifdef __ASIO_H
if ((pAsioHost) && (pAsioHost->IsLoaded()))
  {
  BOOL bWasRunning = bEngRunning;       /* remember old setting              */
  EngineStop();                         /* stop input engine                 */
  pAsioHost->ControlPanel();
  ASIOSampleRate cRate;
  pAsioHost->GetSampleRate(&cRate);
  vstHost.SetSampleRate((float)cRate);
  if (bWasRunning)                      /* if engine was running before      */
    EngineStart();                      /* restart it                        */
  }
#endif	
}

/*****************************************************************************/
/* OnUpdateAsioCpl : updates the visual appearance                           */
/*****************************************************************************/

void CVsthostApp::OnUpdateAsioCpl(CCmdUI* pCmdUI) 
{
#ifdef __ASIO_H
pCmdUI->Enable((pAsioHost) && (pAsioHost->IsLoaded()));
#else
pCmdUI->Enable(FALSE);
#endif
}

/*****************************************************************************/
/* OnAsioChn : called to select the used ASIO channels (only 2 in VSTHost)   */
/*****************************************************************************/

void CVsthostApp::OnAsioChn() 
{
#ifdef __ASIO_H
if ((pAsioHost) && (pAsioHost->IsLoaded()))
  {
  // NOTE: the following logic is relatively unrefined; it simply ignores
  // the ASIO concept of channel groups which *might* lead to strange
  // results if, for example, there's a Group 0 with an odd number of
  // channels followed by a Group 1...

  CAsioChannelSelectDialog dlg;

  CString sNoChannel;
  sNoChannel.LoadString(IDS_NOCHANNEL);
  dlg.saIn.Add(sNoChannel);
  dlg.saOut.Add(sNoChannel);
  CString sCurIn, sCurOut, sCur;
  sCurIn = GetProfileString("Settings", "AsioChnIn");
  sCurOut = GetProfileString("Settings", "AsioChnOut");

  long i, lIn = 0, lOut = 0, lCurIn = 0, lCurOut = 0;
  pAsioHost->GetChannels(&lIn, &lOut);
  ASIOChannelInfo info[2] = {0};

  dlg.nSelIn = 0;
  info[0].isInput = info[1].isInput = ASIOTrue;
  for (i = 0; i < lIn - 1; i += 2)
    {
    info[0].channel = i;
    info[1].channel = i + 1;
    pAsioHost->GetChannelInfo(&info[0]);
    pAsioHost->GetChannelInfo(&info[1]);
    if (!sCurIn.CompareNoCase(info[0].name))
      dlg.nSelIn = dlg.saIn.GetSize();
    dlg.saIn.Add(CString(info[0].name) +
                 " + " +
                 CString(info[1].name));
    }
  dlg.nSelOut = 0;
  info[0].isInput = info[1].isInput = ASIOFalse;
  for (i = 0; i < lOut - 1; i += 2)
    {
    info[0].channel = i;
    info[1].channel = i + 1;
    pAsioHost->GetChannelInfo(&info[0]);
    pAsioHost->GetChannelInfo(&info[1]);
    if (!sCurOut.CompareNoCase(info[0].name))
      dlg.nSelOut = dlg.saOut.GetSize();
    dlg.saOut.Add(CString(info[0].name) +
                  " + " +
                  CString(info[1].name));
    }

  if (dlg.DoModal() == IDOK)
    {
    dlg.nSelIn--;
    if (dlg.nSelIn >= 0)
      {
      info[0].isInput = ASIOTrue;
      info[0].channel = dlg.nSelIn * 2;
      pAsioHost->GetChannelInfo(&info[0]);
      WriteProfileString("Settings", "AsioChnIn", info[0].name);
      }
    else
      WriteProfileString("Settings", "AsioChnIn", NULL);

    dlg.nSelOut--;
    if (dlg.nSelOut >= 0)
      {
      info[0].isInput = ASIOFalse;
      info[0].channel = dlg.nSelOut * 2;
      pAsioHost->GetChannelInfo(&info[0]);
      WriteProfileString("Settings", "AsioChnOut", info[0].name);
      }
    else
      WriteProfileString("Settings", "AsioChnOut", NULL);

    BOOL bWasRunning = bEngRunning;     /* remember old setting              */
    EngineStop();                       /* stop input engine                 */
    pAsioHost->ClearBuffers();
    SetAsioChannels(GetProfileString("Settings", "AsioChnIn"),
                    GetProfileString("Settings", "AsioChnOut"));
    if (bWasRunning)                    /* if engine was running before      */
      EngineStart();                    /* restart it                        */
    }
  }
#endif	
}

/*****************************************************************************/
/* OnUpdateAsioChn : update the visual appearance                            */
/*****************************************************************************/

void CVsthostApp::OnUpdateAsioChn(CCmdUI* pCmdUI) 
{
#ifdef __ASIO_H
pCmdUI->Enable((pAsioHost) && (pAsioHost->IsLoaded()));
#else
pCmdUI->Enable(FALSE);
#endif
}

/*****************************************************************************/
/* SendResult : sends out calculated buffers to current output device        */
/*****************************************************************************/

void CVsthostApp::SendResult(float **pBuffers, int nLength)
{
#ifdef __ASIO_H
if ((pAsioHost) && (pAsioHost->IsLoaded()))
  pAsioHost->OnProcessed(pBuffers);
else
#endif
if (DSoundOut.IsOpen())
  DSoundOut.SendData(pBuffers, nLength);
else
  WaveOut.SendData(pBuffers, nLength);
}

/*****************************************************************************/
/* LoadSetup : loads complete setup from registry                            */
/*****************************************************************************/

void CVsthostApp::LoadSetup()
{
int i, j, nEffect;
CString sName;
CSmpEffect *pEff, *pAfter;
int nLoaded = GetProfileInt("Load", "NumLoaded", 0);

for (i = 0; i < nLoaded; i++)           /* first, load all effects           */
  {
  sName.Format("FX%d_File", i);
  sName = GetProfileString("Load", sName);
  if (LoadEffect(sName, &nEffect))
    {
    pEff = (CSmpEffect *)vstHost.GetAt(nEffect);
    if (pEff)
      {
      pEff->SetLoadNum(i);
      sName.Format("FX%d_Channels", i);
      pEff->SetChnMask((unsigned short)GetProfileInt("Load", sName, 0xFFFF));
      sName.Format("FX%d_Bank", i);
      sName = GetProfileString("Load", sName);
      if (!sName.IsEmpty())
        pEff->LoadBank(sName);
      sName.Format("FX%d_Program", i);
      int nProg = GetProfileInt("Load", sName, -1);
      if (nProg >= 0)
        pEff->EffSetProgram(nProg);
      }
    }
  }

for (i = 0; i < nLoaded; i++)           /* then resolve chaining             */
  {
  pEff = (CSmpEffect *)vstHost.GetAt(i);
  if (!pEff)
    continue;
  sName.Format("FX%d_ChainTo", i);
  j = GetProfileInt("Load", sName, -1);
  if (j >= 0)
    {
    for (nEffect = vstHost.GetSize() - 1; nEffect >= 0; nEffect--)
      {
      pAfter = (CSmpEffect *)vstHost.GetAt(nEffect);
      if ((pAfter) &&
          (pAfter->GetLoadNum() == j))
        break;
      }
    if (nEffect == j)
      pEff->InsertIntoChain(pAfter);
    }
  }

for (i = 0; i < nLoaded; i++)           /* finally show all details          */
  {
  pEff = (CSmpEffect *)vstHost.GetAt(i);
  if (pEff)
    pEff->ShowDetails();
  }
}

/*****************************************************************************/
/* SaveSetup : saves complete setup into registry                            */
/*****************************************************************************/

void CVsthostApp::SaveSetup()
{
int nOldLoaded = GetProfileInt("Load", "NumLoaded", 0);
int nLoaded = vstHost.GetSize();
int i, j, k;
CString sName;

for (i = j = 0; i < nLoaded; i++)       /* save all loaded effects' names    */
  {
  CSmpEffect *pEff = (CSmpEffect *)vstHost.GetAt(i);
  CSmpEffect *pPrev;
  if (pEff)
    {
    sName.Format("FX%d_File", j);
    WriteProfileString("Load", sName, pEff->sName);
    sName.Format("FX%d_Channels", j);
    WriteProfileInt("Load", sName, pEff->GetChnMask());
    sName.Format("FX%d_Bank", j);
    WriteProfileString("Load", sName, pEff->GetChunkFile());
    sName.Format("FX%d_Program", j);
    WriteProfileInt("Load", sName, pEff->EffGetProgram());
    pPrev = pEff->GetPrev();
    k = -1;
    if (pPrev)
      {
      for (k = nLoaded - 1; k >= 0; k--)
        if (pPrev == vstHost.GetAt(k))
          break;
      }
    sName.Format("FX%d_ChainTo", j);
    WriteProfileInt("Load", sName, k);
    j++;
    }
  }
WriteProfileInt("Load", "NumLoaded", j);
for (; j < nOldLoaded; j++)             /* delete all unloaded ones from reg */
  {
  sName.Format("FX%d_File", j);
  WriteProfileString("Load", sName, NULL);
  sName.Format("FX%d_Channels", j);
  WriteProfileString("Load", sName, NULL);
  sName.Format("FX%d_Bank", j);
  WriteProfileString("Load", sName, NULL);
  sName.Format("FX%d_Program", j);
  WriteProfileString("Load", sName, NULL);
  sName.Format("FX%d_ChainTo", j);
  WriteProfileString("Load", sName, NULL);
  }
}

/*****************************************************************************/
/* OnMidikeyb : shows / hides the MIDI keyboard window                       */
/*****************************************************************************/

void CVsthostApp::OnMidikeyb() 
{
if (pMidiKeyb)                          /* if already there                  */
  pMidiKeyb->SetActiveWindow();         /* just activate it.                 */
else                                    /* otherwise create dialog           */
  {
  pMidiKeyb = new CMidiKeybDlg;
  if (pMidiKeyb &&
      (!((CMidiKeybDlg *)pMidiKeyb)->Create(CMidiKeybDlg::IDD)))
    {
    AfxMessageBox(IDS_MKBDNOTREG, MB_ICONERROR);
    delete pMidiKeyb;
    pMidiKeyb = NULL;
    }
  else
    pMidiKeyb->ShowWindow(SW_SHOW);
  }
}

/*****************************************************************************/
/* OnUpdateMidikeyb : updates the visual appearance                          */
/*****************************************************************************/

void CVsthostApp::OnUpdateMidikeyb(CCmdUI* pCmdUI) 
{
pCmdUI->SetCheck(!!pMidiKeyb);	        /* show whether it's there           */
}

/*****************************************************************************/
/* OnMidikeybProperties : sets the properties of the MIDI keyboard           */
/*****************************************************************************/

void CVsthostApp::OnMidikeybProperties() 
{
if (pMidiKeyb)
  pMidiKeyb->PostMessage(WM_SYSCOMMAND, IDM_MIDIKEYB_PROPERTIES);
}

/*****************************************************************************/
/* OnUpdateMidikeybProperties : updates the visual appearance                */
/*****************************************************************************/

void CVsthostApp::OnUpdateMidikeybProperties(CCmdUI* pCmdUI) 
{
pCmdUI->Enable(!!pMidiKeyb);            /* show whether it's there           */
}
