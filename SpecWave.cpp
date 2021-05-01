/*****************************************************************************/
/* SPECWAVE.CPP : class implementation for specialized Wave classes          */
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
#include <math.h>

#include "resource.h"                   /* Project resources                 */
#include "SpecWave.h"                   /* private definitions               */

#include "WorkThread.h"                 /* the work thread                   */
#include "vsthost.h"                    /* application specific header       */

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#ifdef WIN32
#define new DEBUG_NEW
#endif
#endif

/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/

#define GetApp()  ((CVsthostApp *)AfxGetApp())

#define WAVEOUTBUFSIZE   38400L         /* length of Wave output buffer      */
// Attention: this is laid out for a 1/20 second, 96KHz, 32 bit maximum...
// if you have a sound card with more than THAT, you need to adjust this value

/*****************************************************************************/
/* Global Data                                                               */
/*****************************************************************************/

CSpecWaveInDevice WaveIn;               /* Wave Input Device                 */
CSpecWaveOutDevice WaveOut;             /* Wave Output Device                */

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

/*===========================================================================*/
/* Class CSpecWaveInDevice                                                   */
/*===========================================================================*/

/*****************************************************************************/
/* CSpecWaveInDevice message map                                             */
/*****************************************************************************/

BEGIN_MESSAGE_MAP(CSpecWaveInDevice, CWaveInDevice)
    //{{AFX_MSG_MAP(CSpecWaveInDevice)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*****************************************************************************/
/* CSpecWaveInDevice : constructor                                           */
/*****************************************************************************/

CSpecWaveInDevice::CSpecWaveInDevice()
{
pWorkThread = NULL;
pBuffers[0] = pBuffers[1] = NULL;
nBufSize = -1;
}

/*****************************************************************************/
/* ~CSpecWaveInDevice : destructor                                           */
/*****************************************************************************/

CSpecWaveInDevice::~CSpecWaveInDevice()
{
if (pBuffers[0])
  delete[] pBuffers[0];
if (pBuffers[1])
  delete[] pBuffers[1];
}

/*****************************************************************************/
/* Data : incoming data processing                                           */
/*****************************************************************************/

void CSpecWaveInDevice::Data(CWaveBuffer &buf)
{
if ((!pWorkThread) ||                   /* if no work thread there           */
    (pWorkThread->IsWorking()))
  {
  TRACE0("Ignoring incoming data because Work Thread is not there or busy\n");
  return;                               /* ignore incoming data              */
  }

int nAlign = BlockAlignment();
if (!nAlign)
  {
  TRACE1("Ignoring %d bytes of incoming data because of BlockAlignment 0!\n", buf.Length());
  return;
  }
int i, j, k = buf.Length() / nAlign;
short sample;

TRACE2("Incoming block of %d samples at %d...\n", k, buf.GetStamp());

// Of course, it would be better to invest a lot of time into evaluating
// better ways to convert from sample format to VST Host's float values.

if (k > nBufSize)                       /* if buffer size larger than last   */
  {
  for (i = 0; i < 2; i++)               /* (re-)allocate larger buffers      */
    {
    if (pBuffers[i])
      delete[] pBuffers[i];
    if (!(pBuffers[i] = new float[k]))
      break;
    }
  }
else
  i = 2;
                                        /* convert to VST format, simple     */
if (i >= 2)                             /* if all buffers allocated          */
  {
  nBufSize = k;                         /* remember current buffer size      */
  for (i = 0; i < k; i++)               /* convert buffer to VST format      */
    {
    for (j = 0; j < 2; j++)
      {
// much room here for optimizations...
      sample = GetSample(buf, i, j);    /* get sample in 16bit signed form   */
                                        /* convert to float -.99999..+.99999 */
      pBuffers[j][i] = (sample + .5f) * (1.f /  32767.503f);
      }
    }

  pWorkThread->Process(pBuffers, k, 2,  /* pass on converted buffer to work  */
                       buf.GetStamp());
  }

}

/*===========================================================================*/
/* Class CSpecWaveOutDevice                                                  */
/*===========================================================================*/

/*****************************************************************************/
/* CSpecWaveOutDevice message map                                            */
/*****************************************************************************/

BEGIN_MESSAGE_MAP(CSpecWaveOutDevice, CWaveOutDevice)
    //{{AFX_MSG_MAP(CSpecWaveOutDevice)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*****************************************************************************/
/* CSpecWaveOutDevice : constructor                                          */
/*****************************************************************************/

CSpecWaveOutDevice::CSpecWaveOutDevice()
{
dwBufMask = 0;                          /* no used buffers so far            */
nAllocBuf = 0;
pBuffers = NULL;
nUsedBufs = 0;
AllocateBuffers(0);
}

/*****************************************************************************/
/* ~CSpecWaveOutDevice : destructor                                          */
/*****************************************************************************/

CSpecWaveOutDevice::~CSpecWaveOutDevice()
{
Close();
FreeBuffers();
}

/*****************************************************************************/
/* Close : closes an open connection                                         */
/*****************************************************************************/

BOOL CSpecWaveOutDevice::Close()
{
Reset();
int nSleeps = 0;
while (nUsedBufs)
  {
  Sleep(50);
  if (++nSleeps > 20)                   /* wait up to a second for buffers   */
    {
    TRACE0("CSpecWaveOutDevice::Close: aborting wait for used buffers\n");
    break;
    }
  }
return CWaveOutDevice::Close();
}

