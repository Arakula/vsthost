/*****************************************************************************/
/* MFCMIDI.H : class definition for MIDI base classes                        */
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

#ifndef __INCLUDE_MFCMIDI_H__
#define __INCLUDE_MFCMIDI_H__

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif // _MSC_VER > 1000

#ifndef __cplusplus
#error mfcmidi.h is for use with C++
#endif

/*****************************************************************************/
/* Necessary includes                                                        */
/*****************************************************************************/

/*****************************************************************************/
/* MIDI definitions                                                          */
/*****************************************************************************/

#define ISSTATUS(bData)         ((bData) & 0x80)
#define FILTERCHANNEL(bStatus)  ((BYTE)((bStatus) & 0xf0))
#define FILTERSTATUS(bStatus)   ((BYTE)((bStatus) & 0x0f))

#define STATUS_NOTEOFF          0x80
#define STATUS_NOTEON           0x90
#define STATUS_POLYPHONICKEY    0xa0
#define STATUS_CONTROLCHANGE    0xb0
#define STATUS_PROGRAMCHANGE    0xc0
#define STATUS_CHANNELPRESSURE  0xd0
#define STATUS_PITCHBEND        0xe0

#define STATUS_SYS              0xf0
#define STATUS_SYSEX            0xf0
#define STATUS_QFRAME           0xf1
#define STATUS_SONGPOINTER      0xf2
#define STATUS_SONGSELECT       0xf3
#define STATUS_F4               0xf4
#define STATUS_F5               0xf5
#define STATUS_TUNEREQUEST      0xf6
#define STATUS_EOX              0xf7
#define STATUS_TIMINGCLOCK      0xf8
#define STATUS_F9               0xf9
#define STATUS_START            0xfa
#define STATUS_CONTINUE         0xfb
#define STATUS_STOP             0xfc
#define STATUS_FD               0xfd
#define STATUS_ACTIVESENSING    0xfe
#define STATUS_SYSTEMRESET      0xff

#define OPENDEFAULT             ((DWORD)-1)

#define MAX_DETACHED            1024    /* max.# of detached MIDIHDRs        */
                                        /* size of the global ringbuffer     */
#define MIDIIN_RINGSIZE         (16 * 4096)

/*****************************************************************************/
/* CMidiMsg : a MIDI message                                                 */
/*****************************************************************************/

