#ifndef HF_IO

#define HF_IO

#ifdef __cplusplus
   extern "C" {
#endif

#define NUM_FMT 0       /* 0 IEEE Little Endian (80x86, DEC Risc) */
#define PREC 1          /* storing Matlab file as floats rather than longs */

#define El(vv, xq, yq)  vv[((yq) * xsize) + xq]  /* elmt of array */
#define El1(vv, xq, yq) vv[((yq) * xsize1)+ xq]  /* elmt of 1st array */
#define El2(vv, xq, yq) vv[((yq) * xsize2)+ xq]  /* elmt of 2nd array */
#define El3(vv, xq, yq) vv[((yq) * xsize3)+ xq]  /* elmt of 3rd array */
#define Im(vv, xq, yq)  vv[xsize*ysize + ((yq) * xsize) + xq] /* imag.part */
#define Im2(vv, xq, yq) vv[xsize2*ysize2 +((yq)*xsize2) + xq] /* imag.part */

#define Elmod(v,x,y) v[(((y)>=0)?(y)%ysize:(((y)%ysize)+ysize)%ysize)*xsize \
		     + (((x)>=0)?(x)%xsize:(((x)%xsize)+xsize)%xsize) ]
#define Elclip(v,x,y) v[(((y)<0?0:(((y)>=ysize)?(ysize-1):(y))) * xsize)\
		      + ((x)<0?0:(((x)>=xsize)?(xsize-1):(x)))]

#define X_WRAP(x)  x=(((x)>=0)?(x)%xsize:(((x)%xsize)+xsize)%xsize)
#define Y_WRAP(y)  y=(((y)>=0)?(y)%ysize:(((y)%ysize)+ysize)%ysize)

#define X_CLIP(x)  x=((x)<0?0:(((x)>=xsize)?(xsize-1):(x)))
#define Y_CLIP(y)  y=((y)<0?0:(((y)>=ysize)?(ysize-1):(y)))

/* Matlab Level 1.0 format data file header */
typedef struct
{
	long type;      /* type */
	long mrows;     /* row dimension */
	long ncols;     /* column dimension */
	long imagf;     /* flag indicating imag part */
	long namelen;    /* name length (including NULL) */
} Fmatrix;

#define PGM  0          /* PGM ascii type (arbitrary) */
#define TGA  1          /* POV-Ray 2.2 TGA heightfield format 16-bit */
#define PNG  2          /* PNG format (not yet supported) */
#define PG8  3          /* binary PGM format 8-bit */
#define OCT  4          /* Octave ascii format (arbitrary) */
#define MAT  5          /* Matlab Level 1.0 binary format 32bit float */
#define GIF  6          /* CompuServe (tm) GIF format 8-bit */
#define POT  7          /* Fractint, PoV-Ray "continuous potential" 16-bit */
#define RAW  8          /* RAW ascii format triangle vertices (x,y,z) */

#define MAX_PIXVAL 65536              /* ascii integer output max value */

typedef struct
{        /* Heightfield structure type */
  float *a;          /* 2-D array of values */
  unsigned int xsize;
  unsigned int ysize;             /* x- and y-dimensions of array */
  float min;
  float max;         /* max and min values in array */
  int c;             /* TRUE if matrix is complex, FALSE if real */
  char name[15];     /* name of HF */
} hfield;

/* read heightfield  */
int hf_read(hfield *hfp, char *fname);
int read_mfile(hfield *hfp, char *f);  /* read matlab file */
int write_mfile(hfield hf, char *f, char *m); /* write matlab */
                              /* write hf array to PGM ascii or binary */
int hf_writeb(hfield hf, char *fname);
int hf_write(hfield hf, char *fname);
int gwrite(hfield hf, char *fname); /* write as GIF */
int pot_write(hfield hf,  char *fname);  /* write to POT format */
int pot_read(hfield *hf,  char *fname);      /* read from POT */
int tga_read(hfield *hf,  char *fname);   /* read from TGA */

unsigned char * tga_load(const char *fname,int *wid,int *hit,int *bpp);

int tga_write(hfield hf, char *fname);    /* write to TGA (Targa Truevision) */
int tga_save(const char *fname,const unsigned char *image,int wid,int hit);
int tga_save_alpha(const char *fname,const unsigned char *image,int wid,int hit);
int tga_save_gray(const char *fname,const unsigned char *image,int wid,int hit);
int png_read(hfield *hf,  char *fname);   /* read from PNG file */
int png_write(hfield hf, char *fname);    /* write to PNG 16-bit grey format */
int raw_write(hfield hf, char *fname);    /* write to RAW ascii format file */
int oct_write(hfield hf, char *f, char *m); /* Octave fmt */
int oct_read(hfield *hf, char *fname); /* Octave fmt */
int gread(char *fname);                 /* read hf array from GIF format */
int gread1(hfield *hfp, char *fname);
                                        /* general (top-level) hf i/o */
         /* ready heightfield determining type from filename extension */
int readn_hf(hfield *hfp, char *fname_in);
int read_hf(hfield *hfp, char *fname_in, int itype);
int writen_hf(hfield hf, char *fname_out);
int write_hf(hfield hf, char *fname_out, int otype);

/* extensions of recognized filetypes */
#define Intype_string "PGM|MAT|GIF|POT|TGA"
#define Outtype_string "PGM|PG8|MAT|OCT|GIF|POT|TGA"

int h_newr(hfield *hf, int xs, int ys, char *name); /* create real HF */
int h_newc(hfield *hf, int xs, int ys, char *name); /* create real HF */

#ifdef __cplusplus
   }                         // End of 'extern "C"' declaration
#endif

#endif
