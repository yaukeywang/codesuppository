#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

#pragma warning(disable:4996)
#include "tga_io.h"

#define U unsigned int

int tga_write(hfield hf, char *fname)
{
  int xsize, ysize;
  int ix, iy;
  FILE *outfile;
  double temp;

  unsigned char red, green, blue;
  unsigned char IDLength;             /* length of Identifier String */
  unsigned char CoMapType;            /* 0 = no map */
  unsigned char ImgType;              /* image type (see below for values) */
  unsigned char Index_lo, Index_hi;   /* index of first color map entry */
  unsigned char Length_lo, Length_hi; /* number of entries in color map */
  unsigned char CoSize;               /* size colormap entry (15,16,24,32) */
  unsigned char X_org_lo, X_org_hi;   /* x origin of image */
  unsigned char Y_org_lo, Y_org_hi;   /* y origin of image */
  unsigned char Width_lo, Width_hi;   /* width of image */
  unsigned char Height_lo, Height_hi; /* height of image */
  unsigned char PixelSize;            /* pixel size (8,16,24,32) */

    /* ------------- open TGA file ----------- */

  xsize = hf.xsize;
  ysize = hf.ysize;
  outfile = fopen(fname, "wb");
  if (outfile == NULL) {
    fprintf(stderr,"Trouble opening TGA file %s\n",fname);
    return(1);
  }

    /*----------- Write out data to TGA format file ------- */
    /* eighteen-byte header */
  
     /* b0-b7 */
    IDLength = 0; CoMapType = 0; ImgType = 2; Index_lo = 0; Index_hi=0;
    Length_lo = 0; Length_hi = 0; CoSize = 0; X_org_lo = 0; X_org_hi = 0;
    temp = xsize / 256;
    Width_hi = (unsigned char) temp;     /* low and high bytes of x width */
    Width_lo = (unsigned char)(xsize % 256);
    temp = ysize / 256;
    Height_hi = (unsigned char) temp;
    Height_lo = (unsigned char)(ysize % 256);
    PixelSize = 24;
    Y_org_lo = 0;
    Y_org_hi = 0;

    fprintf(outfile,"%c",IDLength);
    fprintf(outfile,"%c",CoMapType);
    fprintf(outfile,"%c",ImgType);
    fprintf(outfile,"%c",Index_lo);
    fprintf(outfile,"%c", Index_hi);
    fprintf(outfile,"%c",Length_lo);
    fprintf(outfile,"%c",Length_hi);
    fprintf(outfile,"%c",CoSize);
    fprintf(outfile,"%c",X_org_lo); fprintf(outfile,"%c",X_org_hi);
    fprintf(outfile,"%c",Y_org_lo); fprintf(outfile,"%c",Y_org_hi);
    fprintf(outfile,"%c",Width_lo);
    fprintf(outfile,"%c",Width_hi);              /* file in lo-byte, hi-byte order b12,b13*/
    fprintf(outfile,"%c",Height_lo);
    fprintf(outfile,"%c",Height_hi);    /* ysize b14, b15 */
    fprintf(outfile,"%c",PixelSize);
    fprintf(outfile,"%c",(unsigned char) 0x20);        /* descriptor byte b17 */

    blue = 0;
    for (iy = 0; iy < ysize; iy++) 
    {
     for (ix = 0; ix < xsize; ix++) 
     {
       red = (unsigned char)((U)(65535.0*El(hf.a,ix,iy))/256); /* high byte */
       green = (unsigned char)((U)(65535.0*El(hf.a,ix,iy)) % 256); /* low byte */
       fprintf(outfile,"%c%c%c",blue,green,red);
     }
    }
  
  fclose(outfile);
  return(0);

} /* end tga_write() */


