/*****************************************************************************/
/* CVSTHost.cpp: implementation of the CVSTHost class.                       */
/*****************************************************************************/

#ifdef WIN32

#include <windows.h>                    /* Windows header files              */

#elif MAC

#endif

#include <stdio.h>                      /* file I/O prototypes               */
#include <math.h>                       /* math prototypes                   */

#include "CVSTHost.h"                   /* private prototypes                */

/*****************************************************************************/
/* Global Data                                                               */
/*****************************************************************************/

const int MyVersion = 1;                /* highest known VST FX version      */

/*===========================================================================*/
/* CFxBase class members                                                     */
/*===========================================================================*/

static char szChnk[] = "CcnK";          /* set up swapping flag              */
static long lChnk = 'CcnK';
bool CFxBase::NeedsBSwap = !!memcmp(szChnk, &lChnk, 4);

/*****************************************************************************/
/* SwapBytes : swaps bytes for big/little-endian difference                  */
/*****************************************************************************/

void CFxBase::SwapBytes(long &l)
{
unsigned char *b = (unsigned char *)&l;
long intermediate =  ((long)b[0] << 24) |
                     ((long)b[1] << 16) |
                     ((long)b[2] << 8) |
                     (long)b[3];
l = intermediate;
}

void CFxBase::SwapBytes(float &f)
{
long *pl = (long *)&f;
SwapBytes(*pl);
}


/*===========================================================================*/
/* CFxBank class members                                                     */
/*===========================================================================*/

/*****************************************************************************/
/* CFxBank : constructor                                                     */
/*****************************************************************************/

CFxBank::CFxBank(char *pszFile)
{
Init();                                 /* initialize data areas             */
if (pszFile)                            /* if a file name has been passed    */
  LoadBank(pszFile);                    /* load the corresponding bank       */
}

CFxBank::CFxBank(int nPrograms, int nParams)
{
Init();                                 /* initialize data areas             */
SetSize(nPrograms, nParams);            /* set new size                      */
}

CFxBank::CFxBank(int nChunkSize)
{
Init();                                 /* initialize data areas             */
SetSize(nChunkSize);                    /* set new size                      */
}

/*****************************************************************************/
/* Init : initializes all data areas                                         */
/*****************************************************************************/

void CFxBank::Init()
{
bBank = NULL;                           /* no bank data loaded               */
Unload();                               /* reset all parameters              */
}

/*****************************************************************************/
/* ~CFxBank : destructor                                                     */
/*****************************************************************************/

CFxBank::~CFxBank()
{
Unload();                               /* unload all data                   */
}

/*****************************************************************************/
/* DoCopy : combined for copy constructor and assignment operator            */
/*****************************************************************************/

CFxBank & CFxBank::DoCopy(const CFxBank &org)
{
unsigned char *nBank = NULL;
if (org.nBankLen)
  {
  unsigned char *nBank = new unsigned char[org.nBankLen];
  if (!nBank)
    throw(int)1;
  memcpy(nBank, org.bBank, org.nBankLen);
  }
Unload();                               /* remove previous data              */
bBank = nBank;                          /* and copy in the other one's       */
bChunk = org.bChunk;
nBankLen = org.nBankLen;
strcpy(szFileName, org.szFileName);
return *this;
}

/*****************************************************************************/
/* SetSize : sets new size                                                   */
/*****************************************************************************/

bool CFxBank::SetSize(int nPrograms, int nParams)
{
int nTotLen = sizeof(fxSet) - sizeof(fxProgram);
int nProgLen = sizeof(fxProgram) + (nParams - 1) * sizeof(float);
nTotLen += nPrograms * nProgLen;
unsigned char *nBank = new unsigned char[nTotLen];
if (!nBank)
  return false;

Unload();
bBank = nBank;
nBankLen = nTotLen;
bChunk = false;

memset(nBank, 0, nTotLen);              /* initialize new bank               */
fxSet *pSet = (fxSet *)bBank;
pSet->chunkMagic = cMagic;
pSet->byteSize = 0;
pSet->fxMagic = bankMagic;
pSet->version = MyVersion;
pSet->numPrograms = nPrograms;

unsigned char *bProg = (unsigned char *)pSet->programs;
for (int i = 0; i < nPrograms; i++)
  {
  fxProgram * pProg = (fxProgram *)(bProg + i * nProgLen);
  pProg->chunkMagic = cMagic;
  pProg->byteSize = 0;
  pProg->fxMagic = fMagic;
  pProg->version = 1;
  pProg->numParams = nParams;
  for (int j = 0; j < nParams; j++)
    pProg->params[j] = 0.0;
  }
return true;
}

bool CFxBank::SetSize(int nChunkSize)
{
int nTotLen = sizeof(fxChunkSet) + nChunkSize - 8;
unsigned char *nBank = new unsigned char[nTotLen];
if (!nBank)
  return false;

Unload();
bBank = nBank;
nBankLen = nTotLen;
bChunk = true;

memset(nBank, 0, nTotLen);              /* initialize new bank               */
fxChunkSet *pSet = (fxChunkSet *)bBank;
pSet->chunkMagic = cMagic;
pSet->byteSize = 0;
pSet->fxMagic = chunkBankMagic;
pSet->version = MyVersion;
pSet->numPrograms = 1;
pSet->chunkSize = nChunkSize;

return true;
}

