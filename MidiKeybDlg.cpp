/*****************************************************************************/
/* MidiKeybDlg.cpp : implementation file                                     */
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

#include "mfcmidi.h"
#include "MidiKeybDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*===========================================================================*/
/* CMidiKeybDlg modeless dialog class members                                */
/*===========================================================================*/

/*****************************************************************************/
/* CMidiKeybDlg : constructor                                                */
/*****************************************************************************/

CMidiKeybDlg::CMidiKeybDlg()
	: CDialog()
{
	//{{AFX_DATA_INIT(CMidiKeybDlg)
	//}}AFX_DATA_INIT
}

/*****************************************************************************/
/* DoDataExchange : data exchange between dialog and object                  */
/*****************************************************************************/

void CMidiKeybDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMidiKeybDlg)
	//}}AFX_DATA_MAP
}

/*****************************************************************************/
/* CMidiKeybDlg message map                                                  */
/*****************************************************************************/

BEGIN_MESSAGE_MAP(CMidiKeybDlg, CDialog)
	//{{AFX_MSG_MAP(CMidiKeybDlg)
	ON_WM_CLOSE()
	ON_WM_INITMENUPOPUP()
	ON_WM_SYSCOMMAND()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*****************************************************************************/
/* CMidiKeybDlg event sink map                                               */
/*****************************************************************************/

BEGIN_EVENTSINK_MAP(CMidiKeybDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CMidiKeybDlg)
	ON_EVENT(CMidiKeybDlg, IDC_MKBDCTRL, 1 /* MidiMsg */, OnMidiMsgMkbdctrl, VTS_I4)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/*****************************************************************************/
/* OnClose : called when the window is closed                                */
/*****************************************************************************/

void CMidiKeybDlg::OnClose() 
{
CDialog::OnClose();
DestroyWindow();                        /* destroy the window                */
delete this;                            /* and remove ourselves.             */
GetApp()->SetMidiKeybDlg(NULL);
}

/*****************************************************************************/
/* OnMidiMsgMkbdctrl : called when a MIDI message comes in from kbd          */
/*****************************************************************************/

void CMidiKeybDlg::OnMidiMsgMkbdctrl(long msg) 
{                                       /* pass on to VST Host               */
GetApp()->vstHost.OnMidiIn(CMidiMsg(msg));
}

/*****************************************************************************/
/* OnInitMenuPopup : called when the menu is initialized                     */
/*****************************************************************************/

void CMidiKeybDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
if (pPopupMenu->GetMenuItemID(0) != IDM_MIDIKEYB_PROPERTIES)
  {
  CString sProperties;
  sProperties.LoadString(IDS_PROPERTIES);
  pPopupMenu->InsertMenu(0, MF_BYPOSITION, IDM_MIDIKEYB_PROPERTIES, sProperties);
  pPopupMenu->InsertMenu(1, MF_SEPARATOR | MF_BYPOSITION);
  }
CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
}

/*****************************************************************************/
/* OnSysCommand : called when a system command comes in                      */
/*****************************************************************************/

void CMidiKeybDlg::OnSysCommand(UINT nID, LPARAM lParam) 
{
                                        /* allow property redefinition       */
if ((nID & 0xFFF0) == IDM_MIDIKEYB_PROPERTIES)
  {
  LPUNKNOWN lpUnk = GetDlgItem(IDC_MKBDCTRL)->GetControlUnknown();
  LPOLEOBJECT lpObj;
  if (SUCCEEDED(lpUnk->QueryInterface(IID_IOleObject, (LPVOID *)&lpObj)))
    {
    IOleClientSite *lpClSite;
    if (SUCCEEDED(lpObj->GetClientSite(&lpClSite)))
      {
      CRect rc;
      GetClientRect(&rc);
      
      lpObj->DoVerb(OLEIVERB_PROPERTIES, NULL, lpClSite, 0, GetSafeHwnd(), &rc);
      lpClSite->Release();
      }
    lpObj->Release();
    }
  }
else                                    /* all other system menu commands    */
  CDialog::OnSysCommand(nID, lParam);
}

/*****************************************************************************/
/* OnSize : called when the dialog is resized                                */
/*****************************************************************************/

void CMidiKeybDlg::OnSize(UINT nType, int cx, int cy) 
{
CDialog::OnSize(nType, cx, cy);
CWnd *pWnd = GetDlgItem(IDC_MKBDCTRL);
if (pWnd)
  pWnd->MoveWindow(0, 0, cx, cy);
}
