#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include "../../include/common/spatial_awareness_system/spatial_awareness_system.h"
using namespace SPATIAL_AWARENESS_SYSTEM;

enum UnitTestType
{
  UTT_NONE,
  UTT_APPEARED_DISAPPEARED,
  UTT_ENTERED_DEPARTED,
  UTT_ITERATE_AWARE,
};

enum UnitTestState
{
  UTS_NONE,
  UTS_EXPECT_APPEARED,
  UTS_EXPECT_DISAPPEARED,
  UTS_EXPECT_ENTERED,
  UTS_EXPECT_DEPARTED,
};

static SecondsType pump_time = 1.0/60.0;

class UnitTest :  public SPATIAL_AWARENESS_SYSTEM::SpatialAwarenessObserver, public SPATIAL_AWARENESS_SYSTEM::SpatialAwarenessIteratorCallback
{
public:
  UnitTest(void)
  {
    mState = UTS_NONE;
    mType  = UTT_NONE;
    mInsidePump = false;
    mSAS = 0;
  }


  ~UnitTest(void)
  {
    release();
  }


  void init(void)
  {
    mState = UTS_NONE;
    mType  = UTT_NONE;
    mInsidePump = false;
    mSAS = Factory::Create(SAS_LAZY_KDTREE,this);
    assert(mSAS);
    if ( mSAS )
    {
      mSAS->SetUpdatePeriod(pump_time);
    }
  }

  void release(void)
  {
    if ( mSAS )
    {
      Factory::Destroy(mSAS);
      mSAS = 0;
    }
  }

  bool unitTest(UnitTestType type)
  {
    bool ret = true;

    init();

    mType = type;

    switch ( type )
    {
      case UTT_ITERATE_AWARE:
        ret = testIterateAware();
        break;
      case UTT_APPEARED_DISAPPEARED:
        ret = testAppearedDisappeared();
        break;
      case UTT_ENTERED_DEPARTED:
        ret = testEnteredDeparted();
        break;
    }

    release();

    return ret;
  }

  bool testAppearedDisappeared(void)
  {
    assert( mSAS );
    if ( mSAS )
    {

      mOk = true;

      createEntity(1,0,0,0,10);
      createEntity(2,0,0,0,0);

      if ( mOk )
      {
        mState = UTS_EXPECT_APPEARED;
        mOk    = false;
        pump();
        assert( mOk );
        deleteEntity(2);
        mState = UTS_EXPECT_DISAPPEARED;
        pump();
        deleteEntity(1);
      }
    }
    else
    {
      mOk = false;
    }

    return mOk;
  }

  void setPosition(ID id,float x,float y,float z)
  {
    assert(mSAS);
    if ( mSAS )
    {
      float p[3] = { x, y, z };
      if ( !mSAS->UpdateEntityPosition(id,p) )
      {
        assert(0);
        mOk = false;
      }
    }
  }

  bool testEnteredDeparted(void)
  {
    assert( mSAS );
    if ( mSAS )
    {

      mOk = true;

      createEntity(1,0,0,0,10);           // create entity 1 with an awareness range of 10
      createEntity(2,50,50,50,0);         // create entity 2, but it is outside of awareness.

      if ( mOk )
      {
        pump();
        setPosition(2,0,0,0);             // now move the entity into awareness range.
        mState = UTS_EXPECT_ENTERED;      // we expect an entered event
        pump();                           // run the pump loop
        assert( mOk );

        setPosition(2,50,50,50);          // now move it outside of range
        mState = UTS_EXPECT_DEPARTED;     // we expect a departed event
        pump();
        assert(mOk);

        deleteEntity(2);
        deleteEntity(1);
        pump();

      }
    }
    else
    {
      mOk = false;
    }

    return mOk;
  }

  void pump(void)
  {
    assert(mInsidePump==false);
    mInsidePump = true;
    assert(mSAS);
    mSAS->Pump(pump_time);
    mInsidePump = false;
  }

  void deleteEntity(ID id)
  {
    assert(mSAS);
    if ( mSAS )
    {
      if ( mSAS->DeleteEntity(id) )
      {
      }
      else
      {
        assert(0);
        mOk = false;
      }
    }
  }

  void createEntity(ID id,float x,float y,float z,float range)
  {
    assert( mSAS );
    if ( mSAS->AddEntity( id ) )
    {
      float pos[3];
      pos[0] = x;
      pos[1] = y;
      pos[2] = z;

      if ( mSAS->UpdateEntityPosition(id,pos) )
      {
        if ( mSAS->UpdateEntityAwarenessRange(id,range) )
        {
        }
        else
        {
          assert(0);
          mOk = false;
        }
      }
      else
      {
        assert(0);
        mOk = false;
      }
    }
    else
    {
      assert(0);
      mOk = false;
    }
  }

