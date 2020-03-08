#include "stdafx.h"

#include "Object.h"
#include "CarTree.h"

#include "resource.h"

#include "ObjectEditor.h"
#include "ObjectEditorDoc.h"
#include "ObjectEditorView.h"
#include "DataValue.h"

#include "Jam.h"

void GPOObject::LoadTree(CCarTree *tree)
{
	char buffer[256];
	carNode = tree->insertInfoNode(tree->m_CarTree.GetRootItem(),"GPO Object",TO_ID(IDB_OBJECT));

	wsprintf(buffer,"Points %d",numPoints);
	HTREEITEM pts = tree->insertInfoNode(carNode,buffer,TO_ID(IDB_POINT));
	for(int i=0;i<numPoints;i++)
	{
	   if (!pointData[i].getDesc().empty())
	   {
		 wsprintf(buffer,"p[%d] %d %d %d %s",i,pointData[i].x,pointData[i].y,pointData[i].z,pointData[i].getDesc().c_str());
	   }
	   else
	   {
	     wsprintf(buffer,"p[%d] %d %d %d",i,pointData[i].x,pointData[i].y,pointData[i].z);
	   }
	   HTREEITEM ptItem = tree->insertInfoNode(pts,buffer,TO_ID(IDB_POINT));
	   tree->m_CarTree.SetItemData(ptItem,(DWORD)&pointData[i]);
	   pointData[i].node = ptItem;
	   pointData[i].idx = i;
	   pointData[i].object = this;
	}

	wsprintf(buffer,"Verticies %d",numVerticies);
	HTREEITEM vertex = tree->insertInfoNode(carNode,buffer,TO_ID(IDB_VERTICIES));
	for(int i=0;i<numVerticies;i++)
	{
	  
	   sprintf(buffer,"v[%d] %d -> %d =%d cm",i+1,vertexData[i].from,vertexData[i].to,(int)vertexData[i].LengthCM());
	   HTREEITEM verItem = tree->insertInfoNode(vertex,buffer,TO_ID(IDB_VERTICIES));
	   tree->m_CarTree.SetItemData(verItem,(DWORD)&vertexData[i]);
	   vertexData[i].node = verItem;
	   vertexData[i].idx = i;
	   vertexData[i].object = this;
	}

	wsprintf(buffer,"ScaleData %d",numScale);
	HTREEITEM scalep = tree->insertInfoNode(carNode,buffer,TO_ID(IDB_SCALE));
	for(int i=0;i<numScale;i++)
	{
	  
	   if (i<31)
	   {
	     wsprintf(buffer,"s[%d] %d",i,scaleData[i].val);
	   }
	   else
	   {
		 wsprintf(buffer,"s[%d] %d",i,scaleData[i].val);
	   }
	   HTREEITEM scaleItem = tree->insertInfoNode(scalep,buffer,TO_ID(IDB_SCALE));
	   scaleData[i].node = scaleItem;
	   scaleData[i].idx = i;
	   scaleData[i].object = this;
	   tree->m_CarTree.SetItemData(scaleItem,(DWORD)&scaleData[i]);
	}

	wsprintf(buffer,"TextureData %d",textureSize);
	HTREEITEM textureP = tree->insertInfoNode(carNode,buffer,TO_ID(IDB_POLYGONS));

	for(int i = 0; i< textureSize;i++)
	{
	
		char argsStr[9999];

		strcpy(argsStr,"Args=");

		for(int j=0;j<textureData[i].numArgs;j++)
		{
			char msg[20];
			wsprintf(msg,"%d ",(short)textureData[i].Args[j].val);
			strcat(argsStr,msg);
		}


		wsprintf(buffer,"[%d] %d %d (0x%x) %s",i,textureData[i].numl,textureData[i].numh,textureData[i].cmd.data,argsStr);
		//wsprintf(buffer,"[%d] %d %d (0x%x)",i,textureData[i].numl,textureData[i].numh,textureData[i].cmd);
		HTREEITEM textItem = tree->insertInfoNode(textureP,buffer,TO_ID(IDB_POLYGONS));
		HTREEITEM textureItem = textureData[i].LoadTree(tree,textItem);
		textureData[i].node = textItem;
		textureData[i].idx = i;
		tree->m_CarTree.SetItemData(textItem,(DWORD)&textureData[i]);
	}



	wsprintf(buffer,"UnkData %d",numUnks);
	HTREEITEM unkp = tree->insertInfoNode(carNode,buffer,TO_ID(IDB_UNK));
	for(int i=0;i<numUnks;i+=8)
	{
	  
	   wsprintf(buffer,"u[%d] %d ",i,unkData[i].data);
	   HTREEITEM verItem = tree->insertInfoNode(unkp,buffer,TO_ID(IDB_UNK));
	   tree->m_CarTree.SetItemData(verItem,(DWORD)&unkData[i]);
	   unkData[i].node = verItem;
	   unkData[i].idx = i;
	   unkData[i].object = this;
	}

	int numSmallUnk  = (header->textureBegin - header->scaleEnd)/2;


	wsprintf(buffer,"UnkData2 %d",numSmallUnk);
	HTREEITEM scaleq = tree->insertInfoNode(carNode,buffer,TO_ID(IDB_UNK));
	for(int i=0;i<numSmallUnk;i++)
	{
	   wsprintf(buffer,"s[%d] %d",i,unkscaleData[i].data);
	   HTREEITEM item = tree->insertInfoNode(scaleq,buffer,TO_ID(IDB_UNK));
	   unkscaleData[i].node = item;
	   unkscaleData[i].idx = i;
	   unkscaleData[i].object = this;
	   tree->m_CarTree.SetItemData(item,(DWORD)&unkscaleData[i]);
	}

	wsprintf(buffer,"JamFiles %d",numberOfJamNames);
	HTREEITEM jamp = tree->insertInfoNode(carNode,buffer,TO_ID(IDB_UNK));
	for(int i=0;i<numberOfJamNames;i++)
	{
	  wsprintf(buffer,"JamFile %s",jamFileNames[i]);
	  HTREEITEM item = tree->insertInfoNode(jamp,buffer,TO_ID(IDB_UNK));
	}

}


