/*****************************************************************************/
/* ChildFrm.cpp : implementation of the CChildFrame class                    */
/*****************************************************************************/

#include "stdafx.h"
#include "vsthost.h"
#include "effectwnd.h"
#include "effeditwnd.h"
#include "effmidichn.h"
#include "EffChainDlg.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GetApp()  ((CVsthostApp *)AfxGetApp())

/*****************************************************************************/
/* plugCanDos : all plugCanDo() strings for VST 2.0                          */
/*****************************************************************************/

const char* plugCanDos [] =
  {
  "sendVstEvents",
  "sendVstMidiEvent",
  "sendVstTimeInfo",
  "receiveVstEvents",
  "receiveVstMidiEvent",
  "receiveVstTimeInfo",
  "offline",
  "plugAsChannelInsert",
  "plugAsSend",
  "mixDryWet",
  "noRealTime",
  "multipass",
  "metapass",
  "1in1out",
  "1in2out",
  "2in1out",
  "2in2out",
  "2in4out",
  "4in2out",
  "4in4out",
  "4in8out",	// 4:2 matrix to surround bus
  "8in4out",	// surround bus to 4:2 matrix
  "8in8out"
#if VST_2_1_EXTENSIONS
  ,
  "midiProgramNames",
  "conformsToWindowRules"     // mac: doesn't mess with grafport. general: may want
  // to call sizeWindow (). if you want to use sizeWindow (),
  // you must return true (1) in canDo ("conformsToWindowRules")
#endif // VST_2_1_EXTENSIONS
  
#if VST_2_3_EXTENSIONS
  ,
  "bypass"
#endif // VST_2_3_EXTENSIONS
};

/*****************************************************************************/
/* Runtime Class implementation                                              */
/*****************************************************************************/

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

/*****************************************************************************/
/* CChildFrame message map                                                   */
/*****************************************************************************/

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
		ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
		ON_WM_SETFOCUS()
		ON_WM_CREATE()
	ON_COMMAND(IDM_EFF_EDIT, OnEffEdit)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(IDM_EFF_EDIT, OnUpdateEffEdit)
	ON_COMMAND(IDM_EFF_EDIT_PARMS, OnEffEditParms)
	ON_COMMAND(IDM_EFF_LOAD, OnEffLoad)
	ON_COMMAND(IDM_EFF_SAVE, OnEffSave)
	ON_COMMAND(IDM_EFF_SAVEAS, OnEffSaveas)
	ON_COMMAND(IDM_EFF_SELPROGRAM, OnEffSelprogram)
	ON_UPDATE_COMMAND_UI(IDM_EFF_SELPROGRAM, OnUpdateEffSelprogram)
	ON_COMMAND(IDM_SEL_MIDICHN, OnSelMidichn)
	ON_COMMAND(IDM_CHAIN_AFTER, OnChainAfter)
	ON_COMMAND(IDM_UNCHAIN, OnUnchain)
	ON_UPDATE_COMMAND_UI(IDM_CHAIN_AFTER, OnUpdateChainAfter)
	ON_UPDATE_COMMAND_UI(IDM_UNCHAIN, OnUpdateUnchain)
	ON_COMMAND(IDM_PROGRAM_NEXT, OnProgramNext)
	ON_COMMAND(IDM_PROGRAM_PREV, OnProgramPrev)
	ON_UPDATE_COMMAND_UI(IDM_PROGRAM_NEXT, OnUpdateProgramNext)
	ON_UPDATE_COMMAND_UI(IDM_PROGRAM_PREV, OnUpdateProgramPrev)
	//}}AFX_MSG_MAP
    ON_COMMAND_RANGE(IDM_EFF_PROGRAM_0, IDM_EFF_PROGRAM_0+999, OnSetProgram)
END_MESSAGE_MAP()

/*****************************************************************************/
/* CChildFrame : constructor                                                 */
/*****************************************************************************/

CChildFrame::CChildFrame()
{
nEffect = -1;	
pEditWnd = pParmWnd = 0;
nCanEdit = -1;
m_hEditMenu = ::LoadMenu(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_EFFECTTYPE));
}

