#include "vsofont.h"
#include <stdio.h>

#define FONT_VERSION 1

void main(int argc,const char **argv)
{
    if ( argc != 2 )
    {
        printf("Usage: VsoFont <font_name>\r\n");
        printf("\r\n");
        printf("Converts a VSO Font into a more reasonable binary format.\r\n");
    }
    else
    {
		const char *fname = argv[1];
		printf("Loading VSO Font File: %s\r\n", fname );
        struct vsofont *font=0;
        int ret = vsofont_read_font_from_file((char *)fname,&font);
        if ( ret == 1 )
        {
            printf("Successfully loaded the font.\r\n");
            printf("Saving font as 'font.bin'\r\n");

            FILE *fph = fopen("font.bin", "wb");
            if ( fph )
            {
				unsigned int ccount = 0;
				unsigned int icount = 0;
                unsigned int maxVertex = 0;

                for (int i=0; i<256; i++)
                {
                    struct vsocharacter *c = font->characters[i];
                    if ( c )
                    {
						ccount++;
						icount+=c->indicesn;
                        for (int i=0; i<c->indicesn; i++)
                        {
                            if ( font->indices_type == 0 )
                            {
                                const unsigned char *ci = (const unsigned char *)c->indices;
                                unsigned int v = ci[i];
                                if ( v > maxVertex )
                                    maxVertex = v;
                            }
                            else
                            {
                                const unsigned short *ci = (const unsigned short *)c->indices;
                                unsigned int v = ci[i];
                                if ( v > maxVertex )
                                    maxVertex = v;
                            }
                        }
                    }
                }
				printf("Saving %d vertices.\r\n", maxVertex);
				printf("Cell Size: %d,%d\r\n", font->grid_dx, font->grid_dy );
				printf("Font contains %d characters consuming %d indices.\r\n",ccount,icount);

                fwrite("FONT",4,1,fph);
                unsigned int version = FONT_VERSION;
                fwrite(&version,sizeof(unsigned int),1,fph);
				fwrite(&font->grid_dx, sizeof(unsigned int),1,fph);
				fwrite(&font->grid_dy, sizeof(unsigned int),1,fph);
                fwrite(&maxVertex, sizeof(unsigned int),1,fph);
                fwrite(&ccount, sizeof(unsigned int),1,fph);
                fwrite(&icount, sizeof(unsigned int),1,fph);
                fwrite(font->vertices, sizeof(float)*maxVertex*2, 1, fph );

                for (int i=0; i<256; i++)
                {
                    struct vsocharacter *c = font->characters[i];
                    if ( c )
                    {
                        unsigned char cc = (unsigned char)i;
                        fwrite(&cc,sizeof(char),1,fph);
                        fwrite(&c->indicesn,sizeof(unsigned short),1,fph);
						ccount++;
						icount+=c->indicesn;
                        for (int i=0; i<c->indicesn; i++)
                        {
                            if ( font->indices_type == 0 )
                            {
                                const unsigned char *ci = (const unsigned char *)c->indices;
                                unsigned short v = ci[i];
                                if ( v > maxVertex )
                                    maxVertex = v;
                                fwrite(&v,sizeof(unsigned short),1,fph);
                            }
                            else
                            {
                                const unsigned short *ci = (const unsigned short *)c->indices;
                                unsigned short v = ci[i];
                                fwrite(&v,sizeof(unsigned short),1,fph);
                                if ( v > maxVertex )
                                    maxVertex = v;
                            }
                        }
                    }
                }

                fclose(fph);
            }
            else
            {
                printf("Failed to open 'font.bin' for write access.\r\n");
            }


            vsofont_free(font);
        }
        else
        {
            printf("Failed to load the VSO font.\r\n");
        }
    }
}
