// ObjectEditorView.cpp : implementation of the CObjectEditorView class
//

#include "stdafx.h"
#include "ObjectEditor.h"

#include "ObjectEditorDoc.h"
#include "ObjectEditorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <gl\gl.h>
#include <gl\glu.h>


#define WR_PROFILE(x) AfxGetApp()->WriteProfileInt(strSection, #x,x);
#define WR_PROFILES(x) AfxGetApp()->WriteProfileString(strSection, #x,x);
#define PROFILE(x,b) x =  AfxGetApp()->GetProfileInt(strSection, #x,b) == 1 ? TRUE : FALSE;
#define PROFILES(x,b) x =  AfxGetApp()->GetProfileString(strSection, #x,b);

#include "JAM.h"

void Mak256SquareImage(CBitmap &jamBitmap,CBitmap &currentImg,int nWidth,int nHeight,int hr,int vr,int ow,int oh)
{
	   
       CDC memdcA,memdcB;
	   CBitmap *poldbmps,*poldbmpd;
	   // Create a compatible memory DC
       memdcA.CreateCompatibleDC( NULL );
	   memdcB.CreateCompatibleDC( NULL );
	   
 
	   // Select the bitmap into the DC
       poldbmps = memdcA.SelectObject(&jamBitmap);	   
	   poldbmpd = memdcB.SelectObject(&currentImg);

	   int subImageSizeH = (ow/hr);
	   int subImageSizeV = (oh/vr);

	   for(int i=0;i<hr;i++)
	   {
	       for(int j=0;j<vr;j++)
		   {
			 int xpos = (i*subImageSizeH);
			 int ypos = (j*subImageSizeV);
	         memdcB.StretchBlt( xpos,ypos, subImageSizeH,subImageSizeV, &memdcA, 0,0, nWidth, nHeight,SRCCOPY );
		   }
	   }

	   memdcA.SelectObject( poldbmps );
	   memdcB.SelectObject( poldbmpd );
}

unsigned char *TextureMap(CDC *pDC ,CBitmap &cbmp,int width,int height,unsigned char *lpvBits)
{ 
  if (lpvBits == NULL)
  {
  BITMAP bmp;
  
  cbmp.GetBitmap( &bmp );

  if (bmp.bmWidth > 1024) return NULL;
  if (bmp.bmHeight > 1024) return NULL;

  CDC*          bitmapDc = new CDC;
  bitmapDc -> CreateCompatibleDC(pDC);

  CBitmap* oldBitMap = (CBitmap*) bitmapDc -> SelectObject( &cbmp );

  //unsigned char *lpvBits = new unsigned char[bmp.bmWidth * bmp.bmHeight *3];
  lpvBits = new unsigned char[bmp.bmWidth * bmp.bmHeight *3];
  BITMAPINFO *bmpinfo = new BITMAPINFO;

  bmpinfo->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
  bmpinfo->bmiHeader.biWidth         = bmp.bmWidth;
  bmpinfo->bmiHeader.biHeight        = bmp.bmHeight;
  bmpinfo->bmiHeader.biPlanes        = 1;
  bmpinfo->bmiHeader.biBitCount      = 24;
  bmpinfo->bmiHeader.biCompression   = BI_RGB;
  bmpinfo->bmiHeader.biSizeImage     = bmp.bmWidth * bmp.bmHeight * 3;
  bmpinfo->bmiHeader.biXPelsPerMeter = 0;
  bmpinfo->bmiHeader.biYPelsPerMeter = 0;
  bmpinfo->bmiHeader.biClrImportant  = 0;
  bmpinfo->bmiHeader.biClrUsed       = 0;

  if (!::GetDIBits( bitmapDc -> GetSafeHdc(), HBITMAP( cbmp ), 0,bmp.bmHeight, lpvBits, bmpinfo, DIB_RGB_COLORS ))
  {
   AfxMessageBox("GetDIBits error");
  }

  unsigned int size = (bmpinfo->bmiHeader.biSizeImage);

  for(unsigned int i=0;i<size;i+=3)
  {
	  unsigned char *ptr =lpvBits+i;
	  unsigned char *r = ptr;
	  unsigned char *g = ptr+1;
	  unsigned char *b = ptr+2;

	  unsigned char red = *r;
	  unsigned char blue = *b;
	  *b = red;
	  *r = blue;
  }

  bitmapDc -> SelectObject( oldBitMap );
  delete bitmapDc;
  delete bmpinfo;
  }

  GLint val;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE,&val);

  glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0,GL_RGB, GL_UNSIGNED_BYTE, lpvBits);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);  
  glEnable(GL_TEXTURE_2D);  
  glShadeModel(GL_SMOOTH);  

  
  //delete[] lpvBits;
  
  return lpvBits;
}



