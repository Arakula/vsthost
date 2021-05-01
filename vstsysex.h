/*****************************************************************************/
/* vstsysex.h : the missing SysEx definitions from audioeffectx.h            */
/*****************************************************************************/

#ifndef __vstsysex_h__
#define __vstsysex_h__

#ifndef VST_2_4_EXTENSIONS
struct VstMidiSysexEvent  // to be casted from a VstEvent
  {
  long type; // kVstSysExType
  long byteSize; // 24
  long deltaFrames; // sample frames related to the current block start sample position
  long flags; // none defined yet

  long dumpBytes; // byte size of sysexDump
  long resvd1; // zero

  char *sysexDump;

  long resvd2; // zero
  };
#endif

#endif  // defined(__vstsysex_h__)