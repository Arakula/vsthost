/*****************************************************************************/
/* ShellSelDlg.cpp : implementation file                                     */
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
#include "ShellSelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*===========================================================================*/
/* CShellSelDlg dialog class members                                         */
/*===========================================================================*/

/*****************************************************************************/
/* CShellSelDlg : constructor                                                */
/*****************************************************************************/

CShellSelDlg::CShellSelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShellSelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShellSelDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

nSelID = -1;
}

/*****************************************************************************/
/* DoDataExchange : data exchange between dialog and object                  */
/*****************************************************************************/

void CShellSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShellSelDlg)
	DDX_Control(pDX, IDC_NAMES, lbNames);
	//}}AFX_DATA_MAP
}

/*****************************************************************************/
/* CShellSelDlg message map                                                  */
/*****************************************************************************/

BEGIN_MESSAGE_MAP(CShellSelDlg, CDialog)
	//{{AFX_MSG_MAP(CShellSelDlg)
	ON_LBN_DBLCLK(IDC_NAMES, OnDblclkNames)
	ON_LBN_SELCHANGE(IDC_NAMES, OnSelchangeNames)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*****************************************************************************/
/* OnInitDialog : dialog initialization                                      */
/*****************************************************************************/

BOOL CShellSelDlg::OnInitDialog() 
{
CDialog::OnInitDialog();

lbNames.ResetContent();
for (int i = 0; i < saNames.GetSize(); i++)
  lbNames.AddString(saNames[i]);

OnSelchangeNames();                     /* set up OK state                   */

return TRUE;  // return TRUE unless you set the focus to a control
              // EXCEPTION: OCX Property Pages should return FALSE
}

/*****************************************************************************/
/* OnDblclkNames : called when an entry in the list is double-clicked        */
/*****************************************************************************/

void CShellSelDlg::OnDblclkNames() 
{
PostMessage(WM_COMMAND, IDOK);	
}

/*****************************************************************************/
/* OnOK : called when the OK button is clicked                               */
/*****************************************************************************/

void CShellSelDlg::OnOK() 
{
nSelID = lbNames.GetCurSel();
if (nSelID < 0)
  return;
	
CDialog::OnOK();
}

/*****************************************************************************/
/* OnSelchangeNames : called when the name selection changes                 */
/*****************************************************************************/

void CShellSelDlg::OnSelchangeNames() 
{
int nIdx = lbNames.GetCurSel();
GetDlgItem(IDOK)->EnableWindow(nIdx >= 0);
}
