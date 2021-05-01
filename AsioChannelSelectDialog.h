#if !defined(AFX_ASIOCHANNELSELECTDIALOG_H__A0A33283_2677_11D9_82A9_40000010EAB2__INCLUDED_)
#define AFX_ASIOCHANNELSELECTDIALOG_H__A0A33283_2677_11D9_82A9_40000010EAB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AsioChannelSelectDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAsioChannelSelectDialog dialog

class CAsioChannelSelectDialog : public CDialog
{
// Construction
public:
	CStringArray saOut;
	CStringArray saIn;
	CAsioChannelSelectDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAsioChannelSelectDialog)
	enum { IDD = IDD_ASIOCHN };
	int		nSelIn;
	int		nSelOut;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAsioChannelSelectDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAsioChannelSelectDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASIOCHANNELSELECTDIALOG_H__A0A33283_2677_11D9_82A9_40000010EAB2__INCLUDED_)
