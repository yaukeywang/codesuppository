#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "UserMemAlloc.h"
#include "stable.h"
#include "keyvalue.h"
#include "inparser.h"
#include "stringdict.h"
#include "sutil.h"

#include "KeyValueSpec.h"

#pragma warning(disable:4189)

class MyKeyValue : public KeyValueDataItem
{
public:
  MyKeyValue(HeU32 index,const char *name)
  {
    mName = name;
    mType = KVT_NONE;
    t.mString = 0;
    tmin.mMinValue = 0;
    tmax.mMaxValue = 0;
    mIndex = index;
  }

  MyKeyValue(const MyKeyValue &/*k*/)
  {
    HE_ASSERT(0); // should never happen..the index field being copied would not make sense...
  }

  ~MyKeyValue(void)
  {
    release();
  }

  void add(std::string &str,const char *f)
  {
    std::string t;
    if ( f ) t = f;
    str+=t;
  }

  void add(std::string &str,HeI32 v)
  {
    char scratch[512];
    sprintf(scratch,"%d", v );
    add(str,scratch);
  }

  void add(std::string &str,HeF32 v)
  {
    const char *fv = FloatString(v);
    add(str,fv);
  }

  void getSpecification(std::string &str)
  {
    add(str,mName);
    add(str,"=");
    switch ( mType )
    {
      case KVT_BOOLEAN:
        add(str,"boolean,");
        add(str, t.mState ? "true" : "false" );
        break;
      case KVT_INTEGER:
        add(str,"integer,");
        add(str, t.mValue );
        add(str,",");
        add(str, tmin.mMinValue );
        add(str,",");
        add(str,tmax.mMaxValue);
        break;
      case KVT_FLOAT:
        add(str,"float,");
        add(str,t.mValueF);
        add(str,",");
        add(str,tmin.mMinValueF);
        add(str,",");
        add(str,tmax.mMaxValueF);
        break;
      case KVT_STRING:
        add(str,"string,");
        add(str,t.mString);
        break;
      case KVT_VECTOR3:
        add(str,"vector3,");
        add(str,"(");
        add(str, t.mValueF );
        add(str,",");
        add(str, tmin.mMinValueF );
        add(str,",");
        add(str,tmax.mMaxValueF);
        add(str,")");
        break;
    }
    add(str,"|");
  }

  void get(KeyValueDataItem &d)
  {
    d.mName = mName;
    d.mType = mType;
    d.mIndex = mIndex;
    d.tmin.mMinValue = 0;
    d.tmax.mMaxValue = 0;

    switch ( mType )
    {
      case KVT_BOOLEAN:
        d.t.mState = t.mState;
        break;
      case KVT_INTEGER:
        d.t.mValue = t.mValue;
        d.tmin.mMinValue = tmin.mMinValue;
        d.tmax.mMaxValue = tmax.mMaxValue;
        break;
      case KVT_FLOAT:
      case KVT_VECTOR3:
        d.t.mValueF = t.mValueF;
        d.tmin.mMinValueF = tmin.mMinValueF;
        d.tmax.mMaxValueF = tmax.mMaxValueF;
        break;
      case KVT_STRING:
        d.t.mString = t.mString;
        break;
    }

  }

  void release(void)
  {
    if ( mType == KVT_STRING && t.mString )
    {
      MEMALLOC_FREE(t.mString);
    }
    mType = KVT_NONE;
    t.mString = 0;
  }

