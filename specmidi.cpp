/*****************************************************************************/
/* SPECMIDI.CPP : class implementation for specialized MIDI classes          */
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
