/*****************************************************************************/
/* SpecAsioHost.cpp: implementation of the CSpecAsioHost class.              */
/*****************************************************************************/

#include "stdafx.h"
#include "WorkThread.h"
#include "SpecAsioHost.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*===========================================================================*/
/* CSpecAsioHost class members                                               */
/*===========================================================================*/

/*****************************************************************************/
/* CSpecAsioHost : constructor                                               */
/*****************************************************************************/

CSpecAsioHost::CSpecAsioHost()
{
pWorkThread = 0;
pBuffers[0] = pBuffers[1] = 0;
allocSize = 0;
}

/*****************************************************************************/
/* ~CSpecAsioHost : destructor                                               */
/*****************************************************************************/

CSpecAsioHost::~CSpecAsioHost()
{
for (int i = 0; i < 2; i++)
  if (pBuffers[i])
    delete[] pBuffers[i];
}

/*****************************************************************************/
/* AllocBuffers : (re-)allocates the buffers                                 */
/*****************************************************************************/

bool CSpecAsioHost::AllocBuffers(long nBufSz)
{
bool bRC = true;
int nAllocd = 0;
if (nBufSz <= allocSize)
  return true;

for (int i = 0; i < 2; i++)
  {
  if (pBuffers[i])
    delete[] pBuffers[i];
  if ((pBuffers[i] = new float[nBufSz]) != 0)
    nAllocd++;
  else if (i)
    {
    delete[] pBuffers[0];
    pBuffers[0] = 0;
    }
  }

if (nAllocd < 2)
  {
  allocSize = 0;
  return false;
  }
else
  {
  allocSize = nBufSz;
  return true;
  }
}

/*****************************************************************************/
/* LoadDriver : loads a driver & allocates the necessary buffers             */
/*****************************************************************************/

bool CSpecAsioHost::LoadDriver(char *sDriver, int &nBufSz)
{
if (!CAsioHost::LoadDriver(sDriver, nBufSz))
#if defined(_DEBUG) || defined(_DEBUGFILE)
  {
  TRACE2("CAsioHost::LoadDriver(%s) failed: \"%s\"\n",
         sDriver, driverInfo.errorMessage);
  return false;
  }
TRACE("CAsioHost::LoadDriver(%s) loaded: \"%s\" V%d Asio%d\n",
      sDriver,
      driverInfo.name, driverInfo.driverVersion,
      driverInfo.asioVersion);
#else
  return false;
#endif
return AllocBuffers(nBufSz);
}

/*****************************************************************************/
/* OnBufferSizeChange : called when the driver wants to switch the buffer sz */
/*****************************************************************************/

long CSpecAsioHost::OnBufferSizeChange(long lNewSize)
{
return AllocBuffers(lNewSize);
}

/*****************************************************************************/
/* OnSwitch : called when a buffer switch occurs                             */
/*****************************************************************************/

