/*****************************************************************************/
/* WaveDev.h : header file                                                   */
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

#if !defined(AFX_WAVEDEV_H__EB38C086_B0C3_11D5_8171_4000001054B2__INCLUDED_)
#define AFX_WAVEDEV_H__EB38C086_B0C3_11D5_8171_4000001054B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*****************************************************************************/
/* CWaveDeviceDialog dialog                                                  */
/*****************************************************************************/

class CWaveDeviceDialog : public CDialog
{
// Construction
public:
	CWaveDeviceDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWaveDeviceDialog)
	enum { IDD = IDD_WAVEDEV };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaveDeviceDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWaveDeviceDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeOutputport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public :
    CString InName;                     /* input device name                 */
    CString OutName;                    /* output device name                */
    int nBufSize;                       /* size of one buffer                */

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAVEDEV_H__EB38C086_B0C3_11D5_8171_4000001054B2__INCLUDED_)