int tga_save(const char *fname,const unsigned char *image,int wid,int hit)
{
  int xsize, ysize;
  int ix, iy;
  FILE *outfile;
  double temp;

  unsigned char red, green, blue;
  unsigned char IDLength;             /* length of Identifier String */
  unsigned char CoMapType;            /* 0 = no map */
  unsigned char ImgType;              /* image type (see below for values) */
  unsigned char Index_lo, Index_hi;   /* index of first color map entry */
  unsigned char Length_lo, Length_hi; /* number of entries in color map */
  unsigned char CoSize;               /* size colormap entry (15,16,24,32) */
  unsigned char X_org_lo, X_org_hi;   /* x origin of image */
  unsigned char Y_org_lo, Y_org_hi;   /* y origin of image */
  unsigned char Width_lo, Width_hi;   /* width of image */
  unsigned char Height_lo, Height_hi; /* height of image */
  unsigned char PixelSize;            /* pixel size (8,16,24,32) */

    /* ------------- open TGA file ----------- */

  xsize = wid;
  ysize = hit;
  outfile = fopen(fname, "wb");
  if (outfile == NULL) {
    fprintf(stderr,"Trouble opening TGA file %s\n",fname);
    return(1);
  }

    /*----------- Write out data to TGA format file ------- */
    /* eighteen-byte header */

     /* b0-b7 */
    IDLength = 0; CoMapType = 0; ImgType = 2; Index_lo = 0; Index_hi=0;
    Length_lo = 0; Length_hi = 0; CoSize = 0; X_org_lo = 0; X_org_hi = 0;
    temp = xsize / 256;
    Width_hi = (unsigned char) temp;     /* low and high bytes of x width */
    Width_lo = (unsigned char)(xsize % 256);
    temp = ysize / 256;
    Height_hi = (unsigned char) temp;
    Height_lo = (unsigned char)(ysize % 256);
    PixelSize = 24;
    Y_org_lo = 0;
    Y_org_hi = 0;

    fprintf(outfile,"%c",IDLength);
    fprintf(outfile,"%c",CoMapType);
    fprintf(outfile,"%c",ImgType);
    fprintf(outfile,"%c",Index_lo);
    fprintf(outfile,"%c", Index_hi);
    fprintf(outfile,"%c",Length_lo);
    fprintf(outfile,"%c",Length_hi);
    fprintf(outfile,"%c",CoSize);
    fprintf(outfile,"%c",X_org_lo); fprintf(outfile,"%c",X_org_hi);
    fprintf(outfile,"%c",Y_org_lo); fprintf(outfile,"%c",Y_org_hi);
    fprintf(outfile,"%c",Width_lo);
    fprintf(outfile,"%c",Width_hi);              /* file in lo-byte, hi-byte order b12,b13*/
    fprintf(outfile,"%c",Height_lo);
    fprintf(outfile,"%c",Height_hi);    /* ysize b14, b15 */
    fprintf(outfile,"%c",PixelSize);
    fprintf(outfile,"%c",(unsigned char) 0x20);        /* descriptor byte b17 */

    blue = 0;
    for (iy = 0; iy < ysize; iy++) {
     for (ix = 0; ix < xsize; ix++)
     {
       red = *image++;
       green = *image++;
       blue = *image++;
       fprintf(outfile,"%c%c%c",blue,green,red);
     }
    }

  fclose(outfile);
  return(0);

} /* end tga_save() */