/*****************************************************************************/
/* ~CChildFrame : destructor                                                 */
/*****************************************************************************/

CChildFrame::~CChildFrame()
{
}

/*****************************************************************************/
/* PreCreateWindow : called before a window is created                       */
/*****************************************************************************/

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
if( !CMDIChildWnd::PreCreateWindow(cs) )
  return FALSE;

cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
cs.lpszClass = AfxRegisterWndClass(0);

return TRUE;
}

/*****************************************************************************/
/* CChildFrame diagnostics                                                   */
/*****************************************************************************/

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/*****************************************************************************/
/* OnFileClose : called when File / Close is selected                        */
/*****************************************************************************/

void CChildFrame::OnFileClose() 
{
	// To close the frame, just send a WM_CLOSE, which is the equivalent
	// choosing close from the system menu.

SendMessage(WM_CLOSE);
}

/*****************************************************************************/
/* OnCreate : called upon window creation                                    */
/*****************************************************************************/

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
  return -1;
	
// CListBox - based window creation
if (!m_wndView.Create(WS_CHILD | WS_VISIBLE |
                          WS_VSCROLL |
                          LBS_NOTIFY |
                          LBS_WANTKEYBOARDINPUT |
                          LBS_NOINTEGRALHEIGHT,
                      CRect(0, 0, 0, 0),
                      this,
                      AFX_IDW_PANE_FIRST))
  {
  TRACE0("Failed to create view window\n");
  return -1;
  }

SetIcon(AfxGetApp()->LoadIcon(IDI_EFFINFO), TRUE);
return 0;
}

/*****************************************************************************/
/* OnSetFocus : called when the MDI Child gets the focus                     */
/*****************************************************************************/

void CChildFrame::OnSetFocus(CWnd* pOldWnd) 
{
CMDIChildWnd::OnSetFocus(pOldWnd);

m_wndView.SetFocus();
}

/*****************************************************************************/
/* OnCmdMsg : called when a WM_COMMAND comes in                              */
/*****************************************************************************/

BOOL CChildFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
// let the view have first crack at the command
if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
  return TRUE;
	
// otherwise, do default handling
return CMDIChildWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

/*****************************************************************************/
/* SetNewTitle : forces new window title                                     */
/*****************************************************************************/

void CChildFrame::SetNewTitle(LPCSTR lpszText)
{
sTitle.Format("%d I: %s", nEffect, lpszText);
SetupTitle();
}

/*****************************************************************************/
/* SetupTitle : sets the current window title                                */
/*****************************************************************************/

void CChildFrame::SetupTitle()
{
CEffect *pEffect = GetApp()->vstHost.GetAt(nEffect);
CString sProg;
char szBuf[256] = "";
pEffect->EffGetProgramName(szBuf);
sProg.Format(" Program %d: %s", pEffect->EffGetProgram(), szBuf);
SetTitle(sTitle + sProg);
OnUpdateFrameTitle(TRUE);
}

/*****************************************************************************/
/* SetEffect : sets effect #                                                 */
/*****************************************************************************/

void CChildFrame::SetEffect(int nEffect)
{
this->nEffect = nEffect;
CSmpEffect *pEffect = (CSmpEffect *)GetApp()->vstHost.GetAt(nEffect);
char szBuf[256] = "";
if (pEffect->EffGetProductString(szBuf))/* if V2 plugin                      */
  SetNewTitle(szBuf);                   /* use plugin info                   */
else                                    /* if V1 plugin                      */
  {
  CString sFile(pEffect->sName);
  int nrbsl = sFile.ReverseFind('\\');
  SetNewTitle(sFile.Mid(nrbsl + 1));    /* use plugin file name              */
  }
pEffect->SetFrameWnd(this);
ShowDetails();                          /* show all relevant internal data   */
}

/*****************************************************************************/
/* OnUpdateEffEdit : determines whether effect edit there                    */
/*****************************************************************************/

void CChildFrame::OnUpdateEffEdit(CCmdUI* pCmdUI) 
{
if (nCanEdit < 0)                       /* do that only once.                */
  {
  ERect *prc = NULL;
  long lResult = GetApp()->vstHost.EffEditGetRect(nEffect, &prc);
  nCanEdit = !!prc;
  }
pCmdUI->Enable((BOOL)nCanEdit);
}

