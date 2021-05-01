/*****************************************************************************/
/* SmpVSTHost.cpp: CSmpVSTHost / CSmpEffect implementation                   */
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

#include "stdafx.h"

#include "ChildFrm.h"
#include "EffectWnd.h"
#include "EffSecWnd.h"
#include "specmidi.h"
#include "specwave.h"
#include "vstsysex.h"
#include "SmpVSTHost.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#pragma intrinsic(memcpy,memset)

/*===========================================================================*/
/* CSmpVSTHost class members                                                 */
/*===========================================================================*/

static const int nMaxMidi = 2000;       /* max. # MIDI messages buffered for */
                                        /* EffProcess()                      */
static const int nMaxSysEx = 100;       /* max. # MIDI SysEx messages        */
                                        /* buffered for EffProcess()         */

/*****************************************************************************/
/* CSmpVSTHost : constructor                                                 */
/*****************************************************************************/

CSmpVSTHost::CSmpVSTHost()
{
// for this sample project, I've assumed a maximum buffer size
// of 1/3 second at 96KHz - presumably, this is MUCH too much
// and should be tweaked to more reasonable values, since it
// requires a machine with lots of main memory this way...
// user configurability would be nice, of course.
for (int i = 0; i < 2; i++)
  pOutputs[i] = new float[32000];
pMidis = new DWORD[nMaxMidi * 2];
nMidis = 0;
pSysEx = new CMidiMsg[nMaxSysEx];
nSysEx = 0;
InitializeCriticalSection(&cs);
}

/*****************************************************************************/
/* ~CSmpVSTHost : destructor                                                 */
/*****************************************************************************/

CSmpVSTHost::~CSmpVSTHost()
{
for (int i = 0; i < 2; i++)
  delete[] pOutputs[i];
if (pMidis)
  delete[] pMidis;
if (pSysEx)
  delete[] pSysEx;
}

/*****************************************************************************/
/* OnAudioMasterCallback : redefine the callback a bit                       */
/*****************************************************************************/

