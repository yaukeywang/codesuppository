#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "MeshImport/MeshImport.h"
#include "MeshSystemHelper.h"
#include "common/snippets/UserMemAlloc.h"
#include "common/snippets/fmem.h"
#include "common/snippets/SendTextMessage.h"
#include "RenderDebug/RenderDebug.h"

class MyMeshSystemHelper : public MeshSystemHelper
{
public:
  MyMeshSystemHelper(void)
  {
    mMeshSystem = 0;
  }

  ~MyMeshSystemHelper(void)
  {
    if ( mMeshSystem )
    {
      gMeshImport->releaseMeshSystem(mMeshSystem);
    }
  }

  virtual MESHIMPORT::MeshSystem * getMeshSystem(void) const
  {
    return mMeshSystem;
  }

  virtual void debugRender(void)
  {
    if ( mMeshSystem )
    {
      gRenderDebug->DebugBound(mMeshSystem->mAABB.mMin, mMeshSystem->mAABB.mMax, 0xFFFFFF );
      for (unsigned int i=0; i<mMeshSystem->mMeshCount; i++)
      {
        debugRender( mMeshSystem->mMeshes[i] );
      }
    }
  }

  void debugRender(MESHIMPORT::Mesh *m)
  {
    unsigned int color = gRenderDebug->getDebugColor(true);
    gRenderDebug->DebugBound(m->mAABB.mMin, m->mAABB.mMax, 0xFFFF00 );
    for (unsigned int i=0; i<m->mSubMeshCount; i++)
    {
      debugRender( m->mSubMeshes[i],color);
      color = gRenderDebug->getDebugColor();
    }
  }

  void debugRender(MESHIMPORT::SubMesh *m,unsigned int color)
  {
    gRenderDebug->DebugBound(m->mAABB.mMin, m->mAABB.mMax, color);
    for (unsigned int i=0; i<m->mTriCount; i++)
    {
      unsigned int i1 = m->mIndices[i*3+0];
      unsigned int i2 = m->mIndices[i*3+1];
      unsigned int i3 = m->mIndices[i*3+2];
      const MESHIMPORT::MeshVertex &v1 = m->mVertices[i1];
      const MESHIMPORT::MeshVertex &v2 = m->mVertices[i2];
      const MESHIMPORT::MeshVertex &v3 = m->mVertices[i3];
      gRenderDebug->DebugSolidTri(v1.mPos,v2.mPos,v3.mPos, color );
    }
  }

  virtual bool importMesh(const char *fname)
  {
    bool ret = false;

    unsigned int len;
    unsigned char *data = getLocalFile(fname,len);
    if ( data )
    {
      mMeshSystem = gMeshImport->createMeshSystem(fname,data,len,0);
      if ( mMeshSystem )
      {
        SEND_TEXT_MESSAGE(0,"Successfully imported mesh '%s'\r\n", fname );
        ret = true;
      }
    }
    else
    {
      SEND_TEXT_MESSAGE(0,"Failed to load file '%s'\r\n", fname );
    }
    return ret;
  }

private:
  MESHIMPORT::MeshSystem  *mMeshSystem;
};

MeshSystemHelper * createMeshSystemHelper(void)
{
  MyMeshSystemHelper *h = MEMALLOC_NEW(MyMeshSystemHelper);
  return static_cast< MeshSystemHelper *>(h);
}


void   releaseMeshSystemHelper(MeshSystemHelper *m)
{
  MyMeshSystemHelper *s = static_cast< MyMeshSystemHelper *>(m);
  MEMALLOC_DELETE(MyMeshSystemHelper,s);
}