/*****************************************************************************/
/* OnEffEdit : called to open an edit window                                 */
/*****************************************************************************/

void CChildFrame::OnEffEdit() 
{
if (pEditWnd)
  pEditWnd->MDIActivate();
else
  {
  CEffect *pEffect = GetApp()->vstHost.GetAt(nEffect);
  if (!pEffect)
    return;

  ERect *prc = NULL;
  long lResult = pEffect->EffEditGetRect(&prc);
  if (prc)
    {
    CEffectWnd *pWnd =
        (CEffectWnd *)GetApp()->CreateChild(RUNTIME_CLASS(CEffectWnd),
                                            IDR_EFFECTTYPE,
                                            m_hEditMenu);
    if (pWnd)
      {
      pWnd->ShowWindow(SW_SHOWNORMAL);
      pWnd->SetEffect(nEffect);
      pWnd->SetMain(this);
      pEditWnd = pWnd;
      pWnd->SetEffSize(prc);
      ((CSmpEffect *)pEffect)->SetEditWnd(pWnd);
      lResult = pEffect->EffEditOpen(pWnd->GetSafeHwnd());
      pWnd->SetupTitle();
      }
    }
  }
}

/*****************************************************************************/
/* OnEffEditParms : opens edit parameter window                              */
/*****************************************************************************/

void CChildFrame::OnEffEditParms() 
{
if (pParmWnd)
  pParmWnd->MDIActivate();
else
  {
  CVSTHost &Host = GetApp()->vstHost;
  CEffect *pEffect = Host.GetAt(nEffect);

  CEffEditWnd *pWnd =
      (CEffEditWnd *)GetApp()->CreateChild(RUNTIME_CLASS(CEffEditWnd),
                                          IDR_EFFECTTYPE,
                                          m_hEditMenu);
  if (pWnd)
    {
    pWnd->ShowWindow(SW_SHOWNORMAL);
    pWnd->SetEffect(nEffect);
    pWnd->SetMain(this);
    pParmWnd = pWnd;
    pWnd->SetupTitle();
    ((CSmpEffect *)pEffect)->SetParmWnd(pWnd);
    }
  }
}

/*****************************************************************************/
/* EditClosed : called by effect edit window when closed                     */
/*****************************************************************************/

void CChildFrame::EditClosed()
{
((CSmpEffect *)GetApp()->vstHost.GetAt(nEffect))->SetEditWnd(0);
pEditWnd = NULL;
}

/*****************************************************************************/
/* ParmClosed : called by effect parm window when closed                     */
/*****************************************************************************/

void CChildFrame::ParmClosed()
{
((CSmpEffect *)GetApp()->vstHost.GetAt(nEffect))->SetParmWnd(0);
pParmWnd = NULL;
}

/*****************************************************************************/
/* OnClose : called when the window is being closed                          */
/*****************************************************************************/

void CChildFrame::OnClose() 
{
BOOL bWasRunning = GetApp()->IsEngineRunning();
GetApp()->EngineStop();                 /* stop input engine                 */
if (pEditWnd)                           /* if edit window open               */
  pEditWnd->SendMessage(WM_CLOSE);      /* close it                          */
if (pParmWnd)                           /* if parm window open               */
  pParmWnd->SendMessage(WM_CLOSE);      /* close it                          */

CSmpEffect *pEff = (CSmpEffect *)GetApp()->vstHost.GetAt(nEffect);
CSmpEffect *pPrev = pEff->GetPrev();
CSmpEffect *pNext = pEff->GetNext();

pEff->SetFrameWnd(NULL);                /* remove frame window from effect   */
GetApp()->vstHost.RemoveAt(nEffect);    /* remove the effect from memory     */
if (pPrev)                              /* if chained after another window   */
  pPrev->ShowDetails();                 /* make sure to reshow its details   */
if (pNext)                              /* if chained before another window  */
  pNext->ShowDetails();                 /* make sure to reshow its details   */
if (bWasRunning)                        /* if it was running before          */
  GetApp()->EngineStart();              /* restart input engine              */
CMDIChildWnd::OnClose();
}