/*****************************************************************************/
/* LoadBank : loads a bank file                                              */
/*****************************************************************************/

bool CFxBank::LoadBank(char *pszFile)
{
FILE *fp = fopen(pszFile, "rb");        /* try to open the file              */
if (!fp)                                /* upon error                        */
  return false;                         /* return an error                   */
bool brc = true;                        /* default to OK                     */
unsigned char *nBank = NULL;
try
  {
  fseek(fp, 0, SEEK_END);               /* get file size                     */
  size_t tLen = (size_t)ftell(fp);
  rewind(fp);

  nBank = new unsigned char[tLen];      /* allocate storage                  */
  if (!nBank)
    throw (int)1;
                                        /* read chunk set to determine cnt.  */
  if (fread(nBank, 1, tLen, fp) != tLen)
    throw (int)1;
  fxSet *pSet = (fxSet *)nBank;         /* position on set                   */
  if (NeedsBSwap)                       /* eventually swap necessary bytes   */
    {
    SwapBytes(pSet->chunkMagic);
    SwapBytes(pSet->byteSize);
    SwapBytes(pSet->fxMagic);
    SwapBytes(pSet->version);
    SwapBytes(pSet->fxID);
    SwapBytes(pSet->fxVersion);
    SwapBytes(pSet->numPrograms);
    }
  if ((pSet->chunkMagic != cMagic) ||   /* if erroneous data in there        */
      (pSet->version > MyVersion) ||
      ((pSet->fxMagic != bankMagic) &&
       (pSet->fxMagic != chunkBankMagic)))
    throw (int)1;                       /* get out                           */

  if (pSet->fxMagic == bankMagic)
    {
    fxProgram * pProg = pSet->programs; /* position on 1st program           */
    int nProg = 0;
    while (nProg < pSet->numPrograms)   /* walk program list                 */
      {
      if (NeedsBSwap)                   /* eventually swap necessary bytes   */
        {
        SwapBytes(pProg->chunkMagic);
        SwapBytes(pProg->byteSize);
        SwapBytes(pProg->fxMagic);
        SwapBytes(pProg->version);
        SwapBytes(pProg->fxID);
        SwapBytes(pProg->fxVersion);
        SwapBytes(pProg->numParams);
        }
                                        /* if erroneous data                 */
      if ((pProg->chunkMagic != cMagic)|| 
          (pProg->fxMagic != fMagic))
        throw (int)1;                   /* get out                           */
      if (NeedsBSwap)                   /* if necessary                      */
        {                               /* swap all parameter bytes          */
        int j;
        for (j = 0; j < pProg->numParams; j++)
          SwapBytes(pProg->params[j]);
        }
      unsigned char *pNext = (unsigned char *)(pProg + 1);
      pNext += (sizeof(float) * (pProg->numParams - 1));
      if (pNext > nBank + tLen)         /* VERY simple fuse                  */
        throw (int)1;
      
      pProg = (fxProgram *)pNext;
      nProg++;
      }
    }
                                        /* if it's a chunk file              */
  else if (pSet->fxMagic == chunkBankMagic)
    {
    fxChunkSet * pCSet = (fxChunkSet *)nBank;
    if (NeedsBSwap)                     /* eventually swap necessary bytes   */
      {
      SwapBytes(pCSet->chunkSize);
                                        /* size check - must not be too large*/
      if (pCSet->chunkSize + sizeof(*pCSet) - 8 > tLen)
        throw (int)1;
      }
    }

  Unload();                             /* otherwise remove eventual old data*/
  bBank = nBank;                        /* and put in new data               */
  nBankLen = (int)tLen;
  bChunk = (pSet->fxMagic == chunkBankMagic);
  }
catch(...)
  {
  brc = false;                          /* if any error occured, say NOPE    */
  if (nBank)                            /* and remove loaded data            */
    delete[] nBank;
  }

fclose(fp);                             /* close the file                    */
return brc;                             /* and return                        */
}

/*****************************************************************************/
/* SaveBank : save bank to file                                              */
/*****************************************************************************/