void GPOObject::parse(int begin)
{
	header = (t_objheader*)&data;
	memoryOffset = begin;

	if (header->Magic!= 0xffff8002)
	{
		char msg[256];
		wsprintf(msg,"Bad Magic Number in exe 0x%x",header->Magic);
		AfxMessageBox(msg,MB_OK);
		valid = FALSE;
		return;
	}

	valid = TRUE;

	int start=106;

	TRACE("Magic %x\n",header->Magic);
	TRACE("Id %x\n",header->id);
	TRACE("scaleBegin %x\n",header->scaleBegin);
	TRACE("scaleEnd %x\n",header->scaleEnd);
	TRACE("textureBegin %x\n",header->textureBegin);
	TRACE("textureEnd %x\n",header->textureEnd);
	TRACE("pointsBegin %x\n",header->pointsBegin);
	TRACE("fileEnd %x\n",header->fileEnd);
	TRACE("vertexEnd %x\n",header->vertexEnd);
	int diff = header->vertexBegin-header->pointsBegin;
	TRACE("pointsBegin %x %d\n#######",diff,diff);

	TRACE("scaleBegin=%x\n",start+(header->scaleBegin-header->scaleBegin));
	TRACE("scaleEnd=%x\n",start+(header->scaleEnd-header->scaleBegin));
	TRACE("textureBegin=%x\n",start+(header->textureBegin-header->scaleBegin));
	TRACE("textureEnd=%x\n",start+(header->textureEnd-header->scaleBegin));
	TRACE("pointsBegin=%x\n",start+(header->pointsBegin-header->scaleBegin));

	//numVerticies = (header->size8/8);
	numScale  = (header->scaleEnd - header->scaleBegin)/2;
	numPoints =	(header->vertexBegin - header->pointsBegin)/8;
	numUnks   = (header->fileEnd -header->vertexEnd)/2;
	int numSmallUnk  = (header->textureBegin - header->scaleEnd)/2;

	TRACE("numVerticies=%d\n",numVerticies);

	numVerticies = 1+(((header->vertexEnd-4) - header->vertexBegin)/4);

	TRACE("numVerticies=%d\n",numVerticies);
	TRACE("numScale=%d\n",numScale);
	TRACE("numPoints=%d\n",numPoints);

	//scaleData  = new Scale[numScale];	
	//pointData  = new Point3D[numPoints];
	//vertexData = new Vertex[numVerticies];
	scaleData  = new Scale[255];	
	pointData  = new Point3D[255];
	vertexData = new Vertex[255];

	unkscaleData  = new DatVal[numSmallUnk];
	textureData = new TextureCmd[1000];
	unkData = new DatVal[30000];
	updateData(TRUE,TRUE);
}

