/*****************************************************************************/
/* EffectWnd.cpp : implementation file                                       */
/*****************************************************************************/

/******************************************************************************
Copyright (C) 2006  Hermann Seib

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
******************************************************************************/

#include "stdafx.h"
#include "vsthost.h"
#include "childfrm.h"
#include "EffectWnd.h"
#include "prognamedlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GetApp()  ((CVsthostApp *)AfxGetApp())

/*===========================================================================*/
/* CEffectWnd class members                                                  */
/*===========================================================================*/

IMPLEMENT_DYNCREATE(CEffectWnd, CMDIChildWnd)

/*****************************************************************************/
/* CEffectWnd : constructor                                                  */
/*****************************************************************************/

CEffectWnd::CEffectWnd()
{
pHost = &GetApp()->vstHost;
pMain = NULL;
pView = NULL;
nEffect = -1;
rcEffFrame.SetRectEmpty();
rcEffClient.SetRectEmpty();
// memset(&vstw, 0, sizeof(vstw));
}

/*****************************************************************************/
/* ~CEffectWnd : destructor                                                  */
/*****************************************************************************/

CEffectWnd::~CEffectWnd()
{
if (pView)
  delete pView;
}

/*****************************************************************************/
/* CEffectWnd message map                                                    */
/*****************************************************************************/

BEGIN_MESSAGE_MAP(CEffectWnd, CMDIChildWnd)
	//{{AFX_MSG_MAP(CEffectWnd)
	ON_WM_CLOSE()
	ON_COMMAND(IDM_EFF_PROGRAM_NAME, OnEffProgramName)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_COMMAND(IDM_EFF_EDIT_PARMS, OnEffEditParms)
	ON_COMMAND(IDM_EFF_EDIT, OnEffEdit)
	ON_UPDATE_COMMAND_UI(IDM_EFF_EDIT, OnUpdateEffEdit)
	ON_COMMAND(IDM_EFF_INFO, OnEffInfo)
	ON_COMMAND(IDM_EFF_RESIZE, OnEffResize)
	ON_COMMAND(IDM_EFF_CHECKSIZE, OnEffChecksize)
	ON_COMMAND(IDM_EFF_LOAD, OnEffLoad)
	ON_COMMAND(IDM_EFF_SAVE, OnEffSave)
	ON_COMMAND(IDM_EFF_SAVEAS, OnEffSaveas)
	ON_COMMAND(IDM_EFF_SELPROGRAM, OnEffSelprogram)
	ON_UPDATE_COMMAND_UI(IDM_EFF_SELPROGRAM, OnUpdateEffSelprogram)
	ON_COMMAND(IDM_SEL_MIDICHN, OnSelMidichn)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	ON_COMMAND(IDM_CHAIN_AFTER, OnChainAfter)
	ON_COMMAND(IDM_UNCHAIN, OnUnchain)
	ON_UPDATE_COMMAND_UI(IDM_CHAIN_AFTER, OnUpdateChainAfter)
	ON_UPDATE_COMMAND_UI(IDM_UNCHAIN, OnUpdateUnchain)
	ON_COMMAND(IDM_PROGRAM_NEXT, OnProgramNext)
	ON_COMMAND(IDM_PROGRAM_PREV, OnProgramPrev)
	ON_UPDATE_COMMAND_UI(IDM_PROGRAM_NEXT, OnUpdateProgramNext)
	ON_UPDATE_COMMAND_UI(IDM_PROGRAM_PREV, OnUpdateProgramPrev)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
    ON_COMMAND_RANGE(IDM_EFF_PROGRAM_0, IDM_EFF_PROGRAM_0+999, OnSetProgram)
END_MESSAGE_MAP()

/*****************************************************************************/
/* CloseEditWnd : closes the Edit window, if necessary                       */
/*****************************************************************************/

void CEffectWnd::CloseEditWnd()
{
CSmpEffect *pEffect = (CSmpEffect *)pHost->GetAt(nEffect);
                                        /* if edit window still open         */
if ((pEffect) && (pEffect->GetEditWnd()))
  {
  pEffect->EnterCritical();             /* make sure we're not processing    */
  pEffect->EffEditClose();              /* tell effect edit window's closed  */
  pMain->EditClosed();                  /* tell main window it's gone        */
  pEffect->LeaveCritical();             /* re-enable processing              */
  }
}

/*****************************************************************************/
/* OnClose : called when the editor window is closed                         */
/*****************************************************************************/

void CEffectWnd::OnClose() 
{
CloseEditWnd();                         /* close edit window                 */
CMDIChildWnd::OnClose();                /* and allow base class to work      */
}

