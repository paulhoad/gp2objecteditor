
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#include "stdafx.h"

#include "Object.h"
#include "CarTree.h"

#include "resource.h"

#include "DataValue.h"
#include "VertexDataValue.h"
#include "Point3DDataValue.h"

extern char * getFullPathToJamFileName(char *filename);

static long GetLengthofFile(const char *name)
{
  FILE *fp;
  long i=0;
  unsigned char da[1];

  fp = fopen(name,"rb");
  if (fp == NULL) return -1;
  while(fread(&da,sizeof(unsigned char),1,fp)>0){
     i++;
  }
  fclose(fp);
  return i;
}



int Object3D::GetPCByte(int offset)
{
        int buffer = data[offset];
		return buffer;
}


int Object3D::GetPCWord(int offset)
{
        unsigned char buffer[2];

		buffer[0] = 0x000000FF & data[offset];
		buffer[1] = 0x000000FF & data[offset+1];
        int  PCword;
        PCword = buffer[1]*256 +
                 buffer[0];
        return 0x0000FFFF & PCword;
}

long Object3D::GetPCDWord(int offset)
{
        unsigned char tdata[4];
        int  PCword;

		tdata[0] = data[offset];
		tdata[1] = data[offset+1];
		tdata[2] = data[offset+2];
		tdata[3] = data[offset+3];

		PCword = tdata[3]*256*256*256 +
                 tdata[2]*256*256 +
                 tdata[1]*256 +
                 tdata[0];
        return PCword;

}








void Observer::Open(CCarTree *tree)
{
	//AfxMessageBox("Default Observer Open");
}

void Scale::Open(CCarTree *tree)
{
	CDataValue *dlg = new CDataValue();

	dlg->m_DataVal = val;
	int result = dlg->DoModal();

	if (result!=IDCANCEL)
	{
		//update model
		val = dlg->m_DataVal;
		setChanged();
		updateLabel(tree);
		object->UpdatePoints();
	}
}


void Scale::patch()
{
	    int high = val/256;
		int low  = val%256;

		object->data[offset]=low;
		object->data[offset+1]=high;
}


void Scale::updateLabel(CCarTree *tree)
{
	   char buffer[256];

	   wsprintf(buffer,"s[%d] %d ",idx,val);

	   tree->m_CarTree.SetItemText(node,buffer);
}

void t_Arg::updateLabel(CCarTree *tree)
{
	   char msg[256];
	   wsprintf(msg,"%d -> %s",(short)val,desc);
	   tree->m_CarTree.SetItemText(node,msg);
}

HTREEITEM TextureCmd::LoadTree(CCarTree *tree,HTREEITEM node)
{
	    char msg[256];
	    wsprintf(msg,"Cmd %x",cmd.getVal());
		HTREEITEM scaleItem = tree->insertInfoNode(node,msg,TO_ID(IDB_POLYGONS));
	    tree->m_CarTree.SetItemData(scaleItem,(DWORD)&cmd);
	    for(int j=0;j<numArgs;j++)
		{	
			if (Args[j].desc == NULL)
			{
			   Args[j].desc = strdup("Unknown");
			}
			wsprintf(msg,"%d -> %s",(short)Args[j].val,Args[j].desc);
			HTREEITEM scaleItem = tree->insertInfoNode(node,msg,TO_ID(IDB_POLYGONS));
			Args[j].node = scaleItem;
			tree->m_CarTree.SetItemData(scaleItem,(DWORD)&Args[j]);
		}
		return node;
}

void TextureCmd::parse()
{
	if (cmd.getVal() >= 0x0 && cmd.getVal() < 0x7F)
	{
		int startArg = 5;
		if (Args[3].val==0x80)
		{
			startArg+=4;
		}
		if (cmd.getVal()==0xa)
		{
			startArg=1;
		}
		if (cmd.getVal()==0x0)
		{
			startArg=1;
		}
		/*
		//make pts list
		for(int i=startArg;i<numArgs;i+=2)
		{
			int val=(Args[i].val+256*Args[i+1].val);
			if (val!=0)
			{
			 PtsList[numPts++] = val;
			}
		}
		*/
	}
}

void Vertex::Open(CCarTree *tree)
{
	CVertexDataValue *dlg = new CVertexDataValue();

	dlg->m_From = from;
	dlg->m_To = to;
	int result = dlg->DoModal();

	if (result!=IDCANCEL)
	{
		//update model
		from = dlg->m_From;
		to = dlg->m_To;
		setChanged();
		updateLabel(tree);
	}
}