bool CFxBank::SaveBank(char *pszFile)
{
if (!IsLoaded())
  return false;
                                        /* create internal copy for mod      */
unsigned char *nBank = new unsigned char[nBankLen];
if (!nBank)                             /* if impossible                     */
  return false;
memcpy(nBank, bBank, nBankLen);

fxSet *pSet = (fxSet *)nBank;           /* position on set                   */
int numPrograms = pSet->numPrograms;
if (NeedsBSwap)                         /* if byte-swapping needed           */
  {
  SwapBytes(pSet->chunkMagic);
  SwapBytes(pSet->byteSize);
  SwapBytes(pSet->fxMagic);
  SwapBytes(pSet->version);
  SwapBytes(pSet->fxID);
  SwapBytes(pSet->fxVersion);
  SwapBytes(pSet->numPrograms);
  }
if (bChunk)
  {
  fxChunkSet *pCSet = (fxChunkSet *)nBank;
  if (NeedsBSwap)                       /* if byte-swapping needed           */
    SwapBytes(pCSet->chunkSize);
  }
else
  {
  fxProgram * pProg = pSet->programs;   /* position on 1st program           */
  int numParams = pProg->numParams;
  int nProg = 0;
  while (nProg < numPrograms)           /* walk program list                 */
    {
    if (NeedsBSwap)                     /* eventually swap all necessary     */
      {
      SwapBytes(pProg->chunkMagic);
      SwapBytes(pProg->byteSize);
      SwapBytes(pProg->fxMagic);
      SwapBytes(pProg->version);
      SwapBytes(pProg->fxID);
      SwapBytes(pProg->fxVersion);
      SwapBytes(pProg->numParams);
      for (int j = 0; j < numParams; j++)
        SwapBytes(pProg->params[j]);
      }
    unsigned char *pNext = (unsigned char *)(pProg + 1);
    pNext += (sizeof(float) * (numParams - 1));
    if (pNext > nBank + nBankLen)       /* VERY simple fuse                  */
      break;
    
    pProg = (fxProgram *)pNext;
    nProg++;
    }
  }

bool brc = true;                        /* default to OK                     */
FILE *fp = NULL;
try
  {
  fp = fopen(pszFile, "wb");            /* try to open the file              */
  if (!fp)                              /* upon error                        */
    throw (int)1;                       /* return an error                   */
  if (fwrite(nBank, 1, nBankLen, fp) != (size_t)nBankLen)
    throw (int)1;
  }
catch(...)
  {
  brc = false;
  }
if (fp)
  fclose(fp);
delete[] nBank;

return brc;
}

/*****************************************************************************/
/* Unload : removes a loaded bank from memory                                */
/*****************************************************************************/

void CFxBank::Unload()
{
if (bBank)
  delete[] bBank;
*szFileName = '\0';                     /* reset file name                   */
bBank = NULL;                           /* reset bank pointer                */
nBankLen = 0;                           /* reset bank length                 */
bChunk = false;                         /* and of course it's no chunk.      */
}

/*****************************************************************************/
/* GetProgram : returns pointer to one of the loaded programs                */
/*****************************************************************************/

fxProgram * CFxBank::GetProgram(int nProgNum)
{
if ((!IsLoaded()) || (bChunk))          /* if nothing loaded or chunk file   */
  return NULL;                          /* return OUCH                       */

fxSet *pSet = (fxSet *)bBank;           /* position on set                   */
fxProgram * pProg = pSet->programs;     /* position on 1st program           */
#if 1
int nProgLen = sizeof(fxProgram) + (pProg->numParams - 1) * sizeof(float);
unsigned char *pThatProg = ((unsigned char *)pProg) + (nProgNum * nProgLen);
pProg = (fxProgram *)pThatProg;
#else
/*---------------------------------------------------------------------------*/
/* presumably, the following logic is overkill; if all programs have the     */
/* same number of parameters, a simple multiplication would do.              */
/* But that's not stated anywhere in the VST SDK...                          */
/*---------------------------------------------------------------------------*/
int i;
for (i = 0; i < nProgNum; i++)
  {
  unsigned char *pNext = (unsigned char *)(pProg + 1);
  pNext += (sizeof(float) * (pProg->numParams - 1));
  if (pNext > bBank + nBankLen)         /* VERY simple fuse                  */
    return NULL;

  pProg = (fxProgram *)pNext;
  }
#endif
return pProg;
}

/*===========================================================================*/
/* CEffect class members                                                     */
/*===========================================================================*/

/*****************************************************************************/
/* CEffect : constructor                                                     */
/*****************************************************************************/

CEffect::CEffect(CVSTHost *pHost)
{
this->pHost = pHost;
pEffect = NULL;
sName = NULL;
bEditOpen = false;
bNeedIdle = false;
bWantMidi = false;

#ifdef WIN32

hModule = NULL;
sDir = NULL;

#elif MAC

// yet to be done

#endif
}

/*****************************************************************************/
/* ~CEffect : destructor                                                     */
/*****************************************************************************/

CEffect::~CEffect()
{
Unload();

#ifdef WIN32

#elif MAC

#endif
}

/*****************************************************************************/
/* Load : loads the effect module                                            */
/*****************************************************************************/

