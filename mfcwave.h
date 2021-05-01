/*****************************************************************************/
/* MFCWAVE.H : class definition for wave base classes                        */
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

#if !defined(_MFCWAVE_H__INCLUDED_)
#define _MFCWAVE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __cplusplus
#error mfcwave.h is for use with C++
#endif

/*****************************************************************************/
/* Necessary includes                                                        */
/*****************************************************************************/

/*****************************************************************************/
/* Wave definitions                                                          */
/*****************************************************************************/

#define WAVEOPENDEFAULT         ((DWORD)-1)
                                        /* default values:                   */
#define WAVEINHDRS      10              /* # Wave Input Buffer Headers       */
#define WAVEINBUFSIZE   4410L           /* length of Wave input buffer       */

#define WMAP_NAME  "Wave Mapper"

/*****************************************************************************/
/* CWaveBuffer : a wave buffer encapsulated                                  */
/*****************************************************************************/

class CWaveBuffer
{
public:
    CWaveBuffer(DWORD dwLen = 0);       /* constructors                      */
    CWaveBuffer(LPVOID lpBuf, DWORD dwLen);
    CWaveBuffer(LPWAVEHDR lpHdr);
    CWaveBuffer (CWaveBuffer const & org)
      { Init(); DoCopy(org); }
    virtual ~CWaveBuffer()              /* destructor                        */
      { Empty(); }

    operator LPWAVEHDR();               /* allow use in base calls           */

    DWORD Length();                     /* get data length                   */
                                        /* assignment operator               */
    CWaveBuffer & operator=(CWaveBuffer const &org) 
      { return DoCopy(org); }
    CWaveBuffer & operator=(LPWAVEHDR lphdr)
      { Set(lphdr); return *this; }
                                        /* set to input data                 */
    void Set(DWORD dwLen = 0);
    void Set(LPVOID lpBuf, DWORD dwLen);
    void Set(LPWAVEHDR lpHdr);
                                        /* set / get timestamp               */
    void SetStamp(DWORD dwTS) { dwStamp = dwTS; }
    DWORD GetStamp() { return dwStamp; }

protected:
    BOOL bAlloc;                        /* flag whether self-allocated       */
    LPWAVEHDR lpHdr;                    /* wave message header               */
    HGLOBAL hGlPtr;                     /* global pointer                    */
    DWORD dwStamp;                      /* timestamp                         */

protected:
    void Init();                        /* initialize memory                 */
    void Empty();                       /* empty memory                      */
                                        /* copy another wave buffer          */
    CWaveBuffer & DoCopy(CWaveBuffer const &org)
      {
      Empty();                          /* remove any previous contents      */

      if (org.lpHdr)                    /* if original has a buffer          */
                                        /* copy original's buffer            */
        Set(org.lpHdr->lpData, org.lpHdr->dwBufferLength);
      dwStamp = org.dwStamp;
      return *this;                     /* pass back pointer to ourselves    */
      }
};

/*****************************************************************************/
/* CWaveDevice : wave in/output device virtual base class                    */
/*****************************************************************************/

class CWaveDevice : public CWnd
{
// Construction
public:
    CWaveDevice();                      /* constructor                       */

// Attributes
public:
    BOOL IsOpen()                       /* return whether device is open     */
      { return bIsOpen; }
    LPCSTR DeviceName ( )               /* return opened device' name        */
      { return bIsOpen ? szDevName : ""; }

// Operations
public:
    virtual BOOL Open(LPCSTR szName,    /* open with device name             */
        LPWAVEFORMATEX pwfx = NULL,
        DWORD dwCallback = WAVEOPENDEFAULT,
        DWORD dwCallbackInstance = WAVEOPENDEFAULT,
        DWORD fdwOpen = CALLBACK_WINDOW);
    virtual BOOL Open(int iID,          /* open with device ID               */
        LPWAVEFORMATEX pwfx = NULL,
        DWORD dwCallback = WAVEOPENDEFAULT,
        DWORD dwCallbackInstance = WAVEOPENDEFAULT,
        DWORD fdwOpen = CALLBACK_WINDOW);
    virtual BOOL Reset() = 0;           /* reset device                      */
                                        /* prepare LPWAVEHDR                 */
    virtual BOOL PrepareHeader(LPWAVEHDR lpHdr) = 0;
                                        /* unprepare LPWAVEHDR               */
    virtual BOOL UnprepareHeader(LPWAVEHDR lpHdr) = 0;
    virtual BOOL Close();               /* close device                      */
    virtual BOOL GetPosition(LPMMTIME pmmt,
                             UINT cbmmt = sizeof(MMTIME)) = 0;
    virtual DWORD Message(UINT msg,     /* send special message              */
                          DWORD dw1,
                          DWORD dw2) = 0;

    virtual void OnEvent() = 0;         /* event handler                     */
    void SetupWaveformat(LPWAVEFORMATEX pwf, DWORD dwSamplesPerSec = 44100, WORD wBitsPerSample = 16, WORD nChannels = 2);

// Attributes

