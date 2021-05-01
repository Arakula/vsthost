/*****************************************************************************/
/* ChildFrm.h : interface of the CChildFrame class                           */
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


#if !defined(AFX_CHILDFRM_H__9052BAFB_9D44_11D5_8163_4000001054B2__INCLUDED_)
#define AFX_CHILDFRM_H__9052BAFB_9D44_11D5_8163_4000001054B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChildView.h"

/*****************************************************************************/
/* CChildFrame class declaration                                             */
/*****************************************************************************/

class CEffectWnd;
class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
    friend class CEffectWnd;            /* these are allowed to call         */
    friend class CEffEditWnd;           /* OnEffEdit.../OnUpdateEffEdit...   */
public:
	CChildFrame();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	void ShowDetails();
	void ResizeToContent();
	void OnSetProgram();
	void EditClosed();
	void ParmClosed();
    int  GetEffect() { return nEffect; }
	void SetEffect(int nEffect);
    void SetEditWnd(CEffectWnd *pWnd) { pEditWnd = pWnd; }
    HMENU GetEditMenu() { return m_hEditMenu; }
	void SetNewTitle(LPCSTR lpszText);
	// view for the client area of the frame.
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	int nEffect;
	CChildView m_wndView;

// Generated message map functions
protected:
	void SetupTitle();
	CString sTitle;
	void SaveBank(CString sName);
	int nCanEdit;
	HMENU m_hEditMenu;
	CEffectWnd * pEditWnd;
	CEffectWnd * pParmWnd;
	//{{AFX_MSG(CChildFrame)
	afx_msg void OnFileClose();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEffEdit();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateEffEdit(CCmdUI* pCmdUI);
	afx_msg void OnEffEditParms();
	afx_msg void OnEffLoad();
	afx_msg void OnEffSave();
	afx_msg void OnEffSaveas();
	afx_msg void OnEffSelprogram();
	afx_msg void OnUpdateEffSelprogram(CCmdUI* pCmdUI);
	afx_msg void OnSelMidichn();
	afx_msg void OnChainAfter();
	afx_msg void OnUnchain();
	afx_msg void OnUpdateChainAfter(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUnchain(CCmdUI* pCmdUI);
	afx_msg void OnProgramNext();
	afx_msg void OnProgramPrev();
	afx_msg void OnUpdateProgramNext(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProgramPrev(CCmdUI* pCmdUI);
    afx_msg void OnDisplayUpdate();
	//}}AFX_MSG
    afx_msg void OnSetProgram(UINT nID);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRM_H__9052BAFB_9D44_11D5_8163_4000001054B2__INCLUDED_)
