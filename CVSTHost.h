/*****************************************************************************/
/* CVSTHost.h: interface for the CVSTHost class.                             */
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


#if !defined(VSTHOST_H__INCLUDED_)
#define VSTHOST_H__INCLUDED_

#include "audioeffectx.h"               /* VST header files                  */

#if defined(VST_2_4_EXTENSIONS)
#else
typedef int VstInt32;                   /* this one's heavily used in V2.4++ */
// ... and Steinberg goofed big time by making this 'typedef int VstInt32' in
// the original VST SDK 2.4, which is architecture-dependent...
#endif


#if !defined(VST_2_1_EXTENSIONS)
struct VstFileSelect;
//---Structure and enum used for keyUp/keyDown-----
struct VstKeyCode
{
	long character;
	unsigned char virt;     // see enum VstVirtualKey
	unsigned char modifier; // see enum VstModifierKey
};
struct MidiProgramName;
struct MidiProgramCategory;
struct MidiKeyName;
#endif

/*===========================================================================*/
/* structures for .fxb / .fxp files                                          */
/*===========================================================================*/

// These structures haven't been officially documented before VST SDK V2.2.
// Since then, they have undergone quite "interesting" changes in the SDK,
// which makes it quite a pain to maintain a link between them and VSTHost's
// file-handling classes. That is simply too dumb.
// From now on, I'll use my OWN structures to describe the contents of
// .fxb / .fxp files. In C++ style, which is 10 times easier to read
// and maintain. Of course, they'll match the latest available SDK.

/*****************************************************************************/
/* Constants used in the files - copied verbatim from vstfxstore.h           */
/*****************************************************************************/

#define cMagic 		'CcnK'
#define fMagic		'FxCk'
#define bankMagic	'FxBk'
#define chunkGlobalMagic	'FxCh'
#define chunkPresetMagic	'FPCh'
#define chunkBankMagic		'FBCh'

/*****************************************************************************/
/* SFxHeader : header for all chunks                                         */
/*****************************************************************************/

struct SFxHeader
  {
  VstInt32 chunkMagic;                  /* 'CcnK' in any case                */
  VstInt32 byteSize;                    /* size of this chunk, excluding     */
                                        /* chunkMagic & byteSize             */
  };

/*****************************************************************************/
/* SFxBase : base structure for all chunks                                   */
/*****************************************************************************/

struct SFxBase : public SFxHeader
  {
  VstInt32 fxMagic;                     /* for programs:                     */
                                        /*   'FxCk' (regular) or             */
                                        /*   'FPCh' (opaque chunk)           */
                                        /* for banks:                        */
                                        /*   'FxBk' (regular) or             */
                                        /*   'FBCh' (opaque chunk)           */
  VstInt32 version;                     /* format version                    */
                                        /* for programs: 1                   */
                                        /* for banks: 1 or 2                 */
  VstInt32 fxID;                        /* fx unique ID                      */
  VstInt32 fxVersion;                   /* fx version                        */
  };

/*****************************************************************************/
/* SFxProgramBase : base structure for all programs                          */
/*****************************************************************************/

struct SFxProgramBase : public SFxBase
  {
  VstInt32 numParams;                   /* number of parameters              */
  char prgName[28];                     /* program name (ASCIIZ)             */
  };

/*****************************************************************************/
/* SFxProgram : a program in single-parameter style                          */
/*****************************************************************************/

struct SFxProgram : public SFxProgramBase
  {
  float params[1];                      /* in reality, a variable-sized array*/
                                        /* with numParams parameter values   */
  };

/*****************************************************************************/
/* SFxProgramChunk : a program in chunk style                                */
/*****************************************************************************/

struct SFxProgramChunk : public SFxProgramBase
  {
  VstInt32 size;                        /* size of program data in bytes     */
  char chunk[1];                        /* variable-sized opaque array       */
  };

/*****************************************************************************/
/* SFxBankBase : base structure for a bank of programs                       */
/*****************************************************************************/

struct SFxBankBase : public SFxBase
  {
  VstInt32 numPrograms;                 /* number of programs in bank        */
  VstInt32 currentProgram;              /* if version 2, current program,    */
                                        /* else irrelevant                   */
  char future[124];                     /* reserved (should be zero)         */
  };

/*****************************************************************************/
/* SFxBank : structure for a bank of programs consisting of parameters       */
/*****************************************************************************/

struct SFxBank : public SFxBankBase
  {
  SFxProgram programs[1];               /* variable number of programs       */
  };

/*****************************************************************************/
/* SFxBankChunk : structure for a bank of programs as an opaque chunk        */
/*****************************************************************************/

struct SFxBankChunk : public SFxBankBase
  {
  VstInt32 size;                        /* size of bank data in bytes        */
  char chunk[1];                        /* variable-sized opaque array       */
  };

#if !defined(VST_2_3_EXTENSIONS)
struct VstSpeakerArrangement;
struct VstPatchChunkInfo;
#endif

#if !defined(VST_2_1_EXTENSIONS)
enum                                    /* V2.1 dispatcher opcodes           */
  {
  effEditKeyDown = effNumV2Opcodes,
  effEditKeyUp,
  effSetEditKnobMode,
  effGetMidiProgramName,
  effGetCurrentMidiProgram,
  effGetMidiProgramCategory,
  effHasMidiProgramsChanged,
  effGetMidiKeyName,
  effBeginSetProgram,
  effEndSetProgram,
  effNumV2_1Opcodes
  };
