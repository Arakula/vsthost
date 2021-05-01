/*****************************************************************************/
/* WorkThread.h : handles the VSTHost work thread                            */
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

#if !defined(AFX_WORKTHREAD_H__F3670C2B_D773_4EB7_A6EB_3143C43962DD__INCLUDED_)
#define AFX_WORKTHREAD_H__F3670C2B_D773_4EB7_A6EB_3143C43962DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*****************************************************************************/
/* Thread CWorkThread                                                        */
/*****************************************************************************/

class CSmpVSTHost;
class CWorkThread
{
public:
	CWorkThread(CSmpVSTHost *pSetHost = NULL);
	virtual ~CWorkThread();

protected:
	HANDLE hEvent;                      /* event if using event-based logic  */
	CWinThread * pEvtThread;            /* event thread procedure            */
    BOOL volatile bEvtCancel;           /* event thread cancel flag          */
	CSmpVSTHost * pHost;
	int nChannels;
	int nBufSize;
	float **pBuffers;
    DWORD dwStamp;
	BOOL bProcessing;
	// CCriticalSection cs;
    CMutex mtx;
    CSingleLock cs;

// Attribute
public:

// Operationen
public:
	BOOL IsWorking();
	BOOL Process(float **pBuffers, int nSamples, int nChannels = 2, DWORD dwStamp = 0);
	BOOL ProcessImmediately(float **pBuffers, int nSamples, int nChannels = 2, DWORD dwStamp = 0);

// Implementierung
protected:
    static UINT EvtThreadProc(LPVOID pParam);
	BOOL StartEvt();
    void KillEvt();
    void OnWorkthread();

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_WORKTHREAD_H__F3670C2B_D773_4EB7_A6EB_3143C43962DD__INCLUDED_