long CSmpVSTHost::OnAudioMasterCallback
    (
    int nEffect,
    long opcode,
    long index,
    long value,
    void *ptr,
    float opt
    )
{
#if defined(_DEBUG) || defined(_DEBUGFILE)

char szDebug[1024];
int nLen;
switch (opcode)
  {
  case audioMasterAutomate :
    nLen = sprintf(szDebug, "callback: %d audioMasterAutomate %d, %f", nEffect, index, opt);
    break;
  case audioMasterVersion :
    nLen = sprintf(szDebug, "callback: %d audioMasterVersion", nEffect);
    break;
  case audioMasterCurrentId :
    nLen = sprintf(szDebug, "callback: %d audioMasterCurrentId", nEffect);
    break;
  case audioMasterIdle :
    nLen = sprintf(szDebug, "callback: %d audioMasterIdle", nEffect);
    break;
  case audioMasterPinConnected :
    nLen = sprintf(szDebug, "callback: %d audioMasterPinConnected %d", nEffect, index);
    break;
                                        /* VST 2.0 additions...              */
  case audioMasterWantMidi :
    nLen = sprintf(szDebug, "callback: %d audioMasterWantMidi %d", nEffect, value);
    break;
  case audioMasterGetTime :
    nLen = sprintf(szDebug, "callback: %d audioMasterGetTime %d", nEffect, value);
    break;
  case audioMasterProcessEvents :
    nLen = sprintf(szDebug, "callback: %d audioMasterProcessEvents %d", nEffect, ((VstEvents *)ptr)->numEvents);
    break;
  case audioMasterSetTime :
    nLen = sprintf(szDebug, "callback: %d audioMasterSetTime %d", nEffect, value);
    break;
  case audioMasterTempoAt :
    nLen = sprintf(szDebug, "callback: %d audioMasterTempoAt %d", nEffect, value);
    break;
  case audioMasterGetNumAutomatableParameters :
    nLen = sprintf(szDebug, "callback: %d audioMasterGetNumAutomatableParameters", nEffect);
    break;
  case audioMasterGetParameterQuantization :
    nLen = sprintf(szDebug, "callback: %d audioMasterGetParameterQuantization", nEffect);
    break;
  case audioMasterIOChanged :
    nLen = sprintf(szDebug, "callback: %d audioMasterIOChanged", nEffect);
    break;
  case audioMasterNeedIdle :
    nLen = sprintf(szDebug, "callback: %d audioMasterNeedIdle", nEffect);
    break;
  case audioMasterSizeWindow :
    nLen = sprintf(szDebug, "callback: %d audioMasterSizeWindow %d,%d", nEffect, index, value);
    break;
  case audioMasterGetSampleRate :
    nLen = sprintf(szDebug, "callback: %d audioMasterGetSampleRate", nEffect);
    break;
  case audioMasterGetBlockSize :
    nLen = sprintf(szDebug, "callback: %d audioMasterGetBlockSize", nEffect);
    break;
  case audioMasterGetInputLatency :
    nLen = sprintf(szDebug, "callback: %d audioMasterGetInputLatency", nEffect);
    break;
  case audioMasterGetOutputLatency :
    nLen = sprintf(szDebug, "callback: %d audioMasterGetOutputLatency", nEffect);
    break;
  case audioMasterGetPreviousPlug :
    nLen = sprintf(szDebug, "callback: %d audioMasterGetPreviousPlug", nEffect);
    break;
  case audioMasterGetNextPlug :
    nLen = sprintf(szDebug, "callback: %d audioMasterGetNextPlug", nEffect);
    break;
  case audioMasterWillReplaceOrAccumulate :
    nLen = sprintf(szDebug, "callback: %d audioMasterWillReplaceOrAccumulate", nEffect);
    break;
  case audioMasterGetCurrentProcessLevel :
    nLen = sprintf(szDebug, "callback: %d audioMasterGetCurrentProcessLevel", nEffect);
    break;
  case audioMasterGetAutomationState :
    nLen = sprintf(szDebug, "callback: %d audioMasterGetAutomationState", nEffect);
    break;
  case audioMasterOfflineStart :
    nLen = sprintf(szDebug, "callback: %d audioMasterOfflineStart %d,%d", nEffect, value, index);
    break;
  case audioMasterOfflineRead :
    nLen = sprintf(szDebug, "callback: %d audioMasterOfflineRead %d,%d", nEffect, value, index);
    break;
  case audioMasterOfflineWrite :
    nLen = sprintf(szDebug, "callback: %d audioMasterOfflineWrite %d", nEffect, value);
    break;
  case audioMasterOfflineGetCurrentPass :
    nLen = sprintf(szDebug, "callback: %d audioMasterOfflineGetCurrentPass", nEffect);
    break;
  case audioMasterOfflineGetCurrentMetaPass :
    nLen = sprintf(szDebug, "callback: %d audioMasterOfflineGetCurrentMetaPass", nEffect);
    break;
  case audioMasterSetOutputSampleRate :
    nLen = sprintf(szDebug, "callback: %d audioMasterSetOutputSampleRate %f", nEffect, opt);
    break;
#ifdef VST_2_3_EXTENSIONS
  case audioMasterGetOutputSpeakerArrangement :
    nLen = sprintf(szDebug, "callback: %d audioMasterGetOutputSpeakerArrangement", nEffect);
    break;
#else
  case audioMasterGetSpeakerArrangement :
    nLen = sprintf(szDebug, "callback: %d audioMasterGetSpeakerArrangement", nEffect);
    break;
#endif
  case audioMasterGetVendorString :
    nLen = sprintf(szDebug, "callback: %d audioMasterGetVendorString", nEffect);
    break;
  case audioMasterGetProductString :
    nLen = sprintf(szDebug, "callback: %d audioMasterGetProductString", nEffect);
    break;
  case audioMasterGetVendorVersion :
    nLen = sprintf(szDebug, "callback: %d audioMasterGetVendorVersion", nEffect);
    break;
  case audioMasterVendorSpecific :
    nLen = sprintf(szDebug, "callback: %d audioMasterVendorSpecific %d,%d,%d,%08lX,%f", nEffect, opcode, index, value, ptr, opt);
    break;
  case audioMasterSetIcon :
    nLen = sprintf(szDebug, "callback: %d audioMasterSetIcon", nEffect);
    break;
  case audioMasterCanDo :
    nLen = sprintf(szDebug, "callback: %d audioMasterCanDo \"%s\"", nEffect, ptr);
    break;
  case audioMasterGetLanguage :
    nLen = sprintf(szDebug, "callback: %d audioMasterGetLanguage", nEffect);
    break;
  case audioMasterOpenWindow :
    nLen = sprintf(szDebug, "callback: %d audioMasterOpenWindow", nEffect);
    break;
  case audioMasterCloseWindow :
    nLen = sprintf(szDebug, "callback: %d audioMasterCloseWindow", nEffect);
    break;
  case audioMasterGetDirectory :
    nLen = sprintf(szDebug, "callback: %d audioMasterGetDirectory", nEffect);
    break;
  case audioMasterUpdateDisplay :
    nLen = sprintf(szDebug, "callback: %d audioMasterUpdateDisplay", nEffect);
    break;
    //---from here VST 2.1 extension opcodes------------------------------------------------------
#ifdef VST_2_1_EXTENSIONS
  case audioMasterBeginEdit :
    nLen = sprintf(szDebug, "callback: %d audioMasterBeginEdit Parameter %d", nEffect, index);
    break;
  case audioMasterEndEdit :
    nLen = sprintf(szDebug, "callback: %d audioMasterEndEdit Parameter %d", nEffect, index);
    break;
  case audioMasterOpenFileSelector :
    nLen = sprintf(szDebug, "callback: %d audioMasterOpenFileSelector", nEffect);
    break;
#endif
    //---from here VST 2.2 extension opcodes------------------------------------------------------
#ifdef VST_2_2_EXTENSIONS
  case audioMasterCloseFileSelector :
    nLen = sprintf(szDebug, "callback: %d audioMasterCloseFileSelector", nEffect);
    break;
  case audioMasterEditFile :
    nLen = sprintf(szDebug, "callback: %d audioMasterEditFile \"%s\"", nEffect, ptr);
    break;
  case audioMasterGetChunkFile :
    nLen = sprintf(szDebug, "callback: %d audioMasterGetChunkFile", nEffect);
    break;
#endif
    //---from here VST 2.3 extension opcodes------------------------------------------------------
#ifdef VST_2_1_EXTENSIONS
  case audioMasterGetInputSpeakerArrangement :
    nLen = sprintf(szDebug, "callback: %d audioMasterGetInputSpeakerArrangement", nEffect);
    break;
#endif
  }
#endif

long lRC = CVSTHost::OnAudioMasterCallback(nEffect, opcode, index, value, ptr, opt);

#if defined(_DEBUG) || defined(_DEBUGFILE)
switch (opcode)
  {
  case audioMasterGetChunkFile :
    nLen += sprintf(szDebug + nLen, " \"%s\"", ptr);
    break;
  }
sprintf(szDebug + nLen, " RC=%d", lRC);
TRACE1("%s\n", szDebug);
#endif

return lRC;
}

/*****************************************************************************/
/* OnSetParameterAutomated : make sure parameter changes are reflected       */
/*****************************************************************************/

bool CSmpVSTHost::OnSetParameterAutomated(int nEffect, long index, float value)
{
CSmpEffect *pEffect = (CSmpEffect *)GetAt(nEffect);
if (pEffect)
  pEffect->OnSetParameterAutomated(index, value);
return CVSTHost::OnSetParameterAutomated(nEffect, index, value);
}

/*****************************************************************************/
/* OnMidiIn : called when a MIDI message comes in                            */
/*****************************************************************************/

void CSmpVSTHost::OnMidiIn(CMidiMsg &msg)
{
if (msg[0] > 0xf0)                      /* VSTInstruments don't expect       */
  return;                               /* realtime messages!                */
else if (msg[0] == 0xf0)                /* but SysEx are processed           */
  {
  EnterCriticalSection(&cs);            /* must not interfere!               */
  if ((pSysEx) &&                       /* if allocated, and                 */
      (nSysEx < nMaxSysEx))             /* if still space for another one    */
    pSysEx[nSysEx++] = msg;             /* copy it in                        */
  LeaveCriticalSection(&cs);
  }
else
  {
  EnterCriticalSection(&cs);            /* must not interfere!               */
  if (nMidis < 2 * nMaxMidi)            /* if enough place to do it,         */
    {
    pMidis[nMidis++] = msg;             /* buffer the message                */
    pMidis[nMidis++] = msg.GetStamp();  /* and its timestamp                 */
//  TRACE2("MIDI Message %08lX at %d\n", (DWORD)msg, msg.GetStamp());
    }
  LeaveCriticalSection(&cs);
  }
}

/*****************************************************************************/
/* OnProcessEvents : called when an effect sends events                      */
/*****************************************************************************/

