
#ifndef _INCLUDED_JAM_DEF_H_
#define  _INCLUDED_JAM_DEF_H_


#include "sys/stat.h"
#include "bmp.h"


typedef struct {
     unsigned short    num_items;
     unsigned short    image_total_size;
} JAM_HDR;

typedef struct {
     unsigned char     x;
     unsigned char     y;
     unsigned short    idx_02;
     unsigned short    width;
     unsigned short    height;
     unsigned short    idx_08;
     unsigned short    idx_0A;
     unsigned short    image_ptr; //TODO: long ptr?
     unsigned short    idx_0E;
     unsigned short    palette_size_div4;
     unsigned short    jam_id;
     unsigned short    idx_14;
     unsigned char     idx_16;
     unsigned char     idx_17;
     unsigned char     idx_18[8];
} JAM_ENTRY;

class JAMEntry : public CObject
{
   public:

   unsigned char *jamData;
   BMP           *jamBmp;
   JAM_ENTRY     *info;

   JAMEntry()
   {	  
	   info = NULL;
	   imageAdjusted = FALSE;
	   jamBmp = NULL;
	   jamData = NULL;
   }

   ~JAMEntry();
   
   BOOL imageAdjusted;

   unsigned char *AdjustImage(int size,unsigned char *imgPtr);

   unsigned char *palette1;
   unsigned char *palette2;
   unsigned char *palette3;
   unsigned char *palette4;
   unsigned char *img_ptr;
};


class JAM : public CObject
{
   public:
    JAM_HDR    *header;
	JAMEntry *entry;
	CString filename;
	FILE *fpJam;
	BOOL valid;
	unsigned char *jammemory;
	unsigned char *jamimagememory;

	JAM(CString filename):
		filename(filename),entry(NULL),header(NULL)
	{
		 valid = FALSE;
		 jammemory = NULL;
	}

	~JAM();
	
	BOOL LoadBMPImage( CBitmap& bitmap, int m_Width,int m_Height);
	BOOL LoadSingleBMPImage( CBitmap& bitmap, int m_Width,int m_Height,int index);

	BOOL DrawAllJams(CDC *pDC,double currentScale, BOOL drawOutlines=TRUE);

	void OpenToRead(BOOL notifyErrors)
	{
	   fpJam = fopen(filename,"rb");
	   if (fpJam==NULL)
	   {
		   if (notifyErrors)
		   {
		    char msg[256];
		    wsprintf(msg,"Failed to load JAM File (%s)",filename);
		    AfxMessageBox(msg);
		   }
		   return;
	   }
	   valid = TRUE;
	   Read();
	   fclose(fpJam);
	   
	}

	void Read()
	{
	   if (valid)
	   {
	    if (entry!=NULL)
	    {
		   // jam file has already been read
		   return;
	    }
	   unsigned char* ptr;
       int len = GetFileLength();
	   jammemory = (unsigned char*)malloc(len*sizeof(unsigned char));

	   fread(jammemory,1,len,fpJam);
	   UnJam(jammemory,len);

	   header = (JAM_HDR*)jammemory;
	   ptr = jammemory;

	   entry = new JAMEntry[header->num_items];
	   ptr+=sizeof(JAM_HDR);

	   for(int i=0;i<header->num_items;i++)
	   {
		 (entry[i].info) = (JAM_ENTRY *)ptr;
		 ptr+=sizeof(JAM_ENTRY);
	   }
	   for(int i=0;i<header->num_items;i++)
	   {
		  entry[i].palette1 = ptr;
		  //entry[i].palette1 = jammemory+=entry[i].info->image_ptr;
		  ptr+=entry[i].info->palette_size_div4;
		  entry[i].palette2 = ptr;
		  ptr+=entry[i].info->palette_size_div4;
		  entry[i].palette3 = ptr;
		  ptr+=entry[i].info->palette_size_div4;
		  entry[i].palette4 = ptr;
		  ptr+=entry[i].info->palette_size_div4;
	   }
	   jamimagememory = ptr;
	   }
	}

	int getNumberOfImages()
	{
		if (header)
		  return header->num_items;
		else
		  return 0;
	}

	int getMinX()
	{
		int minX = 0;

		for(int i=0;i<header->num_items;i++)
		{
			minX = min(minX,entry[i].info->x);
		}
		return minX;
	}

	int getMinY()
	{
		int minY = 0;

		for(int i=0;i<header->num_items;i++)
		{
			minY = min(minY,entry[i].info->y);
		}
		return minY;
	}

	int getMaxX()
	{
		int maxX = 0;

		for(int i=0;i<header->num_items;i++)
		{
			maxX = max(maxX,entry[i].info->x+entry[i].info->width);
		}
		return maxX;
	}

	int getMaxY()
	{
		int maxY = 0;

		for(int i=0;i<header->num_items;i++)
		{
			maxY = max(maxY,entry[i].info->y+entry[i].info->height);
		}
		return maxY;
	}

	int getImageWidth(int img)
	{
		return entry[img].info->width;
	}

	int getImageHeight(int img)
	{
		return entry[img].info->height;
	}

	int getImageX(int img)
	{
		return entry[img].info->x;
	}

	int getImageId(int img)
	{
		return entry[img].info->jam_id;
	}


	int getImageIndex(int id)
	{
	   int subimages = getNumberOfImages();
	   for(int j=0;j<subimages;j++)
	   {
		int idx = getImageId(j);
		if (idx == id) return j;
	   }
	   return -1;
	}

	int getImageY(int img)
	{
		return entry[img].info->y;
	}

	int getPalleteSize(int img)
	{
		return entry[img].info->palette_size_div4;
	}

	unsigned char *getPalletePtr(int img)
	{
		return entry[img].palette1;		  
	}

	unsigned char *getImagePtr(int img);
	
	void UnJam(void    *ptr,unsigned int len) ;

	
	int GetFileLength()
	{
		struct _stat buf;
        int result;
   
        /* Get data associated with "stat.c": */
        result = _fstat( _fileno(fpJam), &buf );

        /* Check if statistics are valid: */
        return (int)(buf.st_size );
	}

};


#endif