#endif

#if !defined(VST_2_3_EXTENSIONS)
enum                                    /* V2.3 dispatcher opcodes           */
  {
  effGetSpeakerArrangement = effNumV2_1Opcodes,
  effShellGetNextPlugin,
  effStartProcess,
  effStopProcess,
  effSetTotalSampleToProcess,
  effSetPanLaw,
  effBeginLoadBank,
  effBeginLoadProgram,
  effNumV2_3Opcodes
  };
#endif

#if !defined(VST_2_4_EXTENSIONS)
enum                                    /* V2.4 dispatcher opcodes           */
  {
  effSetProcessPrecision = effNumV2_3Opcodes,
  effGetNumMidiInputChannels,
  effGetNumMidiOutputChannels,
  effNumV2_4Opcodes
  };
enum                                    /* V2.4 flags                        */
  {
  effFlagsCanDoubleReplacing = 1 << 12,
  };
enum VstMidiEventFlags                  /* V2.4 MIDI Event flags             */
  {
  kVstMidiEventIsRealtime = 1 << 0
  };
enum VstAutomationStates                /* V2.4 automation state definitions */
  {
  kVstAutomationUnsupported = 0,
  kVstAutomationOff,
  kVstAutomationRead,
  kVstAutomationWrite,
  kVstAutomationReadWrite
  };
#endif

#if defined(VST_2_4_EXTENSIONS)
#include "aeffEditor.h"
#else
#include "AEffEditor.hpp"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*****************************************************************************/
/* CFxBase : base class for FX Bank / Program Files                          */
/*****************************************************************************/

class CFxBase
{
public:

protected:
	static bool NeedsBSwap;

protected:
	static void SwapBytes(float &f);
	static void SwapBytes(long &l);
#if defined(VST_2_4_EXTENSIONS)
	static void SwapBytes(VstInt32 &vi);
#endif

};

/*****************************************************************************/
/* CFxBank : class for an Fx Bank file                                       */
/*****************************************************************************/

class CFxBank : public CFxBase
{
public:
    CFxBank(char *pszFile = 0);
    CFxBank(int nPrograms, int nParams);
    CFxBank(int nChunkSize);
    CFxBank(CFxBank const &org) { DoCopy(org); }
	virtual ~CFxBank();
    CFxBank & operator=(CFxBank const &org) { return DoCopy(org); }
public:
    bool SetSize(int nPrograms, int nParams);
    bool SetSize(int nChunkSize);
	bool LoadBank(char *pszFile);
	bool SaveBank(char *pszFile);
	void Unload();
    bool IsLoaded() { return !!bBank; }
    bool IsChunk() { return bChunk; }

    // access functions
public:
    long GetVersion() { if (!bBank) return 0; return ((SFxBase*)bBank)->version; }
    long GetFxID() { if (!bBank) return 0; return ((SFxBase*)bBank)->fxID; }
    void SetFxID(long id) { if (bBank) ((SFxBase*)bBank)->fxID = id; if (!bChunk) for (int i = GetNumPrograms() -1; i >= 0; i--) GetProgram(i)->fxID = id; }
    long GetFxVersion() { if (!bBank) return 0; return ((SFxBase*)bBank)->fxVersion; }
    void SetFxVersion(long v) { if (bBank) ((SFxBase*)bBank)->fxVersion = v; if (!bChunk) for (int i = GetNumPrograms() -1; i >= 0; i--) GetProgram(i)->fxVersion = v; }
    long GetNumPrograms() { if (!bBank) return 0; return ((SFxBankBase*)bBank)->numPrograms; }
    long GetNumParams() { if (bChunk) return 0; return GetProgram(0)->numParams; }
    long GetChunkSize() { if (!bChunk) return 0; return ((SFxBankChunk *)bBank)->size; }
    void *GetChunk() { if (!bChunk) return 0; return ((SFxBankChunk *)bBank)->chunk; }
    bool SetChunk(void *chunk) { if (!bChunk) return false; memcpy(((SFxBankChunk *)bBank)->chunk, chunk, ((SFxBankChunk *)bBank)->size); return true; }

	SFxProgram * GetProgram(int nProgNum);

    char * GetProgramName(int nProgram)
      {
      SFxProgram *p = GetProgram(nProgram);
      if (!p)
        return NULL;
      return p->prgName;
      }
    void SetProgramName(int nProgram, char *name = "")
      {
      SFxProgram *p = GetProgram(nProgram);
      if (!p)
        return;
      strncpy(p->prgName, name, sizeof(p->prgName));
      p->prgName[sizeof(p->prgName)-1] = '\0';
      }
    float GetProgParm(int nProgram, int nParm)
      {
      SFxProgram *p = GetProgram(nProgram);
      if (!p || nParm > p->numParams)
        return 0;
#ifndef chunkGlobalMagic                /* VST SDK 2.4 rev2?                 */
      return p->content.params[nParm];
#else
      return p->params[nParm];
#endif
      }
    bool SetProgParm(int nProgram, int nParm, float val = 0.0)
      {
      SFxProgram *p = GetProgram(nProgram);
      if (!p || nParm > p->numParams)
        return false;
      if (val < 0.0)
        val = 0.0;
      if (val > 1.0)
        val = 1.0;
#ifndef chunkGlobalMagic                /* VST SDK 2.4 rev2?                 */
      p->content.params[nParm] = val;
#else
      p->params[nParm] = val;
#endif
      return true;
      }

protected:
	char szFileName[256];
	unsigned char * bBank;
	int nBankLen;
	bool bChunk;

protected:
    void Init();
	CFxBank & DoCopy(CFxBank const &org);
};