/*****************************************************************************/
/* ShowDetails : shows all the details about the attached effect             */
/*****************************************************************************/

void CChildFrame::ShowDetails()
{
CSmpVSTHost &Host = GetApp()->vstHost;
CSmpEffect *pEffect = (CSmpEffect *)Host.GetAt(nEffect);
if (!pEffect)
  return;

CString sOut;

m_wndView.ResetContent();
char szBuf[256] = "";
sOut.Format("Effect #%d: %s",
            nEffect,
            pEffect->sName);
m_wndView.AddString(sOut);
if (pEffect->EffGetProductString(szBuf))/* if V2 plugin                      */
  m_wndView.AddString(szBuf);
m_wndView.AddString("");

sOut.Format("%d programs", pEffect->pEffect->numPrograms);
m_wndView.AddString(sOut);
sOut.Format("%d parameters", pEffect->pEffect->numParams);
m_wndView.AddString(sOut);
sOut.Format("%d inputs", pEffect->pEffect->numInputs);
m_wndView.AddString(sOut);
sOut.Format("%d outputs", pEffect->pEffect->numOutputs);
m_wndView.AddString(sOut);
sOut.Format("Flags: %08lXH", pEffect->pEffect->flags);
m_wndView.AddString(sOut);
if (pEffect->pEffect->flags & effFlagsHasEditor)
  m_wndView.AddString("  Has Editor");
if (pEffect->pEffect->flags & effFlagsHasClip)
  m_wndView.AddString("  Has VU Clip");
if (pEffect->pEffect->flags & effFlagsHasVu)
  m_wndView.AddString("  Returns VU values");
if (pEffect->pEffect->flags & effFlagsCanMono)
  m_wndView.AddString("  Can process mono");
if (pEffect->pEffect->flags & effFlagsCanReplacing)
  m_wndView.AddString("  Supports in place output");
if (pEffect->pEffect->flags & effFlagsProgramChunks)
  m_wndView.AddString("  Program data are handled in formatless chunks");
if (pEffect->pEffect->flags & effFlagsIsSynth)
  m_wndView.AddString("  Is a synth");
if (pEffect->pEffect->flags & effFlagsNoSoundInStop)
  m_wndView.AddString("  Does not produce sound when input is all silence");
if (pEffect->pEffect->flags & effFlagsExtIsAsync)
  m_wndView.AddString("  For external dsp; plug returns immediately from process()");
if (pEffect->pEffect->flags & effFlagsExtHasBuffer)
  m_wndView.AddString("  For external dsp; has its own output buffer");

if (pEffect->pEffect->initialDelay)
  {
  sOut.Format("Initial Delay: %d", pEffect->pEffect->initialDelay);
  m_wndView.AddString(sOut);
  }

char sUID[5];
int i;
for (i = 0; i < 4; i++)
  {
  sUID[i] = ((char *)&pEffect->pEffect->uniqueID)[3 - i];
  if (!sUID[i])
    sUID[i] = ' ';
  }
sUID[i] = '\0';
sOut.Format("Unique ID: '%s' (%08lXH)", sUID, pEffect->pEffect->uniqueID);
m_wndView.AddString(sOut);

sOut.Format("Version %d", pEffect->pEffect->version);
m_wndView.AddString(sOut);

i = pEffect->EffGetNumProgramCategories();
if (i >= 1)
  {
  sOut.Format("%d program categories", i);
  m_wndView.AddString(sOut);
  i = pEffect->EffGetPlugCategory();
  switch (i)
    {
    case kPlugCategUnknown :
      sOut = "Unknown";
      break;
    case kPlugCategEffect :
      sOut = "Effect";
      break;
    case kPlugCategSynth :
      sOut = "Synth";
      break;
    case kPlugCategAnalysis :
      sOut = "Analysis";
      break;
    case kPlugCategMastering :
      sOut = "Mastering";
      break;
    case kPlugCategSpacializer :
      sOut = "Spacializer";
      break;
    case kPlugCategRoomFx :
      sOut = "RoomFx";
      break;
    case kPlugSurroundFx :
      sOut = "SurroundFx";
      break;
    default :
      sOut.Format("%d", i);
      break;
    }
  m_wndView.AddString(CString("Main Program category: ") + sOut);
  }

bool bCanDoDisp = false;
for (i = 0; i < (sizeof(plugCanDos) / sizeof(plugCanDos[0])); i++)
  {
  int j = pEffect->EffCanDo(plugCanDos[i]);
  if (j > 0)
    {
    if (!bCanDoDisp)
      {
      m_wndView.AddString("PlugIn can do:");
      bCanDoDisp = true;
      }
    m_wndView.AddString(CString("  ") + plugCanDos[i]);
    }
  }

CString sFile = pEffect->GetChunkFile();
if (!sFile.IsEmpty())
  {
  m_wndView.AddString("");
  m_wndView.AddString(CString("Loaded file: ") + sFile);
  }

CSmpEffect *pPrev = pEffect->GetPrev();
CSmpEffect *pNext = pEffect->GetNext();
int nEffNo;
if (pPrev || pNext)
  m_wndView.AddString("");
if (pPrev)
  {
  for (nEffNo = Host.GetSize() - 1; nEffNo >= 0; nEffNo--)
    if (Host.GetAt(nEffNo) == pPrev)
      break;
  sOut.Format("Chained after %d: %s", nEffNo, pPrev->GetDisplayName());
  m_wndView.AddString(sOut);
  }
if (pNext)
  {
  for (nEffNo = Host.GetSize() - 1; nEffNo >= 0; nEffNo--)
    if (Host.GetAt(nEffNo) == pNext)
      break;
  sOut.Format("Chained before %d: %s", nEffNo, pNext->GetDisplayName());
  m_wndView.AddString(sOut);
  }

ResizeToContent();
SetupTitle();
}

