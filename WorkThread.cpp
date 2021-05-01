/*****************************************************************************/
/* WorkThread.cpp : handles the VSTHost work thread                          */
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


#include "stdafx.h"
#include "vsthost.h"
#include "WorkThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*===========================================================================*/
/* CWorkThread class members                                                 */
/*===========================================================================*/

/*****************************************************************************/
/* CWorkThread : constructor                                                 */
/*****************************************************************************/

CWorkThread::CWorkThread(CSmpVSTHost *pSetHost)
    : cs(&mtx)
{
bProcessing = FALSE;
if (pSetHost)
  pHost = pSetHost;
else
  pHost = &((CVsthostApp *)AfxGetApp())->vstHost;

hEvent = 0;
pEvtThread = NULL;
bEvtCancel = FALSE;
StartEvt();
}

/*****************************************************************************/
/* ~CWorkThread : destructor                                                 */
/*****************************************************************************/

CWorkThread::~CWorkThread()
{
KillEvt();
}

/*****************************************************************************/
/* EvtThreadProc : event thread procedure                                    */
/*****************************************************************************/

UINT CWorkThread::EvtThreadProc(LPVOID pParam)
{
UINT result;
CWorkThread *pThd = (CWorkThread *)pParam;

while (!pThd->bEvtCancel)
  {
  result = WaitForSingleObject(pThd->hEvent,INFINITE);
  if ((result == WAIT_OBJECT_0) &&
      (!pThd->bEvtCancel))
    pThd->OnWorkthread();
  else
    break;
  }
pThd->bEvtCancel = FALSE;
return 0;
}

/*****************************************************************************/
/* StartEvt : creates an event handle & thread                               */
/*****************************************************************************/

BOOL CWorkThread::StartEvt()
{
KillEvt();                              /* make sure none's there            */

                                        /* then create a new event handle    */
hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
if (!hEvent)                            /* upon error                        */
  return FALSE;                         /* return error                      */

                                        /* then create the thread            */
pEvtThread = AfxBeginThread(EvtThreadProc, this,
                            THREAD_PRIORITY_NORMAL, 0,
                            CREATE_SUSPENDED, NULL);   
if (!pEvtThread)                        /* upon error                        */
  {
  KillEvt();                            /* kill event handle                 */
  return FALSE;                         /* and return error                  */
  }

pEvtThread->m_bAutoDelete = TRUE;       /* let it go!                        */
pEvtThread->ResumeThread();
                                        /* work thread has higher priority   */
//pEvtThread->SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
pEvtThread->SetThreadPriority(THREAD_PRIORITY_HIGHEST);

return TRUE;
}

/*****************************************************************************/
/* KillEvt : kills an eventually running event thread & event handle         */
/*****************************************************************************/

void CWorkThread::KillEvt()
{
if (hEvent)                             /* if event handle allocated         */
  {
  TRACE0("Killing Workthread thread\n");
  if (pEvtThread)                       /* if thread still running           */
    {
    bEvtCancel = TRUE;                  /* set cancel flag                   */
    SetEvent(hEvent);                   /* tell thread to die                */
    while (bEvtCancel)                  /* wait until it does.               */
      {
      TRACE0("Waiting for thread to stop\n");
      Sleep(50);
      }
    pEvtThread = 0;                     /* and thread handle                 */
    }
  CloseHandle(hEvent);                  /* and close the event handle        */
  hEvent = 0;                           /* and reset it...                   */
  }
}

/*****************************************************************************/
/* Process : processes an incoming VST buffer                                */
/*****************************************************************************/

BOOL CWorkThread::Process
    (
    float **pBuffers,
    int nSamples,
    int nChannels,
    DWORD dwStamp
    )
{
if (IsWorking())
  return FALSE;

if (cs.Lock(1000))
  {
  bProcessing = TRUE;
  cs.Unlock();
  }
else
  return FALSE;

this->pBuffers = pBuffers;
this->nBufSize = nSamples;
this->nChannels = nChannels;
this->dwStamp = dwStamp;
SetEvent(hEvent);                       /* tell thread to go to work         */
return TRUE;
}

/*****************************************************************************/
/* ProcessImmediately : processes an incoming VST buffer in current thread   */
/*****************************************************************************/

BOOL CWorkThread::ProcessImmediately
    (
    float **pBuffers,
    int nSamples,
    int nChannels,
    DWORD dwStamp
    )
{
if (IsWorking())
  return FALSE;

if (cs.Lock(1000))
  {
  bProcessing = TRUE;
  cs.Unlock();
  }
else
  return FALSE;

this->pBuffers = pBuffers;
this->nBufSize = nSamples;
this->nChannels = nChannels;
this->dwStamp = dwStamp;
OnWorkthread();
return TRUE;
}

/*****************************************************************************/
/* IsWorking : returns whether currently processing a thread                 */
/*****************************************************************************/

BOOL CWorkThread::IsWorking()
{
if (!cs.Lock(1000))
  return TRUE;
BOOL bIsWorking = bProcessing;
cs.Unlock();
return bIsWorking;
}

/*****************************************************************************/
/* OnWorkthread : called to process an incoming buffer                       */
/*****************************************************************************/

void CWorkThread::OnWorkthread() 
{
// TRACE0("Work Thread beginning work\n");
pHost->OnSamples(pBuffers,              /* send converted buffer to VST Host */
                 nBufSize,
                 nChannels,
                 dwStamp);
cs.Lock(1000);
bProcessing = FALSE;
cs.Unlock();
// TRACE0("Work Thread ending work\n");
}