/*****************************************************************************/
/* CFxProgram : class for an Fx Program file                                 */
/*****************************************************************************/

// not really designed yet...
class CFxProgram : public CFxBase
{
};

/*****************************************************************************/
/* CEffect : class definition for audio effect objects                       */
/*****************************************************************************/

class CVSTHost;
class CEffect
{
public:
    CEffect(CVSTHost *pHost);
    virtual ~CEffect();

public:
    CVSTHost *pHost;
    AEffect *pEffect;
    char *sName;
    bool bEditOpen;
	bool bNeedIdle;
    bool bInEditIdle;
    bool bWantMidi;
    bool bInSetProgram;
    long nIndex;                        /* index in VSTHost plugin array     */
    long nUniqueId;                     /* unique plugin ID (shell plugin)   */
    CEffect *pMasterEffect;             /* for Shell-type plugins            */

#ifdef WIN32

    HMODULE hModule;
    char *sDir;

#elif MAC

// yet to do
// no idea how things look here...

#endif

    virtual bool Load(const char *name);
    virtual bool Unload();

    void SetIndex(int nNewIndex) { nIndex = nNewIndex; }
    long GetIndex() { return nIndex; }

    virtual bool LoadBank(char *name);
    virtual bool SaveBank(char *name);

    virtual long EffDispatch(long opCode, long index=0, long value=0, void *ptr=0, float opt=0.);
	virtual void EffProcess(float **inputs, float **outputs, long sampleframes);
	virtual void EffProcessReplacing(float **inputs, float **outputs, long sampleframes);
	virtual void EffProcessDoubleReplacing(double** inputs, double** outputs, long sampleFrames);
	virtual void EffSetParameter(long index, float parameter);
	virtual float EffGetParameter(long index);

