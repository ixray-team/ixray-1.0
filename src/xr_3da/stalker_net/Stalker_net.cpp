// Stalker_net.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Stalker_net.h"
#include "Stalker_netDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CStalker_netApp

BEGIN_MESSAGE_MAP(CStalker_netApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CStalker_netApp construction

CStalker_netApp::CStalker_netApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}
CStalker_netApp::~CStalker_netApp()
{
	Core._destroy();
}

// The one and only CStalker_netApp object

CStalker_netApp theApp;

// Calculating build number from XR_3DA for correct resources reading

// computing build id
XRCORE_API	LPCSTR	build_date;
XRCORE_API	u32		build_id;

//#define NO_SINGLE
#define NO_MULTI_INSTANCES

static LPSTR month_id[12] = {
	"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
};

static int days_in_month[12] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static int start_day	= 31;	// 31
static int start_month	= 1;	// January
static int start_year	= 1999;	// 1999

void compute_build_id	()
{
	build_date			= __DATE__;

	int					days;
	int					months = 0;
	int					years;
	string16			month;
	string256			buffer;
	strcpy_s				(buffer,__DATE__);
	sscanf				(buffer,"%s %d %d",month,&days,&years);

	for (int i=0; i<12; i++) {
		if (_stricmp(month_id[i],month))
			continue;

		months			= i;
		break;
	}

	build_id			= (years - start_year)*365 + days - start_day;

	for (int i=0; i<months; ++i)
		build_id		+= days_in_month[i];

	for (int i=0; i<start_month-1; ++i)
		build_id		-= days_in_month[i];
}

typedef void DUMMY_STUFF (const void*,const u32&,void*);
XRCORE_API DUMMY_STUFF	*g_temporary_stuff;

#define TRIVIAL_ENCRYPTOR_DECODER
#include "..\trivial_encryptor.h"

// CStalker_netApp initialization

BOOL CStalker_netApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	// Decoding resources and calculating build number
	g_temporary_stuff = trivial_encryptor::decode;
	compute_build_id();

	Core._initialize("Stalker_net", NULL, TRUE);
	
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
//	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CStalker_netDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

