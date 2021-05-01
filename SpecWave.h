/*****************************************************************************/
/* SpecWave.h: interface for specialized Wave classes                        */
/*****************************************************************************/

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