    virtual void EffOpen() { EffDispatch(effOpen); }
    virtual void EffClose() { EffDispatch(effClose); }
    virtual void EffSetProgram(long lValue) { EffBeginSetProgram(); EffDispatch(effSetProgram, 0, lValue); EffEndSetProgram(); }
    virtual long EffGetProgram() { return EffDispatch(effGetProgram); }
    virtual void EffSetProgramName(char *ptr) { EffDispatch(effSetProgramName, 0, 0, ptr); }
    virtual void EffGetProgramName(char *ptr) { EffDispatch(effGetProgramName, 0, 0, ptr); }
    virtual void EffGetParamLabel(long index, char *ptr) { EffDispatch(effGetParamLabel, index, 0, ptr); }
    virtual void EffGetParamDisplay(long index, char *ptr) { EffDispatch(effGetParamDisplay, index, 0, ptr); }
    virtual void EffGetParamName(long index, char *ptr) { EffDispatch(effGetParamName, index, 0, ptr); }
    virtual void EffSetSampleRate(float fSampleRate) { EffDispatch(effSetSampleRate, 0, 0, 0, fSampleRate); }
    virtual void EffSetBlockSize(long value) { EffDispatch(effSetBlockSize, 0, value); }
    virtual void EffMainsChanged(bool bOn) { EffDispatch(effMainsChanged, 0, bOn); }
    virtual void EffSuspend() { EffDispatch(effMainsChanged, 0, false); }
    virtual void EffResume() { EffDispatch(effMainsChanged, 0, true); }
    virtual float EffGetVu() { return (float)EffDispatch(effGetVu) / (float)32767.; }
    virtual long EffEditGetRect(ERect **ptr) { return EffDispatch(effEditGetRect, 0, 0, ptr); }
    virtual long EffEditOpen(void *ptr) { long l = EffDispatch(effEditOpen, 0, 0, ptr); /* if (l > 0) */ bEditOpen = true; return l; }
    virtual void EffEditClose() { EffDispatch(effEditClose); bEditOpen = false; }
    virtual void EffEditIdle() { if ((!bEditOpen) || (bInEditIdle)) return; bInEditIdle = true; EffDispatch(effEditIdle); bInEditIdle = false; }
#if MAC
    virtual void EffEditDraw(void *ptr) { EffDispatch(nEffect, effEditDraw, 0, 0, ptr); }
    virtual long EffEditMouse(long index, long value) { return EffDispatch(nEffect, effEditMouse, index, value); }
    virtual long EffEditKey(long value) { return EffDispatch(effEditKey, 0, value); }
    virtual void EffEditTop() { EffDispatch(effEditTop); }
    virtual void EffEditSleep() { EffDispatch(effEditSleep); }
#endif
    virtual long EffIdentify() { return EffDispatch(effIdentify); }
    virtual long EffGetChunk(void **ptr, bool isPreset = false) { return EffDispatch(effGetChunk, isPreset, 0, ptr); }
    virtual long EffSetChunk(void *data, long byteSize, bool isPreset = false) { EffBeginSetProgram(); long lResult = EffDispatch(effSetChunk, isPreset, byteSize, data); EffEndSetProgram(); return lResult;}
                                        /* VST 2.0                           */
    virtual long EffProcessEvents(VstEvents* ptr) { return EffDispatch(effProcessEvents, 0, 0, ptr); }
    virtual long EffCanBeAutomated(long index) { return EffDispatch(effCanBeAutomated, index); }
    virtual long EffString2Parameter(long index, char *ptr) { return EffDispatch(effString2Parameter, index, 0, ptr); }
    virtual long EffGetNumProgramCategories() { return EffDispatch(effGetNumProgramCategories); }
    virtual long EffGetProgramNameIndexed(long category, long index, char* text) { return EffDispatch(effGetProgramNameIndexed, index, category, text); }
    virtual long EffCopyProgram(long index) { return EffDispatch(effCopyProgram, index); }
    virtual long EffConnectInput(long index, bool state) { return EffDispatch(effConnectInput, index, state); }
    virtual long EffConnectOutput(long index, bool state) { return EffDispatch(effConnectOutput, index, state); }
    virtual long EffGetInputProperties(long index, VstPinProperties *ptr) { return EffDispatch(effGetInputProperties, index, 0, ptr); }
    virtual long EffGetOutputProperties(long index, VstPinProperties *ptr) { return EffDispatch(effGetOutputProperties, index, 0, ptr); }
    virtual long EffGetPlugCategory() { return EffDispatch(effGetPlugCategory); }
    virtual long EffGetCurrentPosition() { return EffDispatch(effGetCurrentPosition); }
    virtual long EffGetDestinationBuffer() { return EffDispatch(effGetDestinationBuffer); }
    virtual long EffOfflineNotify(VstAudioFile* ptr, long numAudioFiles, bool start) { return EffDispatch(effOfflineNotify, start, numAudioFiles, ptr); }
    virtual long EffOfflinePrepare(VstOfflineTask *ptr, long count) { return EffDispatch(effOfflinePrepare, 0, count, ptr); }
    virtual long EffOfflineRun(VstOfflineTask *ptr, long count) { return EffDispatch(effOfflineRun, 0, count, ptr); }
    virtual long EffProcessVarIo(VstVariableIo* varIo) { return EffDispatch(effProcessVarIo, 0, 0, varIo); }
    virtual long EffSetSpeakerArrangement(VstSpeakerArrangement* pluginInput, VstSpeakerArrangement* pluginOutput) { return EffDispatch(effSetSpeakerArrangement, 0, (long)pluginInput, pluginOutput); }
    virtual long EffSetBlockSizeAndSampleRate(long blockSize, float sampleRate) { return EffDispatch(effSetBlockSizeAndSampleRate, 0, blockSize, 0, sampleRate); }
    virtual long EffSetBypass(bool onOff) { return EffDispatch(effSetBypass, 0, onOff); }
    virtual long EffGetEffectName(char *ptr) { return EffDispatch(effGetEffectName, 0, 0, ptr); }
    virtual long EffGetErrorText(char *ptr) { return EffDispatch(effGetErrorText, 0, 0, ptr); }
    virtual long EffGetVendorString(char *ptr) { return EffDispatch(effGetVendorString, 0, 0, ptr); }
    virtual long EffGetProductString(char *ptr) { return EffDispatch(effGetProductString, 0, 0, ptr); }
    virtual long EffGetVendorVersion() { return EffDispatch(effGetVendorVersion); }
    virtual long EffVendorSpecific(long index, long value, void *ptr, float opt) { return EffDispatch(effVendorSpecific, index, value, ptr, opt); }
    virtual long EffCanDo(const char *ptr) { return EffDispatch(effCanDo, 0, 0, (void *)ptr); }
    virtual long EffGetTailSize() { return EffDispatch(effGetTailSize); }
    virtual long EffIdle() { if (bNeedIdle) return EffDispatch(effIdle); else return 0; }
    virtual long EffGetIcon() { return EffDispatch(effGetIcon); }
    virtual long EffSetViewPosition(long x, long y) { return EffDispatch(effSetViewPosition, x, y); }
    virtual long EffGetParameterProperties(long index, VstParameterProperties* ptr) { return EffDispatch(effGetParameterProperties, index, 0, ptr); }
    virtual long EffKeysRequired() { return EffDispatch(effKeysRequired); }
    virtual long EffGetVstVersion() { return EffDispatch(effGetVstVersion); }
                                        /* VST 2.1 extensions                */
    virtual long EffKeyDown(VstKeyCode &keyCode) { return EffDispatch(effEditKeyDown, keyCode.character, keyCode.virt, 0, keyCode.modifier); }
    virtual long EffKeyUp(VstKeyCode &keyCode) { return EffDispatch(effEditKeyUp, keyCode.character, keyCode.virt, 0, keyCode.modifier); }
    virtual void EffSetKnobMode(long value) { EffDispatch(effSetEditKnobMode, 0, value); }
    virtual long EffGetMidiProgramName(long channel, MidiProgramName* midiProgramName) { return EffDispatch(effGetMidiProgramName, channel, 0, midiProgramName); }
    virtual long EffGetCurrentMidiProgram (long channel, MidiProgramName* currentProgram) { return EffDispatch(effGetCurrentMidiProgram, channel, 0, currentProgram); }
    virtual long EffGetMidiProgramCategory (long channel, MidiProgramCategory* category) { return EffDispatch(effGetMidiProgramCategory, channel, 0, category); }
    virtual long EffHasMidiProgramsChanged (long channel) { return EffDispatch(effHasMidiProgramsChanged, channel); }
    virtual long EffGetMidiKeyName(long channel, MidiKeyName* keyName) { return EffDispatch(effGetMidiKeyName, channel, 0, keyName); }
    virtual long EffBeginSetProgram() { bInSetProgram = !!EffDispatch(effBeginSetProgram); return bInSetProgram; }
    virtual long EffEndSetProgram() { bInSetProgram = false; return EffDispatch(effEndSetProgram); }
                                        /* VST 2.3 Extensions                */
    virtual long EffGetSpeakerArrangement(VstSpeakerArrangement** pluginInput, VstSpeakerArrangement** pluginOutput) { return EffDispatch(effGetSpeakerArrangement, 0, (long)pluginInput, pluginOutput); }
    virtual long EffSetTotalSampleToProcess (long value) { return EffDispatch(effSetTotalSampleToProcess, 0, value); }
    virtual long EffGetNextShellPlugin(char *name) { return EffDispatch(effShellGetNextPlugin, 0, 0, name); }
    virtual long EffStartProcess() { return EffDispatch(effStartProcess); }
    virtual long EffStopProcess() { return EffDispatch(effStopProcess); }
    virtual long EffSetPanLaw(long type, float val) { return EffDispatch(effSetPanLaw, 0, type, 0, val); }
    virtual long EffBeginLoadBank(VstPatchChunkInfo* ptr) { return EffDispatch(effBeginLoadBank, 0, 0, ptr); }
    virtual long EffBeginLoadProgram(VstPatchChunkInfo* ptr) { return EffDispatch(effBeginLoadProgram, 0, 0, ptr); }
                                        /* VST 2.4 extensions                */
    virtual long EffSetProcessPrecision(long precision) { return EffDispatch(effSetProcessPrecision, 0, precision, 0); }
    virtual long EffGetNumMidiInputChannels() { return EffDispatch(effGetNumMidiInputChannels, 0, 0, 0); }
    virtual long EffGetNumMidiOutputChannels() { return EffDispatch(effGetNumMidiOutputChannels, 0, 0, 0); }

// overridables
public:
    virtual long OnGetUniqueId() { return nUniqueId; }
	virtual void * OnGetDirectory();
    virtual void OnSizeEditorWindow(long width, long height) { }
    virtual bool OnUpdateDisplay() { return false; }
    virtual void * OnOpenWindow(VstWindow* window) { return 0; }
    virtual bool OnCloseWindow(VstWindow* window) { return false; }
    virtual bool OnIoChanged() { return false; }
    virtual long OnGetNumAutomatableParameters() { return (pEffect) ? pEffect->numParams : 0; }

};

