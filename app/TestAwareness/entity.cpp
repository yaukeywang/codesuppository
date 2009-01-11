#include "entity.h"
#include "rand.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <float.h>
#include <math.h>

#pragma warning(disable:4996)
#include <list>

#include "../../include/common/spatial_awareness_system/spatial_awareness_system.h"
using namespace SPATIAL_AWARENESS_SYSTEM;


#define RUN_TEST 0

#define DATA_LOG 0

#define EVENT_BASED 0 // render via event based tracking (as opposed to direct iteration)
#define ALLOW_DELETES 1

SpatialAwarenessStrategy gStrategy=SAS_BUFFER;

//#define LIFETIME 0.1f
#define LIFETIME 30.0f

ID gMaxId=1000;

static HeF32 ranf(void)
{
  HeI32 v = rand()&0x7FFF;
  HeF32 fv = v*(1.0f/32767);
  return fv;
}

class Entity;

typedef std::list< Entity * > EntityList;

class Entity
{
public:
  Entity(void)
  {
    generate();
  }

  void runTest(int index)
  {
    mPos[0] = 0;
    mPos[1] = (HeF32)(index)*60+120;
    mPos[2] = 0;

    mDir[0] = ranf()*0.03f+0.2f;
    mDir[1] = 0;
    mDir[2] = 0;

    mRange  = 180;
    mStatic = false;
    mLifeTime = 100000;
  }

  void generate(void)
  {
    mPos[0] = ranf()*SCREEN_WIDTH;
    mPos[1] = ranf()*SCREEN_HEIGHT;
    mPos[2] = 0;

    if ( ranf() < 0.65f  )
    {
      mStatic = true;
      mDir[0] = 0;
      mDir[1] = 0;
      mDir[2] = 0;
      mColor = 0xFFFFFF;
      mRange = 0;
    }
    else
    {
      mStatic = false;
      mDir[0] = (ranf()-0.5f)*ranf()*1;
      mDir[1] = (ranf()-0.5f)*ranf()*1;
      mDir[2] = 0;
      if ( ranf() < 0.25f )
      {
        mDir[0] = mDir[1] = 0;
      }
      mColor = 0x00FFFF;
      mRange = ranf()*60+20;
    }

//    mDir[0] = mDir[1] = mDir[2] = 0;

#if ALLOW_DELETES
    mLifeTime = ranf()*LIFETIME;
#endif

  }

  void respawn(FILE *fph)
  {
#if DATA_LOG
    if(fph)
    {
      fprintf(fph,"respawn(%d)\r\n", (HeI32)mId);
      fflush(fph);
    }
#endif
    mSAS->DeleteEntity(mId,true);
#if EVENT_BASED
    mEntities.clear();
#endif
    generate();
    mSAS->AddEntity(mId);
    mSAS->UpdateEntityPosition(mId,mPos);
    mSAS->UpdateEntityAwarenessRange(mId,mRange);
  }

  void process(HeF32 dtime,FILE *fph)
  {
#if ALLOW_DELETES
    mLifeTime-=dtime;
    if ( mLifeTime < 0 )
    {
      respawn(fph);
    }
#endif
    if ( !mStatic )
    {
      if ( mDir[0] != 0 || mDir[1] != 0 )
      {
        mPos[0]+=mDir[0];
        mPos[1]+=mDir[1];
        mPos[2]+=mDir[2];

//        mPos[0] = SCREEN_WIDTH/2;
//        mPos[1] = SCREEN_HEIGHT/2;

#if RUN_TEST
        mDir[0]+=(ranf()*0.002f)-0.001f;
#endif

        if ( mPos[0] < 0 ) mDir[0]*=-1;
        if ( mPos[0] >= SCREEN_WIDTH ) mDir[0]*=-1;

        if ( mPos[1] < 0 ) mDir[1]*=-1;
        if ( mPos[1] >= SCREEN_HEIGHT ) mDir[1]*=-1;

        mSAS->UpdateEntityPosition(mId,mPos);
      }
    }
  }



  void render(void)
  {
    DrawPoint( (HeI32) mPos[0], (HeI32)mPos[1], mColor);
    if ( !mStatic )
    {
#if 0
      HeI32 r  = (HeI32)mRange;

      HeI32 x1 = (HeI32)mPos[0];
      HeI32 y1 = (HeI32)mPos[1];

      HeI32 x2 = x1+r;
      HeI32 y2 = y1+r;

      x1-=r;
      y1-=r;

      DrawLine(x1,y1,x2,y1,0xFFFFFF);
      DrawLine(x1,y2,x2,y2,0xFFFFFF);

      DrawLine(x1,y1,x1,y2,0xFFFFFF);
      DrawLine(x2,y1,x2,y2,0xFFFFFF);
#endif

#if 1
      DrawCircle( (HeI32) mPos[0], (HeI32)mPos[1], (HeI32)mRange, mColor);
#endif


#if EVENT_BASED
      EntityList::iterator i;
      for (i=mEntities.begin(); i!=mEntities.end(); ++i)
      {
        Entity *e = (*i);
        DrawLine( (HeI32) mPos[0], (HeI32)mPos[1], (HeI32)e->mPos[0], (HeI32)e->mPos[1], 0x0000FF );
      }
#endif

    }


  }

  void init(HeI32 id,SpatialAwarenessSystem *sas)
  {
    mId = (ID)id;
    mSAS = sas;
    mSAS->AddEntity(mId);
    mSAS->UpdateEntityPosition(mId,mPos);
    mSAS->UpdateEntityAwarenessRange(mId,mRange);
  }

#if EVENT_BASED

  void add(Entity *e)
  {
    mEntities.push_back(e);
  }