void GPOObject::updateData(BOOL updateTexture,BOOL reread)
{
	int start=78;

	int numSmallUnk  = (header->textureBegin - header->scaleEnd)/2;

	for(int i=0;i<numScale;i++)
	{
		int offset = start+(i*2);
		int val = GetPCWord(offset);
		//TRACE("Scale %d = %d\n",i,val);

		if (val > 0x8000) val = -(0x10000-val);
		if (reread) scaleData[i].val    = val;
		scaleData[i].offset = offset;
		scaleData[i].object = this;
	}

	
	for(int i=0;i<numSmallUnk;i++)
	{
	   int offset = start+(header->scaleEnd-header->scaleBegin)+(i*2);
	   int data   = (int)GetPCWord(offset);	
	   if (reread) unkscaleData[i].data = data;
	}




	for(int i=0;i<numVerticies;i++)
	{
		//int offset = 4+start+(header->vertexBegin-header->scaleBegin)+(i*4);
		int offset = start+(header->vertexBegin-header->scaleBegin)+(i*4);
		vertexData[i].offset = offset;
		vertexData[i].object = this;
		int from = GetPCByte(offset);
		int to   = GetPCByte(offset+1);
		int A    = GetPCByte(offset+2);
		int B    = GetPCByte(offset+3);
		if (reread) vertexData[i].from = from;
		if (reread) vertexData[i].to = to;
	}

	for(int i=0;i<numUnks;i++)
	{
	   int offset = start+(header->vertexEnd-header->scaleBegin)+(i*2);
	   int data   = (int)GetPCWord(offset);	
	   if (reread) unkData[i].data = data;
	}

	for(int i=0;i<numPoints;i++)
	{
		int offset = start+(header->pointsBegin-header->scaleBegin)+(i*8);
		//int offset = 0x1126+(i*8);
		pointData[i].offset = offset;
		pointData[i].object = this;
		int x   = (int)GetPCWord(offset);
		int y   = (int)GetPCWord(offset+2);
		int z   = (int)GetPCWord(offset+4);
		int u   = (int)GetPCWord(offset+6);

		if (reread) pointData[i].ox = x;
		if (reread) pointData[i].oy = y;

		//TRACE("%x (0x%x 0x%x 0x%x)\n",offset,x,y,z);

		if (reread)
		{
		if (z > 0x8000) z = -(0x10000-z);

		if (x < 0x8000)
		{	
		 if (x > 0x80 && x < 0xFF)
		 {
			int idx = (x-0x84)/4;
			x = scaleData[idx].val;
			x = -x;
		 }
		 else if (x > 0)
		 {
		   int idx = (x-0x4)/4;
		   x = scaleData[idx].val;
		 }
		
		 if (y > 0x80 && y < 0xFF)
		 {
			int idx = (y-0x84)/4;
			y = scaleData[idx].val;
			y = -y;
		 }
		 else if (y > 0)
		 {
			int idx = (y-0x04)/4;
			y = scaleData[idx].val;
		
		 }
		}
		else
		{
		   int pidx = x - 0x8000;
		   x = pointData[pidx].x;
		   y = pointData[pidx].y;
		}

		pointData[i].x = x;
		pointData[i].y = y;
		pointData[i].z = z;
		}
	}


	if (updateTexture)
	{
	int count = start+(header->textureBegin-header->scaleBegin);
	int end   = start+(header->textureEnd-header->scaleBegin);

	//textureSize = end-count;

	int idx=0;
	textureSize = 0;

	if (reread)
	{
	 while(count < end)
	 {
		textureData[idx].offset = count;
		int numl = GetPCByte(count++);
		int numh = GetPCByte(count++);
		int cmd  = GetPCWord(count);count+=2;
		textureData[idx].numl = numl;
		textureData[idx].numh = numh;
		textureData[idx].cmd.data = cmd;
		textureData[idx].object = this;
		

		int nArgs=0;

		switch(cmd)
		{
		   default:
			   {
				   CString msg;
				   msg.Format("Unknown Command %x - Please report",cmd);
				   AfxMessageBox(msg);
			   }break;
		   case 0x80:
			   {
				textureData[idx].Args[nArgs].val = GetPCByte(count);count+=2;
				textureData[idx].Args[nArgs++].desc = _strdup( "Point Index");
				textureData[idx].Args[nArgs].val = GetPCByte(count);count+=2;
				textureData[idx].Args[nArgs++].desc = _strdup( "Always 255");
				textureData[idx].Args[nArgs].val = GetPCByte(count);count+=2;
				textureData[idx].Args[nArgs++].desc = _strdup( "Jam Id");
				textureData[idx].numArgs = nArgs;
			   }break;
		   case 0x10:
		   case 0x11:
		   case 0x12:
		   case 0x13:
		   case 0x14:
		   case 0x15:
		   case 0x16:
		   case 0x17:
		   case 0x18:
		   case 0x19:
		   case 0x1a:
		   case 0x1b:
		   case 0x1c:
		   case 0x1d:
		   case 0x1e:
		   case 0x1f:
			   {
			   int tid =  GetPCWord(count);count+=2;
			   textureData[idx].Args[nArgs].val = tid; // texture
			   textureData[idx].textureid = &textureData[idx].Args[nArgs].val;
			   textureData[idx].Args[nArgs++].desc = _strdup( "Texture");
			   int rotation = GetPCWord(count);count+=2;
			   textureData[idx].Args[nArgs].val =  rotation;// rotation
			   textureData[idx].Args[nArgs++].desc = _strdup( "Rotation");

			   if (rotation >= 0x80)
			   {
				 // read flags
				 int flags = GetPCByte(count);count+=2;
				 textureData[idx].Args[nArgs].val = flags;
				 textureData[idx].Args[nArgs++].desc = _strdup( "Num Flag Identifier");

				 if (flags == 0x1){
					 textureData[idx].Args[nArgs].val = GetPCWord(count);count+=2;
					 textureData[idx].hres = &textureData[idx].Args[nArgs].val;
					 textureData[idx].Args[nArgs++].desc = _strdup( "Horizontal Repeat (A)");
					 textureData[idx].Args[nArgs].val = GetPCWord(count);count+=2;
					 textureData[idx].vres = &textureData[idx].Args[nArgs].val;
					 textureData[idx].Args[nArgs++].desc = _strdup( "Vertical Repeat (B)");
				 }
				 else if (flags == 0x11)
				 {
					 textureData[idx].Args[nArgs].val = GetPCWord(count);count+=2;
					 textureData[idx].Args[nArgs++].desc = _strdup( "Flag Vertical/Horizontal Repeat (C)");
				     textureData[idx].Args[nArgs].val = GetPCWord(count);count+=2;
					 textureData[idx].Args[nArgs++].desc = _strdup( "Flag Vertical/Horizontal Repeat (D)");
					 textureData[idx].Args[nArgs].val = GetPCWord(count);count+=2;
					 textureData[idx].Args[nArgs++].desc = _strdup( "Flag Vertical/Horizontal Repeat (D1)");
				 }				 
				 else if (flags == 0x3)
				 {
					 textureData[idx].Args[nArgs].val = GetPCWord(count);count+=2;
					 textureData[idx].Args[nArgs++].desc = _strdup( "Flag Vertical/Horizontal Repeat (E)");
				     textureData[idx].Args[nArgs].val = GetPCWord(count);count+=2;
					 textureData[idx].Args[nArgs++].desc = _strdup( "Flag Vertical/Horizontal Repeat (F)");
					 textureData[idx].Args[nArgs].val = GetPCWord(count);count+=2;
					 textureData[idx].Args[nArgs++].desc = _strdup( "Flag Vertical/Horizontal Repeat (G)");
					 textureData[idx].Args[nArgs].val = GetPCWord(count);count+=2;
					 textureData[idx].Args[nArgs++].desc = _strdup( "Flag Vertical/Horizontal Repeat (H)");
				 }
				 else if (flags == 0x4)
				 {
					 textureData[idx].Args[nArgs].val = GetPCWord(count);count+=2;
					 textureData[idx].Args[nArgs++].desc = _strdup( "Flag Vertical/Horizontal Repeat (I)");
				 }
				 else if (flags == 0x5)
				 {
					 textureData[idx].Args[nArgs].val = GetPCWord(count);count+=2;
					 textureData[idx].hres = &textureData[idx].Args[nArgs].val;
					 textureData[idx].Args[nArgs++].desc = _strdup( "Horizontal Repeat (J)");
					 textureData[idx].Args[nArgs].val = GetPCWord(count);count+=2;
					 textureData[idx].Args[nArgs++].desc = _strdup( "Flag Vertical/Horizontal Repeat (K)");
					 textureData[idx].Args[nArgs].val = GetPCWord(count);count+=2;
					 textureData[idx].Args[nArgs++].desc = _strdup( "Flag Vertical/Horizontal Repeat (L)");
				 }
				 else if (flags == 0x7)
				 {
					 textureData[idx].Args[nArgs].val = GetPCWord(count);count+=2;
					 textureData[idx].Args[nArgs++].desc = _strdup( "Flag Vertical/Horizontal Repeat (M)");
					 textureData[idx].Args[nArgs].val = GetPCWord(count);count+=2;
					 textureData[idx].Args[nArgs++].desc = _strdup( "Flag Vertical/Horizontal Repeat (N)");
					 textureData[idx].Args[nArgs].val = GetPCWord(count);count+=2;
					 textureData[idx].Args[nArgs++].desc = _strdup( "Flag Vertical/Horizontal Repeat (O)");
					 textureData[idx].Args[nArgs].val = GetPCWord(count);count+=2;
					 textureData[idx].Args[nArgs++].desc = _strdup( "Flag Vertical/Horizontal Repeat (P)");
					 textureData[idx].Args[nArgs].val = GetPCWord(count);count+=2;
					 textureData[idx].Args[nArgs++].desc = _strdup( "Flag Vertical/Horizontal Repeat (Q)");
				 }
				 else if (flags == 0x10)
				 {
					 textureData[idx].Args[nArgs].val = GetPCWord(count);count+=2;
					 textureData[idx].Args[nArgs++].desc = _strdup( "Flag Vertical/Horizontal Repeat (M)");
				 }
				 else
				 {
				 }
			   }

			   textureData[idx].numPts = 0;
			   // verticies
			   while(TRUE)
			   {
				int vertex = GetPCWord(count);
				
			    textureData[idx].Args[nArgs].val = vertex;
				textureData[idx].PtsList[textureData[idx].numPts++] = vertex;
				textureData[idx].Args[nArgs++].desc = _strdup( "Vertex");
			    count+=2;	  
				if (vertex == 0) break;
			   }
			   textureData[idx].Args[nArgs-1].desc = _strdup( "Terminator");
			   textureData[idx].numArgs = nArgs;
			   }
			   break;


		}
		textureData[idx].parse();
		textureData[idx].endoffset = count;
		idx++;
		textureSize++;	
	  }
      }
    }
}