  //            0   1   2   3      4        5   6       7   8
  // Usage:   <key><=><type>,<default_value>,<min_value>,<max_value>
  //             0  1    2   3 4 5 6 7 8 9 10
  //          <key><=><type> , ( x , y , z )
  void set(KeyValueType type,HeI32 argc,const char **argv)
  {
    release();

    mType = type;

    const char *defaultValue = 0;
    const char *minValue = 0;
    const char *maxValue = 0;

    if ( argc == 11 && type == KVT_VECTOR3 )
    {
      defaultValue = argv[5];
      minValue     = argv[7];
      maxValue     = argv[9];
    }
    else if ( argc >=5 && strcmp(argv[3],",") == 0 )
    {
      defaultValue = argv[4];
      if ( argc >= 7 && strcmp(argv[5],",") == 0 )
      {
        minValue = argv[6];
        if ( argc == 9 && strcmp(argv[7],",") == 0 )
        {
          maxValue = argv[8];
        }
      }
    }

    switch ( type )
    {
      case KVT_BOOLEAN:
        if ( defaultValue )
          t.mState = getBool(defaultValue);
        else
          t.mState = false;
        break;
      case KVT_INTEGER:
        if ( defaultValue )
          t.mValue = atoi( defaultValue );
        else
          t.mValue = 0;

        if ( minValue )
          tmin.mMinValue = atoi( minValue );
        else
          tmin.mMinValue = HE_MIN_I32;

        if ( maxValue )
          tmax.mMaxValue = atoi(maxValue);
        else
          tmax.mMaxValue = HE_MAX_I32;

        if ( t.mValue < tmin.mMinValue )
          t.mValue = tmin.mMinValue;

        if ( t.mValue > tmax.mMaxValue )
          t.mValue = tmax.mMaxValue;

        break;
      case KVT_FLOAT:
        if ( defaultValue )
          t.mValueF = (float)atof( defaultValue );
        else
          t.mValueF = 0;

        if ( minValue )
          tmin.mMinValueF = (float)atof( minValue );
        else
          tmin.mMinValueF = HE_MIN_F32;

        if ( maxValue )
          tmax.mMaxValueF = (float)atof(maxValue);
        else
          tmax.mMaxValueF = HE_MAX_F32;

        if ( t.mValueF < tmin.mMinValueF )
          t.mValueF = tmin.mMinValueF;

        if ( t.mValueF > tmax.mMaxValueF )
          t.mValueF = tmax.mMaxValueF;

        break;
      case KVT_VECTOR3:
        if ( defaultValue )
          t.mValueF = (float)atof( defaultValue );
        else
          t.mValueF = 0;

        if ( minValue )
          tmin.mMinValueF = (float)atof( minValue );
        else
          tmin.mMinValueF = t.mValueF;

        if ( maxValue )
          tmax.mMaxValueF = (float)atof(maxValue);
        else
          tmax.mMaxValueF = t.mValueF;

        break;
      case KVT_STRING:
        setString(defaultValue);
        break;
    }
  }

  void setString(const char *str)
  {
    if ( t.mString )
    {
      MEMALLOC_FREE(t.mString);
      t.mString = 0;
    }
    if ( str )
    {
      size_t len = strlen(str);
      t.mString = (char *)MEMALLOC_MALLOC(len+1);
      strcpy(t.mString,str);
    }
  }

  HeU32         mIndex;
};

typedef USER_STL::map< StringRef, MyKeyValue * > MyKeyValueMap;

class MyKeyValueSpec : public KeyValueSpec, public InPlaceParserInterface
{
public:
friend class MyKeyValueData;

  MyKeyValueSpec(const StringRef &ref,StringTableInt *keyValueTypes,KeyValueSpecFactory *factory)
  {
    mFactory = factory;
    mUserId = 0;
    mUserData = 0;
    mVersionNumber = 1;
    mIndex = 0;
    mName = ref;
    mKeyValueTypes = keyValueTypes;
    mDefaultData = 0;
  }

  ~MyKeyValueSpec(void)
  {
    release();
  }

  HeU32        getVersionNumber(void) const
  {
    return mVersionNumber;
  }


  const char  *getSpecName(void) const
  {
    return mName.Get();
  }

  HeU32        getSpecCount(void) // return the number of data items in the specification.
  {
    HeU32 ret = 0;

    ret = (HeU32)mValues.size();

    return ret;
  }

