/*****************************************************************************/
/* AsioHost.cpp: implementation of class CAsioHost.                          */
/*****************************************************************************/

#include "stdafx.h"
#include <math.h>
#include "AsioHost.h"

/*===========================================================================*/
/* Helper functions                                                          */
/*===========================================================================*/

/*****************************************************************************/
/* Saturate : keeps value in range without branching                         */
/*****************************************************************************/

inline float Saturate(float input, float fMax)
{
static const float fGrdDiv = 0.5f;
float x1 = fabsf(input + fMax);
float x2 = fabsf(input - fMax);
return fGrdDiv * (x1 - x2);
}

//-----------------------------------------------------------------------------
// conversion from 64 bit ASIOSample/ASIOTimeStamp to double float
#if NATIVE_INT64
  #define ASIO64toDouble(a)  (a)
#else
  const double twoRaisedTo32 = 4294967296.;
  #define ASIO64toDouble(a)  ((a).lo + (a).hi * twoRaisedTo32)
#endif


/*===========================================================================*/
/* Callback functions - relayed to the one and only host object              */
/*===========================================================================*/
// The ASIO 2.0 SDK doesn't support more than one driver at a time.
// While it would be possible to create a derivate that supports
// multiple drivers, I don't want to deviate too much from the SDK.
// Besides, since the callbacks don't give any clue about their originator,
// I'd have to allocate a set of callback functions for each concurrently 
// loaded driver. Much work for a small gain. So...

// this is the one and only ASIO Host object
static CAsioHost *pHost = NULL;

/*****************************************************************************/
/* bufferSwitchTimeInfo : buffer switch callback with time information       */
/*****************************************************************************/

ASIOTime *bufferSwitchTimeInfo
    (
    ASIOTime *timeInfo,
    long index,
    ASIOBool processNow
    )
{
if (!pHost)
  return timeInfo;
return pHost->OnSwitch(timeInfo, index, processNow);
}

/*****************************************************************************/
/* bufferSwitch : old buffer switch callback w/o time information            */
/*****************************************************************************/

void bufferSwitch(long index, ASIOBool processNow)
{
ASIOTime ti = {0};
if (ASIOGetSamplePosition(&ti.timeInfo.samplePosition,
                          &ti.timeInfo.systemTime) == ASE_OK)
  ti.timeInfo.flags = kSystemTimeValid | kSamplePositionValid;
bufferSwitchTimeInfo(&ti, index, processNow);
}

/*****************************************************************************/
/* sampleRateChanged : called when the sample rate changes                   */
/*****************************************************************************/

void sampleRateChanged(ASIOSampleRate sRate)
{
if (pHost)
  pHost->OnSampleRateChanged(sRate);
}

/*****************************************************************************/
/* asioMessages : ASIO extension callback                                    */
/*****************************************************************************/

long asioMessages(long selector, long value, void* message, double* opt)
{
if (!pHost)
  return 0;
return pHost->OnMessage(selector, value, message,  opt);
}

/*===========================================================================*/
/* CAsioHost class members                                                   */
/*===========================================================================*/

// extern bool loadAsioDriver(char *name);

// this is a bad hack to remedy another bad hack in the ASIO stuff...
// in AsioDrivers.cpp, this pointer is set by the function
// loadAsioDriver(), but it's never destroyed in an orderly fashion.
// I, however, don't like such a mess, so loadAsioDriver() isn't called
// but I assume rulership over this pointer instead :-)
extern AsioDrivers* asioDrivers;

/*****************************************************************************/
/* Static class data (designed for exactly one ASIO Host!)                   */
/*****************************************************************************/

ASIOCallbacks CAsioHost::asioCallbacks =
  {
  bufferSwitch,
  sampleRateChanged,
  asioMessages,
  bufferSwitchTimeInfo
  };

/*****************************************************************************/
/* CAsioHost : constructor                                                   */
/*****************************************************************************/

CAsioHost::CAsioHost()
{
if (!asioDrivers)                       /* we are the one and only           */
  asioDrivers = this;                   /* asioDrivers pointer!              */
bLoaded = false;                        /* no driver loaded yet              */
bAllocated = false;                     /* no buffers allocated yet          */
}

