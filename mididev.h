/*****************************************************************************/
/* MIDIDEV.HPP : Header file for MFC MIDI Keyboard Configuration             */
/*****************************************************************************/
/*                                                                           */
/* $Archive::                                                              $ */
/* $Revision::                                                             $ */
/* $Date::                                                                 $ */
/* $Author::                                                               $ */
/* $Modtime::                                                              $ */
/* $Workfile::                                                             $ */
/*                                                                           */
/*****************************************************************************/

#ifndef _INCLUDE_MIDIDEV_HPP_
#define _INCLUDE_MIDIDEV_HPP_

#ifndef __cplusplus
#error MIDIDEV.H is for use with C++
#endif

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/

/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/

/*****************************************************************************/
/* Class Definitions                                                         */
/*****************************************************************************/

                                        /* MIDI Device dialog class          */
class CMidiDeviceDialog : public CDialog
  {
  public:
	CMidiDeviceDialog(CWnd* pParent = NULL);   

// Dialog Data
	//{{AFX_DATA(CMidiDeviceDialog)
	enum { IDD = IDD_MIDIDEV };
	BOOL	bPassThru;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMidiDeviceDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog( void );
    virtual void OnOK ( void );
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
    //{{AFX_MSG(CMidiDeviceDialog)
		// NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

  public :
    CString InName;                     /* input device name                 */
    CString OutName;                    /* output device name                */

  };

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif                                  /* defined _INCLUDE_MIDIDEV_HPP_     */
