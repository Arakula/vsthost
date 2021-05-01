/*****************************************************************************/
/* MidiKeybDlg.h : header file                                               */
/*****************************************************************************/
/*                                                                           */
/* $Archive::                                                              $ */
/* $Revision::                                                             $ */
/* $Date::                                                                 $ */
/* $Author::                                                               $ */
/* $Modtime::                                                              $ */
/* $Workfile::                                                             $ */
/*                                                                           */
/*****************************************************************************/

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