  bool         getSpecItem(HeU32 index,KeyValueDataItem &d)
  {
    bool ret = false;

    HeU32 count = (HeU32)mValues.size();
    HE_ASSERT( index < count );
    if ( index < count )
    {
      MyKeyValueMap::iterator i;
      for (i=mValues.begin(); i!=mValues.end(); ++i)
      {
        MyKeyValue *mkv = (*i).second;
        if ( mkv->mIndex == index )
        {
          ret = true;
          mkv->get(d);
          break;
        }
      }
      HE_ASSERT(ret);
    }

    return ret;
  }

  bool         setSpec(const char *spec) // sets the specification, erasing all previous values.
  {
    bool ret = false;

    release();
    if ( spec )
    {
      InPlaceParser ipp;
      ipp.DefaultSymbols();
      ipp.setLineFeed('|');
      ipp.Parse(spec,this);
      mVersionNumber++;
    }

    return ret;
  }

  bool         addSpec(const char *spec) // add another line to the specification.
  {
    bool ret = false;

    if ( spec )
    {
      InPlaceParser ipp;
      ipp.DefaultSymbols();
      ipp.Parse(spec,this);
      mVersionNumber++;
    }

    return ret;
  }

  const StringRef & getName(void) const { return mName; };

  KeyValueType getType(const char *str) const
  {
    KeyValueType ret = KVT_NONE;

    HeU32 v;
    if ( mKeyValueTypes->Get(str,v) )
    {
      ret = (KeyValueType)v;
    }
    return ret;
  }


	HeI32 ParseLine(HeI32 /*lineno*/,HeI32 argc,const char **argv)   // return TRUE to continue parsing, return FALSE to abort parsing process
  {
    HeI32 ret = 0;


    //            0   1   2   3      4        5   6       7   8
    // Usage:   <key><=><type>,<default_value>,<min_value>,<max_value>
    if ( argc >= 3 )
    {
      if ( strcmp(argv[1],"=") == 0 )
      {
        KeyValueType type = getType(argv[2]);
        if ( type != KVT_NONE )
        {
          StringRef key = mDictionary.Get(argv[0]);
          MyKeyValue *kv = 0;

          MyKeyValueMap::iterator found;
          found = mValues.find(key);
          if ( found == mValues.end() )
          {
            kv = MEMALLOC_NEW(MyKeyValue)(mIndex,key.Get());
            mValues[key] = kv;
            mIndex++;
          }
          else
          {
            kv = (*found).second;
          }
          kv->set(type,argc,argv);
        }
      }
    }


    return ret;
  }

  void release(void)
  {
    MyKeyValueMap::iterator i;
    for (i=mValues.begin(); i!=mValues.end(); ++i)
    {
      MyKeyValue *kv = (*i).second;
      delete kv;
    }
    mValues.clear();
    mIndex = 0;
    if ( mDefaultData )
    {
      mFactory->releaseKeyValueData(mDefaultData);
      mDefaultData = 0;
    }
  }

  HeU32 getCount(void) const
  {
    return (HeU32)mValues.size();
  }

  bool getIndex(const char *key,HeU32 &index)
  {
    bool ret = false;

    MyKeyValue *kv = locateKeyValue(key);
    if ( kv )
    {
      ret = true;
      index = kv->mIndex;
    }
    return ret;
  }

  MyKeyValue * locateKeyValue(const char *key)
  {
    MyKeyValue *ret = 0;

    StringRef ref = mDictionary.Get(key);
    MyKeyValueMap::iterator found;
    found = mValues.find(ref);
    if ( found != mValues.end() )
    {
      ret = (*found).second;
    }
    return ret;
  }

  void         setUserId(HeU32 id)
  {
    mUserId = id;
  }

  HeU32        getUserId(void)
  {
    return mUserId;
  }

  void         setUserData(void *data)
  {
    mUserData = data;
  }

