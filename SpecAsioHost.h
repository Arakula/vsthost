/*****************************************************************************/
/* SpecAsioHost.h: interface for the CSpecAsioHost class.                    */
/*****************************************************************************/

#if !defined(AFX_SPECASIOHOST_H__69E15A61_1C21_11D8_81AA_4000001054B2__INCLUDED_)
#define AFX_SPECASIOHOST_H__69E15A61_1C21_11D8_81AA_4000001054B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AsioHost.h"
#include "resource.h"

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

protected:
    bool AllocBuffers(long nBufSz = 4410);
};

#endif // !defined(AFX_SPECASIOHOST_H__69E15A61_1C21_11D8_81AA_4000001054B2__INCLUDED_)
