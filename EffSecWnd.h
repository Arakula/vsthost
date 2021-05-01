/*****************************************************************************/
/* EffSecWnd.h: interface for the CEffSecWnd class.                          */
/*****************************************************************************/

#if !defined(AFX_EFFSECWND_H__269685E5_A3E2_11D8_8232_4000001054B2__INCLUDED_)
#define AFX_EFFSECWND_H__269685E5_A3E2_11D8_8232_4000001054B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EffectWnd.h"

/*****************************************************************************/
/* CEffSecWnd : secondary effect window                                      */
/*****************************************************************************/

class CEffSecWnd : public CEffectWnd  
{
	DECLARE_DYNCREATE(CEffSecWnd)
// Construction
public:
	CEffSecWnd();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffSecWnd)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEffSecWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEffSecWnd)
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
    virtual LPCSTR GetEditType() { return "S"; }

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFSECWND_H__269685E5_A3E2_11D8_8232_4000001054B2__INCLUDED_)