bool CSmpVSTHost::OnProcessEvents(int nEffect, VstEvents *events)
{
int i;
CMidiMsg msg;
bool bErr = false;

for (i = 0; i < events->numEvents; i++) /* process all sent MIDI events      */
  {
  switch (events->events[i]->type)
    {
    case kVstMidiType :                 /* normal MIDI message?              */
      msg.Set(((VstMidiEvent *)(events->events[i]))->midiData, 3);
      bErr |= !MidiOut.Output(msg);     /* send it to MIDI Out               */
      break;
    case kVstSysExType :                /* SysEx message ?                   */
      msg.Set(((VstMidiSysexEvent *)(events->events[i]))->sysexDump,
              ((VstMidiSysexEvent *)(events->events[i]))->dumpBytes);
      bErr |= !MidiOut.Output(msg);     /* send it to MIDI Out               */
      break;
#if 0 // silently ignore all non-MIDI events
    default :                           /* anything else?                    */
      bErr = true;
      break;
#endif
    }
  }

return !bErr;
}

/*****************************************************************************/
/* CreateMIDIEvents : creates a set of VST MIDI events                       */
/*****************************************************************************/

BYTE * CSmpVSTHost::CreateMIDIEvents
    (
    int nLength,                        /* # samples                         */
    DWORD dwStamp,                      /* timestamp of 1st sample           */
    int nm,                             /* # normal MIDI messages            */
    int ns                              /* # sysex messages                  */
    )
{
if ((!nm) && (!ns))                     /* if neither normal nor sysex there */
  return NULL;                          /* nothing to do in here             */

BYTE *pEvData;
int nHdrLen, nTotLen;
int i;
DWORD dwDelta;
VstMidiEvent *pEv;
VstMidiSysexEvent *pSEv;
int nSysBase;

nHdrLen = sizeof(VstEvents) +           /* calculate header length           */
          ((nm - 1) * sizeof(VstMidiEvent *)) +
          (ns * sizeof(VstMidiSysexEvent *));
nTotLen = nHdrLen +                     /* calculate total events length     */
          (nm * sizeof(VstMidiEvent)) +
          (ns * sizeof(VstMidiSysexEvent));
pEvData = new BYTE[nTotLen];            /* allocate space for events         */
if (pEvData)                            /* if that worked,                   */
  {                                     /* copy in normal and SysEx          */
  VstEvents *pEvents = (VstEvents *) pEvData;
  memset(pEvents, 0, nTotLen);
  pEvents->numEvents = (nm / 2) + ns;
  for (i = 0; i < nm; i += 2)           /* copy in all normal MIDI messages  */
    {
    pEv = ((VstMidiEvent *)(pEvData + nHdrLen)) + (i / 2);
    pEvents->events[i / 2] = (VstEvent *)pEv;
    pEv->type = kVstMidiType;
    pEv->byteSize = sizeof(VstMidiEvent);
    memcpy(pEv->midiData, pMidis + i, sizeof(DWORD));
    // offset assumes a fixed rate of 44.1kHz and drops 1 percent accuracy
    dwDelta = (pMidis[i + 1] - dwStamp) * 44;
    if (dwDelta > (DWORD)nLength)
      dwDelta = nLength - 1;
    pEv->deltaFrames = dwDelta;
                                        /* ALL VSTHost events are realtime   */
    pEv->flags = kVstMidiEventIsRealtime;
    }
  nm >>= 1;
  nSysBase = nHdrLen + (sizeof(VstMidiEvent) * nm);
  for (i = 0; i < ns; i++)              /* copy in all SysEx messages        */
    {
    pSEv = ((VstMidiSysexEvent *)(pEvData + nSysBase)) + i;
    pEvents->events[nm + i] = (VstEvent *)pSEv;
    pSEv->type = kVstSysExType;
    pSEv->byteSize = sizeof(VstMidiSysexEvent);
    pSEv->deltaFrames = pSysEx[i].GetStamp() - dwStamp;
    pSEv->dumpBytes = pSysEx[i].Length();
    pSEv->sysexDump = pSysEx[i];
    pSEv->flags = pSEv->resvd1 = pSEv->resvd2 = 0;
    }
  }

return pEvData;
}

/*****************************************************************************/
/* CreateMIDISubset : creates an event subset for a specific effect          */
/*****************************************************************************/

BYTE * CSmpVSTHost::CreateMIDISubset(void *pEvData, unsigned short wChnMask)
{
if (!pEvData)
  return NULL;

VstEvents *pEvents = (VstEvents *) pEvData;
VstMidiEvent *pEv;
int i, nMatching = 0;
for (i = pEvents->numEvents - 1; i >= 0; i--)
  {
  pEv = (VstMidiEvent *)pEvents->events[i];
  if ((pEv->type == kVstSysExType) ||   /* SysEx goes to all,                */
      (wChnMask &                       /* for normal MIDI channel has to fit*/
       (1 << (pEv->midiData[0] & 0x0F))))
    nMatching++;
  }
if (!nMatching)
  return NULL;

int nHdrLen = sizeof(VstEvents) + 
              ((nMatching - 1) * sizeof(VstMidiEvent *));
BYTE * pEffEvData = new BYTE[nHdrLen];
VstEvents *pEffEvents = (VstEvents *) pEffEvData;
pEffEvents->numEvents = nMatching;
nMatching = 0;
for (i = 0; i < pEvents->numEvents; i++)
  {
  pEv = (VstMidiEvent *)pEvents->events[i];
  if ((pEv->type == kVstSysExType) ||   /* SysEx goes to all,                */
      (wChnMask &                       /* for normal MIDI channel has to fit*/
       (1 << (pEv->midiData[0] & 0x0F))))
    pEffEvents->events[nMatching++] = pEvents->events[i];
  }

return pEffEvData;
}

/*****************************************************************************/
/* PassThruEffect : passes the sample data through one of the effects        */
/*****************************************************************************/

