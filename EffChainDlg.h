/*****************************************************************************/
/* EffChainDlg.h : header file                                               */
/*****************************************************************************/

#if !defined(AFX_EFFCHAINDLG_H__5ABD3223_8938_11D8_8215_4000001054B2__INCLUDED_)
#define AFX_EFFCHAINDLG_H__5ABD3223_8938_11D8_8215_4000001054B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*****************************************************************************/
/* CEffChainDlg dialog                                                       */
/*****************************************************************************/

class CSmpEffect;
class CEffChainDlg : public CDialog
{
// Construction
public:
	CEffChainDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEffChainDlg)
	enum { IDD = IDD_EFFCHAIN };
	CListBox	lbEffects;
	//}}AFX_DATA
    CSmpEffect *pCurEff;
    int nSelEff;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffChainDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEffChainDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkEffects();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFCHAINDLG_H__5ABD3223_8938_11D8_8215_4000001054B2__INCLUDED_)