void GPOObject::write(FILE *fp)
{
	//header = (t_objheader*)&data;
	//memoryOffset = begin;
	int icount = 0;

	int numBools  = (header->pointsBegin - header->textureEnd)/4;
	int numSmallUnk  = (header->textureBegin - header->scaleEnd)/2;

	int numDataElements = 28 + numScale + (numPoints*4) + (numVerticies*2) + numUnks + (numBools*2)+numSmallUnk+1;

	for(int i=0;i<textureSize;i++)	
	{
		numDataElements++;
		numDataElements++;
	    for(int j=0;j<textureData[i].numArgs;j++)
		{
		int val = textureData[i].Args[j].val;
		numDataElements++;
		}
	}
	
	fprintf(fp,"# number of data elements\n");
	fprintf(fp,"%d\n",numDataElements);
	fprintf(fp,"# offset bytes value\n");
	fprintf(fp,"%d %d 0x%x // magic \n",icount,2,0x8002);icount+=2;
	fprintf(fp,"%d %d 0x%x // id\n",icount,2,(short)header->id);icount+=2;
	fprintf(fp,"%d %d 0x%x // scaleBegin %d\n",icount,4,header->scaleBegin,header->scaleBegin);icount+=4;
	fprintf(fp,"%d %d 0x%x // scaleEnd %d\n",icount,4,header->scaleEnd,header->scaleEnd);icount+=4;
	fprintf(fp,"%d %d 0x%x // textureBegin %d\n",icount,4,header->textureBegin,header->textureBegin);icount+=4;
	fprintf(fp,"%d %d 0x%x // pointsBegin %d\n",icount,4,header->pointsBegin,header->pointsBegin);icount+=4;
	fprintf(fp,"%d %d 0x%x // vertexBegin %d\n",icount,4,header->vertexBegin,header->vertexBegin);icount+=4;
	fprintf(fp,"%d %d 0x%x // textureEnd %d\n",icount,4,header->textureEnd,header->textureEnd);icount+=4;
	fprintf(fp,"%d %d 0x%x // vertexEnd %d\n",icount,4,header->vertexEnd,header->vertexEnd);icount+=4;	
	fprintf(fp,"%d %d 0x%x // fileEnd %d\n",icount,4,header->fileEnd,header->fileEnd);icount+=4;
	fprintf(fp,"%d %d 0x%x // fileEnd2 %d\n",icount,4,header->fileEnd2,header->fileEnd2);icount+=4;
	fprintf(fp,"%d %d 0x%x // always0 \n",icount,4,header->always0);icount+=4;
	fprintf(fp,"%d %d 0x%x // always1 \n",icount,4,header->always1);icount+=4;
	fprintf(fp,"%d %d 0x%x // unknown? \n",icount,2,header->unk);icount+=2;
	fprintf(fp,"%d %d 0x%x // size \n",icount,2,header->size);icount+=2;
	fprintf(fp,"%d %d 0x%x // size8 \n",icount,2,header->size8);icount+=2;
	
	/* unknown patron possible values of importance
	54 2 0xd 	//13
    56 2 0xffff 	//65535
    58 2 0x7fff 	//32767
    60 2 0x3ae 	//942
    62 2 0x0 	//0
    64 2 0x0 	//0
    66 2 0xaaa 	//2730
    68 2 0x30 	//48
    70 2 0x30 	//48
    72 2 0x3800 	//14336
    74 2 0x68 	//104
    76 2 0x68 	//104
	*/

	fprintf(fp,"%d %d 0x%x // unknown data squence possible importance \n",icount,2,13);icount+=2;
	fprintf(fp,"%d %d 0x%x // unknown data squence possible importance \n",icount,2,65535);icount+=2;
	fprintf(fp,"%d %d 0x%x // unknown data squence possible importance \n",icount,2,32767);icount+=2;
	fprintf(fp,"%d %d 0x%x // unknown data squence possible importance \n",icount,2,942);icount+=2;
	fprintf(fp,"%d %d 0x%x // unknown data squence possible importance \n",icount,2,0);icount+=2;
	fprintf(fp,"%d %d 0x%x // unknown data squence possible importance \n",icount,2,0);icount+=2;
	fprintf(fp,"%d %d 0x%x // unknown data squence possible importance \n",icount,2,2730);icount+=2;
	fprintf(fp,"%d %d 0x%x // unknown data squence possible importance \n",icount,2,48);icount+=2;
	fprintf(fp,"%d %d 0x%x // unknown data squence possible importance \n",icount,2,48);icount+=2;
	fprintf(fp,"%d %d 0x%x // unknown data squence possible importance \n",icount,2,14336);icount+=2;
	fprintf(fp,"%d %d 0x%x // unknown data squence possible importance \n",icount,2,104);icount+=2;
	fprintf(fp,"%d %d 0x%x // unknown data squence possible importance \n",icount,2,104);icount+=2;
	
	
	for(int i=0;i<numScale;i++)	
	{
		int val = scaleData[i].val;
		fprintf(fp,"%d %d 0x%x // scale[%d] \n",icount,2,val,i);icount+=2;
	}

	

	for(int i=0;i<numSmallUnk;i++)	
	{
		//int val = scaleData[i].val;
		//fprintf(fp,"%d %d 0x%x // Unknown DANGER! \n",icount,2,1024,i);icount+=2;
		int val = unkscaleData[i].data;
		fprintf(fp,"%d %d 0x%x // Unknown DANGER! \n",icount,2,val);icount+=2;
	}
	//fprintf(fp,"%d %d 0x%x // Unknown DANGER! \n",icount,2,255,i);icount+=2;

	int numTextures  = (header->textureEnd - header->textureBegin)/2;
	// Place Texture Data in here
	for(int i=0;i<textureSize;i++)	
	{
	    //textureData[i]
		//char argsStr[1024];

		//strcpy(argsStr,"Args=");
		fprintf(fp,"%d %d 0x%x // Texture Cmd numl\n",icount,2,textureData[i].numl);icount+=2;
		//fprintf(fp,"%d %d 0x%x // Texture Cmd numh\n",icount,2,textureData[i].numh);icount+=2;
		fprintf(fp,"%d %d 0x%x // Texture Cmd cmd\n",icount,2,textureData[i].cmd.getVal());icount+=2;

		for(int j=0;j<textureData[i].numArgs;j++)
		{
		//	char msg[20];
		//	wsprintf(msg,"%x ",textureData[i].Args[j]);
		//	strcat(argsStr,msg);
		//	
	    int val = textureData[i].Args[j].val;
		char * desc = textureData[i].Args[j].desc;
		fprintf(fp,"%d %d 0x%x // Texture Cmd[%d] Arg[%d] = %s\n",icount,2,val,i,j,desc);icount+=2;
		}
		//fprintf(fp,"%d %d 0x%x // Texture Cmd[%d] == %s\n",icount,2,0,i,argsStr);icount+=2;
	}

	//int numBools  = (header->pointsBegin - header->textureEnd)/4;
	// Place Texture Data in here
	for(int i=0;i<numBools;i++)	
	{
	   fprintf(fp,"%d %d 0x%x // unknown (always 1)[%d] \n",icount,2,1,i);icount+=2;
	   fprintf(fp,"%d %d 0x%x // unknown (always 0)[%d] \n",icount,2,0,i);icount+=2;
	}

	// followed by BOOLEAN table?

	for(int i=0;i<numPoints;i++)	
	{
		int val = pointData[i].ox;
		fprintf(fp,"%d %d 0x%x // pt[%d].x \n",icount,2,val,i);icount+=2;
		val = pointData[i].oy;
		fprintf(fp,"%d %d 0x%x // pt[%d].y \n",icount,2,val,i);icount+=2;
		val = pointData[i].z;
		fprintf(fp,"%d %d 0x%x // pt[%d].z \n",icount,2,val,i);icount+=2;
		fprintf(fp,"%d %d 0x%x // data padding \n",icount,2,0);icount+=2;
	}

	for(int i=0;i<numVerticies;i++)	
	{
		int from = vertexData[i].from;
		int to   = vertexData[i].to;

		int val = from+(to*256);
		fprintf(fp,"%d %d 0x%x // vertext data[%d] (%d -> %d) \n",icount,2,val,i,from,to);icount+=2;
		
		fprintf(fp,"%d %d 0x%x // vertex data padding \n",icount,2,0);icount+=2;
	}

	for(int i=0;i<numUnks;i++)	
	{
		int val = unkData[i].data;
		fprintf(fp,"%d %d 0x%x // unknown data[%d] \n",icount,2,val,i);icount+=2;
	}
	fprintf(fp,"%d %d 0x%x // Terminator \n",icount,2,0);icount+=2;


	fprintf(fp,"##JAM_TEXTURES %d\n",numberOfJamNames);
	for(int i=0;i<numberOfJamNames;i++)
	{
	  fprintf(fp,"##JAM_FILE %d 0 %s",i,jamFileNames[i].c_str());
	}


	/*
	for(int i=0;i<78;i++)
	{
		
	}
	numVerticies = header->size8/8;
	numScale  = (header->scaleEnd - header->scaleBegin)/2;
	numPoints =	(header->vertexBegin - header->pointsBegin)/8;
	numUnks   = (header->fileEnd -header->vertexEnd)/2;

	TRACE("numVerticies=%d\n",numVerticies);

	numVerticies = (header->vertexEnd - header->vertexBegin)/4;

	TRACE("numVerticies=%d\n",numVerticies);
	TRACE("numScale=%d\n",numScale);
	TRACE("numPoints=%d\n",numPoints);

	scaleData  = new Scale[numScale];
	pointData  = new Point3D[numPoints];
	vertexData = new Vertex[numVerticies];
	textureData = new TextureCmd[1000];
	unkData = new DatVal[30000];
	*/
}

