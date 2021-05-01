/*****************************************************************************/
/* WaveDev.cpp : implementation file                                         */
/*****************************************************************************/

#include "stdafx.h"

#define USE_DSOUND 0                    /* flag whether to use DirectSound   */
// currently commented out since it doesn't work yet

#include "vsthost.h"
#include "specwave.h"                   /* Special Wave classes              */
#if USE_DSOUND
#include "SpecDSound.h"                 /* Special DirectSound classes       */
#endif
#include "WaveDev.h"

#ifdef __ASIO_H
#include "SpecAsioHost.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*===========================================================================*/
/* CWaveDeviceDialog dialog members                                          */
/*===========================================================================*/

static int nBufSizes[] =
  {
  11025,                                /*     4 per second                  */
   8820,                                /*     5 per second                  */
   7350,                                /*     6 per second                  */
   6300,                                /*     7 per second                  */
   4900,                                /*     9 per second                  */
   4410,                                /*    10 per second                  */
   3675,                                /*    12 per second                  */
   3150,                                /*    14 per second                  */
   2940,                                /*    15 per second                  */
   2450,                                /*    18 per second                  */
   2205,                                /*    20 per second                  */
   2100,                                /*    21 per second                  */
   1764,                                /*    25 per second                  */
   1575,                                /*    28 per second                  */
   1470,                                /*    30 per second                  */
   1260,                                /*    35 per second                  */
   1225,                                /*    36 per second                  */
   1050,                                /*    42 per second                  */
    980,                                /*    45 per second                  */
    900,                                /*    49 per second                  */
    882,                                /*    50 per second                  */
    735,                                /*    60 per second                  */
    700,                                /*    63 per second                  */
    630,                                /*    70 per second                  */
    588,                                /*    75 per second                  */
    525,                                /*    84 per second                  */
    490,                                /*    90 per second                  */
    450,                                /*    98 per second                  */
    441,                                /*   100 per second                  */
    420,                                /*   105 per second                  */
    350,                                /*   126 per second                  */
    315,                                /*   140 per second                  */
    300,                                /*   147 per second                  */
    294,                                /*   150 per second                  */
    252,                                /*   175 per second                  */
    245,                                /*   180 per second                  */
    225,                                /*   196 per second                  */
    210,                                /*   210 per second                  */
    196,                                /*   225 per second                  */
    180,                                /*   245 per second                  */
    175,                                /*   252 per second                  */
    150,                                /*   294 per second                  */
    147,                                /*   300 per second                  */
  };

/*****************************************************************************/
/* CWaveDeviceDialog : constructor                                           */
/*****************************************************************************/

CWaveDeviceDialog::CWaveDeviceDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CWaveDeviceDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWaveDeviceDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
nBufSize = (44100 / 20);
}

/*****************************************************************************/
/* DoDataExchange : data exchange between dialog and object                  */
/*****************************************************************************/

void CWaveDeviceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaveDeviceDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

/*****************************************************************************/
/* CWaveDeviceDialog message map                                             */
/*****************************************************************************/

BEGIN_MESSAGE_MAP(CWaveDeviceDialog, CDialog)
	//{{AFX_MSG_MAP(CWaveDeviceDialog)
	ON_CBN_SELCHANGE(IDC_OUTPUTPORT, OnSelchangeOutputport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*****************************************************************************/
/* OnInitDialog : called upon the WM_INITDIALOG message                      */
/*****************************************************************************/

BOOL CWaveDeviceDialog::OnInitDialog() 
{
CWaveInDeviceList ListIn;               /* input device list                 */
CWaveOutDeviceList ListOut;             /* output device list                */
#if USE_DSOUND
CDSoundInputList DSListIn;              /* DirectSound input device list     */
CDSoundOutputList DSListOut;            /* DirectSound output device list    */
#endif
int i;
CComboBox *c;

TRACE0("Wave Device Dialog Init\n");

c = (CComboBox *)GetDlgItem(IDC_INPUTPORT);
CString sNoWave;
sNoWave.LoadString(IDS_WAVENONE);
c->AddString(sNoWave);
for (i = 0; i <= ListIn.GetUpperBound(); i++)
  {
  TRACE1("In:  MME: %s\n", ListIn[i]);
  c->AddString(CString("MME: ") + ListIn[i]);
  }

#if USE_DSOUND
for (i = 0; i <= DSListIn.GetUpperBound(); i++)
  {
  TRACE1("In:  DS: %s\n", DSListIn[i]);
  c->AddString(CString("DS: ") + DSListIn[i]);
  }
#endif

if (c->SelectString(-1, InName) == CB_ERR)
  c->SetCurSel(0);

c = (CComboBox *)GetDlgItem(IDC_OUTPUTPORT);
c->AddString(sNoWave);
for (i = 0; i <= ListOut.GetUpperBound(); i++)
  {
  TRACE1("Out: MME: %s\n", ListOut[i]);
  c->AddString(CString("MME: ") + ListOut[i]);
  }
#if USE_DSOUND
for (i = 0; i <= DSListOut.GetUpperBound(); i++)
  {
  TRACE1("Out: DS: %s\n", DSListOut[i]);
  c->AddString(CString("DS: ") + DSListOut[i]);
  }
#endif

#ifdef __ASIO_H
CSpecAsioHost *pAsioHost = ((CVsthostApp *)AfxGetApp())->pAsioHost;
if (pAsioHost)
  {
  char *drvs[40];
  for (i = 0; i < (sizeof(drvs)/sizeof(drvs[0])); i++)
    drvs[i] = new char[33];
  i = pAsioHost->getDriverNames(drvs, (sizeof(drvs)/sizeof(drvs[0])));
  if (i > 0)
    for (int j = 0; j < i; j++)
      {
      TRACE1("Out: ASIO: %s\n", drvs[j]);
      c->AddString(CString("ASIO: ") + drvs[j]);
      }
  for (i = 0; i < (sizeof(drvs)/sizeof(drvs[0])); i++)
    delete[] drvs[i];
  }
#endif

if (c->SelectString(-1, OutName) == CB_ERR)
  c->SetCurSel(0);

c = (CComboBox *)GetDlgItem(IDC_BUFSIZE);
CString s;
int nSetIdx = 6;                        /* default to 1/10 second buffer     */
for (i = 0; i < (sizeof(nBufSizes) / sizeof(nBufSizes[0])); i++)
  {
  s.Format("%d samples    (%d b/s)", nBufSizes[i], 44100 / nBufSizes[i]);
  int nIdx = c->AddString(s);
  if (nBufSizes[i] == nBufSize)
    nSetIdx = nIdx;
  }
c->SetCurSel(nSetIdx);
OnSelchangeOutputport();
return CDialog::OnInitDialog();
}

/*****************************************************************************/
/* OnOK : called when OK has been pressed                                    */
/*****************************************************************************/

void CWaveDeviceDialog::OnOK() 
{
GetDlgItemText(IDC_INPUTPORT, InName);
GetDlgItemText(IDC_OUTPUTPORT, OutName);
CString sBs;
GetDlgItemText(IDC_BUFSIZE, sBs);
nBufSize = atoi(sBs);
CDialog::OnOK();
}

/*****************************************************************************/
/* OnSelchangeOutputport : called when the output port selection changes     */
/*****************************************************************************/

void CWaveDeviceDialog::OnSelchangeOutputport() 
{
CString sOutName;
GetDlgItemText(IDC_OUTPUTPORT, sOutName);
GetDlgItem(IDC_INPUTPORT)->EnableWindow(sOutName.Left(6).CompareNoCase("ASIO: "));
}