/*****************************************************************************/
/* OnSize : called when the window is resized                                */
/*****************************************************************************/

void CChildFrame::OnSize(UINT nType, int cx, int cy) 
{
CMDIChildWnd::OnSize(nType, cx, cy);

if (nType != SIZE_MINIMIZED)
  m_wndView.MoveWindow(0, 0, cx, cy);
}

/*****************************************************************************/
/* OnSetProgram : called when the program is changed in an effect edit wnd   */
/*****************************************************************************/

void CChildFrame::OnSetProgram()
{
if (pEditWnd)
  pEditWnd->Update();
if (pParmWnd)
  pParmWnd->Update();
SetupTitle();
}

/*****************************************************************************/
/* OnSetProgram : called to change to another program                        */
/*****************************************************************************/

void CChildFrame::OnSetProgram(UINT nID)
{
CEffect *pEffect = GetApp()->vstHost.GetAt(nEffect);
if (pEffect)
  {
  pEffect->EffSetProgram(nID - IDM_EFF_PROGRAM_0);
  OnSetProgram();
  }
}

/*****************************************************************************/
/* OnProgramNext : called to switch to the next program                      */
/*****************************************************************************/

void CChildFrame::OnProgramNext() 
{
CEffect *pEffect = GetApp()->vstHost.GetAt(nEffect);
if ((pEffect) &&
    (pEffect->pEffect->numPrograms > 1))
  {
  long lProg = pEffect->EffGetProgram() + 1;
  if (lProg >= pEffect->pEffect->numPrograms)
    lProg = 0;
  pEffect->EffSetProgram(lProg);
  OnSetProgram();
  }
}

/*****************************************************************************/
/* OnUpdateProgramNext : updates the visual appearance                       */
/*****************************************************************************/

void CChildFrame::OnUpdateProgramNext(CCmdUI* pCmdUI) 
{
OnUpdateEffSelprogram(pCmdUI);
}

/*****************************************************************************/
/* OnProgramPrev : called to switch to the previous program                  */
/*****************************************************************************/