/*****************************************************************************/
/* CVSTHost class declaration                                                */
/*****************************************************************************/

enum
  {
  eEffLoading = 1000000                 /* special effect # for loading      */
  };

class CVSTHost
{
friend class CEffect;
public:
	CVSTHost();
	virtual ~CVSTHost();

protected:
    void CalcTimeInfo(long lMask = -1);
    VstTimeInfo vstTimeInfo;
    float fSampleRate;
    long lBlockSize;

    int naEffects;
    int nmaEffects;
    void **aEffects;
    CEffect *pLoading;
    static CVSTHost * pHost;

    static long VSTCALLBACK AudioMasterCallback(AEffect *effect, long opcode, long index, long value, void *ptr, float opt);
    int Search(AEffect *pEffect);
    int GetPreviousPlugIn(int nEffect);
    int GetNextPlugIn(int nEffect);
    long EffDispatch(int nEffect, long opCode, long index=0, long value=0, void *ptr=0, float opt=0.);

public:
    virtual int LoadPlugin(const char * lpszName, int nUniqueId = 0);
    int GetSize() { return naEffects; }
    CEffect *GetAt(int nIndex) { if (nIndex == eEffLoading) return pLoading; else if ((nIndex >= 0) && (nIndex < naEffects)) return (CEffect *)aEffects[nIndex]; else return 0; }
    void RemoveAt(int nIndex);
    void RemoveAll();

    void EffProcess(int nEffect, float **inputs, float **outputs, long sampleframes);
    void EffProcessReplacing(int nEffect, float **inputs, float **outputs, long sampleframes);
	void EffProcessDoubleReplacing(int nEffect, double **inputs, double **outputs, long sampleframes);
    void EffSetParameter(int nEffect, long index, float parameter);
    float EffGetParameter(int nEffect, long index);

