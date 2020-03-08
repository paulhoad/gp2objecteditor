// ObjectEditor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ObjectEditor.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "ObjectEditorDoc.h"
#include "ObjectEditorView.h"

#include "FileVersion.h"

#include "Shlobj.h"
#include "htmlhelp.h"
#include "Credits.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void HtmlHelpFunc(HWND caller,LPCSTR filename,UINT cmd,DWORD dwData);

/////////////////////////////////////////////////////////////////////////////
// CObjectEditorApp

BEGIN_MESSAGE_MAP(CObjectEditorApp, CWinApp)
	//{{AFX_MSG_MAP(CObjectEditorApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_EDIT_EDITGP2LOCATION, OnEditEditgp2location)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(ID_HELP_CREDITS, OnHelpCredits)
	ON_COMMAND(ID_HELP_FINDER, OnHelp)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

//ON_COMMAND(ID_HELP, OnHelp)

/////////////////////////////////////////////////////////////////////////////
// CObjectEditorApp construction

CObjectEditorApp::CObjectEditorApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CObjectEditorApp object

CObjectEditorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CObjectEditorApp initialization

BOOL CObjectEditorApp::InitInstance()
{
	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	//Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	SetRegistryKey("GP2ObjectEditorProject\\ObjectEditor");

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	LoadImages();


	// Register document templates

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_OBJECTTYPE,
		RUNTIME_CLASS(CObjectEditorDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CObjectEditorView));
	AddDocTemplate(pDocTemplate);

	EnableShellOpen();
    RegisterShellFileTypes(TRUE);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	m_pMainWnd->DragAcceptFiles();
	

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (cmdInfo.m_nShellCommand != CCommandLineInfo::FileNew)
	{
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	}

	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

