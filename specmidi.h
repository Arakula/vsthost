/*****************************************************************************/
/* SPECMIDI.H : class definition for specialized MIDI classes                */
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

#ifndef __INCLUDE_SPECMIDI_HPP__
#define __INCLUDE_SPECMIDI_HPP__

#ifndef __cplusplus
#error SPECMIDI.H is for use with C++
#endif

/*****************************************************************************/
/* Necessary includes                                                        */
/*****************************************************************************/

#include "mfcmidi.h"                    /* MFC Midi classes                  */

/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/

/*****************************************************************************/
/* CSpecMidiInDevice : special MIDI Input device for Waveterm C              */
/*****************************************************************************/

class CSpecMidiInDevice : public CMidiInDevice
  {
protected:
    BOOL MidiThru;                      /* pass MIDI thru                    */

// Operations
public:
    CSpecMidiInDevice ( );              /* constructor                       */
    ~CSpecMidiInDevice ( );             /* denstructor                       */

    virtual void Data(CMidiMsg &Msg);   /* process incoming data             */

    void SetMidiThru ( BOOL thru )      /* flag whether MIDI Thru            */
      { MidiThru = thru; }
                                        /* allow common start timestamp      */
    void SetStartStamp(DWORD dwTS) { dwStamp = dwTS; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpecMidiInDevice)
	//}}AFX_VIRTUAL

    // Generated message map functions
protected:
#if 0
    //{{AFX_MSG(CSpecMidiInDevice)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
#endif
  };

/*****************************************************************************/
/* CSpecMidiOutDevice : special MIDI Output device for Waveterm C            */
/*****************************************************************************/

class CSpecMidiOutDevice : public CMidiOutDevice
  {
// Operations
  public:
    CSpecMidiOutDevice();
    ~CSpecMidiOutDevice();

    virtual BOOL Output(CMidiMsg &Msg, BOOL bSync = TRUE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpecMidiOutDevice)
	//}}AFX_VIRTUAL

    // Generated message map functions
protected:
#if 0
    //{{AFX_MSG(CSpecMidiOutDevice)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
#endif
  };

/*****************************************************************************/
/* Global Data                                                               */
/*****************************************************************************/

extern CSpecMidiInDevice MidiIn;        /* MIDI Input Device                 */
extern CSpecMidiOutDevice MidiOut;      /* MIDI Output Device                */

#endif                                  /* __INCLUDE_SPECMIDI_HPP__          */
