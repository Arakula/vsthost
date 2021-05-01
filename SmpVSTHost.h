/*****************************************************************************/
/* SmpVSTHost.h: Interface for classes CSmpVSTHost / CSmpEffect              */
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

#if !defined(AFX_SMPVSTHOST_H__EA642421_A9AF_11D5_B6BD_004F4E003207__INCLUDED_)
#define AFX_SMPVSTHOST_H__EA642421_A9AF_11D5_B6BD_004F4E003207__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CVSTHost.h"

/*****************************************************************************/
/* CSmpEffect class declaration                                              */
/*****************************************************************************/

class CEffectWnd;
class CChildFrame;
class CSmpEffect : public CEffect  
{
public:
	CSmpEffect(CVSTHost *pHost);
	virtual ~CSmpEffect();

public:
	bool LoadBank(const char *name);
	void LeaveCritical();
	void EnterCritical();
	virtual bool Load(const char *name);
    virtual bool Unload();
	virtual long EffDispatch(long opCode, long index=0, long value=0, void *ptr=0, float opt=0.);
    virtual void * OnOpenWindow(VstWindow* window);
    virtual bool OnCloseWindow(VstWindow* window);
	virtual void OnSizeEditorWindow(long width, long height);
    void SetFrameWnd(CChildFrame *pFWnd = 0) { pFrameWnd = pFWnd; }
    CChildFrame *GetFrameWnd() { return pFrameWnd; }
    void SetEditWnd(CEffectWnd *pEWnd = 0) { pEditWnd = pEWnd; }
    CEffectWnd *GetEditWnd() { return pEditWnd; }
    void SetParmWnd(CEffectWnd *pPWnd = 0) { pParmWnd = pPWnd; }
    CEffectWnd *GetParmWnd() { return pParmWnd; }
    unsigned short GetChnMask() { return wChnMask; }
    void SetChnMask(unsigned short mask) { wChnMask = mask; }
    bool OnSetParameterAutomated(long index, float value);
    virtual bool OnUpdateDisplay();
    void SetChunkFile(CString sFile) { sChunkFile = sFile; }
    CString GetChunkFile() { return sChunkFile; }
    CString GetDisplayName()
      {
      char szBuf[256] = "";
      if ((EffGetProductString(szBuf)) && (*szBuf))
        return szBuf;
      CString sCutName(sName);
      int nIdx = sCutName.ReverseFind('\\');
      if (nIdx >= 0)
        sCutName = sCutName.Mid(nIdx + 1);
      return sCutName;
      }
    void ShowDetails();
    void InsertIntoChain(CSmpEffect *prev)
      {
      RemoveFromChain();
      EnterCritical();
      if (prev)
        {
        pNext = prev->pNext;
        pPrev = prev;
        prev->pNext = this;
        if (pNext)
          pNext->pPrev = this;
        }
      LeaveCritical();
      }
    void RemoveFromChain()
      {
      EnterCritical();
      if (pPrev)
        pPrev->pNext = pNext;
      if (pNext)
        pNext->pPrev = pPrev;
      pPrev = pNext = NULL;
      LeaveCritical();
      }
    CSmpEffect *GetPrev() { return pPrev; }
    CSmpEffect *GetNext() { return pNext; }
    void SetLoadNum(int nNum) { nLoadNum = nNum; }
    int GetLoadNum(void) { return nLoadNum; }

	void SetInputBuffer(int nBuf, float *pData = 0)
      {
      if (nBuf < pEffect->numInputs)
        // inBufs allocation isn't checked here; this is intentional.
        // We rely on a sensible implementation that doesn't set
        // input buffers before initialization is finished (which
        // would throw out an effect if buffer allocation fails).
        inBufs[nBuf] = pData;
      }

    float ** GetOutputBuffers() { return outBufs; }