    WORD FormatTag()
      { return (IsOpen()) ? wf.wFormatTag : 0; }
    int Channels()
      { return (IsOpen()) ? (int)wf.nChannels : 0; }
    int SamplesPerSecond()
      { return (IsOpen()) ? (int)wf.nSamplesPerSec : 0; }
    int AvgBytesPerSecond()
      { return (IsOpen()) ? (int)wf.nAvgBytesPerSec : 0; }
    int BlockAlignment()
      { return (IsOpen()) ? (int)wf.nBlockAlign : 0; }
    int Bits()
      { return (IsOpen()) ? (int)wf.wBitsPerSample : 0; }
    LPVOID AdditionalFormatData()
      { return ((IsOpen()) && (wf.cbSize)) ? (LPVOID)(&wf + 1) : 0; }

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CWaveDevice)
    //}}AFX_VIRTUAL

// Implementation
public:
    BOOL BoostPriority();
    void SetSample(CWaveBuffer &buf, int nSample, int nChannel, short sValue);
    short GetSample(CWaveBuffer &buf, int nSample, int nChannel);
    int GetSampleCount(CWaveBuffer &buf) { return BlockAlignment() ? buf.Length() / BlockAlignment() : 0; }
    virtual ~CWaveDevice();             /* destructor                        */

    // Generated message map functions
protected:
    //{{AFX_MSG(CWaveDevice)
        // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

protected :
    BOOL bIsOpen;                       /* flag whether opened               */
    WAVEFORMATEX wf;                    /* format for open                   */

protected:
    HANDLE hEvent;                      /* event if using event-based logic  */
    CWinThread * pEvtThread;            /* event thread procedure            */
    BOOL volatile bEvtCancel;           /* event thread cancel flag          */

    BOOL Create();

    static UINT EvtThreadProc(LPVOID pParam);
    BOOL StartEvt();
    void KillEvt();
    void AssureWnd()
      {
      if (!m_hWnd)                      /* if no notification window yet     */
        {
        Create();                       /* create it                         */
        ASSERT(m_hWnd);
        }
      }

    char szDevName[MAXPNAMELEN];        /* device name                       */
};

/*****************************************************************************/
/* CWaveInDeviceList : Wave Input device list                                */
/*****************************************************************************/

class CWaveInDeviceList : public CStringArray
{
public :
    CWaveInDeviceList ( );              /* constructor                       */
};

/*****************************************************************************/
/* CWaveInDevice : Wave Input Device                                         */
/*****************************************************************************/

class CWaveInDevice : public CWaveDevice
{
public:
    CWaveInDevice();                    /* constructor                       */
    virtual ~CWaveInDevice();           /* destructor                        */

// operations
public:
    virtual BOOL Open(LPCSTR szName,    /* open with device name             */
        LPWAVEFORMATEX pwfx = NULL,
        DWORD dwCallback = WAVEOPENDEFAULT,
        DWORD dwCallbackInstance = WAVEOPENDEFAULT,
        DWORD fdwOpen = CALLBACK_WINDOW);
    virtual BOOL Open(int iID,          /* open with device ID               */
        LPWAVEFORMATEX pwfx = NULL,
        DWORD dwCallback = WAVEOPENDEFAULT,
        DWORD dwCallbackInstance = WAVEOPENDEFAULT,
        DWORD fdwOpen = CALLBACK_WINDOW);
    virtual BOOL Start();               /* start recording                   */
    virtual BOOL Stop();                /* stop recording                    */
                                        /* prepare LPWAVEHDR                 */
    virtual BOOL PrepareHeader(LPWAVEHDR lpHdr);
                                        /* unprepare LPWAVEHDR               */
    virtual BOOL UnprepareHeader(LPWAVEHDR lpHdr);
    virtual BOOL Reset();               /* reset device                      */
    virtual BOOL Close();               /* close device                      */
    virtual BOOL GetPosition(LPMMTIME pmmt,
                             UINT cbmmt = sizeof(MMTIME));
    virtual DWORD Message(UINT msg,     /* send special message              */
        DWORD dw1, DWORD dw2);

    virtual void OnEvent();             /* incoming event                    */
                                        /* process incoming data             */
    virtual void Data(CWaveBuffer &buf) { }

    virtual void OnWimOpen() { }
    virtual void OnWimClose() { }
    virtual void OnWimData(LPWAVEHDR lphdr);

    void SetupAllocSize(int nCount = WAVEINHDRS, int nSize = WAVEINBUFSIZE)
      { nAllocCount = nCount; nAllocSize = nSize; }
                                        /* allocate buffer                   */
    BOOL AllocBuf(int nCount = WAVEINHDRS, int nSize = WAVEINBUFSIZE);
    BOOL FreeBuf();                     /* free allocated buffer             */
    BOOL PrepareBuf();                  /* prepare buffer for usage          */
    BOOL UnprepareBuf();                /* unprepare buffer from usage       */