void GPOObject::LoadJamTexture(CBitmap &output,int idx,int hres,int vres)
{
	int vr = vres/256;
	int hr = hres/256;
	if (vres == -99 || (vres % 256) != 0) vr = 1;
	if (hres == -99 || (hres % 256) != 0) hr = 1;

	
	CBitmap temp;
	output.LoadBitmap(IDB_TESTTEXTURE_256);
	JAM *jam = GetJAM(idx);
	if (jam == NULL) return;
	int sidx  = jam->getImageIndex(idx);
	jam->LoadSingleBMPImage(temp,
		  jam->getImageWidth(sidx), 
		  jam->getImageHeight(sidx),sidx);
	Mak256SquareImage(temp,output,jam->getImageWidth(sidx),jam->getImageHeight(sidx),hr,vr,256,256);
}

GLvoid CalculateVectorNormal(GLdouble fVert1[], GLdouble fVert2[],
                             GLdouble fVert3[], GLdouble *fNormalX,
                             GLdouble *fNormalY, GLdouble *fNormalZ)
{
    GLdouble Qx, Qy, Qz, Px, Py, Pz;
 
    Qx = fVert2[0]-fVert1[0];
    Qy = fVert2[1]-fVert1[1];
    Qz = fVert2[2]-fVert1[2];
    Px = fVert3[0]-fVert1[0];
    Py = fVert3[1]-fVert1[1];
    Pz = fVert3[2]-fVert1[2];
 
    *fNormalX = Py*Qz - Pz*Qy;
    *fNormalY = Pz*Qx - Px*Qz;
    *fNormalZ = Px*Qy - Py*Qx;
}

/////////////////////////////////////////////////////////////////////////////
// CObjectEditorView

IMPLEMENT_DYNCREATE(CObjectEditorView, COpenGLView)

BEGIN_MESSAGE_MAP(CObjectEditorView, COpenGLView)
	//{{AFX_MSG_MAP(CObjectEditorView)
	ON_COMMAND(ID_ZOOMIN, OnZoomin)
	ON_COMMAND(ID_ZOOMOUT, OnZoomout)
	ON_COMMAND(ID_SHOWPOINTSNUMBERS, OnShowpointsnumbers)
	ON_UPDATE_COMMAND_UI(ID_SHOWPOINTSNUMBERS, OnUpdateShowpointsnumbers)
	ON_COMMAND(ID_SHOWPOYLGONNUMBERS, OnShowpoylgonnumbers)
	ON_UPDATE_COMMAND_UI(ID_SHOWPOYLGONNUMBERS, OnUpdateShowpoylgonnumbers)
	ON_COMMAND(ID_SHOWVERTICIESNUMBERS, OnShowverticiesnumbers)
	ON_UPDATE_COMMAND_UI(ID_SHOWVERTICIESNUMBERS, OnUpdateShowverticiesnumbers)
	ON_COMMAND(ID_SHOWPOINTS, OnShowpoints)
	ON_UPDATE_COMMAND_UI(ID_SHOWPOINTS, OnUpdateShowpoints)
	ON_COMMAND(ID_SHOWAXIS, OnShowaxis)
	ON_UPDATE_COMMAND_UI(ID_SHOWAXIS, OnUpdateShowaxis)
	ON_COMMAND(ID_SHOWBACKFACE, OnShowbackface)
	ON_UPDATE_COMMAND_UI(ID_SHOWBACKFACE, OnUpdateShowbackface)
	ON_COMMAND(ID_SHOWFILLED, OnShowfilled)
	ON_UPDATE_COMMAND_UI(ID_SHOWFILLED, OnUpdateShowfilled)
	ON_COMMAND(ID_SHOWTEXTURES, OnShowtextures)
	ON_UPDATE_COMMAND_UI(ID_SHOWTEXTURES, OnUpdateShowtextures)
	ON_COMMAND(ID_DRAWLINES, OnDrawlines)
	ON_UPDATE_COMMAND_UI(ID_DRAWLINES, OnUpdateDrawlines)
	ON_COMMAND(ID_ADD_NEWSCALE, OnAddNewscale)
	ON_COMMAND(ID_ADD_NEWPOINT, OnAddNewpoint)
	ON_COMMAND(ID_ADD_NEWVERTEX, OnAddNewvertex)
	ON_COMMAND(ID_EDIT_REFRESH, OnEditRefresh)
	ON_COMMAND(ID_ADD_NEWTEXTURE_3SIDES, OnAddNewtexture3sides)
	ON_COMMAND(ID_ADD_NEWTEXTURE_4SIDES, OnAddNewtexture4sides)
	ON_COMMAND(ID_ADD_NEWTEXTURE_5SIDES, OnAddNewtexture5sides)
	ON_COMMAND(ID_ADD_NEWTEXTURE_6SIDES, OnAddNewtexture6sides)
	ON_COMMAND(ID_ADD_NEWTEXTURE_7SIDES, OnAddNewtexture7sides)
	ON_COMMAND(ID_ADD_NEWJAMFILE, OnAddNewjamfile)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectEditorView construction/destruction

