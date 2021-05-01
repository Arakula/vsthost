// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently

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

#include "vsthost.h"
#include "resource.h"

inline CVsthostApp *GetApp() { return (CVsthostApp *)AfxGetApp(); }

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__9052BAF7_9D44_11D5_8163_4000001054B2__INCLUDED_)
