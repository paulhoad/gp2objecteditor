// ObjectEditorView.h : interface of the CObjectEditorView class
//
/////////////////////////////////////////////////////////////////////////////
#include "OpenGLView.h"

class CObjectEditorView : public COpenGLView
{
protected: // create from serialization only
	CObjectEditorView();
	DECLARE_DYNCREATE(CObjectEditorView)

// Attributes
public:
	CObjectEditorDoc* GetDocument();

	void OpenGLDrawScene(HDC hdc);
	void DrawObjectModel();

	BOOL showPointNumbers;
	BOOL showVertexNumbers;
	BOOL showPolygonNumbers;
	BOOL showPoints;
	BOOL showAxis;
	BOOL showFilled;
	BOOL showCulling;
	BOOL showTextures;
	BOOL showLines;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectEditorView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CObjectEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CObjectEditorView)
	afx_msg void OnZoomin();
	afx_msg void OnZoomout();
	afx_msg void OnShowpointsnumbers();
	afx_msg void OnUpdateShowpointsnumbers(CCmdUI* pCmdUI);
	afx_msg void OnShowpoylgonnumbers();
	afx_msg void OnUpdateShowpoylgonnumbers(CCmdUI* pCmdUI);
	afx_msg void OnShowverticiesnumbers();
	afx_msg void OnUpdateShowverticiesnumbers(CCmdUI* pCmdUI);
	afx_msg void OnShowpoints();
	afx_msg void OnUpdateShowpoints(CCmdUI* pCmdUI);
	afx_msg void OnShowaxis();
	afx_msg void OnUpdateShowaxis(CCmdUI* pCmdUI);
	afx_msg void OnShowbackface();
	afx_msg void OnUpdateShowbackface(CCmdUI* pCmdUI);
	afx_msg void OnShowfilled();
	afx_msg void OnUpdateShowfilled(CCmdUI* pCmdUI);
	afx_msg void OnShowtextures();
	afx_msg void OnUpdateShowtextures(CCmdUI* pCmdUI);
	afx_msg void OnDrawlines();
	afx_msg void OnUpdateDrawlines(CCmdUI* pCmdUI);
	afx_msg void OnAddNewscale();
	afx_msg void OnAddNewpoint();
	afx_msg void OnAddNewtexture();
	afx_msg void OnAddNewvertex();
	afx_msg void OnEditRefresh();
	afx_msg void OnAddNewtexture3sides();
	afx_msg void OnAddNewtexture4sides();
	afx_msg void OnAddNewtexture5sides();
	afx_msg void OnAddNewtexture6sides();
	afx_msg void OnAddNewtexture7sides();
	afx_msg void OnAddNewjamfile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ObjectEditorView.cpp
inline CObjectEditorDoc* CObjectEditorView::GetDocument()
   { return (CObjectEditorDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
