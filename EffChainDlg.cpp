/*****************************************************************************/
/* EffChainDlg.cpp : implementation file                                     */
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

#include "SmpVSTHost.h"
#include "EffChainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEffChainDlg dialog


CEffChainDlg::CEffChainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEffChainDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEffChainDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
pCurEff = NULL;
nSelEff = -1;
}

/*****************************************************************************/
/* DoDataExchange : data exchange between dialog and object                  */
/*****************************************************************************/

void CEffChainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffChainDlg)
	DDX_Control(pDX, IDC_EFFECTS, lbEffects);
	//}}AFX_DATA_MAP
}

/*****************************************************************************/
/* CEffChainDlg message map                                                  */
/*****************************************************************************/

BEGIN_MESSAGE_MAP(CEffChainDlg, CDialog)
	//{{AFX_MSG_MAP(CEffChainDlg)
	ON_LBN_DBLCLK(IDC_EFFECTS, OnDblclkEffects)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*****************************************************************************/
/* OnInitDialog : called when the dialog is initialized                      */
/*****************************************************************************/

BOOL CEffChainDlg::OnInitDialog() 
{
CDialog::OnInitDialog();

if (!pCurEff)                           /* if no current effect,             */
  return TRUE;                          /* start with empty list box         */

int i, j;
CSmpVSTHost &Host = GetApp()->vstHost;
CSmpEffect *pTest;
for (i = 0; i < Host.GetSize(); i++)    /* walk through loaded effects       */
  {
  pTest = (CSmpEffect *)Host.GetAt(i);
  if ((!pTest) || (pTest == pCurEff))   /* if not there or current one       */
    continue;                           /* don't use that one.               */
  CString sOut;
  sOut.Format("%d: %s", i, pTest->GetDisplayName());
  j = lbEffects.AddString(sOut);
  if (j != LB_ERR)
    lbEffects.SetItemData(j, i);
  }
	
return TRUE;  // return TRUE unless you set the focus to a control
              // EXCEPTION: OCX Property Pages should return FALSE
}

/*****************************************************************************/
/* OnOK : called when the OK button is pressed                               */
/*****************************************************************************/

void CEffChainDlg::OnOK() 
{
int nSel = lbEffects.GetCurSel();
if (nSel == LB_ERR)
  nSelEff = -1;
else
  nSelEff = (int)lbEffects.GetItemData(nSel);
CDialog::OnOK();
}

/*****************************************************************************/
/* OnDblclkEffects : called upon double-clicks in the listbox                */
/*****************************************************************************/

void CEffChainDlg::OnDblclkEffects() 
{
int nSel = lbEffects.GetCurSel();
if (nSel != LB_ERR)
  OnOK();
}