void GPOObject::PatchData()
{
	for(int i=0;i<numScale;i++)
	{
		if (scaleData[i].isChanged())
		{
		  scaleData[i].patch();
		}
	}
	for(int i=0;i<numVerticies;i++)
	{
		if (vertexData[i].isChanged())
		{
		  vertexData[i].patch();
		}
	}
	for(int i=0;i<numPoints;i++)
	{
		if (pointData[i].isChanged())
		{
		  pointData[i].patch();
		}
	}

	for(int i=0;i<textureSize;i++)
	{
		if (textureData[i].isChanged())
		{
		  textureData[i].patch();
		}
	}
}

void t_Arg::Open(CCarTree *tree)
    {
	CDataValue *dlg = new CDataValue();

	dlg->m_DataVal = val;
	int result = dlg->DoModal();

	if (result!=IDCANCEL)
	{
		//update model
		val = dlg->m_DataVal;
		updateLabel(tree);
	}
    }

void GPOObject::ParseAdditionalCommands(char *line)
{
	int pos;
	char cmd[256];
	sscanf(line,"##%s%n",cmd,&pos);

	if (!strcmp(cmd,"JAM_FILE"))
	{
		char *ptr = line+=pos;
		int textureid;
		int id;
		char jamName[256];
		sscanf(ptr,"%d %d %s",&id,&textureid,jamName);
		addUniqueJamFile(jamName);		
	}
}

