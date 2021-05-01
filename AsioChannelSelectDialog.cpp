/*****************************************************************************/
/* AsioChannelSelectDialog.cpp : implementation file                         */
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
#include "AsioChannelSelectDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAsioChannelSelectDialog dialog


CAsioChannelSelectDialog::CAsioChannelSelectDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAsioChannelSelectDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAsioChannelSelectDialog)
	nSelIn = -1;
	nSelOut = -1;
	//}}AFX_DATA_INIT
}


void CAsioChannelSelectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAsioChannelSelectDialog)
	DDX_CBIndex(pDX, IDC_INPUT_CHANNELS, nSelIn);
	DDX_CBIndex(pDX, IDC_OUTPUT_CHANNELS, nSelOut);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAsioChannelSelectDialog, CDialog)
	//{{AFX_MSG_MAP(CAsioChannelSelectDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAsioChannelSelectDialog message handlers

BOOL CAsioChannelSelectDialog::OnInitDialog() 
{
int i;
CComboBox *pCb = (CComboBox *)GetDlgItem(IDC_INPUT_CHANNELS);
for (i = 0; i < saIn.GetSize(); i++)
  pCb->AddString(saIn[i]);
pCb = (CComboBox *)GetDlgItem(IDC_OUTPUT_CHANNELS);
for (i = 0; i < saOut.GetSize(); i++)
  pCb->AddString(saOut[i]);

CDialog::OnInitDialog();
	
return TRUE;  // return TRUE unless you set the focus to a control
              // EXCEPTION: OCX Property Pages should return FALSE
}
