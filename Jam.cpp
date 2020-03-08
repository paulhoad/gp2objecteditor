
#include "stdafx.h"
#include "Jam.h"

#include "Pallette.h"

JAM::~JAM()
{

	int size = getNumberOfImages();		

	//for(int i=0;i<size;i++)
	//{
	  //delete &entry[i];
	//}
	delete[] entry;

	if (jammemory) free(jammemory);
}

JAMEntry::~JAMEntry()
{
	   //if (info) free(info);
	   if (jamBmp) 
	   {
		   delete (jamBmp);
		   jamBmp=NULL;
	   }
	   if (jamData) free(jamData);
}

unsigned char *JAM::getImagePtr(int img)
{
	    int offset=0;
	    //for(int i=0;i<img;i++)
		//{
		  //offset+= entry[i].info->width*entry[i].info->height;
	      //offset+= 256*entry[i].info->height;
		//}
		//offset+=entry[img].info->x;
		//int imageptr = entry[img].info->image_ptr;
		int imageptr = offset;
		unsigned char *ptr = jamimagememory;
		ptr += imageptr;
		entry[img].img_ptr = ptr;
		return ptr;
}

BOOL JAM::LoadBMPImage( CBitmap& bitmap, int m_Width,int m_Height)
{		
	    int size = getNumberOfImages();

		int width  = getMaxX();
		int height = getMinX();

		for(int i=0;i<size;i++)
		{
			LoadSingleBMPImage(bitmap,0,0,i);
		}
		return TRUE;
}



unsigned char * JAMEntry::AdjustImage(int image_total_size,unsigned char *imgo)
{
	if (imageAdjusted) return jamData;

	unsigned char *img     = img_ptr;
	
	int m_Width     = info->width;
	int m_Height    = info->height;
	int m_X     = info->x;
	int m_Y     = info->y;
	int m_ImagePal  = info->palette_size_div4;
	//unsigned char * m_PalPtrPal = info->imageptr;

	//m_Width = 256;

	jamData = (unsigned char*)malloc(m_Width*m_Height);
	unsigned char * palptr = palette1;

	//TRACE("%x %d %d\n",img_ptr,m_Width,m_Height);

	unsigned char localPal[256];

	if (m_ImagePal > 0)
	{	 
	 for(int i=0;i<m_ImagePal;i++)
     {
		   unsigned char idx = palptr[i];
		   localPal[i] = idx;
	 }
	}
	else
	{
	 for(int i=0;i<256;i++)
     {
		  localPal[i] = i;
	 }
	}
	 
	 int ox = m_X;
	 int oy = m_Y;
	 for(int i=0;i<m_Height;i++)
	 {
	  for(int j=0;j<m_Width;j++)
	  {
		 int idx = ox+j+((oy+i)*256);
		 int idxD = (i*m_Width)+j;
		 jamData[idxD] = localPal[img[idx]];
	  }
	 }

	imageAdjusted = TRUE;
	return jamData;
}

BOOL JAM::LoadSingleBMPImage( CBitmap& bitmap, int m_Width,int m_Height,int index)
{		
	    if (entry[index].jamBmp==NULL) 
		{
		entry[index].jamBmp = new BMP();
		
	   		
		if (!valid)
		{
			OpenToRead(FALSE);
		}
		unsigned char *img     = getImagePtr(index);
		unsigned char *palptr  = getPalletePtr(index);
		int m_ImagePal         = getPalleteSize(index);

		RGBQUAD *pal = (RGBQUAD *)malloc(256*sizeof(RGBQUAD));

		int palleteSize = sizeof(palette)/sizeof(palette[0]);

		for(int i=0;i<palleteSize;i++)
		{
			pal[i].rgbRed   = palette[i+1].r;
			pal[i].rgbGreen = palette[i+1].g;
			pal[i].rgbBlue  = palette[i+1].b;
			pal[i].rgbReserved = 0;
		}

		unsigned char *newJamData = entry[index].AdjustImage(header->image_total_size,img);

		entry[index].jamBmp->SetPalette(pal);

		//TRACE("w h %d %d\n",m_Width,m_Height);

		entry[index].jamBmp->Create(m_Width,m_Height,newJamData);
		free(pal);
		}

		entry[index].jamBmp->CreateDIB(bitmap);

		
        return TRUE;
}
								
void
JAM::UnJam(
        void    *ptr,
        unsigned int            len
) 
{
        unsigned long   x;
        unsigned int            n;
        unsigned char   *pc;
        unsigned long   *pl;

        x = 0xb082f164UL;
        n = len / 4;
        x |= 1;
        pl = (unsigned long *) ptr;

        while (n--) {
                *pl ^= x;
                ++pl;
                x = x + (x * 4);
        }

        n = len & (4 - 1);
        if (n != 0) {
                pc = (unsigned char *) pl;
                *pc ^= x & 0xff;
                ++pc;
                x >>= 8;
        }
}