void CChildFrame::OnProgramPrev() 
{
CEffect *pEffect = GetApp()->vstHost.GetAt(nEffect);
if ((pEffect) &&
    (pEffect->pEffect->numPrograms > 1))
  {
  long lProg = pEffect->EffGetProgram() - 1;
  if (lProg < 0)
    lProg = pEffect->pEffect->numPrograms - 1;
  pEffect->EffSetProgram(lProg);
  OnSetProgram();
  }
}

/*****************************************************************************/
/* OnUpdateProgramPrev : updates the visual appearance                       */
/*****************************************************************************/

void CChildFrame::OnUpdateProgramPrev(CCmdUI* pCmdUI) 
{
OnUpdateEffSelprogram(pCmdUI);
}

/*****************************************************************************/
/* OnEffLoad : loads a .fxb file                                             */
/*****************************************************************************/

void CChildFrame::OnEffLoad() 
{
CSmpEffect *pEffect = (CSmpEffect *)GetApp()->vstHost.GetAt(nEffect);
if (!pEffect)
  return;

CFileDialog dlg(TRUE,
                "fxb",
                pEffect->GetChunkFile(),
                OFN_ENABLESIZING | OFN_NOCHANGEDIR,
                "Effect Bank Files (.fxb)|*.fxb|All Files|*.*||");
if (dlg.DoModal() != IDOK)
  return;

#if 1
// detailed error messages - better for debugging, but redundant code

CFxBank b((char *)(LPCSTR)dlg.GetPathName());
if (b.IsLoaded())
  {
  if (pEffect->pEffect->uniqueID != b.GetFxID())
    {
    MessageBox("Loaded bank has another ID!", "Load Error", MB_ICONERROR);
    return;
    }
  if (b.IsChunk())
    {
    if (!(pEffect->pEffect->flags & effFlagsProgramChunks))
      {
      MessageBox("Loaded bank contains a formatless chunk, "
                 "but the effect can't handle that!",
                 "Load Error", MB_ICONERROR);
      return;
      }
    pEffect->EffSetChunk(b.GetChunk(), b.GetChunkSize());
    }
  else
    {
    int cProg = pEffect->EffGetProgram();
    int i, j;
    int nParms = b.GetNumParams();
    for (i = 0; i < b.GetNumPrograms(); i++)
      {
      pEffect->EffSetProgram(i);
      pEffect->EffSetProgramName(b.GetProgramName(i));
      for (j = 0; j < nParms; j++)
        pEffect->EffSetParameter(j, b.GetProgParm(i, j));
      
      }
    pEffect->EffSetProgram(cProg);
    }
  pEffect->SetChunkFile(dlg.GetPathName());
  ShowDetails();
  OnSetProgram();
  }

#else
// dense error message - smaller code, but less informative
if (pEffect->LoadBank(dlg.GetPathName()))
  {
  ShowDetails();
  OnSetProgram();
  }
#endif

else
  MessageBox("Bank could not be loaded", NULL, MB_ICONERROR);
}

/*****************************************************************************/
/* OnEffSave : saves current bank                                            */
/*****************************************************************************/

void CChildFrame::OnEffSave() 
{
CSmpEffect *pEffect = (CSmpEffect *)GetApp()->vstHost.GetAt(nEffect);
if (!pEffect)
  return;
CString sFile = pEffect->GetChunkFile();
if (sFile.IsEmpty())
  OnEffSaveas();
else
  SaveBank(sFile);
}

void CChildFrame::OnEffSaveas() 
{
CSmpEffect *pEffect = (CSmpEffect *)GetApp()->vstHost.GetAt(nEffect);
if (!pEffect)
  return;

CFileDialog dlg(FALSE,
                "fxb",
                pEffect->GetChunkFile(),
                OFN_CREATEPROMPT | OFN_ENABLESIZING |
                    OFN_NOCHANGEDIR | OFN_NOREADONLYRETURN |
                    OFN_OVERWRITEPROMPT,
                "Effect Bank Files (.fxb)|*.fxb|All Files|*.*||");
if (dlg.DoModal() == IDOK)
  SaveBank(dlg.GetPathName());
}

/*****************************************************************************/
/* SaveBank saves bank to file                                               */
/*****************************************************************************/

