/*****************************************************************************/
/* DSoundHost.h: DirectSound host functionality                              */
/*****************************************************************************/
/* This class hierarchy implements a double-buffering record/model based on  */
/* Windows' DirectSound                                                      */
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


#if !defined(AFX_DSOUNDHOST_H__2007F5D3_8DE6_11D8_8219_4000001054B2__INCLUDED_)
#define AFX_DSOUNDHOST_H__2007F5D3_8DE6_11D8_8219_4000001054B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*****************************************************************************/
/* CDSoundDeviceList class declaration                                       */
/*****************************************************************************/

class CDSoundDeviceList : public CStringArray  
{
public:
	CDSoundDeviceList();
	virtual ~CDSoundDeviceList();

    LPGUID GetGUID(int nDevNum);
    LPGUID GetGUID(LPCSTR lpszDesc);
    CString GetName(int nDevNum);
    CString GetName(LPCSTR lpszDesc);

protected:
	static BOOL CALLBACK EnumDevices(LPGUID lpGUID, LPCTSTR lpszDesc, LPCTSTR lpszDrvName, LPVOID lpContext);
    CPtrArray Guids;
    CStringArray Names;
};


/*****************************************************************************/
/* CDSoundInputList class declaration                                        */
/*****************************************************************************/

class CDSoundInputList : public CDSoundDeviceList 
{
public:
	CDSoundInputList();
};


/*****************************************************************************/
/* CDSoundOutputList class declaration                                       */
/*****************************************************************************/

class CDSoundOutputList : public CDSoundDeviceList 
{
public:
	CDSoundOutputList();
};

/*****************************************************************************/
/* CDSoundDevice class declaration                                           */
/*****************************************************************************/

class CDSoundDevice
{
public:
    CDSoundDevice();
    virtual ~CDSoundDevice();

	BOOL BoostPriority();

    BOOL IsOpen()                       /* return whether device is open     */
      { return bIsOpen; }
    LPCSTR DeviceName ( )               /* return opened device' name        */
      { return bIsOpen ? szDevName : ""; }
	void SetupWaveformat(LPWAVEFORMATEX pwf, DWORD dwSamplesPerSec = 44100, WORD wBitsPerSample = 16, WORD nChannels = 2);
    DWORD GetBufferSize() { return sbpn[ePosMiddle].dwOffset; }

    virtual BOOL Open(LPCSTR lpszDesc,
                      DWORD dwFlags,
                      DWORD dwSamples, /* size of ONE buffer of double-buff */
                      LPWAVEFORMATEX pwfx);
    virtual BOOL Close();

    virtual BOOL Start(DWORD dwFlags = -1) { return FALSE; }
    virtual BOOL IsStarted() { return FALSE; }
    virtual BOOL Stop() { return TRUE; }
    virtual DWORD GetDriverPos() { return 0; }
    virtual DWORD GetDriverBuf() { return (GetDriverPos() >= GetBufferSize()); }
    virtual DWORD GetBufferPos() { return 0; }
    virtual BOOL GetFormat(LPWAVEFORMATEX lpWF) { return FALSE; }

    virtual BOOL ReadBuffer(int nBuffer, LPVOID lpBuf, DWORD &rdwSamples);
    virtual BOOL WriteBuffer(int nBuffer, LPVOID lpBuf, DWORD &rdwSamples);

    virtual void OnOpen() { }
    virtual void OnSwitch(int nBuffer) { }
    virtual void OnStop() { }

protected:
    char szDevName[MAXPNAMELEN];        /* device name                       */
    BOOL bIsOpen;                       /* flag whether opened               */
    WAVEFORMATEX wf;                    /* format for open                   */
    DSCBUFFERDESC sbd;                  /* secondary buffer description      */
    enum                                /* notification positions:           */
      {
      ePosStart = 0,                    /* Start of Buffer                   */
      ePosMiddle,                       /* Middle of Buffer                  */
      ePosStop,                         /* Playback / Capture Stop           */
      ePosMax
      };
    DSBPOSITIONNOTIFY sbpn[ePosMax];    /* sec.buffer pos. notifications     */
    HANDLE hEvent;                      /* event handle for double-buffering */
	CWinThread * pEvtThread;            /* event thread procedure            */
    BOOL volatile bEvtCancel;           /* event thread cancel flag          */

protected:
    void SetupSBDesc(LPDSCBUFFERDESC pbd, DWORD dwFlags = 0, DWORD dwSamples = 0, LPWAVEFORMATEX pwf = NULL);
    virtual BOOL Open(LPGUID lpGuid);

    virtual BOOL Lock(DWORD dwCursor, DWORD dwBytes, LPVOID *lplpvPtr1, LPDWORD lpdwBytes1, LPVOID *lplpvPtr2 = NULL, LPDWORD lpdwBytes2 = 0, DWORD dwFlags = 0) { return FALSE; }
    virtual BOOL Unlock(LPVOID lpPtr1, DWORD dwBytes1, LPVOID lpPtr2 = NULL, DWORD dwBytes2 = 0) { return FALSE; }