ASIOTime *CSpecAsioHost::OnSwitch(ASIOTime *timeInfo, long index, ASIOBool processNow)
{
                                        /* call base class first to fill vars*/
timeInfo = CAsioHost::OnSwitch(timeInfo, index, processNow);

int i, j;
int nBufs;

#if defined(_DEBUGFILE)
#if defined(_DEBUG) || defined(_DEBUGFILE)
static double last_samples = 0;
TRACE("OnSwitch(%d): diff %d ms, ref %d ms, %d samples\n",
      index,
      sysRefTime - (long)(nanoSeconds / 1000000.0),
      sysRefTime,
      (long)(samples - last_samples));
last_samples = samples;
#endif
#endif

curIndex = index;

// convert input buffers to 32bit floating point
// all ASIO...LSB/MSB conversions are without #ifdefs, since
// this class is for a specialized Win32 ASIOHost.
for (i = nBufs = 0; i < inputBuffers + outputBuffers; i++)
  {
  if (bufferInfos[i].isInput)
    {
    switch (channelInfos[i].type)
      {
      case ASIOSTInt16MSB:
        ReverseEndian2(bufferInfos[i].buffers[index], usedSize);
      case ASIOSTInt16LSB:
        ToFloat16(bufferInfos[i].buffers[index], pBuffers[nBufs], usedSize);
        break;

      case ASIOSTInt24MSB:        // used for 20 bits as well
        ReverseEndian3(bufferInfos[i].buffers[index], usedSize);
      case ASIOSTInt24LSB:
        ToFloat24(bufferInfos[i].buffers[index], pBuffers[nBufs], usedSize);
        break;

      case ASIOSTFloat32MSB:      // IEEE 754 32 bit float
        ReverseEndian4(bufferInfos[i].buffers[index], usedSize);
      case ASIOSTFloat32LSB:
        memcpy(pBuffers[nBufs],
               bufferInfos[i].buffers[index],
               usedSize * sizeof(float));
        break;

      case ASIOSTFloat64MSB:      // IEEE 754 64 bit double float
        ReverseEndian8(bufferInfos[i].buffers[index], usedSize);
      case ASIOSTFloat64LSB:
        ToFloat64(bufferInfos[i].buffers[index], pBuffers[nBufs], usedSize);
        break;
        
      case ASIOSTInt32MSB:        // 32 bit data
        ReverseEndian4(bufferInfos[i].buffers[index], usedSize);
      case ASIOSTInt32LSB:        // 32 bit data 
        ToFloat32(bufferInfos[i].buffers[index], pBuffers[nBufs], 32, usedSize);
        break;
        // these are used for 32 bit data buffer, with different alignment of the data inside
        // 32 bit PCI bus systems can more easily used with these
      case ASIOSTInt32MSB16:      // 32 bit data with 16 bit alignment
        ReverseEndian4(bufferInfos[i].buffers[index], usedSize);
      case ASIOSTInt32LSB16:      // 32 bit data with 18 bit alignment
        ToFloat32(bufferInfos[i].buffers[index], pBuffers[nBufs], 16, usedSize);
        break;
      case ASIOSTInt32MSB18:      // 32 bit data with 18 bit alignment
        ReverseEndian4(bufferInfos[i].buffers[index], usedSize);
      case ASIOSTInt32LSB18:      // 32 bit data with 18 bit alignment
        ToFloat32(bufferInfos[i].buffers[index], pBuffers[nBufs], 18, usedSize);
        break;
      case ASIOSTInt32MSB20:      // 32 bit data with 20 bit alignment
        ReverseEndian4(bufferInfos[i].buffers[index], usedSize);
      case ASIOSTInt32LSB20:      // 32 bit data with 20 bit alignment
        ToFloat32(bufferInfos[i].buffers[index], pBuffers[nBufs], 20, usedSize);
        break;
      case ASIOSTInt32MSB24:      // 32 bit data with 24 bit alignment
        ReverseEndian4(bufferInfos[i].buffers[index], usedSize);
      case ASIOSTInt32LSB24:      // 32 bit data with 24 bit alignment
        ToFloat32(bufferInfos[i].buffers[index], pBuffers[nBufs], 24, usedSize);
        break;
       
      }
    
    nBufs++;
    }
  if (nBufs >= 2)                       /* VSTHost processes only 2 buffers  */
    break;
  }
for (; nBufs < 2; nBufs++)              /* fill up unavailable input buffers */
  {                                     /* as we process EXACTLY 2 buffers   */
  if (pBuffers[nBufs])
    for (j = 0; j < usedSize; j++)
      pBuffers[nBufs][j] = .0f;
  }

pOBuffers[0] = pBuffers[0];             /* copy pointers to output ptrs      */
pOBuffers[1] = pBuffers[1];
if (pWorkThread)                        /* if work thread there, process it  */
  {
  DWORD dwStamp = (DWORD)(nanoSeconds / 1000.0L);
  if (processNow == ASIOTrue)           /* either immediately                */
    pWorkThread->ProcessImmediately(pBuffers, usedSize, 2, dwStamp);
  else                                  /* or in separate thread             */
    pWorkThread->Process(pBuffers, usedSize, 2, dwStamp);
  }
else                                    /* otherwise                         */
  OnProcessed();                        /* just pass on the unprocessed stuff*/

return timeInfo;
}

/*****************************************************************************/
/* OnProcessed : called when the work thread has finished processing         */
/*****************************************************************************/

void CSpecAsioHost::OnProcessed(float **pBuffers)
{
// called from SmpVstHost with the result buffer addresses
pOBuffers[0] = pBuffers[0];
pOBuffers[1] = pBuffers[1];
OnProcessed();
}