void CChildFrame::SaveBank(CString sName)
{
CSmpEffect *pEffect = (CSmpEffect *)GetApp()->vstHost.GetAt(nEffect);
if (!pEffect)
  return;

CFxBank b;

if (pEffect->pEffect->flags & effFlagsProgramChunks)
  {
  void * pChunk;
  int lSize = pEffect->EffGetChunk(&pChunk);
  if (lSize)
    b.SetSize(lSize);
  if (b.IsLoaded())
    b.SetChunk(pChunk);
  }
else
  {
  b.SetSize(pEffect->pEffect->numPrograms,
            pEffect->pEffect->numParams);
  if (b.IsLoaded())
    {
    int i, j;
    int cProg = pEffect->EffGetProgram();
    int nParms = b.GetNumParams();
    for (i = 0; i < b.GetNumPrograms(); i++)
      {
      pEffect->EffSetProgram(i);
      char szName[128];
      pEffect->EffGetProgramName(szName);
      b.SetProgramName(i, szName);
      for (j = 0; j < nParms; j++)
        b.SetProgParm(i, j, pEffect->EffGetParameter(j));
      }
    pEffect->EffSetProgram(cProg);
    }
  }
if (!b.IsLoaded())
  {
  MessageBox("Memory Allocation Error", NULL, MB_ICONERROR);
  return;
  }

b.SetFxID(pEffect->pEffect->uniqueID);
b.SetFxVersion(pEffect->pEffect->version);
if (b.SaveBank((char *)(LPCSTR)sName))
  {
  pEffect->SetChunkFile(sName);
  ShowDetails();
  OnSetProgram();
  }
else
  MessageBox("Error saving file", NULL, MB_ICONERROR);

}

/*****************************************************************************/
/* OnEffSelprogram : select one of the programs                              */
/*****************************************************************************/

void CChildFrame::OnEffSelprogram() 
{
CEffect *pEffect = GetApp()->vstHost.GetAt(nEffect);
if ((!pEffect) ||
    (pEffect->pEffect->numPrograms <= 1))
  return;

CWnd * pWnd = ((CMDIFrameWnd *)AfxGetMainWnd())->MDIGetActive();
if (!pWnd)
  return;
CRect rc;
pWnd->GetWindowRect(&rc);
int off = GetSystemMetrics(SM_CYCAPTION) +
          2 * GetSystemMetrics(SM_CYSIZEFRAME);
CMenu pPopup;
pPopup.CreatePopupMenu();
GetApp()->FillPopup(&pPopup, nEffect);
pPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,
    rc.left + off, rc.top + off,
    pWnd);
}

/*****************************************************************************/
/* OnUpdateEffSelprogram : updates the visual appearance                     */
/*****************************************************************************/

void CChildFrame::OnUpdateEffSelprogram(CCmdUI* pCmdUI) 
{
BOOL bEnable = FALSE;
CEffect *pEffect = GetApp()->vstHost.GetAt(nEffect);
if ((pEffect) &&
    (pEffect->pEffect->numPrograms > 1) &&
    (((CMDIFrameWnd *)AfxGetMainWnd())->MDIGetActive()))
  bEnable = TRUE;
pCmdUI->Enable(bEnable);
}

/*****************************************************************************/
/* OnSelMidichn : called to select the MIDI channels the effect listens on   */
/*****************************************************************************/

void CChildFrame::OnSelMidichn() 
{
CSmpEffect *pEffect = (CSmpEffect *)(GetApp()->vstHost.GetAt(nEffect));
CEffMidiChn dlg(this);
dlg.wChnMask = 	pEffect->GetChnMask();
if (dlg.DoModal())
  pEffect->SetChnMask(dlg.wChnMask);
}

/*****************************************************************************/
/* ResizeToContent : resizes the window to its content's extents             */
/*****************************************************************************/