void CSmpVSTHost::PassThruEffect
    (
    CSmpEffect *pEff,                   /* effect to call                    */
    float **pBuffer,                    /* input buffers                     */
    int nLength,                        /* length of these                   */
    int nChannels,                      /* # channels                        */
    BYTE *pEvData,                      /* Midi events during this period    */
    bool bReplacing
    )
{
BYTE *pEffEvData = CreateMIDISubset(pEvData, pEff->GetChnMask());
float *pBuf1;
int j;                                  /* loop counters                     */

pEff->EnterCritical();                  /* make sure we don't get killed now */

if (pEff->bWantMidi)                    /* if effect wants MIDI events       */
  {
  static VstEvents noEvData = {0};      /* necessary for Reaktor ...         */
  try
    {
    pEff->EffProcessEvents((pEffEvData) ? (VstEvents *)pEffEvData : &noEvData);
    }
  catch(...)
    {
    TRACE0("Serious error in pEff->EffProcessEvents()!\n");
    }
  }

float *pEmpty = GetApp()->GetEmptyBuffer();
for (j = 0; j < nChannels; j++)
  pEff->SetInputBuffer(j, pBuffer[j]);
for (; j < pEff->pEffect->numInputs; j++)
  pEff->SetInputBuffer(j, pEmpty);

                                        /* then process the buffer itself    */
                                        /* clean all output buffers          */
// NB: this has to be done even in EffProcessReplacing() since some VSTIs
// (most notably those from Steinberg... hehe) obviously don't implement 
// processReplacing() as a separate function but rather use process()
for (j = 0; j < pEff->pEffect->numOutputs; j++)
  {
  pBuf1 = pEff->GetOutputBuffer(j);
  if (pBuf1)
    memcpy(pBuf1, pEmpty, nLength * sizeof(float));
  }
                                        /* if replacing implemented          */
if ((bReplacing) &&                     /* and wanted                        */
    (pEff->pEffect->flags & effFlagsCanReplacing))
  pEff->EffProcessReplacing(nLength);   /* do processReplacing               */
else                                    /* otherwise                         */
  pEff->EffProcess(nLength);            /* call process                      */

pEff->LeaveCritical();                  /* reallow killing                   */

if (pEffEvData)                         /* if there are event data for this  */
  delete pEffEvData;                    /* delete 'em                        */
}

/*****************************************************************************/
/* OnSamples : called when sample data come in                               */
/*****************************************************************************/

void CSmpVSTHost::OnSamples
    (
    float **pBuffer,
    int nLength,
    int nChannels,
    DWORD dwStamp
    )
{
int nEffs = 0;                          /* # contributing effects            */
int i, j, k;                            /* loop counters                     */
int nSize = GetSize();                  /* get # affected effects            */
int nm = nMidis;
int ns = nSysEx;
CSmpEffect *pEff, *pNextEff;
float *pBuf1, *pBuf2;
CVsthostApp *pApp = GetApp();
float *pEmpty = pApp->GetEmptyBuffer();

// NO optimizations in here, as well as NO checks for overruns etc...

if (nLength > lBlockSize)               /* prevent buffer overflow!          */
  nLength = lBlockSize;

vstTimeInfo.nanoSeconds = (double)timeGetTime() * 1000000.0L;
CalcTimeInfo();

BYTE *pEvData = CreateMIDIEvents(nLength, dwStamp, nm, ns);

for (i = 0; i < nSize; i++)             /* pass them through all interested  */
  {                                     /* effects                           */
  pEff = (CSmpEffect *)GetAt(i);
  if ((!pEff) ||                        /* if no effect at this position     */
      (pEff->GetPrev()))                /* or it's part of a chain           */
    continue;                           /* skip this one                     */

                                        /* pass through the effect           */
  PassThruEffect(pEff,
                 pBuffer, nLength, nChannels,
                 pEvData,
                 true);

  while ((pNextEff = pEff->GetNext()))  /* while more in chain               */
    {
    PassThruEffect(pNextEff,            /* pass these through next effect    */
                   pEff->GetOutputBuffers(), nLength, nChannels,
                   pEvData,
                   true);
    pEff = pNextEff;                    /* and advance to next in chain      */
    }

  nEffs++;                              /* increment #participating effects  */
  }

if (pEvData)                            /* delete eventual MIDI event data   */
  delete pEvData;
                                        /* make sure we don't lose new events*/
EnterCriticalSection(&cs);
if (nm < nMidis)
  for (i = nm; i < nMidis; i++)
    pMidis[i - nm] = pMidis[i];
nMidis -= nm;
if (ns < nSysEx)
  for (i = ns; i < nSysEx; i++)
    pSysEx[i - ns] = pSysEx[i];
nSysEx -= ns;
LeaveCriticalSection(&cs);

if (!nEffs)                             /* if no effects there               */
  {                                     /* just mute input                   */
  for (j = 0; j < nChannels; j++)
    {
    pBuf1 = pBuffer[j];
    memcpy(pBuf1, pEmpty, nLength * sizeof(float));
    }
  pApp->SendResult(pBuffer, nLength);   /* and blow out some silence         */
  }
else                                    /* otherwise                         */
  {
  int nOut = 0;
  // this is a hard-wired mixer - should be replaced with a "real" mixer
  float fMult = (float)sqrt(1.0f / nEffs);

  for (i = 0; i < GetSize(); i++)       /* merge all generated outputs       */
    {
    // VERY simple version... all loaded effects' outputs are merged
    // into a parallelized stream of stereo samples; no chaining, fixed mixing;
    // no sample rate conversion or the like;
    // the # effects determines a single effect's contribution to the output.
    
    pEff = (CSmpEffect *)GetAt(i);
    if ((!pEff) ||                      /* if no effect at this position     */
        (pEff->GetPrev()))              /* or it's part of a chain           */
      continue;

    while ((pNextEff = pEff->GetNext()))  /* walk to last in chain           */
      pEff = pNextEff;                  /* which has the final buffers       */

    pEff->EnterCritical();              /* make sure we don't get killed now */
    
    for (j = 0; j < 2; j++)             /* do for our 2 outputs              */
      {
      pBuf1 = pOutputs[j];
      pBuf2 = pEff->GetOutputBuffer(j);
      if ((!pBuf1) || (!pBuf2))
        break;
      if (!nOut)
        for (k = 0; k < nLength; k++)   /* loop through the samples          */
          *pBuf1++ = *pBuf2++ *fMult;
      else
        for (k = 0; k < nLength; k++)   /* loop through the samples          */
          *pBuf1++ += *pBuf2++ * fMult;
      }

    pEff->LeaveCritical();              /* reallow killing                   */

    nOut++;
    }

  // now that we've got a populated output buffer set, send it to 
  // the Wave Output device.
  pApp->SendResult(pOutputs, nLength);
  }

vstTimeInfo.samplePos += (float)nLength;
                                        /* in any case, reset "changed" flag */
vstTimeInfo.flags &= ~kVstTransportChanged;
}

/*****************************************************************************/
/* OnCanDo : plugin wants to know whether host can do...                     */
/*****************************************************************************/