bool CEffect::Load(const char *name)
{
if (!pHost)                             /* if VST Host undefined             */
  return false;                         /* return without action             */

#ifdef WIN32

#elif MAC 

#endif

Unload();                               /* make sure nothing else is loaded  */
                                        /* pointer to main function          */
AEffect *(*pMain)(long (*audioMaster)(AEffect *effect,
                                  long opcode,
                                  long index,
                                  long value,
                                  void *ptr,
                                  float opt)) = 0;

#ifdef WIN32

hModule = ::LoadLibrary(name);          /* try to load the DLL               */
if (hModule)                            /* if there, get its main() function */
  pMain = (AEffect * (*)(long (*)(AEffect *,long,long,long,void *,float)))
          ::GetProcAddress(hModule, "main");

#elif MAC

// yet to be done

#endif

if (pMain)                              /* initialize effect                 */
  pEffect = pMain(pHost->AudioMasterCallback);
                                        /* check for correctness             */
if (pEffect && (pEffect->magic != kEffectMagic))
  pEffect = NULL;

if (pEffect)
  {
  sName = new char[strlen(name) + 1];
  if (sName)
    strcpy(sName, name);
#ifdef WIN32
  char *p = strrchr(name, '\\');
  if (p)
    {
    sDir = new char[p - name + 1];
    if (sDir)
      {
      memcpy(sDir, name, p - name);
      sDir[p - name] = '\0';
      }
    }
#elif MAC

// yet to be done
#endif

  }

#ifdef WIN32

#elif MAC 

#endif

return !!pEffect;
}

/*****************************************************************************/
/* Unload : unloads effect module                                            */
/*****************************************************************************/

bool CEffect::Unload()
{
if (!pHost)                             /* if no VST Host there              */
  return false;

EffClose();                             /* make sure it's closed             */
pEffect = NULL;                         /* and reset the pointer             */

#ifdef WIN32

if (hModule)                            /* if DLL instance available         */
  {
  ::FreeLibrary(hModule);               /* remove it.                        */
  hModule = NULL;                       /* and reset the handle              */
  }

if (sDir)                               /* reset module directory            */
  {
  delete[] sDir;
  sDir = NULL;
  }

#elif MAC

// yet to be done!

#endif

if (sName)                              /* reset module name                 */
  {
  delete[] sName;
  sName = NULL;
  }

return true;
}

/*****************************************************************************/
/* LoadBank : loads a .fxb file ... IF it's for this effect                  */
/*****************************************************************************/

bool CEffect::LoadBank(char *name)
{
try
  {
  CFxBank fx(name);                     /* load the bank                     */
  if (!fx.IsLoaded())                   /* if error loading                  */
    throw (int)1;
  }
catch(...)                              /* if any error occured              */
  {
  return false;                         /* return NOT!!!                     */
  }
  
return true;                            /* pass back OK                      */
}

/*****************************************************************************/
/* SaveBank : saves current sound bank to a .fxb file                        */
/*****************************************************************************/

bool CEffect::SaveBank(char *name)
{
return false;                           /* return error for now              */
}

/*****************************************************************************/
/* OnGetDirectory : returns the plug's directory (char* on pc, FSSpec on mac)*/
/*****************************************************************************/

void * CEffect::OnGetDirectory()
{
#ifdef WIN32

return sDir;

#elif MAC

// yet to be done

#endif
}

/*****************************************************************************/
/* EffDispatch : calls an effect's dispatcher                                */
/*****************************************************************************/

long CEffect::EffDispatch
    (
    long opCode,
    long index,
    long value,
    void *ptr,
    float opt
    )
{
if (!pEffect)
  return 0;

return pEffect->dispatcher(pEffect, opCode, index, value, ptr, opt);
}

/*****************************************************************************/
/* EffProcess : calles an effect's process() function                        */
/*****************************************************************************/

void CEffect::EffProcess(float **inputs, float **outputs, long sampleframes)
{
if (!pEffect)
  return;

pEffect->process(pEffect, inputs, outputs, sampleframes);
}

/*****************************************************************************/
/* EffProcessReplacing : calls an effect's processReplacing() function       */
/*****************************************************************************/

void CEffect::EffProcessReplacing(float **inputs, float **outputs, long sampleframes)
{
if ((!pEffect) ||
    (!(pEffect->flags & effFlagsCanReplacing)))
  return;

pEffect->processReplacing(pEffect, inputs, outputs, sampleframes);

}

/*****************************************************************************/
/* EffSetParameter : calls an effect's setParameter() function               */
/*****************************************************************************/

void CEffect::EffSetParameter(long index, float parameter)
{
if (!pEffect)
  return;

pEffect->setParameter(pEffect, index, parameter);
}

/*****************************************************************************/
/* EffGetParameter : calls an effect's getParameter() function               */
/*****************************************************************************/

float CEffect::EffGetParameter(long index)
{
if (!pEffect)
  return 0.;

float frc = pEffect->getParameter(pEffect, index);

return frc;
}

/*===========================================================================*/
/* CVSTHost class members                                                    */
/*===========================================================================*/

CVSTHost * CVSTHost::pHost = NULL;      /* pointer to the one and only host  */

/*****************************************************************************/
/* CVSTHost : constructor                                                    */
/*****************************************************************************/

CVSTHost::CVSTHost()
{
if (pHost)                              /* disallow more than one host!      */
  throw((int)1);

fSampleRate = 44100.;                   /* initialize to sensible values     */
lBlockSize = 1024;
vstTimeInfo.samplePos = 0.0;
vstTimeInfo.sampleRate = fSampleRate;
vstTimeInfo.nanoSeconds = 0.0;
vstTimeInfo.ppqPos = 0.0;
vstTimeInfo.tempo = 120.0;
vstTimeInfo.barStartPos = 0.0;
vstTimeInfo.cycleStartPos = 0.0;
vstTimeInfo.cycleEndPos = 0.0;
vstTimeInfo.timeSigNumerator = 4;
vstTimeInfo.timeSigDenominator = 4;
vstTimeInfo.smpteOffset = 0;
vstTimeInfo.smpteFrameRate = 1;
vstTimeInfo.samplesToNextClock = 0;
vstTimeInfo.flags = 0;

aEffects = 0;                           /* no effects loaded                 */
naEffects = nmaEffects = 0;

pHost = this;                           /* install this instance as the one  */
}

