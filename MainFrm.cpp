// MainFrm.cpp : implementation of the CMainFrame class

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

#include "effectwnd.h"
#include "ChildFrm.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GetApp()  ((CVsthostApp *)AfxGetApp())

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_INITMENUPOPUP()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

/*****************************************************************************/
/* OnCreate : called when the main frame window is created                   */
/*****************************************************************************/

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
  return -1;
m_wndMDIClient.SetBackgroundColor(RGB(31, 63, 191));
m_wndMDIClient.SetBitmap(IDB_MDIBACKGND);
if (!m_wndMDIClient.SubclassWindow (m_hWndMDIClient))
  {
  TRACE ("Failed to subclass MDI client window\n");
  return -1;
  }
if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
    | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
    !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
  {
  TRACE0("Failed to create toolbar\n");
  return -1;      // fail to create
  }

if (!m_wndStatusBar.Create(this) ||
    !m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
  {
  TRACE0("Failed to create status bar\n");
  return -1;      // fail to create
  }

// TODO: Delete these three lines if you don't want the toolbar to
//  be dockable
m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
EnableDocking(CBRS_ALIGN_ANY);
DockControlBar(&m_wndToolBar);

RecalcLayout();

SetTimer(1, 100, NULL);

return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/*****************************************************************************/
/* OnTimer : called when a WM_TIMER message comes in                         */
/*****************************************************************************/

void CMainFrame::OnTimer(UINT nIDEvent) 
{
switch (nIDEvent)
  {
  case 1 :
    {
    CEffect *pEffect;
    int i, j = GetApp()->vstHost.GetSize();
    for (i = 0; i < j; i++)
      {
      pEffect = GetApp()->vstHost.GetAt(i);
      if (pEffect)
        {
        pEffect->EffIdle();
        pEffect->EffEditIdle();
        }
      }
    }
    break;
  }
	
CMDIFrameWnd::OnTimer(nIDEvent);
}

/*****************************************************************************/
/* OnInitMenuPopup : called when a popup menu is initialized                 */
/*****************************************************************************/

void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
                                        /* if Effect Edit menu popping up    */
if ((pPopupMenu->GetMenuItemCount() > 0) &&
    (pPopupMenu->GetMenuItemID(0) == IDM_EFF_PROGRAM_NAME))
  {
  CWnd *pActive = MDIGetActive();
  if (!pActive)
    return;
  int nEffect = -1;

  if (pActive->IsKindOf(RUNTIME_CLASS(CChildFrame)))
    nEffect = ((CChildFrame *)pActive)->GetEffect();
  else if (pActive->IsKindOf(RUNTIME_CLASS(CEffectWnd)))
    nEffect = ((CEffectWnd *)pActive)->GetEffect();

  if (nEffect >= 0)
    GetApp()->FillPopup(pPopupMenu, nEffect);
  }

CMDIFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
}

/*****************************************************************************/
/* OnClose : called when the main window is closed                           */
/*****************************************************************************/

void CMainFrame::OnClose() 
{	
CMDIFrameWnd::OnClose();
}

/*===========================================================================*/
/* CMainMdiClient class members                                              */
/*===========================================================================*/

/*****************************************************************************/
/* CMainMdiClient : constructor                                              */
/*****************************************************************************/

CMainMdiClient::CMainMdiClient()
{
pBmp = 0;
clrBack = RGB(127, 127, 127);
}

/*****************************************************************************/
/* ~CMainMdiClient : destructor                                              */
/*****************************************************************************/

CMainMdiClient::~CMainMdiClient()
{
if (pBmp)
  {
  pBmp->DeleteObject();
  delete pBmp;
  }
}

/*****************************************************************************/
/* CMainMdiClient message map                                                */
/*****************************************************************************/

BEGIN_MESSAGE_MAP(CMainMdiClient, CWnd)
	//{{AFX_MSG_MAP(CMainMdiClient)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/*****************************************************************************/
/* GetSuperWndProcAddr : return the superclass proc address                  */
/*****************************************************************************/

WNDPROC *CMainMdiClient::GetSuperWndProcAddr()
{
static WNDPROC NEAR pfnSuper = NULL;
return &pfnSuper;
}

/*****************************************************************************/
/* OnEraseBkgnd : called to erase the backround                              */
/*****************************************************************************/

BOOL CMainMdiClient::OnEraseBkgnd(CDC* pDC) 
{
#if 1
// allow free background definition
CRect rect;
if (pBmp)                               /* if background image there         */
  {                                     /* paint background image            */
  BITMAP bm;
  CDC dcMem;
  GetClientRect(&rect);                 /* redraw whole window               */
  VERIFY(pBmp->GetObject(sizeof(BITMAP), (LPVOID)&bm));
  dcMem.CreateCompatibleDC(NULL);
  CBitmap* pOldBitmap = dcMem.SelectObject(pBmp);
  int w = rect.Width(), h = rect.Height();
  
  for (register int nX = 0; nX < w; nX += bm.bmWidth)
    for (register int nY = 0; nY < h; nY += bm.bmHeight)
      pDC->BitBlt(nX, nY, bm.bmWidth, bm.bmHeight, &dcMem, 0, 0, SRCCOPY);
    
  dcMem.SelectObject(pOldBitmap);
  }
else                                    /* otherwise use background color    */
  {
  pDC->GetClipBox(&rect);               /* get area to erase                 */

  CBrush backBrush(clrBack);            /* set up background color           */
                                        /* save old brush                    */
  CBrush* pOldBrush = pDC->SelectObject(&backBrush);

                                        /* then fill area with brush         */
  pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(),
              PATCOPY);
  pDC->SelectObject(pOldBrush);         /* use old brush again               */
  }

return TRUE;                            /* pass back "Done"                  */

#else	
// let windows erase the background
return CWnd::OnEraseBkgnd(pDC);
#endif
}

/*****************************************************************************/
/* SetBitmap : allows to define a background bitmap                          */
/*****************************************************************************/

BOOL CMainMdiClient::SetBitmap(UINT idResource)
{
if (pBmp)
  {
  pBmp->DeleteObject();
  delete pBmp;
  }

pBmp = new CBitmap();
BOOL bResult = pBmp->LoadBitmap(idResource);

return bResult;
}
