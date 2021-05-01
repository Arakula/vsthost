// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__9052BAF7_9D44_11D5_8163_4000001054B2__INCLUDED_)
#define AFX_STDAFX_H__9052BAF7_9D44_11D5_8163_4000001054B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define OEMRESOURCE         // for standard windows bitmaps

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxmt.h>          // MFC Multithread support
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <math.h>           // math routines

#include <mmsystem.h>       // Multimedia support
#include <dsound.h>         // DirectSound support

#ifdef _DEBUGFILE
// special - trace to file support
#undef TRACE
#undef TRACE0
#undef TRACE1
#undef TRACE2
#undef TRACE3
#undef TRACE4
void DebugFileTrace(LPCSTR lpszFormat, ...);
#define TRACE DebugFileTrace
#define TRACE0(a) DebugFileTrace(a)
#define TRACE1(a,b) DebugFileTrace((a),(b))
#define TRACE2(a,b,c) DebugFileTrace((a),(b),(c))
#define TRACE3(a,b,c,d) DebugFileTrace((a),(b),(c),(d))
#define TRACE4(a,b,c,d,e) DebugFileTrace((a),(b),(c),(d),(e))
#endif

#ifdef USE_ASIO
#include "asiosys.h"		// platform definition
#include "asio.h"
#include "iasiodrv.h"
#include "asiodrivers.h"
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__9052BAF7_9D44_11D5_8163_4000001054B2__INCLUDED_)