/*****************************************************************************/
/* OnEvent : called when an event comes in                                   */
/*****************************************************************************/

void CSpecWaveOutDevice::OnEvent()
{
for (int i = 0; i < nAllocBuf; i++)
  {
  if (pBuffers[i])                      /* if one of ours                    */
    {
    LPWAVEHDR lphdr = *pBuffers[i];     /* examine the thang                 */
    if (lphdr->dwFlags & WHDR_DONE)     /* if finished playing that          */
      OnWomDone(lphdr);                 /* process it                        */
    }
  }
}

/*****************************************************************************/
/* OnWomDone : called when a buffer has been processed                       */
/*****************************************************************************/

void CSpecWaveOutDevice::OnWomDone(LPWAVEHDR lphdr)
{
UnprepareHeader(lphdr);                 /* unprepare the finished data       */
for (int i = 0; i < nAllocBuf; i++)
  {
  if ((pBuffers[i]) &&                  /* if one of ours                    */
      (lphdr == (LPWAVEHDR)(*pBuffers[i])))
    {
    lphdr->dwFlags &= ~WHDR_DONE;       /* reset DONE flag in header         */
    dwBufMask &= ~(1 << i);             /* reset USED flag                   */
    if (nUsedBufs > 0)
      nUsedBufs--;                      /* decrement # used buffers          */
    }
  }
}

/*****************************************************************************/
/* SendData : sends out a VST-style buffer to the wave output device         */
/*****************************************************************************/

void CSpecWaveOutDevice::SendData(float **pVstData, int nLength)
{
if (!IsOpen())                          /* if not opened                     */
  return;                               /* pass back error                   */

                                        /* calculate max. buffer length      */
int nBufLen = (44100 + nAllocBuf - 1) / nAllocBuf;

int i, j, k, use, off = 0;
short sValue;

while (nLength)                         /* do as often as needed             */
  {
  use = (nLength <= nBufLen) ? nLength : nBufLen;
  for (i = 0; i < nAllocBuf; i++)       /* find first unused output buffer   */
    if ((!(dwBufMask & (1 << i))) &&
        (pBuffers[i]))
      break;
  if (i >= nAllocBuf)                   /* if all in use                     */
    {
    TRACE1("SendData(%d): all buffers used\n", nLength);
    return;                             /* can't process this one...         */
    }
    
  float fsmpl, fMax = 0.f, fCur;        /* diagnostic report value           */
  for (j = 0; j < use; j++)             /* then convert all samples to target*/
    {
    for (k = 0; k < 2; k++)             /* format                            */
      {
      fsmpl = pVstData[k][j + off];
#if defined(_DEBUG) || defined(_DEBUGFILE)
      fCur = fabsf(fsmpl);              /* just us diagnostics in here :-)   */
      if (fCur > fMax)
        fMax = fCur;
#endif
                                        /* really HARD clipping method...    */
      sValue = (short) ((Saturate(fsmpl, 1.0f) * 32767.505f) - .5f);
      SetSample(*pBuffers[i], j, k, sValue);
      }
    }
  
  dwBufMask |= (1 << i);                /* remember this buffer's in use     */
  nUsedBufs++;                          /* increment # used buffers          */

#if 0
  if (nUsedBufs > 2)                    /* if tedency rising                 */
    use = use / 4 + 1;                  /* send only one halfth              */
#endif

  DWORD dwLen = use * BlockAlignment();
  ((LPWAVEHDR)*pBuffers[i])->dwBufferLength = dwLen;
  ((LPWAVEHDR)*pBuffers[i])->dwBytesRecorded = dwLen;
  Output(*pBuffers[i], FALSE);            /* then send it to output            */
  off += use;
  nLength -= use;
#if defined(_DEBUG) || defined(_DEBUGFILE)
  TRACE("SendData(%d): Output buffer %d (%d used); max=%1.5f\n", use, i, nUsedBufs, fMax);
#endif
  }
}

/*****************************************************************************/
/* AllocateBuffers : allocates 1 second of output wave buffers               */
/*****************************************************************************/

void CSpecWaveOutDevice::AllocateBuffers(int nBufs)
{
// Attention: no check whatsoever!
FreeBuffers();
if (!nBufs)
  return;

int nSize = ((44100 + nBufs - 1) / nBufs) * BlockAlignment();
pBuffers = new CWaveBuffer* [nBufs];
for (int i = 0; i < nBufs; i++)
//  pBuffers[i] = new CWaveBuffer(WAVEOUTBUFSIZE);
    pBuffers[i] = new CWaveBuffer(nSize);
nAllocBuf = nBufs;
}

/*****************************************************************************/
/* FreeBuffers : frees all allocated buffers                                 */
/*****************************************************************************/

void CSpecWaveOutDevice::FreeBuffers()
{
// Attention: no check whatsoever!
if (pBuffers)
  {
  for (int i = 0; i < nAllocBuf; i++)
    if (pBuffers[i])
      delete pBuffers[i];
  delete[] pBuffers;
  }
pBuffers = NULL;
nAllocBuf = 0;
}