  void *       getUserData(void)
  {
    return mUserData;
  }

  const char * getSpecification(void)
  {
    const char *ret = 0;

    mSpecification.clear();
    MyKeyValueMap::iterator i;
    for (i=mValues.begin(); i!=mValues.end(); i++)
    {
      MyKeyValue *mkv = (*i).second;
      mkv->getSpecification(mSpecification);
    }

    if ( !mSpecification.empty() )
    {
      ret = mSpecification.c_str();
    }

    return ret;
  }

  HeU32        getDefaultData(void)  // retrieve a data Id that corresponds to the default values for this specification.
  {
    if ( mDefaultData == 0 )
    {
      mDefaultData = mFactory->createKeyValueData(mName.Get(),"");
    }
    return mDefaultData;
  }

private:
  HeU32             mUserId;
  void             *mUserData;
  HeU32             mIndex;
  HeU32             mVersionNumber;
  StringDict        mDictionary;
  StringTableInt   *mKeyValueTypes;
  StringRef         mName;
  MyKeyValueMap     mValues;
  std::string       mSpecification;
  KeyValueSpecFactory *mFactory;
  HeU32             mDefaultData;
};

typedef USER_STL::map< StringRef, MyKeyValueSpec * > MyKeyValueSpecMap;

class KeyData
{
public:
  KeyData(void)
  {
    t.mString = 0;
    mType = KVT_NONE;
    mValueY = 0;
    mValueZ = 0;
  }

  ~KeyData(void)
  {
    if ( mType == KVT_STRING && t.mString )
    {
      MEMALLOC_FREE(t.mString);
    }
  }

  void set(const MyKeyValue &kv)
  {
    mType = kv.mType;
    switch ( mType )
    {
      case KVT_BOOLEAN:
        t.mState = kv.t.mState;
        break;
      case KVT_INTEGER:
        t.mValue = kv.t.mValue;
        break;
      case KVT_FLOAT:
        t.mValueF = kv.t.mValueF;
        break;
      case KVT_STRING:
        setString(kv.t.mString);
        break;
      case KVT_VECTOR3:
        t.mValueF = kv.t.mValueF;
        mValueY = kv.tmin.mMinValueF;
        mValueZ = kv.tmax.mMaxValueF;
        break;
    }
  }

  void setString(const char *str)
  {
    if ( t.mString )
    {
      MEMALLOC_FREE(t.mString);
      t.mString = 0;
    }
    if ( str )
    {
      size_t len = strlen(str);
      t.mString = (char *)MEMALLOC_MALLOC(len+1);
      strcpy(t.mString,str);
    }
  }

  void getDescription(MyKeyValue *mkv,std::string &str) const
  {
    std::string name = mkv->mName;
    str+=name;
    str+="=";

    const char *value = "";
    char temp[512];

    switch ( mType )
    {
      case KVT_BOOLEAN:
        value = t.mState ? "true" : "false";
        break;
      case KVT_INTEGER:
        sprintf(temp,"%d", t.mValue );
        value = temp;
        break;
      case KVT_FLOAT:
        value = FloatString( t.mValueF );
        break;
      case KVT_STRING:
        if ( t.mString )
        {
          value = t.mString;
        }
        break;
      case KVT_VECTOR3:
        sprintf(temp,"(%s,%s,%s)", FloatString( t.mValueF ), FloatString(mValueY), FloatString(mValueZ) );
        value = temp;
        break;
    }
    std::string v = value;
    str+=v;
  }

  KeyValueType  mType;

  union T
  {
    bool        mState;
    int         mValue;
    float       mValueF;
    char *      mString;
  } t;
  float mValueY;
  float mValueZ;

};

