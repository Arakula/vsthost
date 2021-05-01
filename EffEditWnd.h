/*****************************************************************************/
/* EffEditWnd.h : header file                                                */
/*****************************************************************************/

#if !defined(AFX_EFFEDITWND_H__45274173_9C71_11D5_8162_4000001054B2__INCLUDED_)
#define AFX_EFFEDITWND_H__45274173_9C71_11D5_8162_4000001054B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EffectWnd.h"

/*****************************************************************************/
/* CEffEditDlg dialog                                                        */
/*****************************************************************************/

class CChildFrame;
class CEffEditWnd;
class CEffEditDlg : public CDialog
{
friend class CEffEditWnd;
// Construction
public:
	bool OnSetParameterAutomated(long index, float value);
	CEffEditDlg(CWnd* pParent = NULL);   // standard constructor
	virtual BOOL Create(CWnd *pParentWnd=NULL);

// Dialog Data
	//{{AFX_DATA(CEffEditDlg)
	enum { IDD = IDD_PARMS };
	CSliderCtrl	slParmval;
	CStatic	stParmtext;
	CListBox	lbParms;
	//}}AFX_DATA

    void SetMain(CChildFrame *pFrm) { pMain = pFrm; }
	void SetEffect(int nEffect);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffEditDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void SetParmDisp(CEffect *pEffect, int nParm);
	CChildFrame * pMain;
	int nEffect;

	// Generated message map functions
	//{{AFX_MSG(CEffEditDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeParmlist();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/*****************************************************************************/
/* CEffEditWnd window                                                        */
/*****************************************************************************/

class CChildFrame;
class CEffEditWnd : public CEffectWnd
{
	DECLARE_DYNCREATE(CEffEditWnd)
// Construction
public:
	CEffEditWnd();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffEditWnd)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void Update();
	virtual bool OnSetParameterAutomated(long index, float value);
	virtual void SetEffect(int nEff);
	virtual ~CEffEditWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEffEditWnd)
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	virtual CWnd * CreateView();
    virtual HICON RetrieveIcon() { return AfxGetApp()->LoadIcon(IDI_EFFPARM); }
    virtual LPCSTR GetEditType() { return "P"; }
    virtual BOOL NeedView() { return TRUE; }
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFEDITWND_H__45274173_9C71_11D5_8162_4000001054B2__INCLUDED_)