/*****************************************************************************/
/* ~CAsioHost : destructor                                                   */
/*****************************************************************************/

CAsioHost::~CAsioHost()
{
Exit();                                 /* tidy up before we go              */
if (asioDrivers == this)                /* with much regret, we have to      */
  asioDrivers = NULL;                   /* step back from this task :-)      */
}

/*****************************************************************************/
/* LoadDriver : loads an ASIO driver                                         */
/*****************************************************************************/

bool CAsioHost::LoadDriver(char * sDriver, int &nBufSz)
{
if (pHost && (pHost != this))           /* if another CAsioHost is running   */
  return false;                         /* return with error - only 1 allowed*/

if (bLoaded)                            /* make sure to kill off an old one  */
  UnloadDriver();

bLoaded = loadDriver(sDriver);          /* try to load the driver            */
if (!bLoaded)                           /* upon error                        */
  return false;                         /* return at once                    */
// the driver can be referenced as theAsioDriver->... from now on
pHost = this;
try
  {
  if (!Init())
    throw 1;
  if (!GetChannels(&inputChannels, &outputChannels))
    throw 2;
  if (!GetBufferSize(&minSize, &maxSize, &preferredSize, &granularity))
    throw 3;
#if 1
// variant 1 - confine host-defined buffer size to driver range
  if (nBufSz < minSize)
    nBufSz = minSize;
  else if (nBufSz > maxSize)
    nBufSz = maxSize;
#else
// variant 2 - ignore host buffer size, use driver's preferences.
// not really brilliant, but works with less brilliantly written ASIO drivers.
  nBufSz = preferredSize;
#endif
  usedSize = nBufSz;
  if (!GetSampleRate(&sampleRate))
    throw 5;
  if (sampleRate <= 0.0 || sampleRate > 96000.0)
    {
    // Driver does not store it's internal sample rate,
    // so set it to a know one.
    // Usually you should check beforehand, that the selected
    // sample rate is valid with ASIOCanSampleRate().
    if (!SetSampleRate(44100.0))
      throw 6;
    if (!GetSampleRate(&sampleRate))
      throw 7;
    }
  postOutput = OutputReady();           /* check whether driver wants Ready  */
  if (!CreateBuffers(nBufSz))           /* let driver create buffers         */
    throw 8;
  }
catch (...)                             /* upon any error                    */
  {
  removeCurrentDriver();                /* remove eventually loaded driver   */
  bLoaded = false;                      /* and remember we did so            */
  }
return bLoaded;                         /* return whether correctly loaded   */
}

/*****************************************************************************/
/* UnloadDriver : unloads an eventually loaded ASIO driver                   */
/*****************************************************************************/

bool CAsioHost::UnloadDriver()
{
if (bLoaded)                            /* if driver is loaded               */
  {
  Stop();                               /* make sure we're stopped           */
  Exit();                               /* remove the driver                 */
  bLoaded = false;                      /* and remember we did so            */
  return true;
  }
return false;
}

/*****************************************************************************/
/* CreateBuffers : allocates all necessary buffers                           */
/*****************************************************************************/

bool CAsioHost::CreateBuffers(int nBufSz)
{
if (bAllocated)                         /* eventually delete old ones        */
  {
  DeleteBuffers();
  bAllocated = false;
  }

ASIOBufferInfo *info = bufferInfos;
long i;
ASIOError result;

// prepare inputs (Though this is not necessarily required,
// no opened inputs will work, too
inputBuffers = (inputChannels > kMaxInputChannels) ?
                   kMaxInputChannels : inputChannels;
for(i = 0; i < inputBuffers; i++, info++)
  {
  info->isInput = ASIOTrue;
  info->channelNum = i;
  info->buffers[0] = info->buffers[1] = 0;
  }
// prepare outputs
outputBuffers = (outputChannels > kMaxOutputChannels) ?
                    kMaxOutputChannels : outputChannels;
for(i = 0; i < outputBuffers; i++, info++)
  {
  info->isInput = ASIOFalse;
  info->channelNum = i;
  info->buffers[0] = info->buffers[1] = 0;
  }

// create and activate buffers
result = ASIOCreateBuffers(bufferInfos,
                           inputBuffers + outputBuffers,
                           nBufSz,
                           &asioCallbacks);
if (result == ASE_OK)
  // now get all the buffer details, sample word length,
  // name, word clock group and activation
  for (i = 0; i < inputBuffers + outputBuffers; i++)
    {
    channelInfos[i].channel = bufferInfos[i].channelNum;
    channelInfos[i].isInput = bufferInfos[i].isInput;
    result = ASIOGetChannelInfo(&channelInfos[i]);
    if (result != ASE_OK)
      break;
    }
  
if (result == ASE_OK)
  // get the input and output latencies
  // Latencies often are only valid after ASIOCreateBuffers()
  // (input latency is the age of the first sample in the
  //  currently returned audio block)
  // (output latency is the time the first sample in the
  //  currently returned audio block requires to get to the output)
  return GetLatencies(&inputLatency, &outputLatency);

return (result == ASE_OK);
}

