/*****************************************************************************/
/* EffEditWnd.cpp : implementation file                                      */
/*****************************************************************************/

#include "stdafx.h"
#include "vsthost.h"
#include "mainfrm.h"
#include "childfrm.h"
#include "EffEditWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GetApp()  ((CVsthostApp *)AfxGetApp())

/*===========================================================================*/
/* CEffEditDlg dialog                                                        */
/*===========================================================================*/

/*****************************************************************************/
/* CEffEditDlg : constructor                                                 */
/*****************************************************************************/

CEffEditDlg::CEffEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEffEditDlg::IDD, pParent)
{
nEffect = -1;                           /* initialize effect to nothing      */

	//{{AFX_DATA_INIT(CEffEditDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

/*****************************************************************************/
/* Create : creates the dialog                                               */
/*****************************************************************************/

BOOL CEffEditDlg::Create(CWnd* pParentWnd) 
{
return CDialog::Create(IDD, pParentWnd);
}

/*****************************************************************************/
/* DoDataExchange : data exchange between dialog and object                  */
/*****************************************************************************/

void CEffEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffEditDlg)
	DDX_Control(pDX, IDC_PARMVAL, slParmval);
	DDX_Control(pDX, IDC_PARMTEXT, stParmtext);
	DDX_Control(pDX, IDC_PARMLIST, lbParms);
	//}}AFX_DATA_MAP
}

/*****************************************************************************/
/* CEffEditDlg message map                                                   */
/*****************************************************************************/

BEGIN_MESSAGE_MAP(CEffEditDlg, CDialog)
	//{{AFX_MSG_MAP(CEffEditDlg)
	ON_LBN_SELCHANGE(IDC_PARMLIST, OnSelchangeParmlist)
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*****************************************************************************/
/* OnInitDialog : called upon WM_INITDIALOG                                  */
/*****************************************************************************/

BOOL CEffEditDlg::OnInitDialog() 
{
CDialog::OnInitDialog();

slParmval.SetRange(0, 65535);
SetDlgItemText(IDC_RVALUE, "");
	
return TRUE;  // return TRUE unless you set the focus to a control
              // EXCEPTION: OCX Property Pages should return FALSE
}

/*****************************************************************************/
/* PreTranslateMessage : called before message translation                   */
/*****************************************************************************/

BOOL CEffEditDlg::PreTranslateMessage(MSG* pMsg) 
{
// we need this for correct accelerator key handling
CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
HACCEL hAccel = pFrame->GetAccelTable();
if (hAccel &&
    ::TranslateAccelerator(pFrame->m_hWnd, hAccel, pMsg))
  return TRUE;
return CDialog::PreTranslateMessage(pMsg);
}

/*****************************************************************************/
/* SetEffect : initializes dialog to a specific effect                       */
/*****************************************************************************/

void CEffEditDlg::SetEffect(int nEffect)
{
this->nEffect = nEffect;
CEffect *pEffect = GetApp()->vstHost.GetAt(nEffect);
if (pEffect)
  {
  char szTxt[65];
  for (int i = 0; i < pEffect->pEffect->numParams; i++)
    {
    CString sParm;
    sParm.Format("%02d: ", i);
    pEffect->EffGetParamName(i, szTxt);
    sParm += szTxt;
    int idx = lbParms.AddString(sParm);
    lbParms.SetItemData(idx, i);
    }
  lbParms.SetCurSel(0);
  }

OnSelchangeParmlist();
}

/*****************************************************************************/
/* OnSelchangeParmlist : called when the parameter list selection changes    */
/*****************************************************************************/

void CEffEditDlg::OnSelchangeParmlist() 
{
int i = (int)lbParms.GetItemData(lbParms.GetCurSel());
CEffect *pEffect = GetApp()->vstHost.GetAt(nEffect);
float fVal = 0.;

if (pEffect)
  {
  SetParmDisp(pEffect, i);
  fVal = pEffect->EffGetParameter(i);
  }
CString sVal;
sVal.Format("%8f", fVal);
SetDlgItemText(IDC_RVALUE, sVal);
slParmval.SetPos(65535 - (int)(fVal * 65535.));
}

/*****************************************************************************/
/* SetParmDisp : internal function for parameter display                     */
/*****************************************************************************/