class CMidiMsg
  {
  public:
    CMidiMsg(DWORD smsg = 0);
    CMidiMsg(int b1, int b2, int b3=0);
    CMidiMsg(LPMIDIHDR lphdr);
    CMidiMsg(LPSTR data, int len);
    CMidiMsg (CMidiMsg const & org)
      { Init(); DoCopy(org); }
//    virtual ~CMidiMsg()               /* virtual destructor not needed     */
                                        /* as there are no derived classes   */
    ~CMidiMsg()                         /* destructor                        */
       { Empty(); }
                                        /* assignment operator               */
    CMidiMsg & operator=(CMidiMsg const &org) 
      { return DoCopy(org); }
    CMidiMsg & operator=(DWORD smsg)
      { Set(smsg); return *this; }
    CMidiMsg & operator=(LPMIDIHDR lphdr)
      { Set(lphdr); return *this; }
    bool operator==(CMidiMsg &other)
      {
      return ((other.Length() == Length()) &&
              (!memcmp((BYTE *)other, (BYTE *)*this, Length())));
      }
                                        /* set to MIDI message               */
    void Set(DWORD smsg = 0);
    void Set(LPMIDIHDR lphdr);
    void Set(LPSTR data, int len);
    void Set(int b1, int b2, int b3);
    void SetStamp(DWORD dwTS = 0) { dwStamp = dwTS; }
#ifdef _DEBUG
    void SetMsg(UINT wMsg) { this->wMsg = wMsg; }
#endif
                                        /* convert to other data             */
    operator char*();
    operator BYTE*();
    operator DWORD();
    operator LPMIDIHDR();
                                        /* get x-th byte of message          */
    BYTE & operator [](int idx) const
      { return ((CMidiMsg *)this)->GetAt(idx); }
    BYTE & GetAt(int idx)
      {
      if (!lpHdr)
        return ((BYTE *)&sMsg)[idx];
      return ((BYTE *)lpHdr->lpData)[idx];
      }
    WORD Length();                      /* get message length                */
    static BYTE Length(BYTE bStatus);   /* get length of a short message     */
                                        /* get timestamp                     */
    DWORD GetStamp() { return dwStamp; }
    void Output(HMIDIOUT hOut)          /* remember output device            */
      { sMsg = (DWORD)hOut; }
    static int CleanupDetached();       /* clean up detached messages        */
#ifdef _DEBUG
    UINT GetMsg() { return wMsg; }      /* get message code                  */
    LPCSTR GetMsgText()
      {
      static LPCSTR lpszMsgs[] =
        {
        "MIM_OPEN",
        "MIM_CLOSE",
        "MIM_DATA",
        "MIM_LONGDATA",
        "MIM_ERROR",
        "MIM_LONGERROR",
        "MOM_OPEN",
        "MOM_CLOSE",
        "MOM_DONE",
        "MOM_POSITIONCB",
        "MCISIGNAL",
        "MIM_MOREDATA"
        };
      int nIdx = wMsg - MM_MIM_OPEN;
      if ((nIdx < 0) || (nIdx >= (sizeof(lpszMsgs) / sizeof(lpszMsgs[0]))))
        return "?";
      return lpszMsgs[nIdx];
      }
#endif

  protected :
    BOOL bAlloc;                        /* flag whether self-allocated buffer*/
    DWORD sMsg;                         /* short message                     */
    LPMIDIHDR lpHdr;                    /* long message header               */
    HGLOBAL hGlPtr;                     /* global pointer                    */
    DWORD dwStamp;                      /* time stamp                        */
#ifdef _DEBUG
    UINT wMsg;                          /* message code (MM_)MIM_... or 0    */
#endif
                                        /* detached MIDI message handling    */
    static LPMIDIHDR lpDetached[MAX_DETACHED];
    static HMIDIOUT hdDetached[MAX_DETACHED];

  protected:
    void Init();                        /* initialize memory                 */
    void Empty();                       /* clear memory                      */
                                        /* copy another MIDI message         */
    CMidiMsg & DoCopy(CMidiMsg const &org)
      {
      Empty();                          /* remove any previous contents      */

      sMsg = org.sMsg;                  /* store short message               */
      if (org.lpHdr)                    /* if original has a buffer          */
                                        /* copy original's buffer            */
        Set(org.lpHdr->lpData, (int)org.lpHdr->dwBufferLength);
      dwStamp = org.dwStamp;
#ifdef _DEBUG
      wMsg = org.wMsg;
#endif
      return *this;                     /* pass back pointer to ourselves    */
      }
    void FreeHdr()
      {
      GlobalUnlock(hGlPtr);
      GlobalFree(hGlPtr);
      lpHdr = NULL;
      hGlPtr = NULL;
      }
    void DetachHdr();
      
  };

/*****************************************************************************/
/* CMidiDevice : MIDI Device base class                                      */
/*****************************************************************************/

class CMidiDevice : public CObject
  {
// Attributes
  public:
    CMidiDevice();                      /* constructor                       */
    ~CMidiDevice();                     /* destructor                        */

    BOOL IsOpen()                       /* return whether device is open     */
      { return bIsOpen; }
    LPSTR DeviceName ( )                /* return opened device' name        */
      { return bIsOpen ? szDevName : ""; }

// Operations
     virtual BOOL Open(LPCSTR szName,   /* open with device name             */
         DWORD lp1 = OPENDEFAULT,
         DWORD lp2 = OPENDEFAULT,
         DWORD dwFlags = CALLBACK_WINDOW);
     virtual BOOL Open(int iID,         /* open with device ID               */
         DWORD lp1 = OPENDEFAULT,
         DWORD lp2 = OPENDEFAULT,
         DWORD dwFlags = CALLBACK_WINDOW);
     virtual void OnOpen() {}
     virtual BOOL Reset()               /* reset device                      */
       { return TRUE; }
     virtual BOOL Close();              /* close device                      */
     virtual void OnClose() {}
     virtual DWORD Message(UINT msg,    /* send special message              */
        DWORD dw1, DWORD dw2)
       { return 0; }
     virtual LPCSTR GetErrorText(MMRESULT wError)
       { return ""; }

protected:

// Implementation
  private :
    BOOL bIsOpen;                       /* flag whether opened               */

  protected :
    BOOL Create();

    char szDevName[MAXPNAMELEN];        /* device name                       */
    char szErrorText[256];              /* error text buffer                 */
  };