    BOOL IsRecording()
      { return bRecording; }
// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CWaveInDevice)
    //}}AFX_VIRTUAL

    // Generated message map functions
protected:
    //{{AFX_MSG(CWaveInDevice)
    afx_msg LRESULT OnWimOpen(WPARAM w, LPARAM l);
    afx_msg LRESULT OnWimClose(WPARAM w, LPARAM l);
    afx_msg LRESULT OnWimData(WPARAM w, LPARAM l);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

protected:
    WAVEINCAPS wCap;                    /* input device capabilities         */
    BOOL bClosing;                      /* flag whether currently closing    */
    HWAVEIN hDev;                       /* input device handle               */
    BOOL bRecording;                    /* flag whether currently recording  */
    DWORD dwStamp;                      /* internal start time stamp         */
    HGLOBAL hGlobal;                    /* global memory for SysEx buffers   */
    char * pGlobal;                     /* same as locked pointer            */
    LPWAVEHDR *lpGlHdr;                 /* Wave input buffers                */
    int nAllocated;                     /* # allocated input buffers         */
    int nAllocCount;                    /* # buffers to be alloc'd in open   */
    int nAllocSize;                     /* size of each of these             */

protected:
    int PreparedBuffers();
    BOOL AddHeader(LPWAVEHDR lpHdr);    /* add MIDI buffer to device         */
    BOOL CheckHeader(LPWAVEHDR lpHdr);  /* check whether it's one of ours    */
};

/*****************************************************************************/
/* CWaveOutDeviceList : Wave Output device list                              */
/*****************************************************************************/

class CWaveOutDeviceList : public CStringArray
  {
  public :
    CWaveOutDeviceList ( );             /* constructor                       */
  };

/*****************************************************************************/
/* CWaveOutDevice : Wave Output device                                       */
/*****************************************************************************/

class CWaveOutDevice : public CWaveDevice
{
public:
    CWaveOutDevice();                   /* constructor                       */
    virtual ~CWaveOutDevice();          /* destructor                        */

// operations
public:
    virtual BOOL Open(LPCSTR szName,    /* open with device name             */
        LPWAVEFORMATEX pwfx = NULL,
        DWORD dwCb = WAVEOPENDEFAULT,
        DWORD dwCbInst = WAVEOPENDEFAULT,
        DWORD fdwOpen = CALLBACK_WINDOW);
    virtual BOOL Open(int iID,          /* open with device ID               */
        LPWAVEFORMATEX pwfx = NULL,
        DWORD dwCb = WAVEOPENDEFAULT,
        DWORD dwCbInst = WAVEOPENDEFAULT,
        DWORD fdwOpen = CALLBACK_WINDOW);
    virtual BOOL Reset();
    virtual BOOL PrepareHeader(LPWAVEHDR lpHdr);
    virtual BOOL UnprepareHeader(LPWAVEHDR lpHdr);
    virtual BOOL Pause();
    virtual BOOL Restart();
    virtual BOOL BreakLoop();
    virtual BOOL Close();
    virtual BOOL Output(CWaveBuffer &Buf, BOOL bSync = TRUE);
    virtual BOOL GetPosition(LPMMTIME pmmt,
                             UINT cbmmt = sizeof(MMTIME));
    virtual BOOL GetPitch(LPDWORD pdwPitch);
    virtual BOOL SetPitch(DWORD dwPitch);
    virtual BOOL GetPlaybackRate(LPDWORD pdwPlaybackRate);
    virtual BOOL SetPlaybackRate(DWORD dwPlaybackRate);
    virtual BOOL GetVolume(LPDWORD pdwVolume);
    virtual BOOL SetVolume(DWORD dwVolume);

    virtual DWORD Message(UINT msg, DWORD dw1, DWORD dw2);

    virtual void OnEvent() {}           /* incoming event                    */
    virtual void OnWomOpen() {}
    virtual void OnWomClose() {}
    virtual void OnWomDone(LPWAVEHDR lphdr)
      {
      // this MUST be overridden if asynchronous processing is done!
      // in this case, a call to Unprepare() must be done here!
      }

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CWaveOutDevice)
    //}}AFX_VIRTUAL

protected:
    HWAVEOUT hDev;                      /* output device handle              */
    WAVEOUTCAPS wCap;                   /* output device capabilities        */

    //{{AFX_MSG(CWaveOutDevice)
    afx_msg LRESULT OnWomOpen(WPARAM w, LPARAM l);
    afx_msg LRESULT OnWomDone(WPARAM w, LPARAM l);
    afx_msg LRESULT OnWomClose(WPARAM w, LPARAM l);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

protected:
    WAVEOUTCAPS wc;
    BOOL SendingDone(LPWAVEHDR lpHdr);
    static void CALLBACK WaveOutProc
        (
        HMIDIOUT hWaveOut,
        UINT wMsg,
        DWORD dwInstance,
        DWORD dwParam1,
        DWORD dwParam2
        );

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(_MFCWAVE_H__INCLUDED_)
