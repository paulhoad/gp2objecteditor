// Credits.cpp : implementation file
//

#include "stdafx.h"
#include "ObjectEditor.h"
#include "Credits.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCredits dialog


CCredits::CCredits(CWnd* pParent /*=NULL*/)
	: CDialog(CCredits::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCredits)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCredits::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCredits)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCredits, CDialog)
	//{{AFX_MSG_MAP(CCredits)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCredits message handlers