unsigned char * tga_load(const char *fname,int *wid,int *hit,int *bpp)
{
  int ix, iy;
  FILE *infile;
  int xsize, ysize;     /* necessary for EL() macro */

  unsigned char red, green, blue, alpha;
  unsigned char IDLength;             /* length of Identifier String */
  unsigned char CoMapType;            /* 0 = no map */
  unsigned char ImgType;              /* image type (see below for values) */
  unsigned char Index_lo, Index_hi;   /* index of first color map entry */
  unsigned char Length_lo, Length_hi; /* number of entries in color map */
  unsigned char CoSize;               /* size colormap entry (15,16,24,32) */
  unsigned char X_org_lo, X_org_hi;   /* x origin of image */
  unsigned char Y_org_lo, Y_org_hi;   /* y origin of image */
  unsigned char Width_lo, Width_hi;   /* width of image */
  unsigned char Height_lo, Height_hi; /* height of image */
  unsigned char PixelSize;            /* pixel size (8,16,24,32) */
  unsigned char Dbyte;            /* pixel size (8,16,24,32) */
  unsigned char *image;
  unsigned char *dst;
  int BPP;



    /* ------------- open TGA file ----------- */

  infile = fopen(fname, "rb");
  if (infile == NULL) {
    fprintf(stderr,"Trouble opening TGA file %s\n",fname);
    return(0);
  }


    /*----------- Read in data from TGA format file ------- */
    /* eighteen-byte header */

    fscanf(infile,"%c",&IDLength);
    fscanf(infile,"%c",&CoMapType);
    fscanf(infile,"%c",&ImgType);
    fscanf(infile,"%c",&Index_lo);
    fscanf(infile,"%c",&Index_hi);
    fscanf(infile,"%c",&Length_lo);
    fscanf(infile,"%c",&Length_hi);
    fscanf(infile,"%c",&CoSize);
    fscanf(infile,"%c",&X_org_lo); fscanf(infile,"%c",&X_org_hi);
    fscanf(infile,"%c",&Y_org_lo); fscanf(infile,"%c",&Y_org_hi);
    fscanf(infile,"%c",&Width_lo);
    fscanf(infile,"%c",&Width_hi);              /* file in lo-byte, hi-byte order b12,b13*/
    fscanf(infile,"%c",&Height_lo);
    fscanf(infile,"%c",&Height_hi);    /* ysize b14, b15 */
    fscanf(infile,"%c",&PixelSize);
    fscanf(infile,"%c",&Dbyte);        /* descriptor byte b17 */


  xsize = (unsigned int) Width_lo + 256*Width_hi;
  ysize = (unsigned int) Height_lo + 256*Height_hi;

/*  fprintf(stderr,"TGA image type %d PixelSize %d Descriptor %d\n",
    (int)ImgType,PixelSize,Dbyte); */
  
  if ( PixelSize == 32 ) *bpp = 4;
  if ( PixelSize == 24 ) *bpp = 3;
  
  BPP = *bpp;

  dst = image = malloc( xsize*ysize*BPP);
  *wid = xsize;
  *hit = ysize;

  for (iy = 0; iy < ysize; iy++)
  {
    for (ix = 0; ix < xsize; ix++)
    {
      if ( BPP == 3 )
      {
        if (fscanf(infile,"%c%c%c",&blue,&green,&red) == EOF)
        {
      	  printf("Warning: %s supposed to be [%dx%d] but stopped at (%d,%d).\n",fname,xsize,ysize,ix,iy);
      	  ix = xsize; iy = ysize;
        }
        else
        {
          *dst++ = red;
          *dst++ = green;
          *dst++ = blue;
        }
      }
      if ( BPP == 4 )
      {
        if (fscanf(infile,"%c%c%c%c",&blue,&green,&red,&alpha) == EOF)
        {
      	  printf("Warning: %s supposed to be [%dx%d] but stopped at (%d,%d).\n",fname,xsize,ysize,ix,iy);
      	  ix = xsize; iy = ysize;
        }
        else
        {
          *dst++ = alpha;
          *dst++ = red;
          *dst++ = green;
          *dst++ = blue;
        }
      }
    }
  }

  fclose(infile);

  return image;
} /* end tga_read() */


int tga_save_gray(const char *fname,const unsigned char *image,int wid,int hit)
{
  int xsize, ysize;
  int ix, iy;
  FILE *outfile;
  double temp;

  unsigned char red, blue;
  unsigned char IDLength;             /* length of Identifier String */
  unsigned char CoMapType;            /* 0 = no map */
  unsigned char ImgType;              /* image type (see below for values) */
  unsigned char Index_lo, Index_hi;   /* index of first color map entry */
  unsigned char Length_lo, Length_hi; /* number of entries in color map */
  unsigned char CoSize;               /* size colormap entry (15,16,24,32) */
  unsigned char X_org_lo, X_org_hi;   /* x origin of image */
  unsigned char Y_org_lo, Y_org_hi;   /* y origin of image */
  unsigned char Width_lo, Width_hi;   /* width of image */
  unsigned char Height_lo, Height_hi; /* height of image */
  unsigned char PixelSize;            /* pixel size (8,16,24,32) */

    /* ------------- open TGA file ----------- */

  xsize = wid;
  ysize = hit;
  outfile = fopen(fname, "wb");
  if (outfile == NULL) {
    fprintf(stderr,"Trouble opening TGA file %s\n",fname);
    return(1);
  }

    /*----------- Write out data to TGA format file ------- */
    /* eighteen-byte header */

     /* b0-b7 */
    IDLength = 0; CoMapType = 0; ImgType = 2; Index_lo = 0; Index_hi=0;
    Length_lo = 0; Length_hi = 0; CoSize = 0; X_org_lo = 0; X_org_hi = 0;
    temp = xsize / 256;
    Width_hi = (unsigned char) temp;     /* low and high bytes of x width */
    Width_lo = (unsigned char)(xsize % 256);
    temp = ysize / 256;
    Height_hi = (unsigned char) temp;
    Height_lo = (unsigned char)(ysize % 256);
    PixelSize = 24;
    Y_org_lo = 0;
    Y_org_hi = 0;

    fprintf(outfile,"%c",IDLength);
    fprintf(outfile,"%c",CoMapType);
    fprintf(outfile,"%c",ImgType);
    fprintf(outfile,"%c",Index_lo);
    fprintf(outfile,"%c", Index_hi);
    fprintf(outfile,"%c",Length_lo);
    fprintf(outfile,"%c",Length_hi);
    fprintf(outfile,"%c",CoSize);
    fprintf(outfile,"%c",X_org_lo); fprintf(outfile,"%c",X_org_hi);
    fprintf(outfile,"%c",Y_org_lo); fprintf(outfile,"%c",Y_org_hi);
    fprintf(outfile,"%c",Width_lo);
    fprintf(outfile,"%c",Width_hi);              /* file in lo-byte, hi-byte order b12,b13*/
    fprintf(outfile,"%c",Height_lo);
    fprintf(outfile,"%c",Height_hi);    /* ysize b14, b15 */
    fprintf(outfile,"%c",PixelSize);
    fprintf(outfile,"%c",(unsigned char) 0x20);        /* descriptor byte b17 */

    blue = 0;
    for (iy = 0; iy < ysize; iy++) {
     for (ix = 0; ix < xsize; ix++)
     {
       red = *image++;
       fprintf(outfile,"%c%c%c",red,red,red);
     }
    }

  fclose(outfile);
  return(0);

} /* end tga_save() */