    virtual LPGUID GetGuid(LPCSTR lpszDesc) { return NULL; }
    virtual void Release() { }

    static UINT EvtThreadProc(LPVOID pParam);
	BOOL StartEvt();
    void KillEvt();

};

/*****************************************************************************/
/* CDSoundInDevice class declaration                                         */
/*****************************************************************************/

class CDSoundInDevice : public CDSoundDevice
{
public:
    CDSoundInDevice();
    virtual ~CDSoundInDevice();

    // base class overrides
    virtual DWORD GetDriverPos();       /* get driver's current position     */
    virtual DWORD GetBufferPos();       /* get our read position             */
    virtual BOOL Start(DWORD dwFlags = -1);  /* start recording              */
    virtual BOOL IsStarted();           /* return whether started            */
    virtual BOOL Stop();                /* stop recording                    */
    virtual BOOL GetFormat(LPWAVEFORMATEX lpWF);

    virtual BOOL Open(LPCSTR lpszDesc,
                      DWORD dwFlags,
                      DWORD dwSamples, /* size of ONE buffer of double-buff */
                      LPWAVEFORMATEX pwfx)
      { return CDSoundDevice::Open(lpszDesc, dwFlags, dwSamples, pwfx); }
    virtual BOOL Close();

    // input-specific functionality
    virtual BOOL GetCaps(LPDSCBCAPS lpDSCBCaps);

protected:
    LPDIRECTSOUNDCAPTURE lpDS;
    LPDIRECTSOUNDCAPTUREBUFFER lpDSB;
    LPDIRECTSOUNDNOTIFY lpDSN;
    GUID guid;                          /* help buffer                       */

protected:
    virtual LPGUID GetGuid(LPCSTR lpszDesc);
    virtual BOOL Open(LPGUID lpGuid);
    virtual BOOL Lock(DWORD dwCursor, DWORD dwBytes, LPVOID *lplpvPtr1, LPDWORD lpdwBytes1, LPVOID *lplpvPtr2 = NULL, LPDWORD lpdwBytes2 = 0, DWORD dwFlags = 0);
    virtual BOOL Unlock(LPVOID lpPtr1, DWORD dwBytes1, LPVOID lpPtr2 = NULL, DWORD dwBytes2 = 0);
    virtual void Release();

};

/*****************************************************************************/
/* CDSoundOutDevice class declaration                                        */
/*****************************************************************************/

class CDSoundOutDevice : public CDSoundDevice
{
public:
    CDSoundOutDevice();
    virtual ~CDSoundOutDevice();

    // base class overrides
    virtual DWORD GetDriverPos();       /* get driver's current position     */
    virtual BOOL SetDriverPos(DWORD dwPos);
    virtual DWORD GetBufferPos();       /* get our write position            */
    virtual BOOL Start(DWORD dwFlags = -1);  /* start playing                */
    virtual BOOL IsStarted();           /* return whether started            */
    virtual BOOL Stop();                /* stop playing                      */
    virtual BOOL GetFormat(LPWAVEFORMATEX lpWF);

    virtual BOOL Open(LPCSTR lpszDesc,
                      DWORD dwFlags,
                      DWORD dwSamples, /* size of ONE buffer of double-buff */
                      LPWAVEFORMATEX pwfx)
      { return CDSoundDevice::Open(lpszDesc, dwFlags, dwSamples, pwfx); }
    virtual BOOL Close();

    // output-specific functionality
    virtual BOOL GetCaps(LPDSBCAPS lpDSBufferCaps);
    virtual BOOL GetVolume(LPLONG lpVolume);
    virtual BOOL SetVolume(LONG lVolume);
    virtual BOOL GetPan(LPLONG lpPan);
    virtual BOOL SetPan(LONG lPan);

protected:
    LPDIRECTSOUND lpDS;
    LPDIRECTSOUNDBUFFER lpDSB;
    LPDIRECTSOUNDNOTIFY lpDSN;
    GUID guid;                          /* help buffer                       */

protected:
    virtual LPGUID GetGuid(LPCSTR lpszDesc);
    virtual BOOL Open(LPGUID lpGuid);
    virtual BOOL Lock(DWORD dwCursor, DWORD dwBytes, LPVOID *lplpvPtr1, LPDWORD lpdwBytes1, LPVOID *lplpvPtr2 = NULL, LPDWORD lpdwBytes2 = 0, DWORD dwFlags = 0);
    virtual BOOL Unlock(LPVOID lpPtr1, DWORD dwBytes1, LPVOID lpPtr2 = NULL, DWORD dwBytes2 = 0);
    virtual void Release();
};

#endif // !defined(AFX_DSOUNDHOST_H__2007F5D3_8DE6_11D8_8219_4000001054B2__INCLUDED_)