/*****************************************************************************/
/* OnDestroy : called when the window is destroyed                           */
/*****************************************************************************/

void CEffectWnd::OnDestroy() 
{
CloseEditWnd();                         /* close edit window                 */
CMDIChildWnd::OnDestroy();              /* and allow base class to work      */
}

/*****************************************************************************/
/* OnSetProgram : called to change to another program                        */
/*****************************************************************************/

void CEffectWnd::OnSetProgram(UINT nID)
{
pMain->OnSetProgram(nID);
}

/*****************************************************************************/
/* OnEffProgramName : called to change the program name                      */
/*****************************************************************************/

void CEffectWnd::OnEffProgramName() 
{
CEffect *pEffect = GetApp()->vstHost.GetAt(nEffect);
if (pEffect)
  {
  CProgNameDlg dlg(this);
  char szTxt[65] = "";
  pEffect->EffGetProgramName(szTxt);
  dlg.sPgName = szTxt;
  if (dlg.DoModal() == IDOK)
    {
    strcpy(szTxt, dlg.sPgName.Left(64));
    pEffect->EffSetProgramName(szTxt);
    SetupTitle();
    }
  }
}

/*****************************************************************************/
/* SetupTitle : sets up the window title                                     */
/*****************************************************************************/

void CEffectWnd::SetupTitle()
{
CEffect *pEffect = pHost->GetAt(nEffect);
CString sName;
if (pEffect)
  {
  sName.Format("%d %s: ", nEffect, GetEditType());

  if (sTitle.IsEmpty())
    {
    char szBuf[256] = "";
                                        /* if V2 plugin                      */
    if (pEffect->EffGetProductString(szBuf))
      sName += szBuf;                   /* use plugin info                   */
    else                                /* if V1 plugin                      */
      {
      CString sFile(pEffect->sName);
      int nrbsl = sFile.ReverseFind('\\');
      sName += sFile.Mid(nrbsl + 1);    /* use plugin file name              */
      }

    CString sProg;
    pEffect->EffGetProgramName(szBuf);
    sProg.Format(" Program %d: %s", pEffect->EffGetProgram(), szBuf);
    sName += sProg;
    }
  else
    sName += sTitle;
  }

SetTitle(sName);
OnUpdateFrameTitle(TRUE);
}

/*****************************************************************************/
/* OnCreate : called when the window is created                              */
/*****************************************************************************/

int CEffectWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
  return -1;
	
if (NeedView())
  {
  pView = CreateView();
  if (!pView)
    {
    TRACE0("Failed to create view window\n");
    return -1;
    }
//  vstw.winHandle = pView;
  }

SetIcon(RetrieveIcon(), TRUE);
return 0;
}

/*****************************************************************************/
/* OnSetFocus : called when the window receives the input focus              */
/*****************************************************************************/

void CEffectWnd::OnSetFocus(CWnd* pOldWnd) 
{
CMDIChildWnd::OnSetFocus(pOldWnd);
if (pView)
  pView->SetFocus();
}

/*****************************************************************************/
/* GetEffectEditWndSize : calculates the effect window's size                */
/*****************************************************************************/

BOOL CEffectWnd::GetEffectEditWndSize(CRect &rcFrame, CRect &rcClient, ERect *pRect)
{
if ((!pView) && (!pRect))
  {
  CEffect *pEffect = pHost->GetAt(nEffect);
  if (!pEffect)
    return FALSE;
  pEffect->EffEditGetRect(&pRect);
  }
if (!pRect)
  return FALSE;

rcFrame.SetRect(pRect->left, pRect->top, pRect->right, pRect->bottom);
rcFrame.bottom += ::GetSystemMetrics(SM_CYCAPTION) +
             2 * ::GetSystemMetrics(SM_CYFRAME) +
             4 * ::GetSystemMetrics(SM_CYBORDER);
rcFrame.right += 2 * ::GetSystemMetrics(SM_CXFRAME) +
            4 * ::GetSystemMetrics(SM_CXBORDER);

rcClient.left = rcClient.top = 0;
rcClient.right = pRect->right - pRect->left;
rcClient.bottom = pRect->bottom - pRect->top;

return TRUE;
}

/*****************************************************************************/
/* SetEffSize : sets the effect size                                         */
/*****************************************************************************/

void CEffectWnd::SetEffSize(ERect *pRect)
{
GetEffectEditWndSize(rcEffFrame, rcEffClient, pRect);
// vstw.width = rcEffFrame.Width();
// vstw.height = rcEffFrame.Height();
SetWindowPos(&wndTop, 0, 0, rcEffFrame.Width(), rcEffFrame.Height(),
             SWP_NOACTIVATE | SWP_NOMOVE |
                 SWP_NOOWNERZORDER | SWP_NOZORDER);
if (pView)
  pView->MoveWindow(rcEffClient.left, rcEffClient.top,
                    rcEffClient.right, rcEffClient.bottom);
}