class MyKeyValueData : public KeyValueData
{
public:
  MyKeyValueData(MyKeyValueSpec *spec,const char * data)
  {
    mUserId = 0;
    mUserData = 0;
    mSpec  = spec;
    mCount = spec->getCount();
    if ( mCount )
    {
      mData  = MEMALLOC_NEW_ARRAY(KeyData,mCount)[mCount];

      MyKeyValueMap::iterator i;
      for (i=spec->mValues.begin(); i!=spec->mValues.end(); ++i)
      {
        MyKeyValue *kv = (*i).second;

        HeU32 index = kv->mIndex;

        KeyData &dest = mData[index];

        dest.set(*kv);
      }
      setData(data);
    }
    else
    {
      mData = 0;
    }
    mVersionNumber = 1;
  }

  ~MyKeyValueData(void)
  {
    delete []mData;
  }


  virtual HeU32        getVersionNumber(void) const
  {
    return mVersionNumber;
  }

  const char *nextComma(const char *c) const
  {
    const char *ret = 0;
    while ( *c )
    {
      if ( *c == ',' ) 
      {
        ret = c;
        break;
      }
      c++;
    }
    return ret;
  }

  bool         setData(const char * data)// convert ASCII data into the binary results.
  {
    bool ret = false;

    if ( data )
    {
      HeU32 count;
      KeyValue k;
      const char **kv = k.getKeyValues(data,count);
      for (HeU32 i=0; i<count; i++)
      {
        const char *key   = kv[0];
        const char *value = kv[1];

        KeyValueType type;
        KeyData *kd = getKeyData(key,type);

        if ( kd )
        {
          ret = true;
          switch ( type )
          {
            case KVT_BOOLEAN:
              kd->t.mState = getBool( value );
              break;
            case KVT_INTEGER:
              kd->t.mValue = atoi(value);
              break;
            case KVT_FLOAT:
              kd->t.mValueF = (float)atof(value);
              break;
            case KVT_STRING:
              kd->setString(value);
              break;
            case KVT_VECTOR3:
              {
                kd->t.mValueF = (float)atof(value);
                kd->mValueY = kd->t.mValueF;
                kd->mValueZ = kd->t.mValueF;
                const char *comma = nextComma(value);
                if ( comma )
                {
                  comma++;
                  kd->mValueY = (float)atof(comma);
                  comma = nextComma(comma);
                  if ( comma )
                  {
                    comma++;
                    kd->mValueZ = (float)atof(comma);
                  }
                }
              }
              break;
          }
        }


        kv+=2;
      }
    }

    mVersionNumber++;


    return ret;
  }

  const char  *getData(void)
  {
    const char * ret = 0;

    //
    //
    mStringData.clear();

    bool first = true;

    MyKeyValueMap::iterator i;
    for (i=mSpec->mValues.begin(); i!=mSpec->mValues.end(); ++i)
    {
      MyKeyValue *mkv = (*i).second;
      KeyData    *kd  = &mData[mkv->mIndex];


      if ( first )
      {
        first = false;
      }
      else
      {
        mStringData.push_back(',');
      }

      kd->getDescription(mkv,mStringData);
    }

    ret = mStringData.c_str();

    return ret;
  }

  KeyData * getData(const char *key,KeyValueType type)
  {
    KeyData *ret = 0;

    HeU32 index;
    if ( mSpec->getIndex(key,index) )
    {
      if ( index < mCount )
      {
        ret = &mData[index];
        if ( ret->mType != type )
        {
          HE_ASSERT(0);
          ret = 0;
        }
      }
      else
      {
        HE_ASSERT(0);
      }
    }
    return ret;
  }

  KeyData * getKeyData(const char *key,KeyValueType &type)
  {
    KeyData *ret = 0;

    HeU32 index;
    if ( mSpec->getIndex(key,index) )
    {
      if ( index < mCount )
      {
        ret = &mData[index];
        type = ret->mType;
      }
      else
      {
        HE_ASSERT(0);
      }
    }
    return ret;
  }

