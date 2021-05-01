/*****************************************************************************/
/* SpecAsioHost.h: interface for the CSpecAsioHost class.                    */
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

#if !defined(AFX_SPECASIOHOST_H__69E15A61_1C21_11D8_81AA_4000001054B2__INCLUDED_)
#define AFX_SPECASIOHOST_H__69E15A61_1C21_11D8_81AA_4000001054B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AsioHost.h"

// this is a specialized version for VSTHost's MFC sample implementation
class CWorkThread;
class CSpecAsioHost : public CAsioHost  
{
public:
	CSpecAsioHost();
	virtual ~CSpecAsioHost();

	virtual bool LoadDriver(char *sDriver, int &nBufSz);
    virtual long OnBufferSizeChange(long lNewSize);

// additional parameters
public:
    void SetWorkThread(CWorkThread *pThread) { pWorkThread = pThread; }
    void OnProcessed();
    void OnProcessed(float **pBuffers);

    bool GetActiveChannel(int nIndex) { return bActiveChannel[nIndex]; }
    void SetActiveChannel(int nIndex, bool bActive = true) { bActiveChannel[nIndex] = bActive; }
	void ClearBuffers();

// specializations
public:
    virtual void *GetSysRef() { return AfxGetMainWnd()->m_hWnd; }
    virtual ASIOTime *OnSwitch(ASIOTime *timeInfo, long index, ASIOBool processNow);
    virtual long OnResyncRequest()
      {
//      AfxGetMainWnd()->PostMessage(WM_COMMAND, IDM_ENGINE_RESTART);
      return 1L;
      }

#if defined(_DEBUG) || defined(_DEBUGFILE)
    virtual long OnMessage(long selector, long value, void* message, double* opt)
      {
      long lrc = CAsioHost::OnMessage(selector, value, message, opt);
      TRACE("CSpecAsioHost::OnMessage(%d, %d, %08lX, %08lX)=%d\n",
            selector, value, message, opt, lrc);
      return lrc;
      }
#endif

protected:
	CWorkThread * pWorkThread;
    long allocSize;
    float *pBuffers[2];
    float *pOBuffers[2];
    long curIndex;
    bool bActiveChannel[kMaxInputChannels + kMaxOutputChannels];


protected:
    bool AllocBuffers(long nBufSz = 4410);
	void ConvertFromASIO(int nIndex, void *source, float *target, long frames);
	void ConvertToASIO(int nIndex, float *source, void *target, long frames);
};

#endif // !defined(AFX_SPECASIOHOST_H__69E15A61_1C21_11D8_81AA_4000001054B2__INCLUDED_)