void CEffectWnd::SetEffSize(int cx, int cy)
{
CRect rcW;
GetWindowRect(&rcW);

ERect rc;
rc.left = (short)rcW.left;
rc.top = (short)rcW.top;
rc.right = rc.left + cx;
rc.bottom = rc.top + cy;
SetEffSize(&rc);
}

/*****************************************************************************/
/* OnSize : called when the WM_SIZE message comes in                         */
/*****************************************************************************/

void CEffectWnd::OnSize(UINT nType, int cx, int cy) 
{
CMDIChildWnd::OnSize(nType, cx, cy);
if ((nType != SIZE_RESTORED) || (!cx) || (!cy))
  return;
                                        /* check window size afterwards!     */
PostMessage(WM_COMMAND, IDM_EFF_CHECKSIZE);
}

/*****************************************************************************/
/* OnEffChecksize : called to check that the window is not too big now       */
/*****************************************************************************/

void CEffectWnd::OnEffChecksize() 
{
if (!pView)                             /* if not using our own view         */
  GetEffectEditWndSize(rcEffFrame,      /* get current effect edit size idea */
                       rcEffClient);

CRect rcFrame;
GetWindowRect(&rcFrame);

int cx = rcFrame.Width(), cy = rcFrame.Height();
int cxmax = rcEffFrame.Width();
int cymax = rcEffFrame.Height();
bool bReset = false;
if (cx > rcEffFrame.Width())
  {
  cx = cxmax;
  bReset = true;
  }
if (cy > cymax)
  {
  cy = cymax;
  bReset = true;
  }

if (bReset)
  SetWindowPos(&wndTop, 0, 0, cx, cy,
               SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
}

/*****************************************************************************/
/* OnEffResize : resize the effect edit window to proper dimensions          */
/*****************************************************************************/

void CEffectWnd::OnEffResize() 
{
SetEffSize(NULL);
}

/*****************************************************************************/
/* OnSizing : called when the window is being resized                        */
/*****************************************************************************/

void CEffectWnd::OnSizing(UINT fwSide, LPRECT pRect) 
{
CMDIChildWnd::OnSizing(fwSide, pRect);

if (!pView)                             /* if not using our own view         */
  GetEffectEditWndSize(rcEffFrame,      /* get current effect edit size idea */
                       rcEffClient);

if (pRect->right - pRect->left >
    rcEffFrame.right - rcEffFrame.left)
  pRect->right = pRect->left + rcEffFrame.right - rcEffFrame.left;

if (pRect->bottom - pRect->top >
    rcEffFrame.bottom - rcEffFrame.top)
  pRect->bottom = pRect->top + rcEffFrame.bottom - rcEffFrame.top;
	
}

/*****************************************************************************/
/* OnEffEditParms : called to switch to the parameters window                */
/*****************************************************************************/

void CEffectWnd::OnEffEditParms() 
{
pMain->OnEffEditParms();                /* pass on to main window            */
}

/*****************************************************************************/
/* OnEffEdit : called to switch to the Effect's edit window                  */
/*****************************************************************************/

void CEffectWnd::OnEffEdit() 
{
pMain->OnEffEdit();
}

/*****************************************************************************/
/* OnUpdateEffEdit : called to update the menu item / toolbar item           */
/*****************************************************************************/

void CEffectWnd::OnUpdateEffEdit(CCmdUI* pCmdUI) 
{
pMain->OnUpdateEffEdit(pCmdUI);
}

/*****************************************************************************/
/* OnEffInfo : cativates the information window for this effect              */
/*****************************************************************************/

void CEffectWnd::OnEffInfo() 
{
pMain->MDIActivate();
}

/*****************************************************************************/
/* Update : make sure screen is up-to-date                                   */
/*****************************************************************************/

void CEffectWnd::Update()
{
SetupTitle();
Invalidate(FALSE);
}

/*****************************************************************************/
/* OnEffLoad : loads a .fxb file                                             */
/*****************************************************************************/

void CEffectWnd::OnEffLoad() 
{
pMain->OnEffLoad();
}

/*****************************************************************************/
/* OnEffSave : saves current bank                                            */
/*****************************************************************************/

void CEffectWnd::OnEffSave() 
{
pMain->OnEffSave();
}

/*****************************************************************************/
/* OnEffSaveas : saves current bank to a new file                            */
/*****************************************************************************/

void CEffectWnd::OnEffSaveas() 
{
pMain->OnEffSaveas();
}

/*****************************************************************************/
/* OnEffSelprogram : loads a program                                         */
/*****************************************************************************/

void CEffectWnd::OnEffSelprogram() 
{
pMain->OnEffSelprogram();
}

/*****************************************************************************/
/* OnUpdateEffSelprogram : updates the visual appearance                     */
/*****************************************************************************/

void CEffectWnd::OnUpdateEffSelprogram(CCmdUI* pCmdUI) 
{
pMain->OnUpdateEffSelprogram(pCmdUI);
}

/*****************************************************************************/
/* OnSelMidichn : selects MIDI channels for the effect                       */
/*****************************************************************************/

void CEffectWnd::OnSelMidichn() 
{
pMain->OnSelMidichn();
}

/*****************************************************************************/
/* MakeVstKeyCode : converts from Windows to VST                             */
/*****************************************************************************/

void CEffectWnd::MakeVstKeyCode(UINT nChar, UINT nRepCnt, UINT nFlags, VstKeyCode &keyCode)
{
#if defined(VST_2_1_EXTENSIONS)

static struct
  {
  UINT vkWin;
  unsigned char vstVirt;
  } VKeys[] =
  {
    { VK_BACK,      VKEY_BACK      },
    { VK_TAB,       VKEY_TAB       },
    { VK_CLEAR,     VKEY_CLEAR     },
    { VK_RETURN,    VKEY_RETURN    },
    { VK_PAUSE,     VKEY_PAUSE     },
    { VK_ESCAPE,    VKEY_ESCAPE    },
    { VK_SPACE,     VKEY_SPACE     },
//  { VK_NEXT,      VKEY_NEXT      },
    { VK_END,       VKEY_END       },
    { VK_HOME,      VKEY_HOME      },
    { VK_LEFT,      VKEY_LEFT      },
    { VK_UP,        VKEY_UP        },
    { VK_RIGHT,     VKEY_RIGHT     },
    { VK_DOWN,      VKEY_DOWN      },
    { VK_PRIOR,     VKEY_PAGEUP    },
    { VK_NEXT,      VKEY_PAGEDOWN  },
    { VK_SELECT,    VKEY_SELECT    },
    { VK_PRINT,     VKEY_PRINT     },
    { VK_EXECUTE,   VKEY_ENTER     },
    { VK_SNAPSHOT,  VKEY_SNAPSHOT  },
    { VK_INSERT,    VKEY_INSERT    },
    { VK_DELETE,    VKEY_DELETE    },
    { VK_HELP,      VKEY_HELP      },
    { VK_NUMPAD0,   VKEY_NUMPAD0   },
    { VK_NUMPAD1,   VKEY_NUMPAD1   },
    { VK_NUMPAD2,   VKEY_NUMPAD2   },
    { VK_NUMPAD3,   VKEY_NUMPAD3   },
    { VK_NUMPAD4,   VKEY_NUMPAD4   },
    { VK_NUMPAD5,   VKEY_NUMPAD5   },
    { VK_NUMPAD6,   VKEY_NUMPAD6   },
    { VK_NUMPAD7,   VKEY_NUMPAD7   },
    { VK_NUMPAD8,   VKEY_NUMPAD8   },
    { VK_NUMPAD9,   VKEY_NUMPAD9   },
    { VK_MULTIPLY,  VKEY_MULTIPLY  },
    { VK_ADD,       VKEY_ADD,      },
    { VK_SEPARATOR, VKEY_SEPARATOR },
    { VK_SUBTRACT,  VKEY_SUBTRACT  },
    { VK_DECIMAL,   VKEY_DECIMAL   },
    { VK_DIVIDE,    VKEY_DIVIDE    },
    { VK_F1,        VKEY_F1        },
    { VK_F2,        VKEY_F2        },
    { VK_F3,        VKEY_F3        },
    { VK_F4,        VKEY_F4        },
    { VK_F5,        VKEY_F5        },
    { VK_F6,        VKEY_F6        },
    { VK_F7,        VKEY_F7        },
    { VK_F8,        VKEY_F8        },
    { VK_F9,        VKEY_F9        },
    { VK_F10,       VKEY_F10       },
    { VK_F11,       VKEY_F11       },
    { VK_F12,       VKEY_F12       },
    { VK_NUMLOCK,   VKEY_NUMLOCK   },
    { VK_SCROLL,    VKEY_SCROLL    },
    { VK_SHIFT,     VKEY_SHIFT     },
    { VK_CONTROL,   VKEY_CONTROL   },
    { VK_MENU,      VKEY_ALT       },
//  { VK_EQUALS,    VKEY_EQUALS    },

  };

if ((nChar >= 'A') && (nChar <= 'Z'))
  keyCode.character = nChar + ('a' - 'A');
else
  keyCode.character = nChar;
keyCode.virt = 0;
for (int i = 0; i < (sizeof(VKeys)/sizeof(VKeys[0])); i++)
  if (nChar == VKeys[i].vkWin)
    {
    keyCode.virt = VKeys[i].vstVirt;
    break;
    }
keyCode.modifier = 0;
if (GetKeyState(VK_SHIFT) & 0x8000)
  keyCode.modifier |= MODIFIER_SHIFT;
if (GetKeyState(VK_CONTROL) & 0x8000)
  keyCode.modifier |= MODIFIER_CONTROL;
if (GetKeyState(VK_MENU) & 0x8000)
  keyCode.modifier |= MODIFIER_ALTERNATE;
#endif
}

/*****************************************************************************/
/* OnKeyDown : called when a key is pressed on the effect window             */
/*****************************************************************************/

void CEffectWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
#if defined(VST_2_1_EXTENSIONS)
VstKeyCode keyCode;
MakeVstKeyCode(nChar, nRepCnt, nFlags, keyCode);
if (pHost->EffKeyDown(nEffect, keyCode) != 1)
#endif

  CMDIChildWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

/*****************************************************************************/
/* OnKeyUp : called when a key is released on the effect window              */
/*****************************************************************************/

void CEffectWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
#if defined(VST_2_1_EXTENSIONS)
VstKeyCode keyCode;
MakeVstKeyCode(nChar, nRepCnt, nFlags, keyCode);
if (pHost->EffKeyUp(nEffect, keyCode) != 1)
#endif

  CMDIChildWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

/*****************************************************************************/
/* OnSysKeyDown : called when a system key is pressed                        */
/*****************************************************************************/

void CEffectWnd::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
#if defined(VST_2_1_EXTENSIONS)
VstKeyCode keyCode;
MakeVstKeyCode(nChar, nRepCnt, nFlags, keyCode);
if (pHost->EffKeyDown(nEffect, keyCode) != 1)
#endif

  CMDIChildWnd::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

/*****************************************************************************/
/* OnSysKeyUp : called when a system key is released                         */
/*****************************************************************************/

void CEffectWnd::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
#if defined(VST_2_1_EXTENSIONS)
VstKeyCode keyCode;
MakeVstKeyCode(nChar, nRepCnt, nFlags, keyCode);
if (pHost->EffKeyUp(nEffect, keyCode) != 1)
#endif
	
  CMDIChildWnd::OnSysKeyUp(nChar, nRepCnt, nFlags);
}