int tga_save_alpha(const char *fname,const unsigned char *image,int wid,int hit)
{
  int xsize, ysize;
  int ix, iy;
  FILE *outfile;
  double temp;

  unsigned char red, green, blue, alpha;
  unsigned char IDLength;             /* length of Identifier String */
  unsigned char CoMapType;            /* 0 = no map */
  unsigned char ImgType;              /* image type (see below for values) */
  unsigned char Index_lo, Index_hi;   /* index of first color map entry */
  unsigned char Length_lo, Length_hi; /* number of entries in color map */
  unsigned char CoSize;               /* size colormap entry (15,16,24,32) */
  unsigned char X_org_lo, X_org_hi;   /* x origin of image */
  unsigned char Y_org_lo, Y_org_hi;   /* y origin of image */
  unsigned char Width_lo, Width_hi;   /* width of image */
  unsigned char Height_lo, Height_hi; /* height of image */
  unsigned char PixelSize;            /* pixel size (8,16,24,32) */

  const unsigned char *base;


    /* ------------- open TGA file ----------- */

  xsize = wid;
  ysize = hit;
  outfile = fopen(fname, "wb");
  if (outfile == NULL) {
    fprintf(stderr,"Trouble opening TGA file %s\n",fname);
    return(1);
  }

    /*----------- Write out data to TGA format file ------- */
    /* eighteen-byte header */

     /* b0-b7 */
    IDLength = 0; CoMapType = 0; ImgType = 2; Index_lo = 0; Index_hi=0;
    Length_lo = 0; Length_hi = 0; CoSize = 0; X_org_lo = 0; X_org_hi = 0;
    temp = xsize / 256;
    Width_hi = (unsigned char) temp;     /* low and high bytes of x width */
    Width_lo = (unsigned char)(xsize % 256);
    temp = ysize / 256;
    Height_hi = (unsigned char) temp;
    Height_lo = (unsigned char)(ysize % 256);
    PixelSize = 32;
    Y_org_lo = 0;
    Y_org_hi = 0;

    fprintf(outfile,"%c",IDLength);
    fprintf(outfile,"%c",CoMapType);
    fprintf(outfile,"%c",ImgType);
    fprintf(outfile,"%c",Index_lo);
    fprintf(outfile,"%c", Index_hi);
    fprintf(outfile,"%c",Length_lo);
    fprintf(outfile,"%c",Length_hi);
    fprintf(outfile,"%c",CoSize);
    fprintf(outfile,"%c",X_org_lo); fprintf(outfile,"%c",X_org_hi);
    fprintf(outfile,"%c",Y_org_lo); fprintf(outfile,"%c",Y_org_hi);
    fprintf(outfile,"%c",Width_lo);
    fprintf(outfile,"%c",Width_hi);              /* file in lo-byte, hi-byte order b12,b13*/
    fprintf(outfile,"%c",Height_lo);
    fprintf(outfile,"%c",Height_hi);    /* ysize b14, b15 */
    fprintf(outfile,"%c",PixelSize);
    fprintf(outfile,"%c",(unsigned char) 0x20);        /* descriptor byte b17 */

    for (iy = 0; iy < ysize; iy++)
    {
      base = &image[iy*xsize*4];

      for (ix = 0; ix < xsize; ix++)
      {
        alpha = *base++;
        red   = *base++;
        green = *base++;
        blue  = *base++;
        fprintf(outfile,"%c%c%c%c",blue,green,red,alpha);
      }
    }

  fclose(outfile);
  return(0);

} /* end tga_save() */