/*****************************************************************************/
/* ~CVSTHost : destructor                                                    */
/*****************************************************************************/

CVSTHost::~CVSTHost()
{
RemoveAll();                            /* remove all loaded effects         */

if (aEffects)                           /* if effects pointer array there    */
  delete[] aEffects;                    /* delete it                         */

if (pHost == this)                      /* if we're the chosen one           */
  pHost = NULL;                         /* remove ourselves from pointer     */
}

/*****************************************************************************/
/* CalcTimeInfo : calculates time info from nanoSeconds                      */
/*****************************************************************************/

void CVSTHost::CalcTimeInfo(long lMask)
{
// we don't care for the mask in here
static double fSmpteDiv[] =
  {
  24.f,
  25.f,
  24.f,
  30.f,
  29.97f,
  30.f
  };

double dPos = vstTimeInfo.samplePos / vstTimeInfo.sampleRate;
vstTimeInfo.ppqPos = dPos * vstTimeInfo.tempo / 60.L;
                                        /* offset in fractions of a second   */
double dOffsetInSecond = dPos - floor(dPos);
vstTimeInfo.smpteOffset = (long)(dOffsetInSecond *
                                 fSmpteDiv[vstTimeInfo.smpteFrameRate] *
                                 80.L);
}

/*****************************************************************************/
/* AudioMasterCallback : callback to be called by plugins                    */
/*****************************************************************************/

long VSTCALLBACK CVSTHost::AudioMasterCallback
    (
    AEffect *effect,
    long opcode,
    long index,
    long value,
    void *ptr,
    float opt
    )
{
if (!pHost)
  return 0;
return pHost->OnAudioMasterCallback(pHost->Search(effect),
                                    opcode,
                                    index,
                                    value,
                                    ptr,
                                    opt);
}

/*****************************************************************************/
/* OnAudioMasterCallback : callback dispatcher                               */
/*****************************************************************************/