/*****************************************************************************/
/* OnChainAfter : called to setup an effect chain                            */
/*****************************************************************************/

void CEffectWnd::OnChainAfter() 
{
pMain->OnChainAfter();
}

/*****************************************************************************/
/* OnUpdateChainAfter : updates the visual appearance                        */
/*****************************************************************************/

void CEffectWnd::OnUpdateChainAfter(CCmdUI* pCmdUI) 
{
pMain->OnUpdateChainAfter(pCmdUI);
}

/*****************************************************************************/
/* OnUnchain : called to break a chain                                       */
/*****************************************************************************/

void CEffectWnd::OnUnchain() 
{
pMain->OnUnchain();
}

/*****************************************************************************/
/* OnUpdateUnchain : called to update the visual appearance                  */
/*****************************************************************************/

void CEffectWnd::OnUpdateUnchain(CCmdUI* pCmdUI) 
{
pMain->OnUpdateUnchain(pCmdUI);
}

/*****************************************************************************/
/* OnProgramNext : called to switch to the next program                      */
/*****************************************************************************/

void CEffectWnd::OnProgramNext() 
{
pMain->OnProgramNext();
}

/*****************************************************************************/
/* OnUpdateProgramNext : updates the visual appearance                       */
/*****************************************************************************/

void CEffectWnd::OnUpdateProgramNext(CCmdUI* pCmdUI) 
{
pMain->OnUpdateProgramNext(pCmdUI);
}

/*****************************************************************************/
/* OnProgramPrev : called to switch to the previous program                  */
/*****************************************************************************/

void CEffectWnd::OnProgramPrev() 
{
pMain->OnProgramPrev();
}

/*****************************************************************************/
/* OnUpdateProgramPrev : updates the visual appearance                       */
/*****************************************************************************/

void CEffectWnd::OnUpdateProgramPrev(CCmdUI* pCmdUI) 
{
pMain->OnUpdateProgramPrev(pCmdUI);
}
