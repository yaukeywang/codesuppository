#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <math.h>
#include <direct.h>

#include "MeshImport/MeshImport.h"

void main(int argc,const char **argv)
{
  if ( argc == 2 )
  {
    char dirname[256];
    strcpy(dirname,argv[0]);
    int len = strlen(dirname);
    char *scan = &dirname[len-1];
    while ( len )
    {
      if ( *scan == '\\' )
      {
        *scan = 0;
        break;
      }
      scan--;
      len--;
    }

    chdir( dirname );


    gMeshImport = loadMeshImporters(dirname,0);

    if ( gMeshImport )
    {
      unsigned char *data=0;
      unsigned int len = 0;
      const char *fname = argv[1];
      FILE *fph = fopen(fname,"rb");
      if ( fph )
      {
        fseek(fph,0L,SEEK_END);
        len = ftell(fph);
        fseek(fph,0L,SEEK_SET);
        if ( len > 0 )
        {
          data = new unsigned char[len];
          fread(data,len,1,fph);
        }
        fclose(fph);
      }
      if ( data )
      {
#if 0
        MESHIMPORT::MeshSystem *ms = gMeshImport->createMeshSystem(fname,data,len,0);
        if ( ms )
        {
          printf("Success!\r\n");

          unsigned int olen;
          void *data = gMeshImport->serializeMeshSystem(ms,olen,MESHIMPORT::MSF_EZMESH);
          if ( data )
          {
            printf("Saving serialized data as temp.ezm\r\n");
            FILE *fph = fopen("temp.ezm", "wb");
            if ( fph )
            {
              fwrite(data,olen,1,fph);
              fclose(fph);
            }
            else
            {
              printf("Failed to open file for write access.\r\n");
            }
            gMeshImport->releaseSerializeMemory(data);
          }
          gMeshImport->releaseMeshSystem(ms);
        }
        else
        {
          printf("Failed to create MeshSystem for '%s'\r\n", fname );
        }
#endif
        delete []data;
      }
      else
      {
        printf("Failed to load file '%s'\r\n", fname );
      }
    }

  }
  else
  {
    printf("Usage: MeshImport <fname>\r\n");
  }
}
