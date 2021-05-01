/*****************************************************************************/
/* ProgNameDlg.cpp : implementation file                                     */
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
#include "ProgNameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*===========================================================================*/
/* CProgNameDlg dialog                                                       */
/*===========================================================================*/

/*****************************************************************************/
/* CProgNameDlg : constructor                                                */
/*****************************************************************************/

CProgNameDlg::CProgNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgNameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgNameDlg)
	sPgName = _T("");
	//}}AFX_DATA_INIT
}

/*****************************************************************************/
/* DoDataExchange : data exchange between dialog and object                  */
/*****************************************************************************/

void CProgNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgNameDlg)
	DDX_Text(pDX, IDC_PROGNAME, sPgName);
	//}}AFX_DATA_MAP
}

/*****************************************************************************/
/* CProgNameDlg message map                                                  */
/*****************************************************************************/

BEGIN_MESSAGE_MAP(CProgNameDlg, CDialog)
	//{{AFX_MSG_MAP(CProgNameDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
