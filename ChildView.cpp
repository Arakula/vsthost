// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "vsthost.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildView

CChildView::CChildView()
{
nMaxwidth = 0;
nItemHeight = 0;
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CListBox)
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
    cs.style |= WS_HSCROLL;
	cs.style &= ~(WS_BORDER | LBS_SORT);

#if 0
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);
#endif
	return TRUE;
}

/*****************************************************************************/
/* OnCreate : called when the attached window has been created               */
/*****************************************************************************/

int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
if (CListBox::OnCreate(lpCreateStruct) == -1)
  return -1;

CDC dc;                                 /* get font height information       */
dc.CreateCompatibleDC(NULL);            /* if retrievable,                   */

LOGFONT lf = {0};
lf.lfHeight = -MulDiv(8, dc.GetDeviceCaps(LOGPIXELSY), 72);
lstrcpy(lf.lfFaceName, "MS Sans Serif");
lf.lfWeight = FW_NORMAL;
fLb.CreateFontIndirect(&lf);
SetFont(&fLb);
	
return 0;
}

/*****************************************************************************/
/* ResetContent : resets content & size information                          */
/*****************************************************************************/

void CChildView::ResetContent()
{
CListBox::ResetContent();
SetHorizontalExtent(0);
nMaxwidth = 0;
}

/*****************************************************************************/
/* AddString : adds a string to the list box                                 */
/*****************************************************************************/

int CChildView::AddString(LPCSTR lpszItem)
{
int nLine = CListBox::AddString(lpszItem);
if (nLine >= 0)
  {
  CDC dc;
  dc.CreateCompatibleDC(NULL);
  CFont *pOFnt = dc.SelectObject(&fLb);
  CSize sz = dc.GetTextExtent(lpszItem, lstrlen(lpszItem));
  dc.SelectObject(pOFnt);
  if (sz.cx > nMaxwidth)
    {
    nMaxwidth = sz.cx;
    SetHorizontalExtent(nMaxwidth + 5);
    }
  if (sz.cy > nItemHeight)
    nItemHeight = sz.cy;
  }
return nLine;
}

/*****************************************************************************/
/* GetVerticalExtent : returns listbox height in pixels                      */
/*****************************************************************************/

int CChildView::GetVerticalExtent()
{
return GetCount() * nItemHeight;
}
