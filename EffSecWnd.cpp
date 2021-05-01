/*****************************************************************************/
/* EffSecWnd.cpp: implementation of the CEffSecWnd class.                    */
/*****************************************************************************/

#include "stdafx.h"
#include "vsthost.h"
#include "EffSecWnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define GetApp()  ((CVsthostApp *)AfxGetApp())

/*===========================================================================*/
/* CEffSecWnd class members                                                  */
/*===========================================================================*/

IMPLEMENT_DYNCREATE(CEffSecWnd, CEffectWnd)

/*****************************************************************************/
/* CEffSecWnd : constructor                                                  */
/*****************************************************************************/

CEffSecWnd::CEffSecWnd()
{

}

/*****************************************************************************/
/* ~CEffSecWnd : destructor                                                  */
/*****************************************************************************/

CEffSecWnd::~CEffSecWnd()
{

}

/*****************************************************************************/
/* CEffSecWnd message map                                                    */
/*****************************************************************************/

BEGIN_MESSAGE_MAP(CEffSecWnd, CEffectWnd)
	//{{AFX_MSG_MAP(CEffSecWnd)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*****************************************************************************/
/* OnClose : called when the parameter edit window is closed                 */
/*****************************************************************************/

void CEffSecWnd::OnClose() 
{
// we need to skip CEffectWnd::OnClose() here since it would try to close an
// (eventually unopened) effect edit window... and would report the wrong
// window as closed to the main window.

// pMain->ParmClosed();
CWnd::OnClose();
}