    void EffOpen(int nEffect)
      { if (GetAt(nEffect)) GetAt(nEffect)->EffOpen(); }
    void EffClose(int nEffect)
      { if (GetAt(nEffect)) GetAt(nEffect)->EffClose(); }
    void EffSetProgram(int nEffect, long lValue)
      { if (GetAt(nEffect)) GetAt(nEffect)->EffSetProgram(lValue); }
    long EffGetProgram(int nEffect)
      { if (GetAt(nEffect)) return (GetAt(nEffect))->EffGetProgram(); else return 0; }
    void EffSetProgramName(int nEffect, char *ptr)
      { if (GetAt(nEffect)) GetAt(nEffect)->EffSetProgramName(ptr); }
    void EffGetProgramName(int nEffect, char *ptr)
      { if (GetAt(nEffect)) GetAt(nEffect)->EffGetProgramName(ptr); }
    void EffGetParamLabel(int nEffect, long index, char *ptr)
      { if (GetAt(nEffect)) GetAt(nEffect)->EffGetParamLabel(index, ptr); }
    void EffGetParamDisplay(int nEffect, long index, char *ptr)
      { if (GetAt(nEffect)) GetAt(nEffect)->EffGetParamDisplay(index, ptr); }
    void EffGetParamName(int nEffect, long index, char *ptr)
      { if (GetAt(nEffect)) GetAt(nEffect)->EffGetParamName(index, ptr); }
    void EffSetSampleRate(int nEffect, float fSampleRate)
      { if (GetAt(nEffect)) GetAt(nEffect)->EffSetSampleRate(fSampleRate); }
    void EffSetBlockSize(int nEffect, long value)
      { if (GetAt(nEffect)) GetAt(nEffect)->EffSetBlockSize(value); }
    void EffMainsChanged(int nEffect, bool bOn)
      { if (GetAt(nEffect)) GetAt(nEffect)->EffMainsChanged(bOn); }
    void EffSuspend(int nEffect)
      { EffMainsChanged(nEffect, false); }
    void EffResume(int nEffect)
      { EffMainsChanged(nEffect, true); }
    float EffGetVu(int nEffect)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffGetVu(); else return 0.f; }
    long EffEditGetRect(int nEffect, ERect **ptr)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffEditGetRect(ptr); else return 0; }
    long EffEditOpen(int nEffect, void *ptr)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffEditOpen(ptr); else return 0; }
    void EffEditClose(int nEffect)
      { if (GetAt(nEffect)) GetAt(nEffect)->EffEditClose(); }
    void EffEditIdle(int nEffect)
      { if (GetAt(nEffect)) GetAt(nEffect)->EffEditIdle(); }
#if MAC
    void EffEditDraw(int nEffect, void *ptr)
      { if (GetAt(nEffect)) GetAt(nEffect)->EffEditDraw(ptr); }
    long EffEditMouse(int nEffect, long index, long value)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffEditMouse(index, value); else return 0; }
    long EffEditKey(int nEffect, long value)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffEditKey(value); else return 0; }
    void EffEditTop(int nEffect)
      { if (GetAt(nEffect)) GetAt(nEffect)->EffEditTop(); }
    void EffEditSleep(int nEffect)
      { if (GetAt(nEffect)) GetAt(nEffect)->EffEditSleep(); }