  bool         getState(const char *key,bool &state)
  {
    bool ret = false;

    KeyData *data = getData(key,KVT_BOOLEAN);
    if ( data )
    {
      ret = true;
      state = data->t.mState;
    }

    return ret;
  }

  bool         getState(const char *key,HeF32 &state)
  {
    bool ret = false;

    KeyData *data = getData(key,KVT_FLOAT);
    if ( data )
    {
      ret = true;
      state = data->t.mValueF;
    }

    return ret;
  }

  const char * getState(const char *key) // retrieve it as a string.
  {
    const char *ret = 0;

    KeyData *data = getData(key,KVT_STRING);
    if ( data )
    {
      ret = data->t.mString;
    }


    return ret;
  }

  bool         getState(const char *key,HeI32 &value)
  {
    bool ret = false;

    KeyData *data = getData(key,KVT_INTEGER);
    if ( data )
    {
      ret = true;
      value = data->t.mValue;
    }

    return ret;
  }

  bool         getState(const char * key,HeVec3 &value)
  {
    bool ret = false;

    KeyData *data = getData(key,KVT_VECTOR3);
    if ( data )
    {
      ret = true;
      value.x = data->t.mValueF;
      value.y = data->mValueY;
      value.z = data->mValueZ;
    }

    return ret;
  }

  bool         setState(const char *key,bool state)
  {
    bool ret = false;

    KeyData *data = getData(key,KVT_BOOLEAN);
    if ( data )
    {
      data->t.mState = state;
      ret = true;
    }

    return ret;
  }

  bool         setState(const char *key,HeF32 state)
  {
    bool ret = false;

    KeyData *data = getData(key,KVT_FLOAT);
    if ( data )
    {
      data->t.mValueF = state;
      mVersionNumber++;
      ret = true;
    }

    return ret;
  }

  bool setState(const char *key,const char *value)   // retrieve it as a string.
  {
    bool ret = false;

    KeyData *data = getData(key,KVT_STRING);
    if ( data )
    {
      data->setString(value);
      mVersionNumber++;
      ret = true;
    }

    return ret;
  }

  bool         setState(const char *key,HeI32 value)
  {
    bool ret = false;

    KeyData *data = getData(key,KVT_INTEGER);
    if ( data )
    {
      data->t.mValue = value;
      mVersionNumber++;
      ret = true;
    }


    return ret;
  }

  bool         setState(const char * key,const HeVec3 &v)
  {
    bool ret = false;

    KeyData *data = getData(key,KVT_VECTOR3);
    if ( data )
    {
      data->t.mValueF = v.x;
      data->mValueY = v.y;
      data->mValueZ = v.z;
      mVersionNumber++;
      ret = true;
    }


    return ret;
  }

  const char  *getSpecName(void) const // return the name of the parent specification.
  {
    return mSpec->getSpecName();
  }

  HeU32        getDataCount(void) // return the number of data items in the specification.
  {
    return mCount;
  }

  bool         getDataItem(HeU32 index,KeyValueDataItem &d)
  {
    bool ret = false;

    HE_ASSERT( index < mCount );

    if ( index < mCount )
    {
      KeyData &kd = mData[index];
      MyKeyValueMap::iterator i;
      for (i=mSpec->mValues.begin(); i!=mSpec->mValues.end(); ++i)
      {
        MyKeyValue *mkv = (*i).second;
        if ( mkv->mIndex == index )
        {
          ret = true;
          mkv->get(d);
          switch ( mkv->mType )
          {
            case KVT_BOOLEAN:
              d.t.mState = kd.t.mState;
              break;
            case KVT_INTEGER:
              d.t.mValue = kd.t.mValue;
              break;
            case KVT_FLOAT:
              d.t.mValueF = kd.t.mValueF;
              break;
            case KVT_STRING:
              d.t.mString = kd.t.mString;
              break;
            case KVT_VECTOR3:
              d.t.mValueF = kd.t.mValueF;
              d.tmin.mMinValueF = kd.mValueY;
              d.tmax.mMaxValueF = kd.mValueZ;
              break;
          }
          break;
        }
      }
      HE_ASSERT(ret);
    }
    return ret;
  }