bool CSmpVSTHost::OnCanDo(const char *ptr)
{
if ((!strcmp(ptr, "openFileSelector")) ||
    (!strcmp(ptr, "closeFileSelector")) ||
//  (!strcmp(ptr, "supportShell")) ||   /* old-style Waves plugin shell      */
    (!strcmp(ptr, "shellCategory")) ||
    (!strcmp(ptr, "supplyIdle")) )
  return true;

return CVSTHost::OnCanDo(ptr);
}

/*****************************************************************************/
/* OnOpenFileSelector : called when an effect needs a file selector          */
/*****************************************************************************/

static int CALLBACK BrowseCallbackProc
    (
    HWND hwnd, 
    UINT uMsg, 
    LPARAM lParam, 
    LPARAM lpData
    )
{
if (uMsg == BFFM_INITIALIZED)
  {
  if (lpData && *((LPCSTR)lpData))
    ::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
  }
return 0;
}

bool CSmpVSTHost::OnOpenFileSelector (int nEffect, VstFileSelect *ptr)
{
#if defined(VST_2_1_EXTENSIONS)
ptr->reserved = 0;
if (ptr->command == kVstMultipleFilesLoad)
  return false;  // not yet
else if ((ptr->command == kVstFileLoad) ||
         (ptr->command == kVstFileSave))
  {
  CString sFilter;

  for (int i = 0; i < ptr->nbFileTypes; i++)
    {
    sFilter += CString(ptr->fileTypes[i].name) +
               " (*." +
               ptr->fileTypes[i].dosType +
               ")|*." +
               ptr->fileTypes[i].dosType +
               "|";
    }
  if (!sFilter.IsEmpty())
    sFilter += "|";
  CFileDialog dlg((ptr->command != kVstFileSave),
                  NULL,                 /* default extension                 */
                  ptr->initialPath,     /* filename                          */
                  OFN_HIDEREADONLY |    /* flags                             */
                    OFN_OVERWRITEPROMPT
#if 0
// not yet
                    | ((ptr->command == kVstMultipleFilesLoad) ? OFN_ALLOWMULTISELECT : 0)
#endif
                     ,
                  sFilter,              /* filter                            */
                  NULL);                /* parent window                     */
  dlg.m_ofn.lpstrTitle = ptr->title;

  if (dlg.DoModal() == IDOK)
    {
    if (!ptr->returnPath)
      {
      ptr->returnPath = new char[dlg.GetPathName().GetLength() + 1];
      ptr->reserved = 1;
      }
    strcpy(ptr->returnPath, dlg.GetPathName());
    ptr->nbReturnPath = 1;
    return true;
    }
  }
else if (ptr->command == kVstDirectorySelect)
  {
  char szDisplayName[_MAX_PATH];
  BROWSEINFO bi = {0};
  bi.hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
  bi.pszDisplayName = szDisplayName;
  bi.lpszTitle = ptr->title;
  bi.ulFlags = BIF_RETURNONLYFSDIRS;
  bi.lpfn = BrowseCallbackProc;
  bi.lParam = (LPARAM)ptr->initialPath;
  LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
  if (pidl)
    {
    if (SHGetPathFromIDList(pidl, szDisplayName))
      {
      if (!ptr->returnPath)
        {
        ptr->returnPath = new char[strlen(szDisplayName) + 1];
        ptr->reserved = 1;
        }
      strcpy(ptr->returnPath, szDisplayName);
      ptr->nbReturnPath = 1;
      return true;
      }
    }
  }

#endif
return false;
}

/*****************************************************************************/
/* OnCloseFileSelector : called when an effect closes a file selector        */
/*****************************************************************************/

bool CSmpVSTHost::OnCloseFileSelector (int nEffect, VstFileSelect *ptr)
{
if (ptr->reserved == 1)
  {
  delete[] ptr->returnPath;
  ptr->returnPath = 0;
  ptr->reserved = 0;
  }
return false;
}

/*****************************************************************************/
/* OnGetChunkFile : returns current .fxb / .fxp name                         */
/*****************************************************************************/

bool CSmpVSTHost::OnGetChunkFile(int nEffect, void * nativePath)
{
CSmpEffect *pEffect = (CSmpEffect *)GetAt(nEffect);
if (pEffect && pEffect->GetChunkFile().GetLength())
  {
  strcpy((char *)nativePath, pEffect->GetChunkFile());
  return true;
  }
return false;
}

/*****************************************************************************/
/* OnIdle : idle processing                                                  */
/*****************************************************************************/

long CSmpVSTHost::OnIdle
    (
    int nEffect                         /* effect ID that triggered this     */
    )
{
GetApp()->OnEffIdle(nEffect);
return 0;
}


/*===========================================================================*/
/* CSmpEffect class members                                                  */
/*===========================================================================*/

/*****************************************************************************/
/* CSmpEffect : constructor                                                  */
/*****************************************************************************/

CSmpEffect::CSmpEffect(CVSTHost *pHost)
  : CEffect(pHost)
{
InitializeCriticalSection(&cs);

pFrameWnd = 0;
pEditWnd = pParmWnd = 0;
inBufs = outBufs = 0;
nAllocatedInbufs = nAllocatedOutbufs = 0;
wChnMask = 0xffff;
pPrev = pNext = NULL;
nLoadNum = -1;
}

/*****************************************************************************/
/* ~CSmpEffect : destructor                                                  */
/*****************************************************************************/

CSmpEffect::~CSmpEffect()
{
int i;

RemoveFromChain();                      /* remove from chain (if any)        */

EnterCritical();                        /* inhibit others!                   */

if (inBufs)
  {
#if 0
  // these are set by VST Host; it would ruin performance
  // if we copied data into the effect every time
  for (i = 0; i < nAllocatedInbufs; i++)
    if (inBufs[i])
      delete[] inBufs[i];
#endif
  delete[] inBufs;
  }

if (outBufs)
  {
  // these are managed by the effect object, so we 
  // have to delete them here
  for (i = nAllocatedOutbufs - 1; i >= 0; i--)
    if ((outBufs[i]) &&
        ((!i) || (outBufs[i] != outBufs[0])))
      delete[] outBufs[i];
  delete[] outBufs;
  }

LeaveCritical();                        /* reallow others                    */

DeleteCriticalSection(&cs);
}

/*****************************************************************************/
/* Load : loads a plugin                                                     */
/*****************************************************************************/