#endif
    long EffIdentify(int nEffect)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffIdentify(); else return 0; }
    long EffGetChunk(int nEffect, void **ptr, bool isPreset = false)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffGetChunk(ptr, isPreset); else return 0; }
    long EffSetChunk(int nEffect, void *data, long byteSize, bool isPreset = false)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffSetChunk(data, byteSize, isPreset); else return 0; }
                                        /* VST 2.0                           */
    long EffProcessEvents(int nEffect, VstEvents* ptr)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffProcessEvents(ptr); else return 0; }
    long EffCanBeAutomated(int nEffect, long index)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffCanBeAutomated(index); else return 0; }
    long EffString2Parameter(int nEffect, long index, char *ptr)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffString2Parameter(index, ptr); else return 0; }
    long EffGetNumProgramCategories(int nEffect)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffGetNumProgramCategories(); else return 0; }
    long EffGetProgramNameIndexed(int nEffect, long category, long index, char* text)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffGetProgramNameIndexed(category, index, text); else return 0; }
    long EffCopyProgram(int nEffect, long index)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffCopyProgram(index); else return 0; }
    long EffConnectInput(int nEffect, long index, bool state)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffConnectInput(index, state); else return 0; }
    long EffConnectOutput(int nEffect, long index, bool state)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffConnectOutput(index, state); else return 0; }
    long EffGetInputProperties(int nEffect, long index, VstPinProperties *ptr)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffGetInputProperties(index, ptr); else return 0; }
    long EffGetOutputProperties(int nEffect, long index, VstPinProperties *ptr)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffGetOutputProperties(index, ptr); else return 0; }
    long EffGetPlugCategory(int nEffect)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffGetPlugCategory(); else return 0; }
    long EffGetCurrentPosition(int nEffect)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffGetCurrentPosition(); else return 0; }
    long EffGetDestinationBuffer(int nEffect)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffGetDestinationBuffer(); else return 0; }
    long EffOfflineNotify(int nEffect, VstAudioFile* ptr, long numAudioFiles, bool start)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffOfflineNotify(ptr, numAudioFiles, start); else return 0; }
    long EffOfflinePrepare(int nEffect, VstOfflineTask *ptr, long count)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffOfflinePrepare(ptr, count); else return 0; }
    long EffOfflineRun(int nEffect, VstOfflineTask *ptr, long count)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffOfflineRun(ptr, count); else return 0; }
    long EffProcessVarIo(int nEffect, VstVariableIo* varIo)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffProcessVarIo(varIo); else return 0; }
    long EffSetSpeakerArrangement(int nEffect, VstSpeakerArrangement* pluginInput, VstSpeakerArrangement* pluginOutput)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffSetSpeakerArrangement(pluginInput, pluginOutput); else return 0; }
    long EffSetBlockSizeAndSampleRate(int nEffect, long blockSize, float sampleRate)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffSetBlockSizeAndSampleRate(blockSize, sampleRate); else return 0; }
    long EffSetBypass(int nEffect, bool onOff)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffSetBypass(onOff); else return 0; }
    long EffGetEffectName(int nEffect, char *ptr)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffGetEffectName(ptr); else return 0; }
    long EffGetErrorText(int nEffect, char *ptr)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffGetErrorText(ptr); else return 0; }
    long EffGetVendorString(int nEffect, char *ptr)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffGetVendorString(ptr); else return 0; }
    long EffGetProductString(int nEffect, char *ptr)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffGetProductString(ptr); else return 0; }
    long EffGetVendorVersion(int nEffect)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffGetVendorVersion(); else return 0; }
    long EffVendorSpecific(int nEffect, long index, long value, void *ptr, float opt)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffVendorSpecific(index, value, ptr, opt); else return 0; }
    long EffCanDo(int nEffect, const char *ptr)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffCanDo(ptr); else return 0; }
    long EffGetTailSize(int nEffect)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffGetTailSize(); else return 0; }
    long EffIdle(int nEffect)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffIdle(); else return 0; }
    long EffGetIcon(int nEffect)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffGetIcon(); else return 0; }
    long EffSetViewPosition(int nEffect, long x, long y)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffSetViewPosition(x, y); else return 0; }
    long EffGetParameterProperties(int nEffect, long index, VstParameterProperties* ptr)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffGetParameterProperties(index, ptr); else return 0; }
    long EffKeysRequired(int nEffect)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffKeysRequired(); else return 0; }
    long EffGetVstVersion(int nEffect)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffGetVstVersion(); else return 0; }
                                        /* VST 2.1 extensions                */
    long EffKeyDown(int nEffect, VstKeyCode &keyCode)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffKeyDown(keyCode); else return 0; }
    long EffKeyUp(int nEffect, VstKeyCode &keyCode)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffKeyUp(keyCode); else return 0; }
    void EffSetKnobMode(int nEffect, long value)
      { if (GetAt(nEffect)) GetAt(nEffect)->EffSetKnobMode(value); }
    long EffGetMidiProgramName(int nEffect, long channel, MidiProgramName* midiProgramName)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffGetMidiProgramName(channel, midiProgramName); else return 0; }
    long EffGetCurrentMidiProgram(int nEffect, long channel, MidiProgramName* currentProgram)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffGetCurrentMidiProgram(channel, currentProgram); else return 0; }
    long EffGetMidiProgramCategory(int nEffect, long channel, MidiProgramCategory* category)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffGetMidiProgramCategory(channel, category); else return 0; }
    long EffHasMidiProgramsChanged(int nEffect, long channel)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffHasMidiProgramsChanged(channel); else return 0; }
    long EffGetMidiKeyName(int nEffect, long channel, MidiKeyName* keyName)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffGetMidiKeyName(channel, keyName); else return 0; }
    long EffBeginSetProgram(int nEffect)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffBeginSetProgram(); else return 0; }
    long EffEndSetProgram(int nEffect)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffBeginSetProgram(); else return 0; }
                                        /* VST 2.3 Extensions                */
    long EffGetSpeakerArrangement(int nEffect, VstSpeakerArrangement** pluginInput, VstSpeakerArrangement** pluginOutput)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffGetSpeakerArrangement(pluginInput, pluginOutput); else return 0; }
    long EffSetTotalSampleToProcess(int nEffect, long value)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffSetTotalSampleToProcess(value); else return 0; }
    long EffGetNextShellPlugin(int nEffect, char *name)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffGetNextShellPlugin(name); else return 0; }
    long EffStartProcess(int nEffect)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffStartProcess(); else return 0; }
    long EffStopProcess(int nEffect)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffStopProcess(); else return 0; }
    long EffSetPanLaw(int nEffect, long type, float val)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffSetPanLaw(type, val); else return 0; }
    long EffBeginLoadBank(int nEffect, VstPatchChunkInfo* ptr)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffBeginLoadBank(ptr); else return 0; }
    long EffBeginLoadProgram(int nEffect, VstPatchChunkInfo* ptr)
      { if (GetAt(nEffect)) return GetAt(nEffect)->EffBeginLoadProgram(ptr); else return 0; }
                                        /* VST 2.4 Extensions                */
    long EffSetProcessPrecision(int nEffect, long precision)
      { CEffect *pEffect = GetAt(nEffect); if (pEffect) return pEffect->EffSetProcessPrecision(precision); else return 0; }
    long EffGetNumMidiInputChannels(int nEffect)
      { CEffect *pEffect = GetAt(nEffect); if (pEffect) return pEffect->EffGetNumMidiInputChannels(); else return 0; }
    long EffGetNumMidiOutputChannels(int nEffect)
      { CEffect *pEffect = GetAt(nEffect); if (pEffect) return pEffect->EffGetNumMidiOutputChannels(); else return 0; }

