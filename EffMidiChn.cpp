// EffMidiChn.cpp : implementation file
//

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
