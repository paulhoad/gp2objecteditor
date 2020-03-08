// ObjectEditorDoc.cpp : implementation of the CObjectEditorDoc class
//

#include "stdafx.h"
#include "ObjectEditor.h"

#include "ObjectEditorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectEditorDoc

IMPLEMENT_DYNCREATE(CObjectEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CObjectEditorDoc, CDocument)
	//{{AFX_MSG_MAP(CObjectEditorDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectEditorDoc construction/destruction

CObjectEditorDoc::CObjectEditorDoc()
{
	gpoobject = NULL;
}

CObjectEditorDoc::~CObjectEditorDoc()
{
}

BOOL CObjectEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CObjectEditorDoc serialization

void CObjectEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CObjectEditorDoc diagnostics

#ifdef _DEBUG
void CObjectEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CObjectEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CObjectEditorDoc commands

BOOL CObjectEditorDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	// TODO: Add your specialized creation code here
	// car = LoadCar(lpszPathName);
	gpoobject = ImportGPOObject(lpszPathName);
	gpoobject->parse(0);
	//OnFileimport();
	((CObjectEditorApp*)AfxGetApp())->m_MainTree->InsertGPOObject(this,gpoobject);
	
	return TRUE;
}

static char * getObjectline(FILE *fp)
{
	static char buffer[512];

	fscanf(fp,"%[^\n]\n",buffer);
	return buffer;
}


GPOObject * CObjectEditorDoc::ImportGPOObject(CString filename)
{
	  FILE *fp = fopen(filename,"r");

	  if (fp==NULL)
	  {
		  AfxMessageBox("Failed to open import file");
		  return FALSE;
	  }

	  GPOObject *obj = new GPOObject();


	  // get number of datavalues
      int listsize=0;

	  while(!feof(fp))
	  {
		  char *line = getObjectline(fp);

		  if (line == NULL) continue;
		  if (line[0] == '#' && line[1] == '#') 
		  {
			  obj->ParseAdditionalCommands(line);
		  }
		  if (line[0] == '#') continue;
		  if (line[0] == '/' && line[1] == '/') continue;
		  
		  sscanf(line,"%d",&listsize);break;
	  }

	  int objectLength = 0;
	  int count=0;

	  for(int i=0;i<listsize;i++)
	  {
		 char *line = getObjectline(fp);

		  // don't gobble lines
		  if (line[0] == '#') 
		  {
			  i--;
			  continue;
		  }
		  if (line[0] == '/' && line[1] == '/')
		  {
			  i--;
			  continue;
		  }

		  int offset;
		  int size;
		  int value;

		  sscanf(line,"%d %d 0x%x",&offset,&size,&value);
		  objectLength+=size;
		  switch(size)
		  {
            case 2:
			   obj->data[count++] = LOBYTE(value);
			   obj->data[count++] = HIBYTE(value);
			   break;
			case 4:
			   obj->data[count++] = LOBYTE(LOWORD(value));
			   obj->data[count++] = HIBYTE(LOWORD(value));
			   obj->data[count++] = LOBYTE(HIWORD(value));
			   obj->data[count++] = HIBYTE(HIWORD(value));
			   break;
		  }
	  }

	  while(!feof(fp))
	  {
		  char *line = getObjectline(fp);

		  if (line == NULL) continue;
		  if (line[0] == '#' && line[1] == '#') 
		  {
			  obj->ParseAdditionalCommands(line);
		  }
		  if (line[0] == '#') continue;
		  if (line[0] == '/' && line[1] == '/') continue;		
	  }

	  fclose(fp);
	  return obj;
}

BOOL CObjectEditorDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	//return CDocument::OnSaveDocument(lpszPathName);
	FILE *fp = fopen(lpszPathName,"w");
	if (fp == NULL)
	{
		AfxMessageBox("Failed to open file for output");
		return FALSE;
	}
	if (gpoobject && fp)
	{
		gpoobject->write(fp);
		fclose(fp);
	}
	return TRUE; 
}

void CObjectEditorDoc::OnCloseDocument() 
{
	// TODO: Add your specialized code here and/or call the base class
	if (gpoobject)	{
		((CObjectEditorApp*)AfxGetApp())->m_MainTree->m_CarTree.DeleteItem(gpoobject->carNode);
	}
	
	CDocument::OnCloseDocument();
}