/*****************************************************************************/
/* DeleteBuffers : removes allocated buffers                                 */
/*****************************************************************************/

bool CAsioHost::DeleteBuffers()
{
bool bRC = (ASIODisposeBuffers() == ASE_OK);
bAllocated = false;
return bRC;
}

/*****************************************************************************/
/* Init : initializes the driver I/O                                         */
/*****************************************************************************/

bool CAsioHost::Init()
{
memset(&driverInfo, 0, sizeof(driverInfo));
driverInfo.asioVersion = 2;
driverInfo.sysRef = GetSysRef();
return (ASIOInit(&driverInfo) == ASE_OK);
}

/*****************************************************************************/
/* Start : starts the engine                                                 */
/*****************************************************************************/

bool CAsioHost::Start()
{
if (pHost && (pHost != this))
  return false;
pHost = this;
return (ASIOStart() == ASE_OK);
}

/*****************************************************************************/
/* Stop : stops the engine                                                   */
/*****************************************************************************/

bool CAsioHost::Stop()
{
bool bRC = (ASIOStop() == ASE_OK);
pHost = 0;
return bRC;
}

/*****************************************************************************/
/* CanSampleRate : asks loaded driver for sample rate availability           */
/*****************************************************************************/

bool CAsioHost::CanSampleRate(ASIOSampleRate sRate)
{
return (ASIOCanSampleRate(sampleRate) == ASE_OK);
}

/*****************************************************************************/
/* OnSampleRateChanged : called when the sampling rate changes               */
/*****************************************************************************/

void CAsioHost::OnSampleRateChanged(ASIOSampleRate sRate)
{
sampleRate = sRate;                     /* remember new rate                 */
}

/*****************************************************************************/
/* OnMessage : called whenever an ASIO message comes in                      */
/*****************************************************************************/

long CAsioHost::OnMessage(long selector, long value, void* message, double* opt)
{
switch (selector)
  {
  case kAsioSelectorSupported :
    return OnSelectorSupported(value);
  case kAsioEngineVersion :
    return OnEngineVersion();
  case kAsioResetRequest :
    return OnResetRequest();
  case kAsioBufferSizeChange :
    return OnBufferSizeChange(value);
  case kAsioResyncRequest :
    return OnResyncRequest();
  case kAsioLatenciesChanged :
    return OnLatenciesChanged();
  case kAsioSupportsTimeInfo :
    return OnSupportsTimeInfo();
  case kAsioSupportsTimeCode :
    return OnSupportsTimeCode();
  }
return 0;  // unknown message
}

/*****************************************************************************/
/* OnSelectorSupported : called to find out whether a selector is supported  */
/*****************************************************************************/

long CAsioHost::OnSelectorSupported(long lSelector)
{
switch (lSelector)
  {
  case kAsioSelectorSupported :
  case kAsioEngineVersion :
  case kAsioResetRequest :
  case kAsioBufferSizeChange :
  case kAsioResyncRequest :
  case kAsioLatenciesChanged :
  case kAsioSupportsTimeInfo :
  case kAsioSupportsTimeCode :
    return 1L;
  default :
    return 0L;
  }
}

/*****************************************************************************/
/* OnLatenciesChanged : called when driver latencies have changed            */
/*****************************************************************************/

long CAsioHost::OnLatenciesChanged()
{
ASIOGetLatencies(&inputLatency, &outputLatency);
return 1L;
}