char * getFullPathToJamFileName(const char *filename)
{
		  static char buffer[512];
		  CString gp2Location = ((CObjectEditorApp*)AfxGetApp())->getGP2Location();

		  wsprintf(buffer,"%s\\%s",gp2Location,filename);
		  return buffer;
}

JAM *GPOObject::GetJAM(int id)
    {
	int size = numberOfJamNames;
	int count=0;

	for(int i=size-1;i>=0;i--)
	{
	   std::string name  = jamFileNames[i];

	   JAM *jams = new JAM(getFullPathToJamFileName(name.c_str()));
	   jams->OpenToRead(FALSE);

	   int subimages = jams->getNumberOfImages();

	   for(int j=0;j<subimages;j++)
	   {
		int idx = jams->getImageId(j);
		if (idx == id) return jams;
	   }
	}
	return NULL;
   }

void Object3D::AddScaleValue()
{
   int lastOffset = scaleData[numScale].offset;
   scaleData[numScale].val = -99;
   scaleData[numScale].setChanged();
   scaleData[numScale].object = this;
   scaleData[numScale].offset = lastOffset+2;
   numScale++;
   //header->scaleBegin;
   header->scaleEnd+=2;
   header->textureBegin+=2;
   header->pointsBegin+=2;
   header->vertexBegin+=2;
   header->textureEnd+=2;
   header->vertexEnd+=2;
   header->fileEnd+=2;
   header->fileEnd2+=2;
   //updateData(TRUE);
   PatchData();
}

