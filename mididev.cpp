/*****************************************************************************/
/* MIDIDEV.CPP : MIDI Device Selection                                       */
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

#ifndef NDEBUG
#ifdef __WATCOMC__
#pragma off(unreferenced)
#endif
#endif

#include "stdafx.h"                     /* MFC include                       */

#include "specmidi.h"                   /* Special MIDI classes              */
#include "mididev.h"                    /* private prototypes                */

#ifdef _DEBUG
#ifdef WIN32
#define new DEBUG_NEW
#endif
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*****************************************************************************/
/* Global Data                                                               */
/*****************************************************************************/

/*===========================================================================*/
/* CMidiDeviceDialog member functions                                        */
/*===========================================================================*/

BEGIN_MESSAGE_MAP( CMidiDeviceDialog, CDialog )
  //{{AFX_MSG_MAP( CMidiDeviceDialog )
		// NOTE: the ClassWizard will add message map macros here
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*****************************************************************************/
/* CMidiDeviceDialog : constructor                                           */
/*****************************************************************************/

CMidiDeviceDialog::CMidiDeviceDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CMidiDeviceDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMidiDeviceDialog)
	bPassThru = FALSE;
	//}}AFX_DATA_INIT
}

/*****************************************************************************/
/* OnInitDialog : called upon dialog initialization                          */
/*****************************************************************************/

BOOL CMidiDeviceDialog::OnInitDialog ( void )
{
CMidiInDeviceList ListIn;               /* input device list                 */
CMidiOutDeviceList ListOut;             /* output device list                */
int i;
CComboBox *c;

c = (CComboBox *)GetDlgItem(IDC_INPUTPORT);
CString sNoMidi;
sNoMidi.LoadString(IDS_MIDINONE);
c->AddString(sNoMidi);
for (i = 0; i <= ListIn.GetUpperBound(); i++)
  c->AddString(ListIn[i]);
if (c->SelectString(-1, InName) == CB_ERR)
  c->SetCurSel(0);

c = (CComboBox *)GetDlgItem(IDC_OUTPUTPORT);
c->AddString(sNoMidi);
for (i = 0; i <= ListOut.GetUpperBound(); i++)
  c->AddString(ListOut[i]);
if (c->SelectString(-1, OutName) == CB_ERR)
  c->SetCurSel(0);

return CDialog::OnInitDialog();
}

/*****************************************************************************/
/* DoDataExchange : data exchange table                                      */
/*****************************************************************************/

void CMidiDeviceDialog::DoDataExchange
    (
    CDataExchange * pDX
    )
{
CDialog::DoDataExchange(pDX);

    //{{AFX_DATA_MAP(CMidiDeviceDialog)
	DDX_Check(pDX, IDC_PASSTHRU, bPassThru);
	//}}AFX_DATA_MAP
}

/*****************************************************************************/
/* OnOK : called upon dialog termination                                     */
/*****************************************************************************/

void CMidiDeviceDialog::OnOK ( void )
{
GetDlgItemText(IDC_INPUTPORT, InName);
GetDlgItemText(IDC_OUTPUTPORT, OutName);

CDialog::OnOK();
}