/*****************************************************************************/
/* OnSwitch : called when a buffer switch has happened                       */
/*****************************************************************************/

ASIOTime *CAsioHost::OnSwitch
    (
    ASIOTime *timeInfo,                 /* time info                         */
    long index,                         /* buffer to fill now                */
    ASIOBool processNow                 /* process immediately or not        */
    )
{
// store the timeInfo for later use
tInfo = *timeInfo;

// get the time stamp of the buffer, not necessary if no
// synchronization to other media is required
if (timeInfo->timeInfo.flags & kSystemTimeValid)
  nanoSeconds = ASIO64toDouble(timeInfo->timeInfo.systemTime);
else
  nanoSeconds = 0;

if (timeInfo->timeInfo.flags & kSamplePositionValid)
  samples = ASIO64toDouble(timeInfo->timeInfo.samplePosition);
else
  samples = 0;

if (timeInfo->timeCode.flags & kTcValid)
  tcSamples = ASIO64toDouble(timeInfo->timeCode.timeCodeSamples);
else
  tcSamples = 0;

// get the system reference time
#if WINDOWS
sysRefTime = timeGetTime();
#elif MAC
static const double twoRaisedTo32 = 4294967296.;
UnsignedWide ys;
Microseconds(&ys);
double r = ((double)ys.hi * twoRaisedTo32 + (double)ys.lo);
sysRefTime = (unsigned long)(r / 1000.);
#endif

return timeInfo;
}

/*****************************************************************************/
/* ReverseEndian2 : reverts 2-byte entities in place                         */
/*****************************************************************************/

void CAsioHost::ReverseEndian2(void* buffer, long frames)
{
char *a = (char *)buffer;
char c;

while (--frames >= 0)
  {
  c = a[0];
  a[0] = a[1];
  a[1] = c;
  a += 2;
  }
}

/*****************************************************************************/
/* ReverseEndian3 : reverts 3-byte entities in place                         */
/*****************************************************************************/

void CAsioHost::ReverseEndian3(void* buffer, long frames)
{
char *a = (char *)buffer;
char c; 

while (--frames >= 0)
  {
  c = a[0];
  a[0] = a[2];
  a[2] = c;
  a += 3;
  }
}

/*****************************************************************************/
/* ReverseEndian4 : reverts 4-byte entities in place                         */
/*****************************************************************************/

void CAsioHost::ReverseEndian4(void* buffer, long frames)
{
char *a = (char *)buffer;
char c;

while (--frames >= 0)
  {
  c = a[0];
  a[0] = a[3];
  a[3] = c;
  c = a[1];
  a[1] = a[2];
  a[2] = c;
  a += 4;
  }
}

/*****************************************************************************/
/* ReverseEndian8 : reverts 8-byte entities in place                         */
/*****************************************************************************/

void CAsioHost::ReverseEndian8(void* buffer, long frames)
{
char *a = (char *)buffer;
char c;

while (--frames >= 0)
  {
  c = a[0];
  a[0] = a[7];
  a[7] = c;
  c = a[1];
  a[1] = a[6];
  a[6] = c;
  c = a[2];
  a[2] = a[5];
  a[5] = c;
  c = a[3];
  a[3] = a[4];
  a[4] = c;
  a += 8;
  }
}

/*****************************************************************************/
/* ToFloat16 : converts 16-bit signed ints to float, simple method           */
/*****************************************************************************/

void CAsioHost::ToFloat16(void *source, float *target, long frames)
{
// not extremely fast, but exact and portable
short *src = (short *)source;
while (--frames >= 0)
  *target++ = ((*src++) + .5f) * (1.0f / 32767.503f);
}

/*****************************************************************************/
/* FromFloat16 : converts float to 16-bit signed ints, simple method         */
/*****************************************************************************/

void CAsioHost::FromFloat16(float *source, void *target, long frames)
{
// not extremely fast, but exact and portable
short *dst = (short *)target;
float finter;
while (--frames >= 0)
  {
  finter = Saturate(*source++, 1.f);
  *dst++ = (short)((finter * 32767.505f) - .5f);
  }
}

/*****************************************************************************/
/* ToFloat24 : converts 24-bit signed ints to float, simple method           */
/*****************************************************************************/

