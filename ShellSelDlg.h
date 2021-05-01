/*****************************************************************************/
/* ShellSelDlg.h : header file                                               */
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

#if !defined(AFX_SHELLSELDLG_H__F25A7333_B17B_11D9_82CF_40000010EAB2__INCLUDED_)
#define AFX_SHELLSELDLG_H__F25A7333_B17B_11D9_82CF_40000010EAB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*****************************************************************************/
/* CShellSelDlg dialog                                                       */
/*****************************************************************************/

class CShellSelDlg : public CDialog
{
// Construction
public:
	CShellSelDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CShellSelDlg)
	enum { IDD = IDD_SHELLSEL };
	CListBox	lbNames;
	//}}AFX_DATA

    CStringArray saNames;
    int nSelID;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShellSelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShellSelDlg)
	afx_msg void OnDblclkNames();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeNames();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHELLSELDLG_H__F25A7333_B17B_11D9_82CF_40000010EAB2__INCLUDED_)
