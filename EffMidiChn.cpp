// EffMidiChn.cpp : implementation file

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
#include "EffMidiChn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEffMidiChn dialog


CEffMidiChn::CEffMidiChn(CWnd* pParent /*=NULL*/)
	: CDialog(CEffMidiChn::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEffMidiChn)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
wChnMask = 0xFFFF;
}


void CEffMidiChn::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffMidiChn)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEffMidiChn, CDialog)
	//{{AFX_MSG_MAP(CEffMidiChn)
	ON_BN_CLICKED(IDC_SET, OnSet)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffMidiChn message handlers

BOOL CEffMidiChn::OnInitDialog() 
{
	CDialog::OnInitDialog();

    for (int i = 0; i < 16; i++)
      ((CButton *)GetDlgItem(IDC_CHN0 + i))->SetCheck(!!(wChnMask & (1 << i)));
    
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEffMidiChn::OnOK() 
{
wChnMask = 0;

for (int i = 0; i < 16; i++)
  if (((CButton *)GetDlgItem(IDC_CHN0 + i))->GetCheck() == 1)
    wChnMask |= (1 << i);
	
CDialog::OnOK();
}

void CEffMidiChn::OnSet() 
{
for (int i = 0; i < 16; i++)
  ((CButton *)GetDlgItem(IDC_CHN0 + i))->SetCheck(1);
}

void CEffMidiChn::OnClear() 
{
for (int i = 0; i < 16; i++)
  ((CButton *)GetDlgItem(IDC_CHN0 + i))->SetCheck(0);
}