  void SAO_appeared(ID entity,ID subject)
  {
    assert(mInsidePump);
    if ( mType == UTT_APPEARED_DISAPPEARED )
    {
      assert( mState == UTS_EXPECT_APPEARED );
      if ( mState == UTS_EXPECT_APPEARED )
      {
        assert( entity == 1 );
        assert( subject == 2 );
        if ( entity == 1 && subject == 2 )
        {
          mOk = true;
        }
      }
    }
  }

  void SAO_disappeared(ID entity,ID subject)
  {
    assert(mInsidePump);
    if ( mType == UTT_APPEARED_DISAPPEARED )
    {
      assert( mState == UTS_EXPECT_DISAPPEARED );
      if ( mState == UTS_EXPECT_DISAPPEARED )
      {
        assert( entity == 1 );
        assert( subject == 2 );
        if ( entity == 1 && subject == 2 )
        {
          mOk = true;
        }
      }
    }
  }

  void SAO_entered(ID entity,ID subject)
  {
    assert(mInsidePump);
    if ( mType == UTT_APPEARED_DISAPPEARED )
    {
      assert(0); //
    }
    if ( mType == UTT_ENTERED_DEPARTED )
    {
      assert( mState == UTS_EXPECT_ENTERED );
      if ( mState == UTS_EXPECT_ENTERED )
      {
        assert( entity == 1 );
        assert( subject == 2 );
        if ( entity == 1 && subject == 2 )
        {
          mOk = true;
        }
      }
    }
  }

  void SAO_departed(ID entity,ID subject)
  {
    assert(mInsidePump);
    if ( mType == UTT_APPEARED_DISAPPEARED )
    {
      assert(0); //
    }
    if ( mType == UTT_ENTERED_DEPARTED )
    {
      assert( mState == UTS_EXPECT_DEPARTED );
      if ( mState == UTS_EXPECT_DEPARTED )
      {
        assert( entity == 1 );
        assert( subject == 2 );
        if ( entity == 1 && subject == 2 )
        {
          mOk = true;
        }
      }
    }
  }


  bool SAI_iterate(ID entity,ID subject)   // return true if you want to continue iterating or false to cancel iteration.
  {
    bool ret = true;
    return ret;
  }

  bool testIterateAware(void)
  {
    bool ret = false;


    createEntity(1,0,0,0,10);           // create entity 1 with an awareness range of 10
    createEntity(2,0,0,0,10);
    createEntity(3,0,0,0,10);
    createEntity(4,0,0,0,10);
    createEntity(5,0,0,0,10);
    createEntity(6,50,0,0,100);
    createEntity(7,50,0,0,100);
    createEntity(8,50,0,0,100);
    createEntity(9,50,0,0,100);
    createEntity(10,50,0,0,100);

    pump();


    IDVector list;
    unsigned int count = mSAS->iterateAwareness(1,list);
    assert ( count == 4 );
    if ( count == 4 )
    {
      bool check[10] = { false, false, false, false, false, false, false, false, false, false };

      int count = 0;

      IDVector::iterator i;
      for (i=list.begin(); i!=list.end(); ++i)
      {
        ID id = (*i);
        int index = (int) id;
        assert (index >= 2 && index <= 10 );
        if ( index >= 2 && index <= 10 )
        {
          index--;
          assert( check[index] == false );
          if ( check[index] == false )
          {
            count++;
            check[index] = true;
          }
        }
      }
      if ( count == 4 )
      {
        ret = true;
      }

    }

    if ( 1 )
    {
      IDVector list;
      unsigned int count = mSAS->iterateAwareOf(1,list);
      assert ( count == 9 );
      if ( count == 9 )
      {
        bool check[10] = { false, false, false, false, false, false, false, false, false, false };

        int count = 0;

        IDVector::iterator i;
        for (i=list.begin(); i!=list.end(); ++i)
        {
          ID id = (*i);
          int index = (int) id;
          assert (index >= 2 && index <= 10 );
          if ( index >= 2 && index <= 10 )
          {
            index--;
            assert( check[index] == false );
            if ( check[index] == false )
            {
              count++;
              check[index] = true;
            }
          }
        }
        if ( count == 9 )
        {
          ret = true;
        }

      }
    }

    return ret;
  }

private:
  bool                     mOk;
  bool                     mInsidePump;
  UnitTestType             mType;
  UnitTestState            mState;
  SpatialAwarenessSystem  *mSAS;
};


bool unit_test(void)
{
  bool ret = true;

  UnitTest ut;

  if ( !ut.unitTest(UTT_APPEARED_DISAPPEARED) )
  {
    ret = false;
  }
  if ( !ut.unitTest(UTT_ENTERED_DEPARTED) )
  {
    ret = false;
  }
  if ( !ut.unitTest(UTT_ITERATE_AWARE) )
  {
    ret = false;
  }


  return ret;
}

