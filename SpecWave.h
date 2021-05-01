/*****************************************************************************/
/* SpecWave.h: interface for specialized Wave classes                        */
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

#if !defined(_SPECWAVE_H__INCLUDED_)
#define _SPECWAVE_H__INCLUDED_

#if defined(_MSC_VER)
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#endif // defined(_MSC_VER)

#ifndef __cplusplus
#error SPECWAVE.H is for use with C++
#endif

/*****************************************************************************/
/* Necessary includes                                                        */
/*****************************************************************************/

#include "mfcwave.h"

/*****************************************************************************/
/* Definitions                                                               */
/*****************************************************************************/

/*****************************************************************************/
/* CSpecWaveInDevice : special wave input device                             */
/*****************************************************************************/

class CWorkThread;
class CSpecWaveInDevice : public CWaveInDevice  
{
public:
	CSpecWaveInDevice();
	virtual ~CSpecWaveInDevice();

public:
    void SetWorkThread(CWorkThread * pThd) { pWorkThread = pThd; }
	virtual void Data(CWaveBuffer &buf);
    int WaitingBuffers() { return PreparedBuffers(); }
                                        /* allow common start timestamp      */
    void SetStartStamp(DWORD dwTS) { dwStamp = dwTS; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpecWaveInDevice)
	//}}AFX_VIRTUAL

    // Generated message map functions
protected:
	CWorkThread * pWorkThread;
	int nBufSize;
	float * pBuffers[2];
    //{{AFX_MSG(CSpecWaveInDevice)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/*****************************************************************************/
/* CSpecWaveOutDevice : special wave output device                           */
/*****************************************************************************/

class CSpecWaveOutDevice : public CWaveOutDevice
{
public:
	virtual BOOL Close();
    int WaitingBuffers() { return nUsedBufs; }
    int AllocatedBuffers() { return nAllocBuf; }
	void FreeBuffers();
	void AllocateBuffers(int nBufs);
	virtual void OnEvent();
	void SendData(float **pVstData, int nLength);
	virtual void OnWomDone(LPWAVEHDR lphdr);
	CSpecWaveOutDevice();
	virtual ~CSpecWaveOutDevice();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpecWaveOutDevice)
	//}}AFX_VIRTUAL

    // Generated message map functions
protected:
    //{{AFX_MSG(CSpecWaveOutDevice)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

protected:
	int nAllocBuf;
	int nUsedBufs;
	DWORD dwBufMask;
    CWaveBuffer **pBuffers;
};

/*****************************************************************************/
/* Global Data                                                               */
/*****************************************************************************/

extern CSpecWaveInDevice WaveIn;        /* Wave Input Device                 */
extern CSpecWaveOutDevice WaveOut;      /* Wave Output Device                */

#endif // !defined(_SPECWAVE_H__INCLUDED_)
