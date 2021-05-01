/*****************************************************************************/
/* EffectWnd.h : header file                                                 */
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

#if !defined(AFX_EFFECTWND_H__E21437F5_9BA1_11D5_8161_4000001054B2__INCLUDED_)
#define AFX_EFFECTWND_H__E21437F5_9BA1_11D5_8161_4000001054B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*****************************************************************************/
/* CEffectWnd window                                                         */
/*****************************************************************************/

class CChildFrame;
class CEffectWnd : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CEffectWnd)
// Construction
public:
	CEffectWnd();

// Attributes
public:

// Operations
public:
    virtual void SetEffect(int nEff) { nEffect = nEff; }
    int  GetEffect() { return nEffect; }
    void SetMain(CChildFrame *pFrm) { pMain = pFrm; }
    void SetupTitle();
    void SetupTitleText(LPCSTR lpszText = NULL) { sTitle = (lpszText) ? lpszText : ""; }
    virtual HICON RetrieveIcon() { return AfxGetApp()->LoadIcon(IDI_EFFEDIT); }
    virtual LPCSTR GetEditType() { return "E"; }
    virtual BOOL NeedView() { return FALSE; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffectWnd)
	public:
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	void CloseEditWnd();
	virtual void Update();
    virtual bool OnSetParameterAutomated(long index, float value) { return false; }
	void SetEffSize(int cx, int cy);
	void SetEffSize(ERect *pRect);
	virtual ~CEffectWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEffectWnd)
	afx_msg void OnClose();
	afx_msg void OnEffProgramName();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnEffEditParms();
	afx_msg void OnEffEdit();
	afx_msg void OnUpdateEffEdit(CCmdUI* pCmdUI);
	afx_msg void OnEffInfo();
	afx_msg void OnEffResize();
	afx_msg void OnEffChecksize();
	afx_msg void OnEffLoad();
	afx_msg void OnEffSave();
	afx_msg void OnEffSaveas();
	afx_msg void OnEffSelprogram();
	afx_msg void OnUpdateEffSelprogram(CCmdUI* pCmdUI);
	afx_msg void OnSelMidichn();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChainAfter();
	afx_msg void OnUnchain();
	afx_msg void OnUpdateChainAfter(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUnchain(CCmdUI* pCmdUI);
	afx_msg void OnProgramNext();
	afx_msg void OnProgramPrev();
	afx_msg void OnUpdateProgramNext(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProgramPrev(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	//}}AFX_MSG
    afx_msg void OnSetProgram(UINT nID);
	DECLARE_MESSAGE_MAP()

protected:
	void MakeVstKeyCode(UINT nChar, UINT nRepCnt, UINT nFlags, VstKeyCode &keyCode);
	BOOL GetEffectEditWndSize(CRect &rcFrame, CRect &rcClient, ERect *pRect = NULL);
    virtual CWnd * CreateView() { return NULL; }

protected:
	CRect rcEffFrame;
	CRect rcEffClient;
	CWnd * pView;
	CChildFrame * pMain;
	CVSTHost * pHost;
	int nEffect;
//    VstWindow vstw;
    CString sTitle;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECTWND_H__E21437F5_9BA1_11D5_8161_4000001054B2__INCLUDED_)
