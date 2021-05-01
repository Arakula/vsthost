/*****************************************************************************/
/* SpecDSound.h: interface for the CSpecDSoundInDevice class.                */
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
