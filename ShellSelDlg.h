/*****************************************************************************/
/* ShellSelDlg.h : header file                                               */
/*****************************************************************************/

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
