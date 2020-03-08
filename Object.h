
#ifndef _INCLUDED_CAR_H
#define _INCLUDED_CAR_H

#include "math.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#include "Observer.h"


class CCarTree;
class JAM;

typedef struct
{
	float r;
	float g;
	float b;
} t_Color;

static t_Color RandomColors[] = {
	{1.0f,0.0f,0.0f},
    {0.0f,1.0f,0.0f},
	{0.0f,0.0f,1.0f},
	{1.0f,1.0f,0.0f},
	{1.0f,0.0f,1.0f},
	{0.0f,1.0f,1.0f},
	{0.5f,0.0f,0.0f},
    {0.0f,0.5f,0.0f},
	{0.0f,0.0f,0.5f},
	{0.5f,0.5f,0.0f},
	{0.5f,0.0f,0.5f},
	{0.0f,0.5f,0.5f},
};

static int numRandomColors = sizeof(RandomColors)/sizeof(RandomColors[0]);


typedef struct
{
   short Magic;
   short id;
   int scaleBegin;
   int scaleEnd;
   int textureBegin;
   int pointsBegin;
   int vertexBegin;
   int textureEnd;
   int vertexEnd;
   int fileEnd;
   int fileEnd2;
   int always0;
   int always1;
   short unk;
   short size;
   short size8;
} t_objheader;


typedef struct
{
    short X;
	short Z;
	short Y;
	short ODirection;
	short OPitch;
	short ORoll;
	short Detail1;
	short Detail2;
} t_IntCamera;




class DatVal: public Observer
{
public:
	DatVal():
	Observer(t_DataVal)
	{
		data = 0;
	}

	int getVal()
	{
		return data;
	}

	void Open(CCarTree *tree);

	void updateLabel(CCarTree *tree);

	int data;
	HTREEITEM node;
	int idx;
};

class t_Arg	: public Observer
{
public:
	t_Arg():
	  Observer(t_TextureCmd)
	{
		val = 0;
		desc = NULL;
	}

	void Open(CCarTree *tree);

	void updateLabel(CCarTree *tree);

	int   val;
	char *desc;
	HTREEITEM node;
};


class TextureCmd : public Observer
{
public:
	TextureCmd():
	Observer(t_TextureCmd)
	{
	    cmd.data=-1;
		numArgs = 0;
		numPts = 0;
		textureid = NULL;
		vres = NULL;
		hres = NULL;
		loadedTextureId = -1;
	}

	int numl;
	int numh;
	DatVal cmd;
	t_Arg Args[20];
	int numArgs;

	CBitmap jamBitmap;
	unsigned char * jamBits;
	int loadedTextureId;


	void parse();

	void Select(CCarTree *tree);

	void Draw(t_Color color);

	void patch();


	HTREEITEM LoadTree(CCarTree *tree,HTREEITEM node);
	
	short PtsList[20];
	int numPts;
	HTREEITEM node;
	int idx;
	int offset;
	int endoffset;

	int *textureid;
	int *vres;
	int *hres;

	double cx;
	double cy;
	double cz;
} ;



class Point3D : public Observer
{
    public:

	Point3D():x(0),y(0),z(0),Observer(t_Point)
	{
		desc.clear();
	}

	Point3D(int x,int y,int z):
	x(x),y(y),z(z),Observer(t_Point)
	{
		desc.clear();
	}

	~Point3D()
	{
	}

	void Open(CCarTree *tree);

	void Select(CCarTree *tree);
	
	int x;
	int y;
	int z;
	//int oz;
	int ox;
	int oy;

	void updateLabel(CCarTree *tree);

	void patch();

	double distance(Point3D *a)
	{
		return sqrt((double)(((a->x-x)*(a->x-x))+((a->y-y)*(a->y-y))+((a->z-z)*(a->z-z))));
	}

	HTREEITEM node;
	int idx;
	int offset;


	std::string desc;

	void setDesc(char *msg)
	{
		desc = msg;
	}

	std::string getDesc()
	{
		return desc;
	}
};


class Vertex : public Observer
{
    public:

	Vertex():
    from(0),to(0),Observer(t_Vertex)
	{}

	Vertex(int f,int t):
	from(f),to(t),Observer(t_Vertex)
	{
	}

	void Open(CCarTree *tree);

	void Select(CCarTree *tree);