bool CSmpEffect::Load(const char *name)
{
if (!CEffect::Load(name))               /* do basic stuff                    */
  return false;

int i, j;                               /* loop counter                      */
if (pEffect->numInputs)                 /* allocate input pointers           */
  {
  inBufs = new float *[pEffect->numInputs];
  if (!inBufs)
    return false;
  for (i = 0; i < pEffect->numInputs; i++)
    inBufs[i] = 0;
  nAllocatedInbufs = pEffect->numInputs;
  }

if (pEffect->numOutputs)                /* allocate output pointers          */
  {
  int nAlloc;
  if (pEffect->numOutputs < 2)
    nAlloc = 2;
  else
    nAlloc = pEffect->numOutputs;
  outBufs = new float *[nAlloc];
  if (!outBufs)
    return false;
                                        /* and the buffers pointed to        */
  for (i = 0; i < pEffect->numOutputs; i++)
    {
    // for this sample project, I've assumed a maximum buffer size
    // of 1/4 second at 96KHz - presumably, this is MUCH too much
    // and should be tweaked to more reasonable values, since it
    // requires a machine with lots of main memory this way...
    // user configurability would be nice, of course.
    outBufs[i] = new float[24000];
    if (!outBufs[i])
      return false;
    for (j = 0; j < 24000; j++)
      outBufs[i][j] = 0.f;
    }
  for (; i < nAlloc; i++)               /* if less than 2, fill up           */
    outBufs[i] = outBufs[0];            /* by replicating buffer 0 pointer   */
  nAllocatedOutbufs = nAlloc;
  }
return true;
}

/*****************************************************************************/
/* Unload : removes an effect from memory                                    */
/*****************************************************************************/

bool CSmpEffect::Unload()
{
bool bRC = false;

//__try
try
  {
  bRC = CEffect::Unload();
  }
//__except (EvalException(GetExceptionCode()))
catch(...)
  {
       // no code here; isn't executed
  }

return bRC;
}

/*****************************************************************************/
/* OnOpenWindow : called to open yet another window                          */
/*****************************************************************************/

void * CSmpEffect::OnOpenWindow(VstWindow* window)
{
CChildFrame *pFrame = GetFrameWnd();
if (pFrame)
  {
  CEffectWnd *pWnd;

  pWnd = (CEffectWnd *)GetApp()->CreateChild(pEditWnd ?
                                                 RUNTIME_CLASS(CEffSecWnd) :
                                                 RUNTIME_CLASS(CEffectWnd),
                                             IDR_EFFECTTYPE,
                                             pFrame->GetEditMenu());
  if (pWnd)
    {
    HWND hWnd = pWnd->GetSafeHwnd();
// ignored at the moment: style, position
    ERect rc = {0};
    rc.right = window->width;
    rc.bottom = window->height;
    pWnd->ShowWindow(SW_SHOWNORMAL);
    pWnd->SetEffect(pFrame->GetEffect());
    pWnd->SetMain(pFrame);
    pWnd->SetEffSize(&rc);
    pWnd->SetupTitleText(window->title);
    if (!pEditWnd)
      {
      pFrame->SetEditWnd(pWnd);
      SetEditWnd(pWnd);
//    EffEditOpen(hWnd);
      }
    pWnd->SetupTitle();
    window->winHandle = hWnd;
    return pWnd->GetSafeHwnd();
    }

  }
return 0;                               /* no effect - no window.            */
}

/*****************************************************************************/
/* OnCloseWindow : called to close a window opened in OnOpenWindow           */
/*****************************************************************************/

bool CSmpEffect::OnCloseWindow(VstWindow* window)
{
if ((!window) || (!::IsWindow((HWND)window->winHandle)))
  return false;

::SendMessage((HWND)window->winHandle, WM_CLOSE, 0, 0);
window->winHandle = 0;
return true;
}

/*****************************************************************************/
/* OnSizeEditorWindow : called to resize the editor window                   */
/*****************************************************************************/

void CSmpEffect::OnSizeEditorWindow(long width, long height)
{
if (pEditWnd)
  pEditWnd->SetEffSize((int)width, (int)height);
}

/*****************************************************************************/
/* OnUpdateDisplay() : called when the display is updated                    */
/*****************************************************************************/

bool CSmpEffect::OnUpdateDisplay()
{
if (pFrameWnd)
  pFrameWnd->PostMessage(WM_COMMAND, IDM_DISPLAY_UPDATE);
return CEffect::OnUpdateDisplay();
}

/*****************************************************************************/
/* EvalException : handle exceptions in called effect                        */
/*****************************************************************************/

int CSmpEffect::EvalException(int n_except)
{
TRACE0("Ouch! Ouch! Effect just died and wants to take us with it! OUCH!\n");

GetApp()->FullStop();
return EXCEPTION_CONTINUE_SEARCH;       /* go to next handler                */
}

/*****************************************************************************/
/* EffDispatch : called for any effect method to be sent                     */
/*****************************************************************************/

