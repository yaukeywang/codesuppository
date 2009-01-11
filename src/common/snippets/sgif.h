#ifndef SGIF_H

#define SGIF_H

#ifdef __cplusplus
extern "C" {
#endif

int	save_screen(const char *filename,const unsigned char *buffer,int wid,int hit,const unsigned char *pal);

#ifdef __cplusplus
};  /* end of extern "C" */
#endif

#endif