CObjectEditorView::CObjectEditorView()
{
	CString strSection       = "Preferences";
	PROFILE(showPointNumbers,FALSE)
	PROFILE(showPolygonNumbers,FALSE)
	PROFILE(showVertexNumbers,FALSE)
	PROFILE(showPoints,FALSE)
	PROFILE(showAxis,TRUE)
	PROFILE(showFilled,FALSE)
	PROFILE(showCulling,FALSE)
	PROFILE(showTextures,FALSE)
	PROFILE(showLines,TRUE)
}

CObjectEditorView::~CObjectEditorView()
{
	CString strSection       = "Preferences";
	WR_PROFILE(showPointNumbers);
	WR_PROFILE(showPolygonNumbers);
	WR_PROFILE(showVertexNumbers);
	WR_PROFILE(showPoints);
	WR_PROFILE(showAxis);
	WR_PROFILE(showFilled);
	WR_PROFILE(showCulling);
	WR_PROFILE(showTextures);
	WR_PROFILE(showLines);
}

BOOL CObjectEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	return COpenGLView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CObjectEditorView drawing

void CObjectEditorView::OnDraw(CDC* pDC)
{
	CObjectEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}

/////////////////////////////////////////////////////////////////////////////
// CObjectEditorView printing

BOOL CObjectEditorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CObjectEditorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CObjectEditorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CObjectEditorView diagnostics