void  Object3D::AddVertexValue()
{
   int lastOffset = vertexData[numVerticies-1].offset;
   vertexData[numVerticies].to = 0;
   vertexData[numVerticies].from = 0;
   vertexData[numVerticies].object = this;
   vertexData[numVerticies].offset = lastOffset+2;
   vertexData[numScale].setChanged();
   numVerticies++;
   //header->scaleBegin;
   //header->scaleEnd+=2;
   //header->textureBegin+=2;
   //header->pointsBegin+=2;
   //header->vertexBegin+=2;
   //header->textureEnd+=2;
   header->vertexEnd+=4;
   header->fileEnd+=4;
   header->fileEnd2+=4;
   header->size+=1;
   header->size8+=8;
   //updateData(TRUE);
   PatchData();
}

void Object3D::AddPointValue()
{
   int lastOffset = pointData[numPoints-1].offset;
   pointData[numPoints].ox = 84;
   pointData[numPoints].oy = 4;
   pointData[numPoints].x = -99;
   pointData[numPoints].y = -99;
   pointData[numPoints].z = -99;
   pointData[numPoints].object = this;
   pointData[numPoints].offset = lastOffset+8;
   numPoints++;
   //header->scaleBegin;
   //header->scaleEnd+=2;
   //header->textureBegin+=2;
   //header->pointsBegin+=2;
   header->vertexBegin+=8;
   //header->textureEnd+=4;
   header->vertexEnd+=8;
   header->fileEnd+=8;
   header->fileEnd2+=8;
   //updateData(FALSE);
   PatchData();
}

