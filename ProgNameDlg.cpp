/*****************************************************************************/
/* ProgNameDlg.cpp : implementation file                                     */
/*****************************************************************************/

#include "stdafx.h"
#include "ProgNameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*===========================================================================*/
/* CProgNameDlg dialog                                                       */
/*===========================================================================*/

/*****************************************************************************/
/* CProgNameDlg : constructor                                                */
/*****************************************************************************/

CProgNameDlg::CProgNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgNameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgNameDlg)
	sPgName = _T("");
	//}}AFX_DATA_INIT
}

/*****************************************************************************/
/* DoDataExchange : data exchange between dialog and object                  */
/*****************************************************************************/

void CProgNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgNameDlg)
	DDX_Text(pDX, IDC_PROGNAME, sPgName);
	//}}AFX_DATA_MAP
}

/*****************************************************************************/
/* CProgNameDlg message map                                                  */
/*****************************************************************************/

BEGIN_MESSAGE_MAP(CProgNameDlg, CDialog)
	//{{AFX_MSG_MAP(CProgNameDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