  void         setUserId(HeU32 id)
  {
    mUserId = id;
  }

  HeU32        getUserId(void)
  {
    return mUserId;
  }

  void         setUserData(void *data)
  {
    mUserData = data;
  }

  void *       getUserData(void)
  {
    return mUserData;
  }

  HeU32                mUserId;
  void                *mUserData;
  HeU32                mCount;
  KeyData             *mData;
  MyKeyValueSpec      *mSpec;
  HeU32                mVersionNumber;
  std::string          mStringData; // the data as a key/value pair CSV string.
};

typedef USER_STL::map< HeU32, MyKeyValueData * > MyKeyValueDataMap;
typedef USER_STL::vector< StringRef > StringRefVector;
typedef USER_STL::vector< KeyValueData * > KeyValueDataVector;

class MyKeyValueSpecFactory : public KeyValueSpecFactory
{
public:

  MyKeyValueSpecFactory(void)
  {
    mGuid = 0;
    mKeyValueTypes.SetCaseSensitive(false);
    mKeyValueTypes.Add("bool",KVT_BOOLEAN);
    mKeyValueTypes.Add("boolean",KVT_BOOLEAN);
    mKeyValueTypes.Add("int",KVT_INTEGER);
    mKeyValueTypes.Add("integer",KVT_INTEGER);
    mKeyValueTypes.Add("float",KVT_FLOAT);
    mKeyValueTypes.Add("string",KVT_STRING);
    mKeyValueTypes.Add("str",KVT_STRING);
    mKeyValueTypes.Add("vector3",KVT_VECTOR3);

  }

  ~MyKeyValueSpecFactory(void)
  {
    {
      MyKeyValueSpecMap::iterator i;
      for (i=mSpecs.begin(); i!=mSpecs.end(); i++)
      {
        MyKeyValueSpec *spec = (*i).second;
        delete spec;
      }
    }
    {
      MyKeyValueDataMap::iterator i;
      for (i=mData.begin(); i!=mData.end(); ++i)
      {
        MyKeyValueData *data = (*i).second;
        delete data;
      }
    }
  }

  KeyValueSpec * createKeyValueSpec(const char *specName,const char *specification)
  {
    KeyValueSpec *ret = 0;


    ret = locateKeyValueSpec(specName);
    if ( ret == 0 )
    {
      StringRef ref = mDictionary.Get(specName);
      MyKeyValueSpec *spec = MEMALLOC_NEW(MyKeyValueSpec)(ref,&mKeyValueTypes,this);
      ret = static_cast< KeyValueSpec *>(spec);
      mSpecs[ref] = spec;
    }

    ret->setSpec(specification);

    return ret;
  }

  void releaseKeyValueSpec(KeyValueSpec *spec)
  {
    if ( spec )
    {
      MyKeyValueSpec *mspec = static_cast< MyKeyValueSpec *>(spec);
      const StringRef &ref = mspec->getName();
      MyKeyValueSpecMap::iterator found;
      found = mSpecs.find(ref);
      if ( found != mSpecs.end() )
      {
#if _DEBUG
        MyKeyValueSpec *_spec = (*found).second;
        HE_ASSERT( mspec == _spec );
#endif
        delete mspec;
        mSpecs.erase(found);
      }
      else
      {
        HE_ASSERT(0);
      }
    }
  }

  bool releaseKeyValueSpec(const char *specName)
  {
    bool ret = false;

    KeyValueSpec *spec = locateKeyValueSpec(specName);
    if ( spec )
    {
      releaseKeyValueSpec(spec);
      ret = true;
    }
    return ret;
  }

