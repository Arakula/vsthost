/*****************************************************************************/
/* SPECMIDI.CPP : class implementation for specialized MIDI classes          */
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

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/

#include "stdafx.h"                     /* MFC include                       */

#include "resource.h"                   /* Project resources                 */
#include "mfcmidi.h"                    /* MFC Midi definitions              */
#include "specmidi.h"                   /* private definitions               */

#include "vsthost.h"                    /* application specific header       */

#ifdef _DEBUG
#ifdef WIN32
#define new DEBUG_NEW
#endif
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/

#define GetApp()  ((CVsthostApp *)AfxGetApp())

/*****************************************************************************/
/* Global Data                                                               */
/*****************************************************************************/

CSpecMidiInDevice MidiIn;               /* MIDI Input Device                 */
CSpecMidiOutDevice MidiOut;             /* MIDI Output Device                */

/*===========================================================================*/
/* Class CSpecMidiInDevice                                                   */
/*===========================================================================*/

/*****************************************************************************/
/* CSpecMidiInDevice message map                                             */
/*****************************************************************************/

#if 0
BEGIN_MESSAGE_MAP(CSpecMidiInDevice, CMidiInDevice)
    //{{AFX_MSG_MAP(CSpecMidiInDevice)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif

/*****************************************************************************/
/* CSpecMidiInDevice : constructor                                           */
/*****************************************************************************/

CSpecMidiInDevice::CSpecMidiInDevice ( )
{
MidiThru = FALSE;
}

/*****************************************************************************/
/* ~CSpecMidiInDevice : destructor                                           */
/*****************************************************************************/

CSpecMidiInDevice::~CSpecMidiInDevice ( )
{
}

/*****************************************************************************/
/* Data : processes incoming MIDI data                                       */
/*****************************************************************************/

void CSpecMidiInDevice::Data
    (
    CMidiMsg & Msg
    )
{
CMidiInDevice::Data(Msg);               /* pass on to higher level           */

if (MidiThru)                           /* if Thru active                    */
  MidiOut.Output(Msg);                  /* send message to output            */

#if 0
// since V1.08, we can do SysEx, too!
if (Msg.Length() <= 3)                  /* if normal MIDI message            */
#endif

GetApp()->vstHost.OnMidiIn(Msg);        /* send to VST effects               */
}

/*===========================================================================*/
/* Class CSpecMidiOutDevice                                                  */
/*===========================================================================*/

/*****************************************************************************/
/* CSpecMidiOutDevice message map                                            */
/*****************************************************************************/

#if 0
BEGIN_MESSAGE_MAP(CSpecMidiOutDevice, CMidiOutDevice)
    //{{AFX_MSG_MAP(CSpecMidiOutDevice)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif

/*****************************************************************************/
/* CSpecMidiOutDevice : constructor                                          */
/*****************************************************************************/

CSpecMidiOutDevice::CSpecMidiOutDevice()
{
}

/*****************************************************************************/
/* ~CSpecMidiOutDevice : destructor                                          */
/*****************************************************************************/

CSpecMidiOutDevice::~CSpecMidiOutDevice()
{
}

/*****************************************************************************/
/* Output : sends MIDI data to output                                        */
/*****************************************************************************/

BOOL CSpecMidiOutDevice::Output
    (
    CMidiMsg &Msg,
    BOOL bSync
    )
{
                                        /* pass on to base class             */
BOOL bRC = CMidiOutDevice::Output(Msg, bSync);

// here some additional processing could be done

return bRC;                             /* if not sending to output, OK      */
}
