/*****************************************************************************/
/* ChildView.h : interface of the CChildView class                           */
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

#if !defined(AFX_CHILDVIEW_H__9052BAFD_9D44_11D5_8163_4000001054B2__INCLUDED_)
#define AFX_CHILDVIEW_H__9052BAFD_9D44_11D5_8163_4000001054B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CChildView window

class CChildView : public CListBox
{
// Construction
public:
	CChildView();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	int GetVerticalExtent();
	int AddString(LPCSTR lpszItem);
	void ResetContent();
	virtual ~CChildView();

	// Generated message map functions
protected:
	CFont fLb;
    int nMaxwidth;
    int nItemHeight;
	//{{AFX_MSG(CChildView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDVIEW_H__9052BAFD_9D44_11D5_8163_4000001054B2__INCLUDED_)