void Vertex::updateLabel(CCarTree *tree)
{
	   char buffer[256];

	   wsprintf(buffer,"v[%d] %d -> %d =%d cm",idx+1,from,to,(int)LengthCM());
	   tree->m_CarTree.SetItemText(node,buffer);
}

void Vertex::patch()
{
	    object->data[offset]=from;
		object->data[offset+1]=to;
}


void Point3D::Open(CCarTree *tree)
{
	CPoint3DDataValue *dlg = new CPoint3DDataValue(object);

	dlg->m_X = x;
	dlg->m_Y = y;
	dlg->m_Z = z;
	dlg->m_OX = ox;
	dlg->m_OY = oy;

	if (ox < 0x8000)
	{
	if (ox > 0x80)
	{
		char msg[256];

		int pos = (ox - 0x84)/4;
		wsprintf(msg,"-s[%d]",pos);
		dlg->m_XScale = msg;
	}
	else
	{
		char msg[256];

		int pos = (ox - 0x4)/4;
		wsprintf(msg,"s[%d]",pos);
		dlg->m_XScale = msg;
	}

	if (oy > 0x80)
	{
		char msg[256];

		int pos = (oy - 0x84)/4;
		wsprintf(msg,"-s[%d]",pos);
		dlg->m_YScale = msg;
	}
	else
	{
		char msg[256];

		int pos = (oy - 0x4)/4;
		wsprintf(msg,"s[%d]",pos);
		dlg->m_YScale = msg;
	}
	}
	else
	{
	   char msg[256];
	   int pos = ox - 0x8000;
	   wsprintf(msg,"p[%d].x",pos);
	   dlg->m_XScale = msg ;
	   wsprintf(msg,"p[%d].y",pos);
	   dlg->m_YScale = msg ;
	}

	//dlg->m_XScale = "s[0]";
	//dlg->m_YScale = "s[0]";

	int result = dlg->DoModal();

	if (result!=IDCANCEL)
	{
		//update model
		ox = dlg->m_OX;
		oy = dlg->m_OY;
		x = dlg->m_X;
		y = dlg->m_Y;
		z = dlg->m_Z;
		setChanged();
		updateLabel(tree);
	}
}

void Point3D::updateLabel(CCarTree *tree)
{
	   char buffer[256];

	   wsprintf(buffer,"p[%d] %d %d %d",idx,x,y,z);
	   tree->m_CarTree.SetItemText(node,buffer);
}

void Point3D::patch()
{
	    object->data[offset]=LOBYTE(ox);
		object->data[offset+1]=HIBYTE(ox);
		object->data[offset+2]=LOBYTE(oy);
		object->data[offset+3]=HIBYTE(oy);
		object->data[offset+4]=LOBYTE(z);
		object->data[offset+5]=HIBYTE(z);
}



void Observer::Select(CCarTree *tree)
{
	//AfxMessageBox("No Select Call",MB_OK);
}


void Point3D::Select(CCarTree *tree)
{
	object->selectedPoint = this;
}

void Vertex::Select(CCarTree *tree)
{
	object->selectedVertex = this;
}

void TextureCmd::Select(CCarTree *tree)
{
	//AfxMessageBox("Selcted Texture");
	object->selectedTextureCmd = this;
}


void DatVal::Open(CCarTree *tree)
{
	CDataValue *dlg = new CDataValue();

	dlg->m_DataVal = data;
	int result = dlg->DoModal();

	if (result!=IDCANCEL)
	{
		//update model
		data = dlg->m_DataVal;
		setChanged();
		updateLabel(tree);
	}
}

void DatVal::updateLabel(CCarTree *tree)
{
	char buffer[256];

	wsprintf(buffer,"u[%d] %d ",idx,data);

	tree->m_CarTree.SetItemText(node,buffer);
}

double Vertex::Length()
{
	    if (from < object->numPoints && to < object->numPoints)
		{
		Point3D *ptA = &object->pointData[from];
		Point3D *ptB = &object->pointData[to];
		return ptA->distance(ptB);
		}
		return -1;
}

double Vertex::LengthCM()
{
		return Length()*0.48076;
}

void TextureCmd::patch()
{
	    object->data[offset]=LOBYTE(numl);
		object->data[offset+1]=HIBYTE(numl);
		object->data[offset+2]=LOBYTE(cmd.getVal());
		object->data[offset+3]=HIBYTE(cmd.getVal());

		int count = offset+4;
		for(int i=0;i<numArgs;i++)
		{
		 object->data[count++]=LOBYTE(Args[i].val);
		 object->data[count++]=HIBYTE(Args[i].val);
		}
		object->data[count++]=LOBYTE(0);
		object->data[count++]=HIBYTE(0);
}




