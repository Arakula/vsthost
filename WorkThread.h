/*****************************************************************************/
/* WorkThread.h : handles the VSTHost work thread                            */
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