void CEffEditDlg::SetParmDisp(CEffect *pEffect, int nParm)
{
char szData[30];
CString sDisp;
if (pEffect)
  {
  pEffect->EffGetParamDisplay(nParm, szData);
  sDisp += szData;
  pEffect->EffGetParamLabel(nParm, szData);
  sDisp += CString(' ') + szData;
  }
stParmtext.SetWindowText(sDisp);
}

/*****************************************************************************/
/* OnVScroll : called when a vertical sb / slider value changes              */
/*****************************************************************************/

void CEffEditDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
if (pScrollBar->GetDlgCtrlID() == IDC_PARMVAL)
  {
  int i = (int)lbParms.GetItemData(lbParms.GetCurSel());
  float fNewVal = float(65535 - slParmval.GetPos()) / 65535.f;
  CEffect *pEffect = GetApp()->vstHost.GetAt(nEffect);
  pEffect->EffSetParameter(i, fNewVal);
  SetParmDisp(pEffect, i);
  }
	
CDialog::OnVScroll(nSBCode, nPos, pScrollBar);

if (nSBCode == SB_ENDSCROLL)            /* if scroll terminated              */
  OnSelchangeParmlist();                /* reset to real current value       */
}

/*****************************************************************************/
/* OnSetParameterAutomated : called when a parameter changes                 */
/*****************************************************************************/

bool CEffEditDlg::OnSetParameterAutomated(long index, float value)
{
if (index == lbParms.GetCurSel())       /* if it's the currently displayed   */
  OnSelchangeParmlist();                /* reget parameter settings          */
return true;
}

/*===========================================================================*/
/* CEffEditWnd class members                                                 */
/*===========================================================================*/

IMPLEMENT_DYNCREATE(CEffEditWnd, CEffectWnd)

/*****************************************************************************/
/* CEffEditWnd : constructor                                                 */
/*****************************************************************************/

CEffEditWnd::CEffEditWnd()
{
}

/*****************************************************************************/
/* ~CEffEditWnd : destructor                                                 */
/*****************************************************************************/

CEffEditWnd::~CEffEditWnd()
{
}

/*****************************************************************************/
/* CEffEditWnd message map                                                   */
/*****************************************************************************/

BEGIN_MESSAGE_MAP(CEffEditWnd, CEffectWnd)
	//{{AFX_MSG_MAP(CEffEditWnd)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*****************************************************************************/
/* CreateView : creates a view for the window                                */
/*****************************************************************************/

CWnd * CEffEditWnd::CreateView()
{
CEffEditDlg *pWnd = new CEffEditDlg;

if (pWnd)
  pWnd->Create(this);
return pWnd;
}

/*****************************************************************************/
/* SetEffect : sets new effect                                               */
/*****************************************************************************/

void CEffEditWnd::SetEffect(int nEff)
{
CEffectWnd::SetEffect(nEff);

CEffEditDlg *pEffDlg = (CEffEditDlg *)pView;
if (pEffDlg)
  {
  pEffDlg->SetEffect(nEffect);
  CRect rc;
  pEffDlg->GetWindowRect(&rc);
  SetEffSize(rc.Width(), rc.Height());
  }
}

/*****************************************************************************/
/* OnClose : called when the parameter edit window is closed                 */
/*****************************************************************************/

void CEffEditWnd::OnClose() 
{
// we need to skip CEffectWnd::OnClose() here since it would try to close an
// (eventually unopened) effect edit window... and would report the wrong
// window as closed to the main window.

pMain->ParmClosed();
CWnd::OnClose();
}

/*****************************************************************************/
/* OnSetParameterAutomated : called when one of the effect's parms changes   */
/*****************************************************************************/

bool CEffEditWnd::OnSetParameterAutomated(long index, float value)
{
CEffEditDlg *pEffDlg = (CEffEditDlg *)pView;
if (pEffDlg)
  return pEffDlg->OnSetParameterAutomated(index, value);
else
  return CEffectWnd::OnSetParameterAutomated(index, value);
}

/*****************************************************************************/
/* Update : makes sure that the dialog is up to date                         */
/*****************************************************************************/

void CEffEditWnd::Update()
{
SetupTitle();
CEffEditDlg *pEffDlg = (CEffEditDlg *)pView;
if (pEffDlg)
  pEffDlg->OnSelchangeParmlist();
}