void CAsioHost::ToFloat24(void *source, float *target, long frames)
{
// not extremely fast, but exact and portable
union
  {
  long lValue;
  char cValue[4];
  } u;
char *src = (char *)source;
char *dst;

u.lValue = 0;
while (--frames >= 0)
  {
#if ASIO_LITTLE_ENDIAN
  dst = &u.cValue[1];
#else
  dst = &u.cValue[0];
#endif
  *dst++ = *src++;
  *dst++ = *src++;
  *dst++ = *src++;
  *target++ = ((u.lValue >> 8) + .5f) * (1.0f / 8388607.75f);
  }
}

/*****************************************************************************/
/* FromFloat24 : converts float to 24-bit signed ints, simple method         */
/*****************************************************************************/

void CAsioHost::FromFloat24(float *source, void *target, long frames)
{
// not extremely fast, but exact and portable
union
  {
  long lValue;
  char cValue[4];
  } u;
char *src;
char *dst = (char *)target;
float finter;

while (--frames >= 0)
  {
  finter = Saturate(*source++, 1.0f);
  u.lValue = ((long)((finter * 8388607.75f) - .5f)) << 8;

#if ASIO_LITTLE_ENDIAN
  src = &u.cValue[1];
#else
  src = &u.cValue[0];
#endif
  *dst++ = *src++;
  *dst++ = *src++;
  *dst++ = *src++;
  }
}

/*****************************************************************************/
/* ToFloat32 : converts 32-bit signed ints to float, simple method           */
/*****************************************************************************/

void CAsioHost::ToFloat32
    (
    void *source,
    float *target,
    long relevantBits,
    long frames
    )
{
// not absolutely exact, not extremely fast... but portable
// the following logic is based on the layout of a 32bit IEEE floating
// point number which has 23 bits mantissa length and thus can't possibly
// store more than 24 relevant bits for numbers in the range -1..+1.
signed char nShift = (relevantBits - 24);
long *src = (long *)source;
long inter;
if (nShift < 0)
  {
  nShift = -nShift;
  while (--frames >= 0)
    {
    inter = (*src++) << nShift;
    *target++ = (inter + .5f) * (1.0f / 8388607.75f);
    }
  }
else
  {
  while (--frames >= 0)
    {
    inter = (*src++) >> nShift;
    *target++ = (inter + .5f) * (1.0f / 8388607.75f);
    }
  }
}

/*****************************************************************************/
/* FromFloat32 : converts float to 32-bit signed ints, simple method         */
/*****************************************************************************/

void CAsioHost::FromFloat32
    (
    float *source,
    void *target,
    long relevantBits,
    long frames
    )
{
// not absolutely exact, not extremely fast... but portable
// the following logic is based on the layout of a 32bit IEEE floating
// point number which has 23 bits mantissa length and thus can't possibly
// store more than 24 relevant bits for numbers in the range -1..+1.
signed char nShift = (relevantBits - 24);
long *dst = (long *)target;
float finter;

if (nShift < 0)
  {
  nShift = -nShift;
  while (--frames >= 0)
    {
    finter = Saturate(*source++, 1.f);
    *dst++ = ((long)((finter * 8388607.75f) - .5f)) >> nShift;
    }
  }
else
  {
  while (--frames >= 0)
    {
    finter = Saturate(*source++, 1.f);
    *dst++ = ((long)((finter * 8388607.75f) - .5f)) << nShift;
    }
  }
}

/*****************************************************************************/
/* ToFloat64 : converts 64bit floats to 32bit float, simple method           */
/*****************************************************************************/

void CAsioHost::ToFloat64(void *source, float *target, long frames)
{
// not absolutely exact, not extremely fast... but portable
double *src = (double *)source;
while (--frames >= 0)
  *target++ = (float)(*src++);
}

/*****************************************************************************/
/* FromFloat64 : converts float to 64bit float, simple method                */
/*****************************************************************************/

void CAsioHost::FromFloat64(float *source, void *target, long frames)
{
// not absolutely exact, not extremely fast... but portable
double *dst = (double *)target;
while (--frames >= 0)
  *dst++ = (double)Saturate(*source++, 1.f);
}