void CChildFrame::ResizeToContent()
{
int nWidth = m_wndView.GetHorizontalExtent() +
             ::GetSystemMetrics(SM_CXVSCROLL);
int nHeight = m_wndView.GetVerticalExtent() +
             ::GetSystemMetrics(SM_CYHSCROLL);
CRect rcWin, rcClt;

GetWindowRect(&rcWin);
GetClientRect(&rcClt);
// rcWin.right = rcWin.left + nWidth + rcWin.Width() - rcClt.Width() + 10;
// rcWin.bottom = rcWin.top + nHeight + rcWin.Height() - rcClt.Height() + 10;
// MoveWindow(&rcWin);
SetWindowPos(&wndTop,
             0, 0,
             rcWin.Width() - rcClt.Width() + nWidth + 10,
             rcWin.Height() - rcClt.Height() + nHeight + 10,
             SWP_NOMOVE | SWP_NOZORDER);

}

/*****************************************************************************/
/* OnChainAfter : called to chain the effect after another effect            */
/*****************************************************************************/

void CChildFrame::OnChainAfter() 
{
CSmpVSTHost &Host = GetApp()->vstHost;
CSmpEffect *pEffect = (CSmpEffect *)Host.GetAt(nEffect);
if (!pEffect)
  return;
CEffChainDlg dlg(this);
dlg.pCurEff = pEffect;
if ((dlg.DoModal() == IDOK) &&
    (dlg.nSelEff >= 0))
  {
  BOOL bWasRunning = GetApp()->IsEngineRunning();
  GetApp()->EngineStop();               /* stop input engine                 */

  CSmpEffect *pPrev = pEffect->GetPrev();
  CSmpEffect *pNext = pEffect->GetNext();
  CSmpEffect *pAfter = (CSmpEffect *)Host.GetAt(dlg.nSelEff);
  pEffect->InsertIntoChain(pAfter);
  if (pPrev)                            /* update old chain elements         */
    pPrev->ShowDetails();
  if (pNext)
    pNext->ShowDetails();
  pPrev = pEffect->GetPrev();           /* get new chain elements            */
  pNext = pEffect->GetNext();
  if (pPrev)                            /* update new chain elements         */
    pPrev->ShowDetails();
  if (pNext)
    pNext->ShowDetails();
  pEffect->ShowDetails();               /* then show this effect's details   */

  if (bWasRunning)                      /* if it was running before          */
    GetApp()->EngineStart();            /* restart input engine              */
  }
}

/*****************************************************************************/
/* OnUpdateChainAfter : updates the visual appearance                        */
/*****************************************************************************/

void CChildFrame::OnUpdateChainAfter(CCmdUI* pCmdUI) 
{
CSmpVSTHost &Host = GetApp()->vstHost;
int nEffects = Host.GetSize();
int i, j = 0;

for (i = 0; i < nEffects; i++)          /* count available effects           */
  if (Host.GetAt(i))
    j++;
pCmdUI->Enable(j > 1);                  /* allow only if more than 1 effect  */
}

/*****************************************************************************/
/* OnUnchain : called to remove the effect from a chain                      */
/*****************************************************************************/

void CChildFrame::OnUnchain() 
{
CSmpVSTHost &Host = GetApp()->vstHost;
CSmpEffect *pEffect = (CSmpEffect *)Host.GetAt(nEffect);
if (!pEffect)
  return;
CSmpEffect *pPrev = pEffect->GetPrev();
CSmpEffect *pNext = pEffect->GetNext();
if ((!pPrev) && (!pNext))
  return;

BOOL bWasRunning = GetApp()->IsEngineRunning();
GetApp()->EngineStop();                 /* stop input engine                 */

pEffect->RemoveFromChain();
if (pPrev)                              /* update old chain elements         */
  pPrev->ShowDetails();
if (pNext)
  pNext->ShowDetails();
pEffect->ShowDetails();                 /* then show this effect's details   */
if (bWasRunning)                        /* if it was running before          */
  GetApp()->EngineStart();              /* restart input engine              */
}

/*****************************************************************************/
/* OnUpdateUnchain : called to update the visual appearance                  */
/*****************************************************************************/

void CChildFrame::OnUpdateUnchain(CCmdUI* pCmdUI) 
{
CSmpEffect *pEffect = (CSmpEffect *)GetApp()->vstHost.GetAt(nEffect);
if (!pEffect)
  pCmdUI->Enable(FALSE);
else
  pCmdUI->Enable(pEffect->GetPrev() || pEffect->GetNext());
}
