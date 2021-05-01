/*****************************************************************************/
/* MidiKeybDlg.h : header file                                               */
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

#if !defined(AFX_MIDIKEYBDLG_H__70067495_0898_11D9_828A_40000010EAB2__INCLUDED_)
#define AFX_MIDIKEYBDLG_H__70067495_0898_11D9_828A_40000010EAB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*****************************************************************************/
/* CMidiKeybDlg dialog declaration                                           */
/*****************************************************************************/

class CMidiKeybDlg : public CDialog
{
// Construction
public:
    CMidiKeybDlg();

// Dialog Data
	//{{AFX_DATA(CMidiKeybDlg)
	enum { IDD = IDD_MIDIKEYB };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMidiKeybDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMidiKeybDlg)
	afx_msg void OnClose();
	afx_msg void OnMidiMsgMkbdctrl(long msg);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIDIKEYBDLG_H__70067495_0898_11D9_828A_40000010EAB2__INCLUDED_)