	void updateLabel(CCarTree *tree);

	void patch();

	double Length();
	double LengthCM();
	
	int from;
	int to;
	HTREEITEM node;
	int idx;
	int offset;

	double cx;
	double cy;
	double cz;
} ;



class Scale	: public Observer
{
public:

	Scale():val(0),offset(0),Observer(t_Scale)
	{
		//ptsDep = 0;
	}

	Scale(int v,int offset):
	val(v),offset(offset),Observer(t_Scale)
	{
		//ptsDep = 0;
	}

	void Open(CCarTree *tree);

	void patch();

	
	

	int val;
	int offset;
	
	//int depend[255];
	//int ptsDep;
	HTREEITEM node;
	int idx;

	void updateLabel(CCarTree *tree);
	
} ;

class UnknownData	: public Observer
{
public:

	UnknownData():val(0),offset(0),Observer(t_Scale)
	{
		//ptsDep = 0;
	}

	UnknownData(int v,int offset):
	val(v),offset(offset),Observer(t_Scale)
	{
		//ptsDep = 0;
	}

	void Open(CCarTree *tree);

	void patch();

	int val;
	int offset;
	
	//int depend[255];
	//int ptsDep;
	HTREEITEM node;
	int idx;

	void updateLabel(CCarTree *tree);
	
} ;

#define OFFSET_NOSE 31
#define OFFSET_PTS  194

	
#define DEF_CAR_LENGTH 54536
#define DEF_CAR_START 0x14C4A8
#define DEF_HELMET_START 0x1599B0
#define DEF_HELMET_LENGTH 4294

class Object3D: public CObject
{
public:
	Object3D()	
	{
		pDoc = NULL;
		selectedPoint = NULL;
		selectedVertex = NULL;
		selectedTextureCmd = NULL;
		scaleData = NULL;
		vertexData = NULL;
		pointData = NULL;
		unkData = NULL;
	}

	virtual ~Object3D()
	{
		if (scaleData) delete[] scaleData;
		if (pointData) delete[] pointData;
		if (vertexData) delete[] vertexData;
		if (textureData) delete[] textureData;

	}

	void clearSelections()
	{
		selectedPoint = NULL;
		selectedVertex = NULL;
		selectedTextureCmd = NULL;
	}

	int GetPCWord(int offset);
	int GetPCByte(int offset);
	long GetPCDWord(int offset);

	Scale  *scaleData;
	DatVal *unkscaleData;
	Vertex *vertexData;
	Point3D *pointData;
	TextureCmd *textureData;
	DatVal  *unkData;

	void UpdatePoints();

	void AddScaleValue();
	void AddVertexValue();
	void AddPointValue();
	void AddTextureCmd(int vsize);
	void AddNewJamFile();

	int numVerticies;
	int numScale;
	int numPoints;
	int textureSize;
	int numUnks;

	Point3D *selectedPoint;
	Vertex  *selectedVertex;
	TextureCmd *selectedTextureCmd;

	BOOL valid;

	BOOL isValid()
	{
		return valid;
	}

	virtual void PatchData(){}
	virtual void updateData(BOOL updateTexture,BOOL reread){}

	CDocument *pDoc;
	unsigned char data[65535];
	t_objheader *header;
	HTREEITEM carNode;

	void addUniqueJamFile(char *file)
	{
		BOOL found=FALSE;
		for(int i=0;i<numberOfJamNames;i++)
		{
           if (std::string(file) != jamFileNames[i])
		   {
			   found = TRUE;
			   break;
		   }
		}
		if (!found)
		{
		   jamFileNames[numberOfJamNames] = file;
		   numberOfJamNames++;
		}
	}

	std::string jamFileNames[200];
    int   numberOfJamNames;
};






class GPOObject: public Object3D
{
   public:

	int memoryOffset;

	GPOObject()
	{
		numberOfJamNames = 0;
	}

	virtual ~GPOObject()
	{
	}

	void parse(int begin);
	//void write(CString filename);
	void write(FILE *fp);

	void LoadTree(CCarTree *tree);
	void PatchData();
	void updateData(BOOL updateTexture,BOOL reread);

	

	void ParseAdditionalCommands(char *line);

		

	void LoadJamTexture(CBitmap &output,int idx,int hres,int vres);

	JAM *GetJAM(int id);
  
};




#endif