void CObjectEditorApp::LoadImages()
{
	if (pImageList!=NULL) delete pImageList;

	pImageList = new CImageList();

	int size = (IDB_MAX - IDB_MIN);
	pImageList->Create(16, 15, TRUE, size, 2);	

	CBitmap bitmap;
	for (UINT iCnt=IDB_MIN; iCnt<=IDB_MAX; iCnt++)
	{
		bitmap.LoadBitmap(iCnt);
		pImageList->Add(&bitmap, (COLORREF)0xFFFFFF);
		bitmap.DeleteObject();
	}	
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP

	char VersionString[30];

	CFileVersion fileVersion;

	// get the name of the programme
	char ModuleName[256];
	GetModuleFileName(0,ModuleName,256);

	fileVersion.Open(ModuleName);

	//wsprintf(VersionString,"%d.%d.%d.%d.%d",MAJOR_VERSION,MINOR_VERSION,DEBUG_VERSION,DEBUG_SUB_VERSION,BUILDNUMBER);
	wsprintf(VersionString,"Version %s",fileVersion.GetProductVersion());
	//SetDlgItemText(IDC_VERSION_STR,VersionString);
	CString ver(VersionString);

	SetDlgItemText(IDC_VERSION,VersionString);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CObjectEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CObjectEditorApp commands

void CObjectEditorApp::OnEditEditgp2location() 
{
	OnEditLocation();
}

void CObjectEditorApp::OnEditLocation() 
{
	char fileName[256];

	strcpy(fileName,"*.*");
	
	BROWSEINFO browser;
	char buffer[MAX_PATH];

	browser.hwndOwner = NULL;
	browser.pszDisplayName = buffer;
	browser.lpszTitle = "Select GP2 Location";
	browser.lpfn = NULL;
	browser.pidlRoot = NULL;
	browser.ulFlags = BIF_RETURNONLYFSDIRS;

	LPITEMIDLIST files;

	files = SHBrowseForFolder(&browser);

	if (files !=NULL)
	{
		char filename[256];

		if (SHGetPathFromIDList(files,filename))
		{
		 CString strSection       = "Preferences";
         CString strStringItem    = "GP2Location";
         CWinApp* pApp = AfxGetApp();

		 pApp->WriteProfileString(strSection, strStringItem, CString(filename));
		}
		else
		{
	     AfxMessageBox("Failed to Locate GP2 directory");
		}
   	}
	else
	{
		AfxMessageBox("Failed to find GP2 directory");
	}
   }



void CObjectEditorApp::OnStartHelp(BOOL reposition)
{
	RECT rect;
	CRect Wrect;
	CString helppath ="object.chm";

	char szModuleName[260];
	GetModuleFileName(GetModuleHandle(NULL),szModuleName,sizeof(szModuleName));
	char *pos = strrchr(szModuleName,'\\');
	if (pos) 
	{
		*pos='\0';
		helppath = szModuleName;
		helppath += "\\object.chm";
	}

	AfxGetMainWnd()->GetWindowRect(&Wrect);

	rect.left = Wrect.left + Wrect.Width();
	rect.top = Wrect.top;
	rect.right = rect.left+500;
	rect.bottom = Wrect.top+Wrect.Height();

	HH_WINTYPE typeInfo;

	CString  m_TypeName = "object.chm";
	CString tempType = ">";
    CString prefix = "mk:@MSITStore:";
    CString tempToc = "";
    CString tempIndex = "";
    CString tempStart = "";
    CString tempHome = "";
	CString m_chmUrl = helppath;
	CString	m_tocUrl = "Table of Contents.hhc";
	CString	m_indexUrl = "index.hhk";
	CString	m_startUrl = "default.htm";
	CString	m_homeUrl = "default.htm";

	prefix += m_chmUrl;
    prefix += "::/";

    tempToc = prefix;
    tempToc += m_tocUrl;
    typeInfo.pszToc = tempToc;

    tempIndex = prefix;
    tempIndex += m_indexUrl;
    typeInfo.pszIndex = tempIndex;

    tempStart = prefix;
    tempStart += m_startUrl;
    typeInfo.pszFile = tempStart;

    tempHome = prefix;
    tempHome += m_homeUrl;
    typeInfo.pszHome = tempHome;

	CString TypeName = 	"object";

	typeInfo.fUniCodeStrings = FALSE;
    typeInfo.pszType = TypeName;
    typeInfo.pszCaption = "Object Editor Help File";
	
    typeInfo.fsValidMembers = HHWIN_PARAM_PROPERTIES | HHWIN_PARAM_TABPOS |
                                                                HHWIN_PARAM_SHOWSTATE | HHWIN_PARAM_TB_FLAGS;
                                                             
	if (reposition) typeInfo.fsValidMembers |= (HHWIN_PARAM_NAV_WIDTH | HHWIN_PARAM_RECT);
	
    typeInfo.fsWinProperties = 0;
	typeInfo.fsWinProperties = HHWIN_PROP_TRI_PANE;
	typeInfo.fNotExpanded = FALSE;
   	typeInfo.curNavType = HHWIN_NAVTYPE_INDEX;
    typeInfo.tabpos = HHWIN_NAVTAB_TOP;        
	typeInfo.nShowState = SW_SHOW;
    typeInfo.hwndCaller = AfxGetMainWnd()->m_hWnd;
    typeInfo.iNavWidth = 200;
	typeInfo.cbStruct = sizeof(typeInfo);
	typeInfo.fsToolBarFlags = 0;
    typeInfo.fsToolBarFlags |= HHWIN_BUTTON_EXPAND;
    typeInfo.fsToolBarFlags |= HHWIN_BUTTON_BACK;
    typeInfo.fsToolBarFlags |= HHWIN_BUTTON_FORWARD;
    typeInfo.fsToolBarFlags |= HHWIN_BUTTON_STOP;
    typeInfo.fsToolBarFlags |= HHWIN_BUTTON_REFRESH;
    typeInfo.fsToolBarFlags |= HHWIN_BUTTON_HOME;
    typeInfo.fsToolBarFlags |= HHWIN_BUTTON_HISTORY;	
    typeInfo.fsToolBarFlags |= HHWIN_BUTTON_INDEX;
	typeInfo.rcWindowPos = rect;

    //HtmlHelpFunc( AfxGetMainWnd()->m_hWnd,NULL, HH_SET_WIN_TYPE,(DWORD) (&typeInfo));

	tempType += TypeName;

	HtmlHelpFunc(AfxGetMainWnd()->m_hWnd,tempType,HH_DISPLAY_TOPIC,NULL);

}

void CObjectEditorApp::OnHelpContentString(CString url)
{
	OnStartHelp(FALSE);

	CString helppath ="object.chm";

	char szModuleName[260];
	GetModuleFileName(GetModuleHandle(NULL),szModuleName,sizeof(szModuleName));
	char *pos = strrchr(szModuleName,'\\');
	if (pos) 
	{
		*pos='\0';
		helppath = szModuleName;
		helppath += "\\object.chm";
	}
	
	CString m_chmUrl = helppath;
	CString TypeName = 	"object";
	CString tempType = ">";
	tempType += TypeName;
	CString prefix = "mk:@MSITStore:";
	prefix += m_chmUrl;
    prefix += "::/";
	prefix += url;
	//HtmlHelpFunc(AfxGetMainWnd()->m_hWnd,tempType,HH_DISPLAY_TOPIC,(DWORD)(LPCSTR)prefix);
}

typedef void (*HTMLHELPPROC)(HWND caller,LPCSTR filename,UINT cmd,DWORD dwData);

void HtmlHelpFunc(HWND caller,LPCSTR filename,UINT cmd,DWORD dwData)
{
	static HINSTANCE hLib = LoadLibrary("hhctrl.ocx");

	if (hLib!=NULL)
	{
		FARPROC lpfnProc;
		lpfnProc = GetProcAddress(hLib,"HtmlHelpA");

		if (lpfnProc)
		{
			HTMLHELPPROC helpFn = (HTMLHELPPROC)lpfnProc;
			helpFn(caller,filename,cmd,dwData);
		}
		else
		{
			AfxMessageBox("TrackEditor ERROR: Cannot Find HtmlHelp Functionality",MB_OK);
		}
		//FreeLibrary(hLib);
	}
	else
	{
		AfxMessageBox("Cannot Use HtmlHelp Functionality Can't find \"hhctl.ocx\"");
	}
}


void CObjectEditorApp::OnHelp() 
{
	OnStartHelp(FALSE);
}

void CObjectEditorApp::OnHelpCredits() 
{
	CCredits *dlg = new CCredits();
	dlg->DoModal();

	if (dlg) delete dlg;
}

void CObjectEditorApp::OnFileNew() 
{
	// TODO: Add your command handler code here
	
}
