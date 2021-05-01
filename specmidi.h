/*****************************************************************************/
/* SPECMIDI.H : class definition for specialized MIDI classes                */
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