void CSpecAsioHost::OnProcessed()
{
int i;
int nBufs;
#if defined(_DEBUG) || defined(_DEBUGFILE)
int nType = -1;
#endif

// convert from float to target format
// convert input buffers to 32bit floating point
// all ASIO...LSB/MSB conversions are without #ifdefs, since
// this class is for a specialized Win32 ASIOHost.
for (i = nBufs = 0; i < inputBuffers + outputBuffers; i++)
  {
  if (!bufferInfos[i].isInput)
    {
#if defined(_DEBUG) || defined(_DEBUGFILE)
    nType = channelInfos[i].type;
#endif
    switch (channelInfos[i].type)
      {
      case ASIOSTInt16LSB:
        FromFloat16(pOBuffers[nBufs], bufferInfos[i].buffers[curIndex], usedSize);
        break;
      case ASIOSTInt24LSB:
        FromFloat24(pOBuffers[nBufs], bufferInfos[i].buffers[curIndex], usedSize);
        break;
      case ASIOSTInt32LSB:
        FromFloat32(pOBuffers[nBufs], bufferInfos[i].buffers[curIndex], 32, usedSize);
        break;
        // these are used for 32 bit data buffer, with different alignment of the data inside
        // 32 bit PCI bus systems can more easily used with these
      case ASIOSTInt32LSB16:      // 32 bit data with 16 bit alignment
        FromFloat32(pOBuffers[nBufs], bufferInfos[i].buffers[curIndex], 16, usedSize);
        break;
      case ASIOSTInt32LSB18:      // 32 bit data with 18 bit alignment
        FromFloat32(pOBuffers[nBufs], bufferInfos[i].buffers[curIndex], 18, usedSize);
        break;
      case ASIOSTInt32LSB20:      // 32 bit data with 20 bit alignment
        FromFloat32(pOBuffers[nBufs], bufferInfos[i].buffers[curIndex], 20, usedSize);
        break;
      case ASIOSTInt32LSB24:      // 32 bit data with 24 bit alignment
        FromFloat32(pOBuffers[nBufs], bufferInfos[i].buffers[curIndex], 24, usedSize);
        break;
      case ASIOSTFloat32LSB:      // IEEE 754 32 bit float
        memcpy(pOBuffers[nBufs],
               bufferInfos[i].buffers[curIndex],
               usedSize * sizeof(float));
        break;
      case ASIOSTFloat64LSB:      // IEEE 754 64 bit double float
        FromFloat64(pOBuffers[nBufs], bufferInfos[i].buffers[curIndex], usedSize);
        break;
        
      case ASIOSTInt16MSB:
        FromFloat16(pOBuffers[nBufs], bufferInfos[i].buffers[curIndex], usedSize);
        ReverseEndian2(bufferInfos[i].buffers[curIndex], usedSize);
        break;
      case ASIOSTInt24MSB:        // used for 20 bits as well
        FromFloat24(pOBuffers[nBufs], bufferInfos[i].buffers[curIndex], usedSize);
        ReverseEndian3(bufferInfos[i].buffers[curIndex], usedSize);
        break;
      case ASIOSTInt32MSB:
        // these are used for 32 bit data buffer, with different alignment of the data inside
        // 32 bit PCI bus systems can more easily used with these
      case ASIOSTInt32MSB16:      // 32 bit data with 16 bit alignment
        FromFloat32(pOBuffers[nBufs], bufferInfos[i].buffers[curIndex], 16, usedSize);
        ReverseEndian4(bufferInfos[i].buffers[curIndex], usedSize);
        break;
      case ASIOSTInt32MSB18:      // 32 bit data with 18 bit alignment
        FromFloat32(pOBuffers[nBufs], bufferInfos[i].buffers[curIndex], 18, usedSize);
        ReverseEndian4(bufferInfos[i].buffers[curIndex], usedSize);
        break;
      case ASIOSTInt32MSB20:      // 32 bit data with 20 bit alignment
        FromFloat32(pOBuffers[nBufs], bufferInfos[i].buffers[curIndex], 20, usedSize);
        ReverseEndian4(bufferInfos[i].buffers[curIndex], usedSize);
        break;
      case ASIOSTInt32MSB24:      // 32 bit data with 24 bit alignment
        FromFloat32(pOBuffers[nBufs], bufferInfos[i].buffers[curIndex], 24, usedSize);
        ReverseEndian4(bufferInfos[i].buffers[curIndex], usedSize);
        break;
      case ASIOSTFloat32MSB:      // IEEE 754 32 bit float
        memcpy(pOBuffers[nBufs],
               bufferInfos[i].buffers[curIndex],
               usedSize * sizeof(float));
        ReverseEndian4(bufferInfos[i].buffers[curIndex], usedSize);
        break;
      case ASIOSTFloat64MSB:      // IEEE 754 64 bit double float
        FromFloat64(pOBuffers[nBufs], bufferInfos[i].buffers[curIndex], usedSize);
        ReverseEndian8(bufferInfos[i].buffers[curIndex], usedSize);
        break;
      }
    
    nBufs++;
    }
  if (nBufs >= 2)                       /* VSTHost processes only 2 buffers  */
    break;
  }

#if defined(_DEBUGFILE)
#if defined(_DEBUG) || defined(_DEBUGFILE)
// get the system reference time
long diff = timeGetTime() - sysRefTime;
TRACE("Output type %d ready after %dms for buffer %d\n", nType, diff, curIndex);
#endif
#endif

if (postOutput)                         /* if driver uses the notification,  */
  OutputReady();                        /* tell it that output is processed  */
}
