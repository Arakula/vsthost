#if !defined(AFX_EFFMIDICHN_H__BDD36841_27B1_11D7_80A8_4000001054B2__INCLUDED_)
#define AFX_EFFMIDICHN_H__BDD36841_27B1_11D7_80A8_4000001054B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EffMidiChn.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEffMidiChn dialog

class CEffMidiChn : public CDialog
{
// Construction
public:
	CEffMidiChn(CWnd* pParent = NULL);   // standard constructor

    WORD wChnMask;

// Dialog Data
	//{{AFX_DATA(CEffMidiChn)
	enum { IDD = IDD_EFFMIDICHN };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffMidiChn)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEffMidiChn)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSet();
	afx_msg void OnClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFMIDICHN_H__BDD36841_27B1_11D7_80A8_4000001054B2__INCLUDED_)
