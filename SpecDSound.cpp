/*****************************************************************************/
/* SpecDSound.cpp: implementation of the DirectSound VSTHost classes         */
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

#include "stdafx.h"

#include "SpecDSound.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*===========================================================================*/
/* Global Data                                                               */
/*===========================================================================*/

CSpecDSoundInDevice DSoundIn;           /* DirectSound Input Device          */
CSpecDSoundOutDevice DSoundOut;         /* DirectSound Output Device         */

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
/* CSpecDSoundInDevice class members                                         */
/*===========================================================================*/

/*****************************************************************************/
/* CSpecDSoundInDevice : constructor                                         */
/*****************************************************************************/

CSpecDSoundInDevice::CSpecDSoundInDevice()
{
nCurBuf = -1;                           /* no current buffer                 */
}

/*****************************************************************************/
/* ~CSpecDSoundInDevice : destructor                                         */
/*****************************************************************************/

CSpecDSoundInDevice::~CSpecDSoundInDevice()
{

}

/*****************************************************************************/
/* OnSwitch : called when the input buffer has been switched                 */
/*****************************************************************************/

void CSpecDSoundInDevice::OnSwitch(int nBuffer)
{
nCurBuf = nBuffer;
if (pWorkThread)
  {
//  pWorkThread->Process();
  }
}

/*===========================================================================*/
/* CSpecDSoundOutDevice class members                                        */
/*===========================================================================*/

/*****************************************************************************/
/* CSpecDSoundOutDevice : constructor                                        */
/*****************************************************************************/

CSpecDSoundOutDevice::CSpecDSoundOutDevice()
{
pwb = NULL;
nCurBuf = -1;                           /* no current buffer                 */
}

/*****************************************************************************/
/* ~CSpecDSoundOutDevice : destructor                                        */
/*****************************************************************************/

CSpecDSoundOutDevice::~CSpecDSoundOutDevice()
{
if (pwb)
  delete[] pwb;
}

/*****************************************************************************/
/* OnOpen : called when Open has been successfully completed                 */
/*****************************************************************************/

void CSpecDSoundOutDevice::OnOpen()
{
if (pwb)
  delete[] pwb;
                                        /* create 16bit calculation buffer   */
int nShorts = GetBufferSize() / sizeof(short);
pwb = new short[nShorts];
if (pwb)
  {
  memset(pwb, 0, GetBufferSize());      /* empty it                          */
  DWORD dwLen = nShorts >> 1;
  WriteBuffer(0, pwb, dwLen);           /* and initialize the DS buffer      */
  WriteBuffer(1, pwb, dwLen);
  SetDriverPos(0);
  }
}

/*****************************************************************************/
/* OnSwitch : called when the output buffer has been switched                */
/*****************************************************************************/

void CSpecDSoundOutDevice::OnSwitch(int nBuffer)
{
nCurBuf = nBuffer;
}

/*****************************************************************************/
/* SendData : sends out a block of data                                      */
/*****************************************************************************/

void CSpecDSoundOutDevice::SendData(float **pVstData, int nLength)
{
if ((!IsStarted()) ||                   /* if not playing                    */
    (!pwb))                             /* or no temp buffer                 */
  return;                               /* return without action             */

int i, j, k, use;                       /* convert to 16bit stereo buffer    */
short *pValue = pwb;
DWORD dwBufLen = GetBufferSize();
use = (nLength <= (int)dwBufLen) ? nLength : (int)dwBufLen;

float fsmpl, fMax = 0.f;                /* diagnostic report value           */
for (j = 0; j < use; j++)               /* then convert all samples to target*/
  {
  for (k = 0; k < 2; k++)               /* format                            */
    {
    fsmpl = pVstData[k][j];
#if defined(_DEBUG) || defined(_DEBUGFILE)
    float fCur = fabsf(fsmpl);          /* just us diagnostics in here :-)   */
    if (fCur > fMax)
      fMax = fCur;
#endif
                                        /* really HARD clipping method...    */
    *pValue++ = (short) ((Saturate(fsmpl, 1.0f) * 32767.505f) - .5f);
    }
  }

i = 1 - GetDriverBuf();
WriteBuffer(i, pwb, dwBufLen);          /* copy that into DS buffer          */
#if defined(_DEBUG) || defined(_DEBUGFILE)
TRACE("SendData(%d): Output buffer %d; max=%1.5f\n", use, i, fMax);
#endif
}