// overridable functions
public:
    virtual CEffect * CreateEffect() { return new CEffect(this); }
	virtual void SetSampleRate(float fSampleRate=44100.);
	virtual void SetBlockSize(long lSize=1024);
	virtual void Process(float **inputs, float **outputs, long sampleframes);
	virtual void ProcessReplacing(float **inputs, float **outputs, long sampleframes);
	virtual void ProcessDoubleReplacing(double **inputs, double **outputs, long sampleframes);

    virtual bool OnGetVendorString(char *text) { strcpy(text, "Seib"); return true; } // forgive this little vanity :-)
    virtual long OnGetHostVendorVersion() { return 1; }
    virtual bool OnGetProductString(char *text) { strcpy(text, "Default CVSTHost"); return true; }
#if 0
    // see text in CVSTHost.cpp (in CVSTHost::OnAudioMasterCallback(audioMasterGetOutputSpeakerArrangement)) on this!
    virtual bool OnGetOutputSpeakerArrangement(int nEffect, VstSpeakerArrangement* pluginInput, VstSpeakerArrangement* pluginOutput) { return false; }
#else
    virtual VstSpeakerArrangement *OnGetOutputSpeakerArrangement(int nEffect) { return 0; }
#endif
    virtual void OnSetOutputSampleRate(int nEffect, float sampleRate) { }
    virtual bool OnOfflineStart(int nEffect, VstAudioFile* audioFiles, long numAudioFiles, long numNewAudioFiles) { return false; }
    virtual bool OnOfflineRead(int nEffect, VstOfflineTask* offline, VstOfflineOption option, bool readSource) { return false; }
    virtual bool OnOfflineWrite(int nEffect, VstOfflineTask* offline, VstOfflineOption option) { return false; }
    virtual long OnOfflineGetCurrentPass(int nEffect) { return 0; }
    virtual long OnOfflineGetCurrentMetaPass(int nEffect) { return 0; }
    virtual long OnGetAutomationState(int nEffect) { return 0; }
    virtual long OnGetCurrentProcessLevel(int nEffect) { return 0; }
    virtual bool OnWillProcessReplacing(int nEffect) { return false; }
    virtual long OnGetOutputLatency(int nEffect) { return 0; }
    virtual long OnGetInputLatency(int nEffect) { return 0; }
	virtual long OnUpdateBlockSize(int nEffect);
    virtual long OnTempoAt(int nEffect, long pos) { return 0; }
	virtual long OnUpdateSampleRate(int nEffect);
	virtual bool OnSizeWindow(int nEffect, long width, long height);
	virtual bool OnNeedIdle(int nEffect);
	virtual long OnAudioMasterCallback(int nEffect, long opcode, long index, long value, void *ptr, float opt);
    virtual long OnGetVersion(int nEffect);
    virtual long OnGetCurrentUniqueId(int nEffect);
	virtual bool OnCanDo(const char *ptr);
	virtual bool OnWantEvents(int nEffect, long filter);
	virtual long OnIdle(int nEffect=-1);
    virtual bool OnInputConnected(int nEffect, long input) { return true; }
    virtual bool OnOutputConnected(int nEffect, long output) { return true; }
    virtual bool OnSetParameterAutomated(int nEffect, long index, float value) { return false; }
    virtual bool OnProcessEvents(int nEffect, VstEvents* events) { return false; }
    virtual VstTimeInfo *OnGetTime(int nEffect, long lMask) { return &vstTimeInfo; }
    virtual bool OnSetTime(int nEffect, long filter, VstTimeInfo *timeInfo) { return false; }
    virtual long OnGetNumAutomatableParameters(int nEffect) { CEffect *pEff = GetAt(nEffect); if (pEff) return pEff->OnGetNumAutomatableParameters(); else return 0; }
    virtual long OnGetParameterQuantization(int nEffect) { return 1; }
    virtual bool OnIoChanged(int nEffect) { CEffect *pEff = GetAt(nEffect); if (pEff) return pEff->OnIoChanged(); else return false; }
    virtual long OnHostVendorSpecific(int nEffect, long lArg1, long lArg2, void* ptrArg, float floatArg) { return 0; }
    virtual long OnGetHostLanguage() { return 0; }
    virtual void * OnOpenWindow(int nEffect, VstWindow* window);
    virtual bool OnCloseWindow(int nEffect, VstWindow* window);
    virtual void * OnGetDirectory(int nEffect);
    virtual bool OnUpdateDisplay(int nEffect);
    // VST 2.1 Extensions
    virtual bool OnBeginEdit(int nEffect) { return false; }
    virtual bool OnEndEdit(int nEffect) { return false; }
    virtual bool OnOpenFileSelector (int nEffect, VstFileSelect *ptr) { return false; }
    // VST 2.2 Extensions
    virtual bool OnCloseFileSelector (int nEffect, VstFileSelect *ptr) { return false; }
    virtual bool OnEditFile(int nEffect, char *ptr) { return false; }
    virtual bool OnGetChunkFile(int nEffect, void * nativePath) { return false; }
    // VST 2.3 Extensions
    virtual VstSpeakerArrangement *OnGetInputSpeakerArrangement(int nEffect) { return 0; }
};

#endif // !defined(VSTHOST_H__INCLUDED_)