#ifdef _DEBUG
void CObjectEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CObjectEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CObjectEditorDoc* CObjectEditorView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CObjectEditorDoc)));
	return (CObjectEditorDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CObjectEditorView message handlers
void DrawText2D(HDC hdc,CScrollView *scview,GLdouble wx,GLdouble wy,GLdouble wz,int idx)
{
        GLdouble sx;
		GLdouble sy;
		GLdouble sz;
		GLdouble model[16];
		GLdouble projection[16];
		GLint view[4];

		glPushMatrix();
		glMatrixMode(GL_MODELVIEW);
		glGetDoublev(GL_MODELVIEW_MATRIX,model);
		glGetDoublev(GL_PROJECTION_MATRIX,projection);
		glGetIntegerv(GL_VIEWPORT,view);
		gluProject(wx,wy,wz,model,projection,view,&sx,&sy,&sz);

		char buffer[10];
		wsprintf(buffer,"%d",idx);
		SelectObject(hdc,GetStockObject(ANSI_VAR_FONT));
		//if (!((CObjectEditorView*)(scview))->showFilled)
		//{
		SetBkMode(hdc,TRANSPARENT);
		//}

		RECT rect;
		scview->GetClientRect(&rect);

		int midy = (rect.top+rect.bottom)/2;
		int midx = (rect.left+rect.right)/2;

		int winx = (int)(sx-midx);
		int winy = (int)(sy-midy);
		
		::TextOut(hdc,(int)(midx+winx),(int)(midy-winy),buffer,static_cast<int>(strlen(buffer)));
		glPopMatrix();
}

void CObjectEditorView::OpenGLDrawScene(HDC hdc)
{
	    
	    CObjectEditorDoc *pDoc = GetDocument();
        

	
        glColor3f (0.0f, 0.0f, 0.0f);
		if (showTextures )glEnable(GL_TEXTURE_2D);
		else glDisable(GL_TEXTURE_2D);

		//if (showLighting)
		//{
		//	glEnable(GL_LIGHTING);
		//}
		//else
		//{
			glDisable(GL_LIGHTING);
		//}

		if (showCulling)
		{
		 glCullFace(GL_FRONT);
		 glEnable(GL_CULL_FACE);
		}
		else
		{
		 glDisable(GL_CULL_FACE);
		}

		if (showAxis)
		{
		float v = 10.0f;

		// draw carthesian axes
	    glBegin(GL_LINES);
		// red x axis
		glColor3f(1.0f,0.f,0.f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(v,0.0f,0.0f);
		
		// green y axis
		glColor3f(0.f,1.f,0.f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(0.0f,v,0.0f);
		// blue z axis
		glColor3f(0.f,0.f,1.f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(0.0f,0.0f,v);
		glEnd();
		}

		//if (!showObject)
		//{
		//  glScalef(0.2f,0.2f,0.2f);
		//}
		//else 
		{
			glScalef(0.002f,0.002f,0.002f);
		}

		//if (showObject) 
			DrawObjectModel();

		if (showPoints)
		{
		 glPointSize(5.0f);
		 
		 glBegin(GL_POINTS);

		 for(int i=0;i<pDoc->gpoobject->numPoints;i++)
		 {	    	
		  glColor3f(1.0f,0.0f,0.0f);
		  GLdouble wx,wy,wz;
		  wx = pDoc->gpoobject->pointData[i].x;
		  wy = pDoc->gpoobject->pointData[i].y;
		  wz = pDoc->gpoobject->pointData[i].z;
		  glVertex3d(wx,wy,wz);
		 }
		 glEnd();
		}

        SwapAnyBuffers(hdc);

		if (showPointNumbers)
		{
		for(int i=0;i<pDoc->gpoobject->numPoints;i++)
		{
		GLdouble wx = pDoc->gpoobject->pointData[i].x;
		GLdouble wy = pDoc->gpoobject->pointData[i].y;
		GLdouble wz = pDoc->gpoobject->pointData[i].z;

		DrawText2D(hdc,this,wx,wy,wz,i);		
		}
		}

		if (showVertexNumbers )
		{
		 for(int i=0;i<pDoc->gpoobject->numVerticies;i++)
		 {
		   double x,y,z;
		   int idxF = pDoc->gpoobject->vertexData[i].from;
		   int idxT = pDoc->gpoobject->vertexData[i].to; 

		   x = (double)pDoc->gpoobject->pointData[idxF].x;
		   y = (double)pDoc->gpoobject->pointData[idxF].y;
		   z = (double)pDoc->gpoobject->pointData[idxF].z;
		   pDoc->gpoobject->vertexData[i].cx = x;
		   pDoc->gpoobject->vertexData[i].cy = y;
		   pDoc->gpoobject->vertexData[i].cz = z;
		   
		   x = (double)pDoc->gpoobject->pointData[idxT].x;
		   y = (double)pDoc->gpoobject->pointData[idxT].y;
		   z = (double)pDoc->gpoobject->pointData[idxT].z;
		   pDoc->gpoobject->vertexData[i].cx += x;
		   pDoc->gpoobject->vertexData[i].cy += y;
		   pDoc->gpoobject->vertexData[i].cz += z;
		   pDoc->gpoobject->vertexData[i].cx /= 2;
		   pDoc->gpoobject->vertexData[i].cy /= 2;
		   pDoc->gpoobject->vertexData[i].cz /= 2;
		   GLdouble wx = pDoc->gpoobject->vertexData[i].cx;
		   GLdouble wy = pDoc->gpoobject->vertexData[i].cy;
		   GLdouble wz = pDoc->gpoobject->vertexData[i].cz;
		   DrawText2D(hdc,this,wx,wy,wz,i+1);
		 }
		}

		if (showPolygonNumbers)
		{
		 for(int i=0;i<pDoc->gpoobject->textureSize;i++)
		 {
			 TextureCmd *cmd =&(pDoc->gpoobject->textureData[i]);

			 GLdouble wx = cmd->cx;
		     GLdouble wy = cmd->cy;
		     GLdouble wz = cmd->cz;
		     DrawText2D(hdc,this,wx,wy,wz,i);
		 }
		}

		
}

void CObjectEditorView::DrawObjectModel()
{
	    CObjectEditorDoc *pDoc = GetDocument();
		if (pDoc->gpoobject == NULL) return;

		
		glPushMatrix();

		glColor3f (0.0f,0.0f,0.0f);
		int vsize = pDoc->gpoobject->numVerticies;

		glDisable(GL_TEXTURE_2D);

		if (showLines)
		{
		for(int i=0;i<vsize;i++)
		{
		   BOOL lineSelected=FALSE;

		   if (pDoc->gpoobject->selectedVertex!=NULL)
		   {
			   if (pDoc->gpoobject->selectedVertex == &pDoc->gpoobject->vertexData[i])
			   {
				   glLineWidth(4.0f);
				   glColor3f (1.0f,0.0f,0.0f);
				   lineSelected=TRUE;
				   
			   }
			   else
			   {
				   glColor3f (0.0f,0.0f,0.0f);
				   glLineWidth(1.0f);
			   }
		   }
		   else
		   {
			   glColor3f (0.0f,0.0f,0.0f);
		   }
		   glBegin(GL_LINES);
		   double x,y,z;
		   int idxF = pDoc->gpoobject->vertexData[i].from;
		   int idxT = pDoc->gpoobject->vertexData[i].to; 
		   x = (double)pDoc->gpoobject->pointData[idxF].x;
		   y = (double)pDoc->gpoobject->pointData[idxF].y;
		   z = (double)pDoc->gpoobject->pointData[idxF].z;
		   glVertex3d(x,y,z);
		   x = (double)pDoc->gpoobject->pointData[idxT].x;
		   y = (double)pDoc->gpoobject->pointData[idxT].y;
		   z = (double)pDoc->gpoobject->pointData[idxT].z;
		   glVertex3d(x,y,z);
		   glEnd();
		}
		}

		if (pDoc->gpoobject->selectedPoint!=NULL)
		{
		   double x,y,z;
		   x = (double)pDoc->gpoobject->selectedPoint->x;
		   y = (double)pDoc->gpoobject->selectedPoint->y;
		   z = (double)pDoc->gpoobject->selectedPoint->z;
		   glColor3f (1.0f,0.0f,0.0f);
		   int size = 200;
		   glPointSize(5.0f);
		   glBegin(GL_POINTS);
		   glVertex3d(x,y,z);
		   glEnd();
		  
		   glColor3f (1.0f,0.0f,0.0f);
		   glBegin(GL_LINES);		   
		   glVertex3d(x-size,y-size,z);
		   glVertex3d(x+size,y+size,z);
		   glEnd();
		   glBegin(GL_LINES);		   
		   glVertex3d(x+size,y-size,z);
		   glVertex3d(x-size,y+size,z);
		   glEnd();
		}

		for(int i=0;i<pDoc->gpoobject->textureSize;i++)
		{
			BOOL selected = FALSE;
			int colorIdx = (i % numRandomColors);
		    t_Color currentColor = RandomColors[colorIdx];

			if (&pDoc->gpoobject->textureData[i] == pDoc->gpoobject->selectedTextureCmd)
			{
				selected = TRUE;
				currentColor = RandomColors[0];
			}

			if (showFilled || selected)
		    {
			int numPts = pDoc->gpoobject->textureData[i].numPts;
            GLdouble norm[10][3];
	        int ptcount=0;
			GLdouble nx,ny,nz;

			pDoc->gpoobject->textureData[i].cx = 0;
			pDoc->gpoobject->textureData[i].cy = 0;
			pDoc->gpoobject->textureData[i].cz = 0;

			for(int j=0; j< numPts;j++)
			{
			  short vertex = pDoc->gpoobject->textureData[i].PtsList[j];
			  if (vertex == -32768) continue;
			  if (vertex == 0) continue;
			  short aver = abs(vertex);

			  if (aver > pDoc->gpoobject->numVerticies) continue;
			  
			  int ptIdx = pDoc->gpoobject->vertexData[aver].from;
			  int ptIdx2 = pDoc->gpoobject->vertexData[aver].to;

			  if (vertex < 0)
			  {
				  ptIdx = pDoc->gpoobject->vertexData[aver].to;
				  ptIdx2 = pDoc->gpoobject->vertexData[aver].from;
			  }
			  double x,y,z;
			  x = (double)pDoc->gpoobject->pointData[ptIdx].x;
		      y = (double)pDoc->gpoobject->pointData[ptIdx].y;
		      z = (double)pDoc->gpoobject->pointData[ptIdx].z;
			  norm[ptcount][0] = x;
			  norm[ptcount][1] = y;
			  norm[ptcount][2] = z;
			  ptcount++;
			  pDoc->gpoobject->textureData[i].cx += x;
			  pDoc->gpoobject->textureData[i].cy += y;
			  pDoc->gpoobject->textureData[i].cz += z;
			}

			pDoc->gpoobject->textureData[i].cx  /= ptcount;
			pDoc->gpoobject->textureData[i].cy  /= ptcount;
			pDoc->gpoobject->textureData[i].cz  /= ptcount;

			if (numPts >=3)
			{			    	
				CalculateVectorNormal(norm[0],norm[1],norm[2],&nx,&ny,&nz);
				nx = nx/100;
			    ny = ny/100;
			    nz = nz/100;
				glNormal3d(nx,ny,ny);
			}

			if (showTextures)
			{	
			 TextureCmd *cmd = &(pDoc->gpoobject->textureData[i]);
			 if (cmd && (cmd->textureid))
			 {
			 int tid = *(cmd->textureid);
			 int vres = 256;
			 int hres = 256;

			 if (cmd->vres){
			   vres = *(cmd->vres);
			 }
			 if (cmd->hres){
			   hres = *(cmd->hres);
			 }

			
			 CBitmap bmp;
		     if (pDoc->gpoobject->textureData[i].loadedTextureId == -1)
			 {
			   if (pDoc->gpoobject->textureData[i].loadedTextureId != tid)
				{				 
			     pDoc->gpoobject->LoadJamTexture(bmp,tid,hres,vres);
				 pDoc->gpoobject->textureData[i].jamBits = NULL;
				 pDoc->gpoobject->textureData[i].loadedTextureId = tid;
				}
			  }
			  pDoc->gpoobject->textureData[i].jamBits = TextureMap(GetDC(),bmp,256,256,pDoc->gpoobject->textureData[i].jamBits);			 
			 }
			}

			if (selected) glDisable(GL_TEXTURE_2D);

			glColor3f (currentColor.r, currentColor.g, currentColor.b);
			BOOL ignoreNextTexCoord=FALSE;
			int  cornerProcessed=0;
			glBegin(GL_POLYGON);
			for(int j=0; j< numPts;j++)
			{
			  short vertex = pDoc->gpoobject->textureData[i].PtsList[j];
			  if (vertex == -32768) 
			  {
				  ignoreNextTexCoord = TRUE;
				  continue;
			  }
			  if (vertex == 0) continue;
			  short aver = abs(vertex);

			  if (aver > pDoc->gpoobject->numVerticies) continue;

			  if (!ignoreNextTexCoord)
				{				 
				 switch(cornerProcessed)
				 {
				  default:
				  case 0:glTexCoord2f(1.0f,0.0f);break;
				  case 1:glTexCoord2f(0.0f,0.0f);break;
				  case 2:
					  if (numPts == 3)
					  {
					  glTexCoord2f(0.5f,1.0f);
					  }
					  else
					  {
					  glTexCoord2f(0.0f,1.0f);
					  }
					  break;
				  case 3:glTexCoord2f(1.0f,1.0f);break;
				 }
				 cornerProcessed++;
			  }
			  ignoreNextTexCoord=FALSE;
			  
			  int ptIdx = pDoc->gpoobject->vertexData[aver].from;
			  int ptIdx2 = pDoc->gpoobject->vertexData[aver].to;

			  if (vertex < 0)
			  {
				  ptIdx = pDoc->gpoobject->vertexData[aver].to;
				  ptIdx2 = pDoc->gpoobject->vertexData[aver].from;
			  }
			  double x,y,z;
			  x = (double)pDoc->gpoobject->pointData[ptIdx].x;
		      y = (double)pDoc->gpoobject->pointData[ptIdx].y;
		      z = (double)pDoc->gpoobject->pointData[ptIdx].z;
		      glVertex3d(x,y,z);
			  x = (double)pDoc->gpoobject->pointData[ptIdx2].x;
		      y = (double)pDoc->gpoobject->pointData[ptIdx2].y;
		      z = (double)pDoc->gpoobject->pointData[ptIdx2].z;
		      //glVertex3d(x,y,z);
			}
			glEnd();
		    }		
		}



		glPopMatrix();
}





void CObjectEditorView::OnZoomout() 
{
	dist = dist*1.2;
    InvalidateRect(NULL,FALSE);
}

void CObjectEditorView::OnZoomin() 
{
	dist = dist*0.8;
	InvalidateRect(NULL,FALSE);
}



void CObjectEditorView::OnShowpointsnumbers() 
{
	showPointNumbers = !showPointNumbers;
	InvalidateRect(NULL,FALSE);
}

void CObjectEditorView::OnUpdateShowpointsnumbers(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(showPointNumbers);
}

void CObjectEditorView::OnShowpoylgonnumbers() 
{
	showPolygonNumbers = !showPolygonNumbers;
	InvalidateRect(NULL,FALSE);
}

void CObjectEditorView::OnUpdateShowpoylgonnumbers(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(showPolygonNumbers);
}

void CObjectEditorView::OnShowverticiesnumbers() 
{
	showVertexNumbers = !showVertexNumbers;
	InvalidateRect(NULL,FALSE);
}

void CObjectEditorView::OnUpdateShowverticiesnumbers(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(showVertexNumbers);
}

void CObjectEditorView::OnShowpoints() 
{
	showPoints = !showPoints;
	InvalidateRect(NULL,FALSE);
}

void CObjectEditorView::OnUpdateShowpoints(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(showPoints);
}

void CObjectEditorView::OnShowaxis() 
{
	showAxis = !showAxis;
	InvalidateRect(NULL,FALSE);
}

void CObjectEditorView::OnUpdateShowaxis(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(showAxis);
}

void CObjectEditorView::OnShowbackface() 
{
	showCulling = !showCulling;
	InvalidateRect(NULL,FALSE);
}

void CObjectEditorView::OnUpdateShowbackface(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(showCulling);
}

void CObjectEditorView::OnShowfilled() 
{
	showFilled = !showFilled;
	InvalidateRect(NULL,FALSE);
}

void CObjectEditorView::OnUpdateShowfilled(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(showFilled);
}

void CObjectEditorView::OnShowtextures() 
{
	showTextures = !showTextures;
	InvalidateRect(NULL,FALSE);
}

void CObjectEditorView::OnUpdateShowtextures(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(showTextures);
}

void CObjectEditorView::OnDrawlines() 
{
	showLines = !showLines;
	InvalidateRect(NULL,FALSE);
}

void CObjectEditorView::OnUpdateDrawlines(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(showLines);
}

void CObjectEditorView::OnAddNewscale() 
{
	CObjectEditorDoc *pDoc = GetDocument();
	pDoc->gpoobject->AddScaleValue();
	OnEditRefresh();

}

void CObjectEditorView::OnAddNewpoint() 
{
	CObjectEditorDoc *pDoc = GetDocument();
	pDoc->gpoobject->AddPointValue();
	OnEditRefresh();
}

void CObjectEditorView::OnAddNewtexture() 
{
	CObjectEditorDoc *pDoc = GetDocument();
	pDoc->gpoobject->AddTextureCmd(4);
	OnEditRefresh();
}

void CObjectEditorView::OnAddNewvertex() 
{
	CObjectEditorDoc *pDoc = GetDocument();
	pDoc->gpoobject->AddVertexValue();
	OnEditRefresh();
}

void CObjectEditorView::OnEditRefresh() 
{
	CObjectEditorDoc *pDoc = GetDocument();
	((CObjectEditorApp*)AfxGetApp())->m_MainTree->Refresh(pDoc->gpoobject);
}

void CObjectEditorView::OnAddNewtexture3sides() 
{
	CObjectEditorDoc *pDoc = GetDocument();
	pDoc->gpoobject->AddTextureCmd(3);
}

void CObjectEditorView::OnAddNewtexture4sides() 
{
	CObjectEditorDoc *pDoc = GetDocument();
	pDoc->gpoobject->AddTextureCmd(4);
}

void CObjectEditorView::OnAddNewtexture5sides() 
{
	CObjectEditorDoc *pDoc = GetDocument();
	pDoc->gpoobject->AddTextureCmd(5);
}

void CObjectEditorView::OnAddNewtexture6sides() 
{
	CObjectEditorDoc *pDoc = GetDocument();
	pDoc->gpoobject->AddTextureCmd(6);
}

void CObjectEditorView::OnAddNewtexture7sides() 
{
	CObjectEditorDoc *pDoc = GetDocument();
	pDoc->gpoobject->AddTextureCmd(7);
}

void CObjectEditorView::OnAddNewjamfile() 
{
	CObjectEditorDoc *pDoc = GetDocument();
	pDoc->gpoobject->AddNewJamFile();
}
