/*****************************************************************************/
/* WaveDev.h : header file                                                   */
/*****************************************************************************/

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