long CSmpEffect::EffDispatch(long opCode, long index, long value, void *ptr, float opt)
{
#if defined(_DEBUG) || defined(_DEBUGFILE)
char szDebug[1024];
int nLen;
switch (opCode)
  {
  case effOpen :
    nLen = sprintf(szDebug, "effDispatch: %d effOpen", nIndex);
    break;
  case effClose :
    nLen = sprintf(szDebug, "effDispatch: %d effClose", nIndex);
    break;
  case effSetProgram :
    nLen = sprintf(szDebug, "effDispatch: %d effSetProgram %d", nIndex, value);
    break;
  case effGetProgram :
    nLen = sprintf(szDebug, "effDispatch: %d effGetProgram", nIndex);
    break;
  case effSetProgramName :
    nLen = sprintf(szDebug, "effDispatch: %d effSetProgramName \"%s\"", nIndex, ptr);
    break;
  case effGetProgramName :
    nLen = sprintf(szDebug, "effDispatch: %d effGetProgramName", nIndex);
    break;
  case effGetParamLabel :
    nLen = sprintf(szDebug, "effDispatch: %d effGetParamLabel %d", nIndex, index);
    break;
  case effGetParamDisplay :
    nLen = sprintf(szDebug, "effDispatch: %d effGetParamDisplay %d", nIndex, index);
    break;
  case effGetParamName :
    nLen = sprintf(szDebug, "effDispatch: %d effGetParamName %d", nIndex, index);
    break;
  case effGetVu :
    nLen = sprintf(szDebug, "effDispatch: %d effGetVu", nIndex);
    break;
  case effSetSampleRate :
    nLen = sprintf(szDebug, "effDispatch: %d effSetSampleRate %f", nIndex, opt);
    break;
  case effSetBlockSize :
    nLen = sprintf(szDebug, "effDispatch: %d effSetBlockSize %d", nIndex, value);
    break;
  case effMainsChanged :
    nLen = sprintf(szDebug, "effDispatch: %d effMainsChanged %d", nIndex, value);
    break;
  case effEditGetRect :
    nLen = sprintf(szDebug, "effDispatch: %d effEditGetRect", nIndex);
    break;
  case effEditOpen :
    nLen = sprintf(szDebug, "effDispatch: %d effEditOpen", nIndex);
    break;
  case effEditClose :
    nLen = sprintf(szDebug, "effDispatch: %d effEditClose", nIndex);
    break;
  case effEditDraw :
    nLen = sprintf(szDebug, "effDispatch: %d effEditDraw", nIndex);
    break;
  case effEditMouse :
    nLen = sprintf(szDebug, "effDispatch: %d effEditMouse", nIndex);
    break;
  case effEditKey :
    nLen = sprintf(szDebug, "effDispatch: %d effEditKey", nIndex);
    break;
  case effEditIdle :
    nLen = sprintf(szDebug, "effDispatch: %d effEditIdle", nIndex);
    break;
  case effEditTop :
    nLen = sprintf(szDebug, "effDispatch: %d effEditTop", nIndex);
    break;
  case effEditSleep :
    nLen = sprintf(szDebug, "effDispatch: %d effEditSleep", nIndex);
    break;
  case effIdentify :
    nLen = sprintf(szDebug, "effDispatch: %d effIdentify", nIndex);
    break;
  case effGetChunk :
    nLen = sprintf(szDebug, "effDispatch: %d effGetChunk %d", nIndex, index);
    break;
  case effSetChunk :
    nLen = sprintf(szDebug, "effDispatch: %d effSetChunk %d,%d", nIndex, index, value);
    break;
  case effProcessEvents :
    nLen = sprintf(szDebug, "effDispatch: %d effProcessEvents %d", nIndex, ((VstEvents *)ptr)->numEvents);
    break;
  case effCanBeAutomated :
    nLen = sprintf(szDebug, "effDispatch: %d effCanBeAutomated %d", nIndex, index);
    break;
  case effString2Parameter :
    nLen = sprintf(szDebug, "effDispatch: %d effString2Parameter %d \"%s\"", nIndex, index, ptr ? ptr : "(null)");
    break;
  case effGetNumProgramCategories :
    nLen = sprintf(szDebug, "effDispatch: %d effGetNumProgramCategories", nIndex);
    break;
  case effGetProgramNameIndexed :
    nLen = sprintf(szDebug, "effDispatch: %d effGetProgramNameIndexed %d,%d", nIndex, index, value);
    break;
  case effCopyProgram :
    nLen = sprintf(szDebug, "effDispatch: %d effCopyProgram %d", nIndex, index);
    break;
  case effConnectInput :
    nLen = sprintf(szDebug, "effDispatch: %d effConnectInput %d,%d", nIndex, index, value);
    break;
  case effConnectOutput :
    nLen = sprintf(szDebug, "effDispatch: %d effConnectOutput %d,%d", nIndex, index, value);
    break;
  case effGetInputProperties :
    nLen = sprintf(szDebug, "effDispatch: %d effGetInputProperties %d", nIndex, index);
    break;
  case effGetOutputProperties :
    nLen = sprintf(szDebug, "effDispatch: %d effGetOutputProperties %d", nIndex, index);
    break;
  case effGetPlugCategory :
    nLen = sprintf(szDebug, "effDispatch: %d effGetPlugCategory", nIndex);
    break;
  case effGetCurrentPosition :
    nLen = sprintf(szDebug, "effDispatch: %d effGetCurrentPosition", nIndex);
    break;
  case effGetDestinationBuffer :
    nLen = sprintf(szDebug, "effDispatch: %d effGetDestinationBuffer", nIndex);
    break;
  case effOfflineNotify :
    nLen = sprintf(szDebug, "effDispatch: %d effOfflineNotify %d,%d", nIndex, value, index);
    break;
  case effOfflinePrepare :
    nLen = sprintf(szDebug, "effDispatch: %d effOfflinePrepare %d", nIndex, value);
    break;
  case effOfflineRun :
    nLen = sprintf(szDebug, "effDispatch: %d effOfflineRun %d", nIndex, value);
    break;
  case effProcessVarIo :
    nLen = sprintf(szDebug, "effDispatch: %d effProcessVarIo", nIndex);
    break;
  case effSetSpeakerArrangement :
    nLen = sprintf(szDebug, "effDispatch: %d effSetSpeakerArrangement", nIndex);
    break;
  case effSetBlockSizeAndSampleRate :
    nLen = sprintf(szDebug, "effDispatch: %d effSetBlockSizeAndSampleRate %d,%f", nIndex, value, opt);
    break;
  case effSetBypass :
    nLen = sprintf(szDebug, "effDispatch: %d effSetBypass %d", nIndex, value);
    break;
  case effGetEffectName :
    nLen = sprintf(szDebug, "effDispatch: %d effGetEffectName", nIndex);
    break;
  case effGetErrorText :
    nLen = sprintf(szDebug, "effDispatch: %d effGetErrorText", nIndex);
    break;
  case effGetVendorString :
    nLen = sprintf(szDebug, "effDispatch: %d effGetVendorString", nIndex);
    break;
  case effGetProductString :
    nLen = sprintf(szDebug, "effDispatch: %d effGetProductString", nIndex);
    break;
  case effGetVendorVersion :
    nLen = sprintf(szDebug, "effDispatch: %d effGetVendorVersion", nIndex);
    break;
  case effVendorSpecific :
    nLen = sprintf(szDebug, "effDispatch: %d effVendorSpecific %d,%d,%08lX,%f", nIndex, index, value, ptr, opt);
    break;
  case effCanDo :
    nLen = sprintf(szDebug, "effDispatch: %d effCanDo \"%s\"", nIndex, ptr);
    break;
  case effGetTailSize :
    nLen = sprintf(szDebug, "effDispatch: %d effGetTailSize", nIndex);
    break;
  case effIdle :
    nLen = sprintf(szDebug, "effDispatch: %d effIdle", nIndex);
    break;
  case effGetIcon :
    nLen = sprintf(szDebug, "effDispatch: %d effGetIcon", nIndex);
    break;
  case effSetViewPosition :
    nLen = sprintf(szDebug, "effDispatch: %d effSetViewPosition %d,%d", nIndex, index, value);
    break;
  case effGetParameterProperties :
    nLen = sprintf(szDebug, "effDispatch: %d effGetParameterProperties %d", nIndex);
    break;
  case effKeysRequired :
    nLen = sprintf(szDebug, "effDispatch: %d effKeysRequired", nIndex);
    break;
  case effGetVstVersion :
    nLen = sprintf(szDebug, "effDispatch: %d effGetVstVersion", nIndex);
    break;
#if defined(VST_2_1_EXTENSIONS)
    //---from here VST 2.1 extension opcodes---------------------------------------------------------
  case effEditKeyDown :
    nLen = sprintf(szDebug, "effDispatch: %d effEditKeyDown '%c',%d,%d", nIndex, index, value, (unsigned char)opt);
    break;
  case effEditKeyUp :
    nLen = sprintf(szDebug, "effDispatch: %d effEditKeyUp '%c',%d,%d", nIndex, index, value, (unsigned char)opt);
    break;
  case effSetEditKnobMode :
    nLen = sprintf(szDebug, "effDispatch: %d effSetEditKnobMode %d", nIndex, value);
    break;
  case effGetMidiProgramName :
    nLen = sprintf(szDebug, "effDispatch: %d effGetMidiProgramName %d", nIndex, index);
    break;
  case effGetCurrentMidiProgram :
    nLen = sprintf(szDebug, "effDispatch: %d effGetCurrentMidiProgram %d", nIndex, index);
    break;
  case effGetMidiProgramCategory :
    nLen = sprintf(szDebug, "effDispatch: %d effGetMidiProgramCategory %d", nIndex, index);
    break;
  case effHasMidiProgramsChanged :
    nLen = sprintf(szDebug, "effDispatch: %d effHasMidiProgramsChanged %d", nIndex, index);
    break;
  case effGetMidiKeyName :
    nLen = sprintf(szDebug, "effDispatch: %d effGetMidiKeyName %d", nIndex, index);
    break;
  case effBeginSetProgram :
    nLen = sprintf(szDebug, "effDispatch: %d effBeginSetProgram", nIndex);
    break;
  case effEndSetProgram :
    nLen = sprintf(szDebug, "effDispatch: %d effEndSetProgram", nIndex);
    break;
#endif
#if defined(VST_2_3_EXTENSIONS)
    //---from here VST 2.3 extension opcodes---------------------------------------------------------
  case effGetSpeakerArrangement :
    nLen = sprintf(szDebug, "effDispatch: %d effGetSpeakerArrangement", nIndex);
    break;
  case effShellGetNextPlugin :
    nLen = sprintf(szDebug, "effDispatch: %d effShellGetNextPlugin", nIndex);
    break;
  case effStartProcess :
    nLen = sprintf(szDebug, "effDispatch: %d effStartProcess", nIndex);
    break;
  case effStopProcess :
    nLen = sprintf(szDebug, "effDispatch: %d effStopProcess", nIndex);
    break;
  case effSetTotalSampleToProcess :
    nLen = sprintf(szDebug, "effDispatch: %d effSetTotalSampleToProcess %d", nIndex, value);
    break;
  case effSetPanLaw :
    nLen = sprintf(szDebug, "effDispatch: %d effSetPanLaw %d,%f", nIndex, value, opt);
    break;
  case effBeginLoadBank :
    nLen = sprintf(szDebug, "effDispatch: %d effBeginLoadBank", nIndex);
    break;
  case effBeginLoadProgram :
    nLen = sprintf(szDebug, "effDispatch: %d effBeginLoadProgram", nIndex);
    break;
#endif
  }
#endif

long lRC = 0;
//__try
  {
  lRC = CEffect::EffDispatch(opCode, index, value, ptr, opt);
  }
//__except (EvalException(GetExceptionCode()))
  {
       // no code here; isn't executed
  }

#if defined(_DEBUG) || defined(_DEBUGFILE)
switch (opCode)
  {
  case effGetProgramName:
  case effGetParamLabel :
  case effGetParamDisplay :
  case effGetParamName :
  case effGetProgramNameIndexed :
  case effGetEffectName :
  case effGetErrorText :
  case effGetVendorString :
  case effGetProductString :
  case effShellGetNextPlugin :
    nLen += sprintf(szDebug + nLen, " \"%s\"", ptr);
    break;
  }

sprintf(szDebug + nLen, " RC=%d", lRC);
TRACE1("%s\n", szDebug);
#endif

return lRC;
}

