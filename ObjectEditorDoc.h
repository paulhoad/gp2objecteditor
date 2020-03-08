// ObjectEditorDoc.h : interface of the CObjectEditorDoc class
//
/////////////////////////////////////////////////////////////////////////////
#include "Object.h"

class CObjectEditorDoc : public CDocument
{
protected: // create from serialization only
	CObjectEditorDoc();
	DECLARE_DYNCREATE(CObjectEditorDoc)

// Attributes
public:
	GPOObject *gpoobject;

	GPOObject * ImportGPOObject(CString filename);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectEditorDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CObjectEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CObjectEditorDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