/*****************************************************************************/
/* CMidiInDeviceList : MIDI Input device list                                */
/*****************************************************************************/

class CMidiInDeviceList : public CStringArray
  {
  typedef CStringArray inherited;       /* base class                        */

  public :
    CMidiInDeviceList() { Load(); }     /* constructor                       */
    virtual ~CMidiInDeviceList();
    LPMIDIINCAPS GetCaps(int nID)
      { return (LPMIDIINCAPS) paCaps.GetAt(nID); }
    void Load();                        /* (re-)load list                    */
  protected:
    void Unload();
    CPtrArray paCaps;
  };

/*****************************************************************************/
/* CMidiInDevice : MIDI Input device                                         */
/*****************************************************************************/

class CMidiInDevice : public CMidiDevice
  {
// Attributes
  public:
    BOOL IsRecording();

// Operations
public:
    CMidiInDevice();
    virtual BOOL Open(LPCSTR szName,    /* open with device name             */
        DWORD lp1 = OPENDEFAULT,
        DWORD lp2 = OPENDEFAULT,
        DWORD dwFlags = CALLBACK_FUNCTION);
    virtual BOOL Open(int iID,          /* open with device ID               */
        DWORD lp1 = OPENDEFAULT,
        DWORD lp2 = OPENDEFAULT,
        DWORD dwFlags = CALLBACK_FUNCTION);
    virtual BOOL Start();               /* start recording                   */
    virtual BOOL Stop();                /* stop recording                    */
    virtual BOOL Reset();               /* reset device                      */
    virtual BOOL Close();               /* close device                      */
    virtual DWORD Message(UINT msg,     /* send special message              */
        DWORD dw1, DWORD dw2);
                                        /* process incoming data             */
    virtual void Data(CMidiMsg &Msg);
    virtual void OnMimOpen() { }
    virtual void OnMimClose() { }
    virtual void OnMimError(CMidiMsg &Msg) { Data(Msg); }
    virtual void OnMimError(LPMIDIHDR lphdr, DWORD dwTS = 0);
    virtual void OnMimLongData(LPMIDIHDR lphdr, DWORD dwTS);
    virtual void OnMimMoreData(CMidiMsg &Msg) { }
    virtual void NoteOn(int channel, int note, int vel) { }
    virtual void NoteOff(int channel, int note, int vel = 64) { }
    virtual void KeyAftertouch(int channel, int note, int vel) { }
    virtual void ChannelAftertouch(int channel, int vel) { }
    virtual void Controller(int channel, int ctrl, int val) { }
    virtual void Patch(int channel, int patch) { }
    virtual void PitchBend(int channel, int val) { }
    virtual void SysEx(CMidiMsg &msg) { }
    virtual void SystemReset() { };
    virtual BOOL GetDevCaps(LPMIDIINCAPS pmCap);
    virtual LPCSTR GetErrorText(MMRESULT wError);

    static  BOOL GetDevCaps(UINT iID, LPMIDIINCAPS pmCap);
    DWORD GetStamp() { return ::timeGetTime() - dwStamp; }

// Implementation
public:
    virtual ~CMidiInDevice();

protected:
    HMIDIIN hDev;                       /* input device handle               */
    HGLOBAL hGlobal;                    /* global memory for SysEx buffers   */
    char * pGlobal;                     /* same as locked pointer            */
    LPMIDIHDR *lpGlHdr;                 /* MIDI input buffers                */
    volatile BOOL bRecording;           /* flag whether currently recording  */
    DWORD dwStamp;                      /* internal start time stamp         */
    MIDIINCAPS mCap;                    /* input device capabilities         */
    volatile BOOL bClosing;             /* flag whether currently closing    */

    static int nInDevs;                 /* # currently loaded CMidiInDevices */
    static HGLOBAL hGlRing;             /* handle for the ring buffer        */
    static LPBYTE pRing;                /* ring buffer for all devices       */
    static volatile LPBYTE pRingRead;   /* current read pointer              */
    static volatile LPBYTE pRingWrite;  /* current write pointer             */

    static HANDLE hEvt;                 /* event to trigger                  */
	static CWinThread * pEvtThread;     /* event thread procedure            */
    static bool volatile bKillThread;   /* thread kill flag                  */
    static UINT EvtThreadProc(LPVOID pParam);

protected:
    BOOL AddHeader(LPMIDIHDR lpHdr);    /* add MIDI buffer to device         */
    BOOL CheckHeader(LPMIDIHDR lpHdr);  /* check whether it's one of ours    */
    BOOL AllocBuf();                    /* allocate buffer                   */
    BOOL FreeBuf();                     /* free allocated buffer             */
    BOOL PrepareBuf();                  /* prepare buffer for usage          */
    BOOL UnprepareBuf();                /* unprepare buffer from usage       */
    int PreparedBuffers();              /* # currently prepared buffers      */

    static void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg,
                                    DWORD dwInstance,
                                    DWORD dwParam1, DWORD dwParam2);
    static void ProcessMessage(UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
    bool StartThread();                 /* start work thread                 */
    bool StopThread();                  /* stop work thread                  */

public:
// Overrides

protected:
  };