long CVSTHost::OnAudioMasterCallback
    (
    int nEffect,
    long opcode,
    long index,
    long value,
    void *ptr,
    float opt
    )
{
switch (opcode)
  {
  case audioMasterAutomate :
    return OnSetParameterAutomated(nEffect, index, opt);
  case audioMasterVersion :
    return OnGetVersion(nEffect);
  case audioMasterCurrentId :
    return nEffect;
  case audioMasterIdle :
    return OnIdle(nEffect);
  case audioMasterPinConnected :
    return !((value) ? 
        OnOutputConnected(nEffect, index) :
        OnInputConnected(nEffect, index));
                                        /* VST 2.0 additions...              */
  case audioMasterWantMidi :
    return OnWantEvents(nEffect, value);
  case audioMasterGetTime :
    return (long)OnGetTime(nEffect, value);
  case audioMasterProcessEvents :
    return OnProcessEvents(nEffect, (VstEvents *)ptr);
  case audioMasterSetTime :
    return OnSetTime(nEffect, value, (VstTimeInfo *)ptr);
  case audioMasterTempoAt :
    return OnTempoAt(nEffect, value);
  case audioMasterGetNumAutomatableParameters :
    return OnGetNumAutomatableParameters(nEffect);
  case audioMasterGetParameterQuantization :
    return OnGetParameterQuantization(nEffect);
  case audioMasterIOChanged :
    return OnIoChanged(nEffect);
  case audioMasterNeedIdle :
    return OnNeedIdle(nEffect);
  case audioMasterSizeWindow :
    return OnSizeWindow(nEffect, index, value);
  case audioMasterGetSampleRate :
    OnUpdateSampleRate(nEffect);
    return 1;
  case audioMasterGetBlockSize :
    OnUpdateBlockSize(nEffect);
    return 1;
  case audioMasterGetInputLatency :
    return OnGetInputLatency(nEffect);
  case audioMasterGetOutputLatency :
    return OnGetOutputLatency(nEffect);
  case audioMasterGetPreviousPlug :
    return GetPreviousPlugIn(nEffect);
  case audioMasterGetNextPlug :
    return GetNextPlugIn(nEffect);
  case audioMasterWillReplaceOrAccumulate :
    return OnWillProcessReplacing(nEffect);
  case audioMasterGetCurrentProcessLevel :
    return OnGetCurrentProcessLevel(nEffect);
  case audioMasterGetAutomationState :
    return OnGetAutomationState(nEffect);
  case audioMasterOfflineStart :
    return OnOfflineStart(nEffect,
                          (VstAudioFile *)ptr,
                          value,
                          index);
    break;
  case audioMasterOfflineRead :
    return OnOfflineRead(nEffect,
                         (VstOfflineTask *)ptr,
                         (VstOfflineOption)value,
                         !!index);
    break;
  case audioMasterOfflineWrite :
    return OnOfflineWrite(nEffect,
                          (VstOfflineTask *)ptr,
                          (VstOfflineOption)value);
    break;
  case audioMasterOfflineGetCurrentPass :
    return OnOfflineGetCurrentPass(nEffect);
  case audioMasterOfflineGetCurrentMetaPass :
    return OnOfflineGetCurrentMetaPass(nEffect);
  case audioMasterSetOutputSampleRate :
    OnSetOutputSampleRate(nEffect, opt);
    return 1;
  case audioMasterGetSpeakerArrangement :
    return OnGetSpeakerArrangement(nEffect, 
                                   (VstSpeakerArrangement *)value,
                                   (VstSpeakerArrangement *)ptr);
  case audioMasterGetVendorString :
    return OnGetVendorString((char *)ptr);
  case audioMasterGetProductString :
    return OnGetProductString((char *)ptr);
  case audioMasterGetVendorVersion :
    return OnGetHostVendorVersion();
  case audioMasterVendorSpecific :
    return OnHostVendorSpecific(nEffect, index, value, ptr, opt);
  case audioMasterSetIcon :
    // undefined in VST 2.0 specification
    break;
  case audioMasterCanDo :
    return OnCanDo((const char *)ptr);
  case audioMasterGetLanguage :
    return OnGetHostLanguage();
  case audioMasterOpenWindow :
    return (long)OnOpenWindow(nEffect, (VstWindow *)ptr);
  case audioMasterCloseWindow :
    return OnCloseWindow(nEffect, (VstWindow *)ptr);
  case audioMasterGetDirectory :
    return (long)OnGetDirectory(nEffect);
  case audioMasterUpdateDisplay :
    return OnUpdateDisplay(nEffect);
                                        /* VST 2.1 additions...              */
#ifdef VST_2_1_EXTENSIONS
  case audioMasterBeginEdit :
    return OnBeginEdit(nEffect);
  case audioMasterEndEdit :
    return OnEndEdit(nEffect);
  case audioMasterOpenFileSelector :
    return OnOpenFileSelector(nEffect, (VstFileSelect *)ptr);
#endif
                                        /* VST 2.2 additions...              */
#ifdef VST_2_2_EXTENSIONS
  case audioMasterCloseFileSelector :
    return OnCloseFileSelector(nEffect, (VstFileSelect *)ptr);
  case audioMasterEditFile :
    return OnEditFile(nEffect, (char *)ptr);
  case audioMasterGetChunkFile :
    return OnGetChunkFile(nEffect, ptr);
#endif

#ifdef VST_2_3_EXTENSIONS
  case audioMasterGetInputSpeakerArrangement :
    return (long)OnGetInputSpeakerArrangement(nEffect);
#endif

  }
return 0L;
}

/*****************************************************************************/
/* EffDispatch : calls an effect's dispatcher                                */
/*****************************************************************************/

long CVSTHost::EffDispatch
    (
    int nEffect,
    long opCode,
    long index,
    long value,
    void *ptr,
    float opt
    )
{
CEffect *pEffect = GetAt(nEffect);
if (!pEffect)
  return 0;
return pEffect->EffDispatch(opCode, index, value, ptr, opt);
}

/*****************************************************************************/
/* EffProcess : calles an effect's process() function                        */
/*****************************************************************************/

void CVSTHost::EffProcess(int nEffect, float **inputs, float **outputs, long sampleframes)
{
CEffect *pEffect = GetAt(nEffect);
if (!pEffect)
  return;
pEffect->EffProcess(inputs, outputs, sampleframes);
}

/*****************************************************************************/
/* EffProcessReplacing : calls an effect's processReplacing() function       */
/*****************************************************************************/

void CVSTHost::EffProcessReplacing(int nEffect, float **inputs, float **outputs, long sampleframes)
{
CEffect *pEffect = GetAt(nEffect);
if (!pEffect)
  return;
pEffect->EffProcessReplacing(inputs, outputs, sampleframes);
}

/*****************************************************************************/
/* EffSetParameter : calls an effect's setParameter() function               */
/*****************************************************************************/

void CVSTHost::EffSetParameter(int nEffect, long index, float parameter)
{
CEffect *pEffect = GetAt(nEffect);
if (!pEffect)
  return;
pEffect->EffSetParameter(index, parameter);
}

/*****************************************************************************/
/* EffGetParameter : calls an effect's getParameter() function               */
/*****************************************************************************/

float CVSTHost::EffGetParameter(int nEffect, long index)
{
CEffect *pEffect = GetAt(nEffect);
if (!pEffect)
  return 0.;
return pEffect->EffGetParameter(index);
}

/*****************************************************************************/
/* Search : finds a passed effect in the internal list                       */
/*****************************************************************************/

int CVSTHost::Search(AEffect *pEffect)
{
for (int i = GetSize() - 1;
     i >= 0;
     i--)
  {
  CEffect *paEffect = GetAt(i);
  if (!paEffect)
    continue;
  if (paEffect->pEffect == pEffect)
    return i;
  }
return -1;
}