  KeyValueSpec * locateKeyValueSpec(const char *specName)
  {
    KeyValueSpec *ret = 0;

    StringRef ref = mDictionary.Get(specName);
    MyKeyValueSpecMap::iterator found;
    found = mSpecs.find(ref);
    if ( found != mSpecs.end() )
    {
      MyKeyValueSpec *spec = (*found).second;
      ret = static_cast< KeyValueSpec *>(spec);
    }

    return ret;
  }

  HeU32          createKeyValueData(const char *specName,const char *data)
  {
    HeU32 ret = 0;

    KeyValueSpec *spec = locateKeyValueSpec(specName);
    if ( spec )
    {
      mGuid++;
      MyKeyValueSpec *mspec = static_cast< MyKeyValueSpec *>(spec);
      MyKeyValueData *mdata = MEMALLOC_NEW(MyKeyValueData)(mspec,data);
      mData[mGuid] = mdata;
      ret = mGuid;
    }

    return ret;
  }

  bool           releaseKeyValueData(HeU32 data)
  {
    bool ret = false;

    MyKeyValueDataMap::iterator found;
    found = mData.find(data);
    if ( found != mData.end() )
    {
      MyKeyValueData *d = (*found).second;
      delete d;
      mData.erase(found);
      ret = true;
    }

    return ret;
  }

  KeyValueData * locateKeyValueData(HeU32 id)
  {
    KeyValueData *ret = 0;

    MyKeyValueDataMap::iterator found;
    found = mData.find(id);
    if ( found != mData.end() )
    {
      MyKeyValueData *d = (*found).second;
      ret = static_cast< KeyValueData *>(d);
    }
    return ret;
  }

  const char **  getKeyValueSpecList(HeU32 &count) // returns a list of all of the currently defined specifications.
  {
    const char **ret = 0;
    count = 0;

    if ( !mSpecs.empty() )
    {
      mSpecList.clear();
      MyKeyValueSpecMap::iterator i;
      for (i=mSpecs.begin(); i!=mSpecs.end(); ++i)
      {
        mSpecList.push_back( (*i).first );
        count++;
      }
      ret = (const char **)&mSpecList[0];
    }


    return ret;
  }

  KeyValueData ** getKeyValueDataList(HeU32 &count)
  {
    KeyValueData **ret = 0;
    count =  0;

    mDataList.clear();
    MyKeyValueDataMap::iterator i;
    for (i=mData.begin(); i!=mData.end(); ++i)
    {
      MyKeyValueData *m = (*i).second;
      KeyValueData   *d = static_cast< KeyValueData *>(m);
      mDataList.push_back(d);
    }
    if ( !mDataList.empty() )
    {
      ret = &mDataList[0];
      count = (HeU32)mDataList.size();
    }

    return ret;
  }

private:
  HeU32             mGuid;
  StringTableInt    mKeyValueTypes;
  StringDict        mDictionary;
  MyKeyValueDataMap mData;
  MyKeyValueSpecMap mSpecs;
  StringRefVector   mSpecList;
  KeyValueDataVector mDataList;
};


KeyValueSpecFactory *gKeyValueSpecFactory=0;


void                  initDefaultKeyValueSpecFactory(void)
{
  if ( gKeyValueSpecFactory == 0 )
  {
    MyKeyValueSpecFactory *f = MEMALLOC_NEW(MyKeyValueSpecFactory);
    gKeyValueSpecFactory = static_cast< KeyValueSpecFactory *>(f);
  }
}


KeyValueSpecFactory * createKeyValueSpecFactory(void)
{
  MyKeyValueSpecFactory *m = MEMALLOC_NEW(MyKeyValueSpecFactory);
  return static_cast< KeyValueSpecFactory *>(m);
}

void                  releaseKeyValueSpecFactory(KeyValueSpecFactory *k)
{
  MyKeyValueSpecFactory *f = static_cast< MyKeyValueSpecFactory *>(k);
  delete f;
}