/*****************************************************************************/
/* OnSetParameterAutomated : called when an effect reports a parm change     */
/*****************************************************************************/

bool CSmpEffect::OnSetParameterAutomated(long index, float value)
{
if (pParmWnd)                           /* pass it on.                       */
  return pParmWnd->OnSetParameterAutomated(index, value);
return false;
}

/*****************************************************************************/
/* EnterCritical : enter critical section                                    */
/*****************************************************************************/

void CSmpEffect::EnterCritical()
{
EnterCriticalSection(&cs);              /* inhibit others!                   */
}

/*****************************************************************************/
/* LeaveCritical : leave critical section                                    */
/*****************************************************************************/

void CSmpEffect::LeaveCritical()
{
LeaveCriticalSection(&cs);              /* reallow others                    */
}

/*****************************************************************************/
/* LoadBank : loads a bank into the effect                                   */
/*****************************************************************************/

bool CSmpEffect::LoadBank(const char *name)
{
CFxBank b((char *)name);
bool brc = false;

if ((!b.IsLoaded()) ||
    (pEffect->uniqueID != b.GetFxID()))
  return false;

if (b.IsChunk())
  {
  if (!(pEffect->flags & effFlagsProgramChunks))
    return false;
  brc = (EffSetChunk(b.GetChunk(), b.GetChunkSize()) > 0);
  }
else
  {
  int cProg = EffGetProgram();
  int i, j;
  int nParms = b.GetNumParams();
  for (i = 0; i < b.GetNumPrograms(); i++)
    {
    EffSetProgram(i);
    EffSetProgramName(b.GetProgramName(i));
    for (j = 0; j < nParms; j++)
      EffSetParameter(j, b.GetProgParm(i, j));
    
    }
  EffSetProgram(cProg);
  brc = true;
  }
if (brc)
  SetChunkFile(name);
return brc;
}

/*****************************************************************************/
/* ShowDetails : shows new details                                           */
/*****************************************************************************/

void CSmpEffect::ShowDetails()
{
if (pFrameWnd)
  pFrameWnd->ShowDetails();
}