/*****************************************************************************/
/* OnIdle : idle processing                                                  */
/*****************************************************************************/

long CVSTHost::OnIdle
    (
    int nEffect                         /* effect ID that triggered this     */
    )
{
int j = GetSize();
for (int i = 0; i < j; i++)
  EffEditIdle(i);
return 0;
}

/*****************************************************************************/
/* SetSampleRate : sets sample rate                                          */
/*****************************************************************************/

void CVSTHost::SetSampleRate(float fSampleRate)
{
if (fSampleRate == this->fSampleRate)   /* if no change                      */
  return;                               /* do nothing.                       */
this->fSampleRate = fSampleRate;        /* remember new sample rate          */
vstTimeInfo.sampleRate = fSampleRate;
int j = GetSize();                      /* inform all loaded plugins         */
for (int i = 0; i < j; i++)
  EffSetSampleRate(i, fSampleRate);
}

/*****************************************************************************/
/* SetBlockSize : sets the block size                                        */
/*****************************************************************************/

void CVSTHost::SetBlockSize(long lSize)
{
if (lSize == lBlockSize)                /* if no change                      */
  return;                               /* do nothing.                       */
lBlockSize = lSize;                     /* remember new block size           */
int j = GetSize();                      /* inform all loaded plugins         */
for (int i = 0; i < j; i++)
  {
  EffSetBlockSize(i, lBlockSize);       /* set new buffer size               */
  EffMainsChanged(i, true);             /* then force resume.                */
  }
}

/*****************************************************************************/
/* LoadPlugin : loads and initializes a plugin                               */
/*****************************************************************************/

int CVSTHost::LoadPlugin(const char * sName)
{
CEffect *pEffect = CreateEffect();      /* allocate an effect                */
if (!pEffect->Load(sName))              /* try to load the thing             */
  {
  delete pEffect;                       /* upon error delete the object      */
  return -1;                            /* and regretfully return error      */
  }

int nIndex;                             /* look for empty slot in array      */
for (nIndex = GetSize() - 1; nIndex >= 0; nIndex--)
  if (!GetAt(nIndex))
    break;
if (nIndex < 0)                         /* if no empty slot available        */
  {
  if (naEffects + 1 > nmaEffects)       /* if no more space in effects array */
    {                                   /* re-allocate effects array         */
    void **anew = new void *[nmaEffects + 5];
    if (anew)
      {
      for (int i = 0; i < naEffects; i++)
        anew[i] = aEffects[i];
      if (aEffects)
        delete[] aEffects;
      aEffects = anew;
      nmaEffects += 5;
      }
    else
      {
      delete pEffect;                   /* upon error delete the object      */
      return -1;                        /* and return with error             */
      }
    }
  nIndex = naEffects++;                 /* remember position                 */
  aEffects[nIndex] = pEffect;           /* add to effect array               */
  }
else                                    /* otherwise                         */
  aEffects[nIndex] = pEffect;           /* put into free slot                */

pEffect->EffOpen();                     /* open the effect                   */
pEffect->EffSetSampleRate(fSampleRate); /* adjust its sample rate            */
// this is a safety measure against some plugins that only set their buffers
// ONCE - this should ensure that they allocate a buffer that's large enough
pEffect->EffSetBlockSize(11025);
pEffect->EffMainsChanged(true);         /* then force resume.                */
pEffect->EffSetBlockSize(lBlockSize);   /* and block size                    */
pEffect->EffMainsChanged(true);         /* then force resume.                */
return nIndex;                          /* return new effect's index         */
}

/*****************************************************************************/
/* RemoveAt : removes one of the effects from memory                         */
/*****************************************************************************/

void CVSTHost::RemoveAt(int nIndex)
{
CEffect *pEffect;

if ((nIndex >= GetSize()) ||
    (!(pEffect = GetAt(nIndex))))
  return;

aEffects[nIndex] = 0;
pEffect->Unload();
delete pEffect;
}

/*****************************************************************************/
/* RemoveAll : remove all of the loaded effects from memory                  */
/*****************************************************************************/

void CVSTHost::RemoveAll()
{
for (int i = GetSize() - 1;             /* delete all loaded effects         */
     i >= 0;
     i--)
  RemoveAt(i);
naEffects = 0;
}

/*****************************************************************************/
/* Process : sends a block through all effects                               */
/*****************************************************************************/
/* NB: this is a horribly inadequate method, doesn't allow routing etc.      */
/*     just a convenient little way of providing data to the effects...      */
/*****************************************************************************/

void CVSTHost::Process(float **inputs, float **outputs, long sampleframes)
{
int j = GetSize();                      /* pass it on to all loaded effects  */
for (int i = 0; i < j; i++)
  EffProcess(i, inputs, outputs, sampleframes);
}

/*****************************************************************************/
/* ProcessReplacing : calls an effect's processReplacing() function          */
/*****************************************************************************/
/* NB: this is a horribly inadequate method, doesn't allow routing etc.      */
/*     just a convenient little way of providing data to the effects...      */
/*****************************************************************************/