void Object3D::AddTextureCmd(int vsize)
{
   //86 2 0x0 // Texture Cmd numl
   //88 2 0x18 // Texture Cmd cmd
   //90 2 0x1a // Texture Cmd[0] Arg[0] = Texture
   //92 2 0x3 // Texture Cmd[0] Arg[1] = Rotation
   // 94 2 0xffff // Texture Cmd[0] Arg[2] = Vertex
   //96 2 0x2 // Texture Cmd[0] Arg[3] = Vertex
   //98 2 0x3 // Texture Cmd[0] Arg[4] = Vertex
   //100 2 0xfffc // Texture Cmd[0] Arg[5] = Vertex
   //102 2 0x0 // Texture Cmd[0] Arg[6] = Terminator
   int size = 24;

   switch(vsize)
   {
	  case 8: size=32;break;
	  case 7: size=30;break;
	  case 6: size=28;break;
	  case 5: size=26;break;
      case 4: size=24;break;
	  case 3: size=22;break;
   }

  
   int lastCommand = textureData[textureSize-1].numl;
   int lastOffset  = textureData[textureSize-1].endoffset;
   textureData[textureSize].offset = lastOffset;
   textureData[textureSize].object = this;
   textureData[textureSize].numl = lastCommand+1;
   textureData[textureSize].numh = 0;
   textureData[textureSize].numArgs = 0;
   textureData[textureSize].cmd.data = 0x18;
   int count = 0;
   textureData[textureSize].Args[count].desc = "Texture";
   textureData[textureSize].Args[count++].val = 99;
   textureData[textureSize].Args[count].desc = "Rotation";
   textureData[textureSize].Args[count++].val = 0x83;
   textureData[textureSize].Args[count].desc = "Flags Id";
   textureData[textureSize].Args[count++].val = 0x11;
   textureData[textureSize].Args[count].desc = "Flags";
   textureData[textureSize].Args[count++].val = 256;
   textureData[textureSize].Args[count].desc = "Flags";
   textureData[textureSize].Args[count++].val = 256;
   for(int i=0;i<vsize;i++)
   {
   textureData[textureSize].Args[count].desc = "Vertex";
   textureData[textureSize].Args[count++].val = i;
   }
   //textureData[textureSize].Args[count++].val = 2;
   //textureData[textureSize].Args[count++].val = 3;
   //textureData[textureSize].Args[count++].val = 4;
   textureData[textureSize].Args[count].desc = "Terminator";
   textureData[textureSize].Args[count++].val = 0;
   textureData[textureSize].numArgs = count;
   textureData[textureSize].endoffset = textureData[textureSize].offset+size;
   header->textureEnd+=size;
   header->pointsBegin+=size+4;
   header->vertexBegin+=size+4;   
   header->vertexEnd+=size+4;
   header->fileEnd+=size+4;
   header->fileEnd2+=size+4;
   textureData[textureSize].setChanged();
   textureSize++;
   PatchData();
   //updateData(TRUE);
   
}

void Object3D::UpdatePoints()
{
    for(int i=0;i<numPoints;i++)
	{
		//int offset = start+(header->pointsBegin-header->scaleBegin)+(i*8);
		//int offset = 0x1126+(i*8);
		//pointData[i].offset = offset;
		//pointData[i].object = this;
		int x   = pointData[i].ox;
		int y   = pointData[i].oy;
		int z   = pointData[i].z;
		
		if (z > 0x8000) z = -(0x10000-z);

		if (x < 0x8000)
		{	
		 if (x > 0x80 && x < 0xFF)
		 {
			int idx = (x-0x84)/4;
			x = scaleData[idx].val;
			x = -x;
		 }
		 else if (x > 0)
		 {
		   int idx = (x-0x4)/4;
		   x = scaleData[idx].val;
		 }
		
		 if (y > 0x80 && y < 0xFF)
		 {
			int idx = (y-0x84)/4;
			y = scaleData[idx].val;
			y = -y;
		 }
		 else if (y > 0)
		 {
			int idx = (y-0x04)/4;
			y = scaleData[idx].val;
		
		 }
		}
		else
		{
		   int pidx = x - 0x8000;
		   x = pointData[pidx].x;
		   y = pointData[pidx].y;
		}

		pointData[i].x = x;
		pointData[i].y = y;
		pointData[i].z = z;
	}
}

static char lpszJamFilter[] = "F1GP2 Jam Files (*.jam)|*.jam|All Files (*.*)|*.*|";

void Object3D::AddNewJamFile()
{
		  char fileName[256];

		  strcpy(fileName,"*.jam");
		  CFileDialog *fdlg = new CFileDialog(TRUE,"jam",(LPCSTR)&fileName,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,lpszJamFilter);
	
	      int result = fdlg->DoModal();

		  if (result != IDCANCEL)
		  {
			  char message[256];
			  CString filename = fdlg->GetFileName();
			  CString path = fdlg->GetPathName();

			  char drive[10];
			  //char directoy[256];
			  char file[256];
			  char ext[256];
			  char directory[256];
			  
			  _splitpath(path,drive,directory,file,ext);

			  CString newDir = CString(directory);
			  newDir.MakeLower();
			  filename.MakeLower();

			  int stidx=0;

			  if ((stidx=newDir.Find("gamejams"))==-1)
			  {								 
				  AfxMessageBox("Cannot find Jam files not in GameJams Tree");
			  }

			  int length = newDir.GetLength();

			  CString adjustedPath = newDir.Right(length-stidx);

			  wsprintf(message,"Setting Jam File to %s",filename);
			  result = AfxMessageBox(message,MB_YESNO);

			  if (result == IDYES)
			  {
			   char buffer[256];
			   wsprintf(buffer,"%s%s",adjustedPath,filename);
	
			   addUniqueJamFile(buffer);
			  }

			  
		  }

		  delete fdlg;
}



