// ObjectEditor.h : main header file for the OBJECTEDITOR application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#include "CarTree.h"


/////////////////////////////////////////////////////////////////////////////
// CObjectEditorApp:
// See ObjectEditor.cpp for the implementation of this class
//

class CObjectEditorApp : public CWinApp
{
public:
	CObjectEditorApp();

	void LoadImages();
	CImageList* pImageList;

	CImageList* getSharedImageList()
	{
		return pImageList;
	}

	void OnJamView(CDocument *pDoc);

	
	CCarTree *m_MainTree;

	CString getGP2Location()
	{
	 CString strSection       = "Preferences";
     CString strStringItem    = "GP2Location";
	 CWinApp* pApp = AfxGetApp();

	 CString circuitsDir = pApp->GetProfileString(strSection, strStringItem,"c:\\gp2\\");
	 return circuitsDir;
	}

	void OnEditLocation(); 

	void OnStartHelp(BOOL reposition);
	void OnHelpContentString(CString url);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectEditorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CObjectEditorApp)
	afx_msg void OnAppAbout();
	afx_msg void OnEditEditgp2location();
	afx_msg void OnHelp();
	afx_msg void OnHelpCredits();
	afx_msg void OnFileNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