	float * GetOutputBuffer(int nBuf)
      {
      if (nBuf < ((nAllocatedOutbufs) ? nAllocatedOutbufs : pEffect->numOutputs))
        // outBufs allocation isn't checked here; this is intentional.
        // We rely on a sensible implementation that doesn't set
        // input buffers before initialization is finished (which
        // would throw out an effect if buffer allocation fails).
        return outBufs[nBuf];
      else
        return 0;
      }

    virtual void EffProcess(long sampleFrames)
      {
      // operate using internally allocated buffers
      CEffect::EffProcess(inBufs, outBufs, sampleFrames);
      TRACE1("effProcess (max=%1.5f)\n", CalcMax(sampleFrames));
      }
    virtual void EffProcessReplacing(long sampleFrames)
      {
      // operate using internally allocated buffers
      CEffect::EffProcessReplacing(inBufs, outBufs, sampleFrames);
      TRACE1("effProcessReplacing (max=%1.5f)\n", CalcMax(sampleFrames));
      }
    float CalcMax(long sampleFrames)
      {
      int nOuts = pEffect->numOutputs;
      int b, s;
      float fCur, *p;
      fMax = 0.0f;
      for (b = 0; b < nOuts; b++)
        {
        p = outBufs[b];
        for (s = 0; s < sampleFrames; s++)
          {
          fCur = fabsf(*p++);
          if (fCur > fMax)
            fMax = fCur;
          }
        }
      return fMax;
      }

protected:
	unsigned short wChnMask;
	int nAllocatedOutbufs;
	int nAllocatedInbufs;
	static int EvalException(int n_except);
    CRITICAL_SECTION cs;

	float ** outBufs;
	float ** inBufs;
    float fMax;
    CChildFrame *pFrameWnd;
	CEffectWnd * pEditWnd;
	CEffectWnd * pParmWnd;
    CString sChunkFile;
    CPtrArray paWnds;                   /* array of secondary VstWindows     */

    // Effect chaining
    CSmpEffect *pPrev;
    CSmpEffect *pNext;
    int nLoadNum;
};

/*****************************************************************************/
/* CSmpVSTHost class declaration                                             */
/*****************************************************************************/

class CMidiMsg;
class CSmpVSTHost : public CVSTHost  
{
public:
	CSmpVSTHost();
	virtual ~CSmpVSTHost();

public:
	BYTE * CreateMIDISubset(void *pEvData, unsigned short wChnMask = 0xffff);
    void OnSamples(float **pBuffer, int nLength, int nChannels = 2, DWORD dwStamp = 0);
	virtual bool OnProcessEvents(int nEffect, VstEvents* events);
	virtual void OnMidiIn(CMidiMsg &msg);
    virtual void PassThruEffect(CSmpEffect *pEff, float **pBuffer, int nLength, int nChannels, BYTE *pEvData, bool bReplacing);

                                        /* create customized effects         */
    virtual CEffect * CreateEffect() { return new CSmpEffect(this); }
    // overridden callback functions
	virtual long OnAudioMasterCallback(int nEffect, long opcode, long index, long value, void *ptr, float opt);
    virtual long OnGetAutomationState(int nEffect) { return kVstAutomationReadWrite; }
	virtual bool OnSetParameterAutomated(int nEffect, long index, float value);
	virtual bool OnCanDo(const char *ptr);
	virtual long OnIdle(int nEffect=-1);
    virtual bool OnOpenFileSelector (int nEffect, VstFileSelect *ptr);
    virtual bool OnCloseFileSelector (int nEffect, VstFileSelect *ptr);
    virtual bool OnGetChunkFile(int nEffect, void * nativePath);

protected:
	CRITICAL_SECTION cs;
	int nMidis;
	DWORD * pMidis;
	float * pOutputs[2];
    int nSysEx;
    CMidiMsg *pSysEx;

protected:
	BYTE * CreateMIDIEvents(int nLength, DWORD dwStamp, int nm, int ns);

};

#endif // !defined(AFX_SMPVSTHOST_H__EA642421_A9AF_11D5_B6BD_004F4E003207__INCLUDED_)
