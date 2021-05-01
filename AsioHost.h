/*****************************************************************************/
/* AsioHost.h: Interface for class CAsioHost.                                */
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


#if !defined(AFX_ASIOHOST_H__ADAC91FD_B25E_4D5D_ABE8_A39B9B575920__INCLUDED_)
#define AFX_ASIOHOST_H__ADAC91FD_B25E_4D5D_ABE8_A39B9B575920__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum
  {
  // number of input and outputs supported by the host application
  // you can change these to higher or lower values
  kMaxInputChannels = 32,
  kMaxOutputChannels = 32
  };


// should be rewritten to be derived from class AsioDriver...
// as this is a VERY simple implementation, we use the basic
// C functions instead

class CAsioHost : public AsioDrivers
{
public:
	CAsioHost();
	virtual ~CAsioHost();

public:
	virtual bool LoadDriver(char *sDriver, int &nBufSz);
    virtual bool UnloadDriver();
    bool IsLoaded() { return bLoaded; }
    virtual void *GetSysRef() { return 0; }
    // for a derived class using MFC under Windows, this 
    // call might return AfxGetMainWnd()->m_hWnd instead

// basic ASIO functionality
    bool Init();
    bool Exit() { return bLoaded ? (ASIOExit() == ASE_OK) : true; }
    bool Start();
    bool Stop();
    bool GetChannels(long *numInputChannels, long *numOutputChannels)
      { return (ASIOGetChannels(numInputChannels, numOutputChannels) == ASE_OK); }
    bool GetLatencies(long *inputLatency, long *outputLatency)
      { return (ASIOGetLatencies(inputLatency, outputLatency) == ASE_OK); }
    bool GetBufferSize(long *minSize, long *maxSize, long *preferredSize, long *granularity)
      { return (ASIOGetBufferSize(minSize, maxSize, preferredSize, granularity) == ASE_OK); }
    bool CanSampleRate(ASIOSampleRate sRate);
    bool GetSampleRate(ASIOSampleRate *currentRate)
      { return (ASIOGetSampleRate(currentRate) == ASE_OK); }
    bool SetSampleRate(ASIOSampleRate sampleRate)
      { return (ASIOSetSampleRate(sampleRate) == ASE_OK); }
    bool GetClockSources(ASIOClockSource *clocks, long *numSources)
      { return (ASIOGetClockSources(clocks, numSources) == ASE_OK); }
    bool SetClockSource(long reference)
      { return (ASIOSetClockSource(reference) == ASE_OK); }
    bool GetSamplePosition(ASIOSamples *sPos, ASIOTimeStamp *tStamp)
      { return (ASIOGetSamplePosition(sPos, tStamp) == ASE_OK); }
    bool GetChannelInfo(ASIOChannelInfo *info)
      { return (ASIOGetChannelInfo(info) == ASE_OK); }
    bool OutputReady()
      { return (ASIOOutputReady() == ASE_OK); }
    bool ControlPanel()
      { return (ASIOControlPanel() == ASE_OK); }
    bool Future(long selector, void *opt)
      { return (ASIOFuture(selector, opt) == ASE_OK); }
    bool TimeCodeRead(bool bEnable = true)
      { return (ASIOFuture(bEnable ? kAsioEnableTimeCodeRead : kAsioDisableTimeCodeRead, 0) == ASE_SUCCESS); }
    bool SetInputMonitor(ASIOInputMonitor *monitor)
      { return (ASIOFuture(kAsioSetInputMonitor, monitor) == ASE_SUCCESS); }


// called from callback functions
    virtual ASIOTime *OnSwitch(ASIOTime *timeInfo, long index, ASIOBool processNow);
    virtual void OnSampleRateChanged(ASIOSampleRate sRate);
    virtual long OnMessage(long selector, long value, void* message, double* opt);
// the messages, spelled out...
    virtual long OnSelectorSupported(long lSelector);
    virtual long OnEngineVersion() { return 2L; }
    virtual long OnResetRequest() { return 1L; }
    virtual long OnBufferSizeChange(long lNewSize) { return 0L; }
    virtual long OnResyncRequest() { return 0L; }
    virtual long OnLatenciesChanged();
    virtual long OnSupportsTimeInfo() { return 1L; }
    virtual long OnSupportsTimeCode() { return 1L; }

protected:
    bool CreateBuffers(int nBufSz = 4410);
    bool DeleteBuffers();

    // conversion helpers
    void ReverseEndian2(void* buffer, long frames);
    void ReverseEndian3(void* buffer, long frames);
    void ReverseEndian4(void* buffer, long frames);
    void ReverseEndian8(void* buffer, long frames);

    void ToFloat16(void *source, float *target, long frames);
    void FromFloat16(float *source, void *target, long frames);
    void ToFloat24(void *source, float *target, long frames);
    void FromFloat24(float *source, void *target, long frames);
    void ToFloat32(void *source, float *target, long relevantBits, long frames);
    void FromFloat32(float *source, void *target, long relevantBits, long frames);
    void ToFloat64(void *source, float *target, long frames);
    void FromFloat64(float *source, void *target, long frames);

protected:
    bool           bLoaded;
    bool           bAllocated;
    static ASIOCallbacks asioCallbacks;

	// ASIOInit()
	ASIODriverInfo driverInfo;

	// ASIOGetChannels()
	long           inputChannels;
	long           outputChannels;

	// ASIOGetBufferSize()
	long           minSize;
	long           maxSize;
	long           preferredSize;
	long           granularity;
    // the size we're actually using
    long           usedSize;

	// ASIOGetSampleRate()
	ASIOSampleRate sampleRate;

	// ASIOOutputReady()
	bool           postOutput;

	// ASIOGetLatencies ()
	long           inputLatency;
	long           outputLatency;

	// ASIOCreateBuffers ()
	long inputBuffers;	// becomes number of actual created input buffers
	long outputBuffers;	// becomes number of actual created output buffers
	ASIOBufferInfo bufferInfos[kMaxInputChannels + kMaxOutputChannels]; // buffer info's

	// ASIOGetChannelInfo()
	ASIOChannelInfo channelInfos[kMaxInputChannels + kMaxOutputChannels]; // channel info's
	// The above two arrays share the same indexing, as the data in them are linked together

	// Information from ASIOGetSamplePosition()
	// data is converted to double floats for easier use, however 64 bit integer can be used, too
	double         nanoSeconds;
	double         samples;
	double         tcSamples;	// time code samples

	// bufferSwitchTimeInfo()
	ASIOTime       tInfo;			// time info state
	unsigned long  sysRefTime;      // system reference time, when bufferSwitch() was called

	// Signal the end of processing in this example
	bool           stopped;

};

#endif // !defined(AFX_ASIOHOST_H__ADAC91FD_B25E_4D5D_ABE8_A39B9B575920__INCLUDED_)