  void remove(Entity *e)
  {
    bool found = false;

    EntityList::iterator i;
    for (i=mEntities.begin(); i!=mEntities.end(); i++)
    {
      if ( (*i) == e )
      {
        mEntities.erase(i);
        found = true;
        break;
      }
    }
    assert(found);
  }
#endif


  void validate(void)
  {
  }

  HeU32            mShowCount;
  ID                      mId;
  HeF32                   mPos[3];
  HeF32                   mDir[3];
  HeF32                   mLifeTime;
  HeU32            mColor;
  bool                    mStatic;
  HeF32                   mRange;
  SpatialAwarenessSystem *mSAS;
#if EVENT_BASED
  EntityList              mEntities;
#endif
};

class EntityFactory : public SPATIAL_AWARENESS_SYSTEM::SpatialAwarenessObserver, public SPATIAL_AWARENESS_SYSTEM::SpatialAwarenessIteratorCallback
{
public:
  EntityFactory(HeI32 count)
  {
#if RUN_TEST
    count = 7;
#endif
    gMaxId = count;
    srand(0);
    mFph = fopen("entity.txt", "wb");
    mSAS = Factory::Create(gStrategy,this);
    mSAS->SetUpdatePeriod(1);

    mSAS->setProperty("DefaultGranularity","4");

    mCount = count;
    mEntities = MEMALLOC_NEW_ARRAY(Entity,count)[count];
    for (HeI32 i=0; i<count; i++)
    {
#if RUN_TEST
      mEntities[i].runTest(i);
#endif

      mEntities[i].init(i,mSAS);
    }
  }

  ~EntityFactory(void)
  {
    Factory::Destroy(mSAS);
    delete []mEntities;
    if ( mFph )
    {
      fclose(mFph);
    }
  }

  void process(HeF32 dtime)
  {
//    dtime = 1.0f / 60.0f;
    mDtime = dtime;
    for (HeI32 i=0; i<mCount; i++)
    {
      mEntities[i].process(dtime,mFph);
    }
#if DATA_LOG
    if ( mFph )
    {
      fprintf(mFph,"Pump\r\n");
    }
#endif

    Factory::PrePump();
    Factory::Pump(dtime);
    Factory::PostPump();

    for (HeI32 i=0; i<mCount; i++)
    {
      mEntities[i].validate();
    }

  }

  void render(void)
  {
    for (HeI32 i=0; i<mCount; i++)
    {
      mEntities[i].render();
    }
#if EVENT_BASED
#else
    mSAS->iterateAll(this);
#endif
  }

  void SAO_entered(ID entity,ID subject)
  {
    assert( entity >= 0 && entity < gMaxId );
    assert( subject >= 0 && subject < gMaxId );
#if EVENT_BASED
     mEntities[ (HeI32) entity ].add( &mEntities[ (HeI32) subject] );
#endif
#if DATA_LOG
    if ( mFph )
    {
      fprintf(mFph,"SAO_entered(%d,%d)\r\n", (HeI32)entity,(HeI32)subject);
      fflush(mFph);
    }
#endif
  }

  void SAO_departed(ID entity,ID subject)
  {
    assert( entity >= 0 && entity < gMaxId );
    assert( subject >= 0 && subject < gMaxId );
#if EVENT_BASED
     mEntities[ (HeI32) entity ].remove( &mEntities[ (HeI32) subject] );
#endif
#if DATA_LOG
    if ( mFph )
    {
      fprintf(mFph,"SAO_departed(%d,%d)\r\n", (HeI32)entity,(HeI32)subject);
      fflush(mFph);
    }
#endif
  }

  bool SAI_iterate(ID entity,ID subject)
  {

    Entity *from = &mEntities[(HeI32)entity];
    Entity *to   = &mEntities[(HeI32)subject];

//    if ( from->mDir[0] == 0 && from->mDir[1] == 0 )
      DrawLine( (HeI32) from->mPos[0], (HeI32)from->mPos[1], (HeI32)to->mPos[0], (HeI32)to->mPos[1], 0x0000FF );

    return true;
  }

  void SAO_appeared(ID entity,ID subject)
  {
    assert( entity >= 0 && entity < gMaxId );
    assert( subject >= 0 && subject < gMaxId );

#if EVENT_BASED
     mEntities[ (HeI32) entity ].add( &mEntities[ (HeI32) subject] );
#endif
#if DATA_LOG
    if ( mFph )
    {
      fprintf(mFph,"SAO_appeared(%d,%d)\r\n", (HeI32)entity,(HeI32)subject);
      fflush(mFph);
    }
#endif
  }

  void SAO_disappeared(ID entity,ID subject)
  {
    assert( entity >= 0 && entity < gMaxId );
    assert( subject >= 0 && subject < gMaxId );

#if EVENT_BASED
     mEntities[ (HeI32) entity ].remove( &mEntities[ (HeI32) subject] );
#endif
#if DATA_LOG
    if ( mFph )
    {
      fprintf(mFph,"SAO_disappeared(%d,%d)\r\n", (HeI32)entity,(HeI32)subject);
      fflush(mFph);
    }
#endif
  }

private:
  SpatialAwarenessSystem  *mSAS;
  HeI32                      mCount;
  HeF32                    mDtime;
  Entity                  *mEntities;
  FILE                    *mFph;
};

static EntityFactory *gEntityFactory=0;

void createEntityFactory(HeI32 count)
{
  gEntityFactory = MEMALLOC_NEW(EntityFactory)(count);
}

void processEntityFactory(HeF32 dtime)
{
  if ( gEntityFactory )
  {
    gEntityFactory->process(dtime);
  }
}

void releaseEntityFactory(void)
{
  delete gEntityFactory;
  gEntityFactory = 0;
}


void renderEntityFactory(void)
{
  if ( gEntityFactory )
  {
    gEntityFactory->render();
  }
}
