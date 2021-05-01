#if !defined(AFX_PROGNAMEDLG_H__84FABB25_A776_11D5_8169_4000001054B2__INCLUDED_)
#define AFX_PROGNAMEDLG_H__84FABB25_A776_11D5_8169_4000001054B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgNameDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProgNameDlg dialog

class CProgNameDlg : public CDialog
{
// Construction
public:
	CProgNameDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProgNameDlg)
	enum { IDD = IDD_EFFPGNAME };
	CString	sPgName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgNameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProgNameDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGNAMEDLG_H__84FABB25_A776_11D5_8169_4000001054B2__INCLUDED_)