/*****************************************************************************/
/* CMidiOutDeviceList : MIDI Output device list                              */
/*****************************************************************************/

class CMidiOutDeviceList : public CStringArray
  {
  typedef CStringArray inherited;       /* base class                        */

  public :
    CMidiOutDeviceList() { Load(); }    /* constructor                       */
    virtual ~CMidiOutDeviceList();
    LPMIDIOUTCAPS GetCaps(int nID)
      { return (LPMIDIOUTCAPS) paCaps.GetAt(nID); }
    void Load();                        /* (re-)load device list             */
  protected:
    void Unload();
    CPtrArray paCaps;
  };

/*****************************************************************************/
/* CMidiOutDevice : MIDI Output device                                       */
/*****************************************************************************/

class CMidiOutDevice : public CMidiDevice
  {
// Attributes
public:

// Operations
public:
    CMidiOutDevice();
    virtual BOOL Open(LPCSTR szName,    /* open with device name             */
        DWORD lp1 = OPENDEFAULT,
        DWORD lp2 = OPENDEFAULT,
        DWORD dwFlags = CALLBACK_FUNCTION);
    virtual BOOL Open(int iID,          /* open with device ID               */
        DWORD lp1 = OPENDEFAULT,
        DWORD lp2 = OPENDEFAULT,
        DWORD dwFlags = CALLBACK_FUNCTION);
    virtual BOOL Reset();
    virtual BOOL Close();
    virtual void OnMomClose() {}
    virtual void OnMomDone(LPMIDIHDR lphdr) { CMidiMsg::CleanupDetached(); }
    virtual void OnMomOpen() {}
    virtual DWORD Message(UINT msg, DWORD dw1, DWORD dw2);
    virtual BOOL CachePatches(UINT uBank,
        WORD FAR* lpwPatchArray, UINT uFlags);
    virtual BOOL CacheDrumPatches(UINT uPatch,
        WORD FAR* lpwKeyArray, UINT uFlags);
    virtual BOOL Output(CMidiMsg &Msg, BOOL bSync = TRUE);
    virtual BOOL NoteOn(int channel, int note, int vel);
    virtual BOOL NoteOff(int channel, int note, int vel = 64);
    virtual BOOL KeyAftertouch(int channel, int note, int vel);
    virtual BOOL ChannelAftertouch(int channel, int vel);
    virtual BOOL Controller(int channel, int ctrl, int val);
    virtual BOOL Controller14(int channel, int ctrl, int val);
    virtual BOOL Patch(int channel, int patch);
    virtual BOOL PitchBend(int channel, int val);
    virtual BOOL SysEx(CMidiMsg &msg) { return Output(msg); }
    virtual BOOL SystemReset();
    virtual BOOL Bank(int channel, int bank);
    virtual BOOL Bank14(int channel, int bank);
    virtual BOOL GetDevCaps(LPMIDIOUTCAPS pmCap);
    virtual LPCSTR GetErrorText(MMRESULT wError);

    static  BOOL GetDevCaps(UINT iID, LPMIDIOUTCAPS pmCap);

// Implementation
public:
    virtual ~CMidiOutDevice();

protected:
    HMIDIOUT hDev;                      /* output device handle              */
    MIDIOUTCAPS mCap;                   /* output device capabilities        */
    BYTE RunningStatus;                 /* running status                    */

protected:
    BOOL SendingDone(LPMIDIHDR lpHdr);
  };

#endif                                  /* __INCLUDE_MFCMIDI_H__             */
