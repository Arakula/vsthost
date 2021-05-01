/*****************************************************************************/
/* EffSecWnd.cpp: implementation of the CEffSecWnd class.                    */
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

#include "EffSecWnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*===========================================================================*/
/* CEffSecWnd class members                                                  */
/*===========================================================================*/

IMPLEMENT_DYNCREATE(CEffSecWnd, CEffectWnd)

/*****************************************************************************/
/* CEffSecWnd : constructor                                                  */
/*****************************************************************************/

CEffSecWnd::CEffSecWnd()
{

}

/*****************************************************************************/
/* ~CEffSecWnd : destructor                                                  */
/*****************************************************************************/

CEffSecWnd::~CEffSecWnd()
{

}

/*****************************************************************************/
/* CEffSecWnd message map                                                    */
/*****************************************************************************/

BEGIN_MESSAGE_MAP(CEffSecWnd, CEffectWnd)
	//{{AFX_MSG_MAP(CEffSecWnd)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*****************************************************************************/
/* OnClose : called when the parameter edit window is closed                 */
/*****************************************************************************/

void CEffSecWnd::OnClose() 
{
// we need to skip CEffectWnd::OnClose() here since it would try to close an
// (eventually unopened) effect edit window... and would report the wrong
// window as closed to the main window.

// pMain->ParmClosed();
CWnd::OnClose();
}
