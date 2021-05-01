/*****************************************************************************/
/* SpecDSound.h: interface for the CSpecDSoundInDevice class.                */
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

#if !defined(AFX_SPECDSOUND_H__8FAFD4E4_9754_11D8_8223_4000001054B2__INCLUDED_)
#define AFX_SPECDSOUND_H__8FAFD4E4_9754_11D8_8223_4000001054B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DSoundDev.h"
#include "mfcwave.h"

/*****************************************************************************/
/* CSpecDSoundInDevice : DirectSound input device for VSTHost                */
/*****************************************************************************/

class CWorkThread;
class CSpecDSoundInDevice : public CDSoundInDevice  
{
public:
	CSpecDSoundInDevice();
	virtual ~CSpecDSoundInDevice();

    virtual void OnSwitch(int nBuffer);

public:
    void SetWorkThread(CWorkThread * pThd) { pWorkThread = pThd; }

protected:
	CWorkThread * pWorkThread;
	float * pBuffers[2];
    int nCurBuf;
};

/*****************************************************************************/
/* CSpecDSoundOutDevice : DirectSound output device for VSTHost              */
/*****************************************************************************/

class CSpecDSoundOutDevice : public CDSoundOutDevice  
{
public:
	CSpecDSoundOutDevice();
	virtual ~CSpecDSoundOutDevice();

    virtual void OnOpen();
    virtual void OnSwitch(int nBuffer);

public:
	void SendData(float **pVstData, int nLength);

protected:
    short *pwb;
    int nCurBuf;
};

/*****************************************************************************/
/* Global Data                                                               */
/*****************************************************************************/

extern CSpecDSoundInDevice DSoundIn;    /* DirectSound Input Device          */
extern CSpecDSoundOutDevice DSoundOut;  /* DirectSound Output Device         */

#endif // !defined(AFX_SPECDSOUND_H__8FAFD4E4_9754_11D8_8223_4000001054B2__INCLUDED_)