void CVSTHost::ProcessReplacing(float **inputs, float **outputs, long sampleframes)
{
int j = GetSize();                      /* pass it on to all loaded effects  */
for (int i = 0; i < j; i++)
  EffProcessReplacing(i, inputs, outputs, sampleframes);
}

/*****************************************************************************/
/* OnCanDo : returns whether the host can do a specific action               */
/*****************************************************************************/

bool CVSTHost::OnCanDo(const char *ptr)
{
if ((!strcmp(ptr, "sendVstMidiEvent")) ||
    (!strcmp(ptr, "receiveVstMidiEvent")) ||
    (!strcmp(ptr, "sizeWindow")) )
  return true;
return false;                           /* per default, no.                  */
}

/*****************************************************************************/
/* GetPreviousPlugIn : returns predecessor to this plugin                    */
/*****************************************************************************/

int CVSTHost::GetPreviousPlugIn(int nEffect)
{
if (nEffect = -1)
  return GetNextPlugIn(nEffect);

int i = nEffect - 1;
if (i >= GetSize())
  i = GetSize() - 1;
for (; i >= 0; i++)
  if (GetAt(i))
    return i;
return -1;
}

/*****************************************************************************/
/* GetNextPlugIn : returns successor to this plugin                          */
/*****************************************************************************/

int CVSTHost::GetNextPlugIn(int nEffect)
{
for (int i = nEffect + 1; i < GetSize(); i++)
  if (GetAt(i))
    return i;
return -1;
}

/*****************************************************************************/
/* OnWantEvents : called when the effect calls wantEvents()                  */
/*****************************************************************************/

bool CVSTHost::OnWantEvents(int nEffect, long filter)
{
CEffect *pEffect = GetAt(nEffect);
if (pEffect)
  {
  pEffect->bWantMidi = true;
  // filter is ignored here. Bad, bad boy :-)
  return true;
  }
return false;
}

/*****************************************************************************/
/* OnNeedIdle : called when the effect calls needIdle()                      */
/*****************************************************************************/

bool CVSTHost::OnNeedIdle(int nEffect)
{
CEffect *pEffect = GetAt(nEffect);
if (pEffect)
  {
  pEffect->bNeedIdle = true;
  return true;
  }
return false;
}

/*****************************************************************************/
/* OnOpenWindow : called to open a new window                                */
/*****************************************************************************/

void * CVSTHost::OnOpenWindow(int nEffect, VstWindow* window)
{
CEffect *pEffect = GetAt(nEffect);
if (pEffect)
  return pEffect->OnOpenWindow(window);
return 0;
}

/*****************************************************************************/
/* OnCloseWindow : called to close a window                                  */
/*****************************************************************************/

bool CVSTHost::OnCloseWindow(int nEffect, VstWindow* window)
{
CEffect *pEffect = GetAt(nEffect);
if (pEffect)
  return pEffect->OnCloseWindow(window);
return 0;
}

/*****************************************************************************/
/* OnSizeWindow : called when the effect calls sizeWindow()                  */
/*****************************************************************************/

bool CVSTHost::OnSizeWindow(int nEffect, long width, long height)
{
CEffect *pEffect = GetAt(nEffect);
if (pEffect)
  {
  pEffect->OnSizeEditorWindow(width, height);
  return true;
  }
return false;
}

/*****************************************************************************/
/* OnUpdateSampleRate : called when the effect calls updateSampleRate()      */
/*****************************************************************************/

long CVSTHost::OnUpdateSampleRate(int nEffect)
{
CEffect *pEffect = GetAt(nEffect);
if (pEffect)
  pEffect->EffSetSampleRate(fSampleRate);
return (long)fSampleRate;
}

/*****************************************************************************/
/* OnUpdateBlockSize : called when the effect calls updateBlockSize()        */
/*****************************************************************************/

long CVSTHost::OnUpdateBlockSize(int nEffect)
{
CEffect *pEffect = GetAt(nEffect);
if (pEffect)
  pEffect->EffSetBlockSize(lBlockSize);
return lBlockSize;
}

/*****************************************************************************/
/* OnGetDirectory : called when the effect calls getDirectory()              */
/*****************************************************************************/

void * CVSTHost::OnGetDirectory(int nEffect)
{
CEffect *pEffect = GetAt(nEffect);
if (pEffect)
  return pEffect->OnGetDirectory();
return 0;
}

/*****************************************************************************/
/* OnUpdateDisplay : called when effect calls updateDisplay()                */
/*****************************************************************************/

bool CVSTHost::OnUpdateDisplay(int nEffect)
{
CEffect *pEffect = GetAt(nEffect);
if (pEffect)
  return pEffect->OnUpdateDisplay();
return false;
}

/*****************************************************************************/
/* OnGetVersion : returns the VST Host VST Version                           */
/*****************************************************************************/

long CVSTHost::OnGetVersion(int nEffect)
{
#if defined(VST_2_3_EXTENSIONS)
return 2300L;
#elif defined(VST_2_2_EXTENSIONS)
return 2200L;
#elif defined(VST_2_1_EXTENSIONS)
return 2100L;
#else 
return 2L;
#endif
}
