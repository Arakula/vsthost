// ProgNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vsthost.h"
#include "ProgNameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgNameDlg dialog


CProgNameDlg::CProgNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgNameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgNameDlg)
	sPgName = _T("");
	//}}AFX_DATA_INIT
}


void CProgNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgNameDlg)
	DDX_Text(pDX, IDC_PROGNAME, sPgName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgNameDlg, CDialog)
	//{{AFX_MSG_MAP(CProgNameDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgNameDlg message handlers
