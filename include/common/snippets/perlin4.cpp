#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>

#include "perlin4.h"
#include "keyvalue.h"

#pragma warning(disable:4996)

namespace PERLIN4
{

enum EquationType
{
  MF_SQUARED,
  MF_SIN,
  MF_COS,
  MF_TAN,
  MF_SINH,
  MF_COSH,
  MF_TANH,
  MF_ASIN,
  MF_ACOS,
  MF_ATAN,
  MF_EXP,
  MF_LOG,
  MF_SQRT,
  MF_LAST
};

class Equation : public NVSHARE::Memalloc
{
public:
  Equation(EquationType type,NxF32 constant,NxF32 scaler,NxF32 minv=-256,NxF32 maxv=256);
  ~Equation(void) { };
  NxF32 Get(NxF32 v) const; // compute it using function.
private:
  EquationType mFunction;
  NxF32 mMin;
  NxF32 mMax;
  NxF32 mConstant;
  NxF32 mScaler;
};

enum EnvelopeType
{
  ET_NULL,      // 0 do nothing, leave it as is.
  ET_SQUARED1,  // 1
  ET_SQUARED2,  // 2
  ET_SQUARED3,  // 3
  ET_ATAN1,     // 4
  ET_ATAN2,     // 5
  ET_ATAN3,     // 6
  ET_ACOS1,     // 7
  ET_ACOS2,     // 8
  ET_ACOS3,     // 9
  ET_ACOS4,     // 10
  ET_COS1,      // 11
  ET_COS2,      // 12
  ET_EXP1,      // 13
  ET_EXP2,      // 14
  ET_EXP3,      // 15
  ET_EXP4,      // 16
  ET_EXP5,      // 17
  ET_LOG1,      // 18
  ET_LOG2,      // 19
  ET_LOG3,      // 20
  ET_TANH,      // 21
  ET_SQRT,      // 22
  ET_LAST
};

class Envelopes : public NVSHARE::Memalloc
{
public:
  Envelopes(void);
  ~Envelopes(void);

  NxF32 Get(NxF32 v,EnvelopeType t) const;

  // convert string into an equation type.
  static EnvelopeType Get(const char *str);

private:

  Equation * Get(EnvelopeType t);

  Equation *mEnvelopes[ET_LAST];
};

Envelopes::Envelopes(void)
{
  for (NxI32 i=ET_NULL; i<ET_LAST; i++)
  {
    mEnvelopes[i] = Get((EnvelopeType)i);
  }
}

Envelopes::~Envelopes(void)
{
  for (NxI32 i=ET_NULL; i<ET_LAST; i++) delete mEnvelopes[i];
}

NxF32 Envelopes::Get(NxF32 v,EnvelopeType t) const
{
  if ( mEnvelopes[t] ) return mEnvelopes[t]->Get(v);
  return v;
}

Equation * Envelopes::Get(EnvelopeType choice)
{
  Equation *e=0;

  switch ( choice )
  {
    case ET_SQUARED1:
    case ET_SQUARED2:
    case ET_SQUARED3:
      {
      NxF32 ratio = NxF32(choice-ET_NULL)/3;
      NxF32 scaler1 = ratio*0.02f;
      e = MEMALLOC_NEW(Equation)(MF_SQUARED,scaler1,1);
      }
      break;
    case ET_ATAN1:
  	case ET_ATAN2:
	  case ET_ATAN3:
      {
	    NxF32 ratio = NxF32(choice-ET_SQUARED3)/3;
      NxF32 scaler1 = 90*ratio;
      NxF32 amp1 = 0.03f;
      e = MEMALLOC_NEW(Equation)(MF_ATAN,amp1,scaler1);
      }
      break;
    case ET_ACOS1:
    case ET_ACOS2:
    case ET_ACOS3:
    case ET_ACOS4:
      {
      #define T_SCALE3 180
      #define T_AMP3 0.011f
      NxF32 ratio = NxF32(choice-ET_ATAN3)/4;
      NxF32 scaler1 = T_SCALE3*ratio;
      NxF32 amp1 = T_AMP3;
      e = MEMALLOC_NEW(Equation)(MF_ACOS,amp1,scaler1);
      }
      break;
    case ET_COS1:
      {
      #define T_SCALE7 128
      #define T_AMP7 0.015f
      NxF32 scaler1 = T_SCALE7;
      NxF32 amp1 = T_AMP7;
      e = MEMALLOC_NEW(Equation)(MF_COS,amp1,scaler1);
      }
      break;
    case ET_COS2:
      {
      #define T_SCALE8 128
      #define T_AMP8 0.02f
      NxF32 scaler1 = T_SCALE8;
      NxF32 amp1 = T_AMP8;
      e = MEMALLOC_NEW(Equation)(MF_COS,amp1,scaler1);
      }
      break;
    case ET_EXP1:
      {
      #define T_SCALE9 128
      #define T_AMP9 0.025f
      NxF32 scaler1 = T_SCALE9;
      NxF32 amp1 = T_AMP9;
      e = MEMALLOC_NEW(Equation)(MF_COS,amp1,scaler1);
      }
      break;
    case ET_EXP2:
      {
      #define T_SCALE11 80
      #define T_AMP11 0.010f
      NxF32 scaler1 = T_SCALE11;
      NxF32 amp1 = T_AMP11;
      e = MEMALLOC_NEW(Equation)(MF_EXP,amp1,scaler1);
      }
      break;
    case ET_EXP3:
      {
      #define T_SCALE12 120
      #define T_AMP12 0.013f
      NxF32 scaler1 = T_SCALE12;
      NxF32 amp1 = T_AMP12;
      e = MEMALLOC_NEW(Equation)(MF_EXP,amp1,scaler1);
      }
      break;
    case ET_EXP4:
      {
      #define T_SCALE13 160
      #define T_AMP13 0.018f
      NxF32 scaler1 = T_SCALE13;
      NxF32 amp1 = T_AMP13;
      e = MEMALLOC_NEW(Equation)(MF_EXP,amp1,scaler1);
      }
      break;
    case ET_EXP5:
      {
      #define T_SCALE14 600
      #define T_AMP14 0.055f
      NxF32 scaler1 = T_SCALE14;
      NxF32 amp1 = T_AMP14;
      e = MEMALLOC_NEW(Equation)(MF_EXP,amp1,scaler1);
      }
      break;
    case ET_LOG1:
      {
      #define T_SCALE15 30
      #define T_AMP15 0.12f
      NxF32 scaler1 = T_SCALE15;
      NxF32 amp1 = T_AMP15;
      e = MEMALLOC_NEW(Equation)(MF_LOG,amp1,scaler1,-4,256);
      }
      break;
    case ET_LOG2:
      {
      #define T_SCALE16 70
      #define T_AMP16 0.03f
      NxF32 scaler1 = T_SCALE16;
      NxF32 amp1 = T_AMP16;
      e = MEMALLOC_NEW(Equation)(MF_LOG,amp1,scaler1,-15,256);
      }
      break;
    case ET_LOG3:
      {
      #define T_SCALE17 60
      #define T_AMP17 0.10f
      NxF32 scaler1 = T_SCALE17;
      NxF32 amp1 = T_AMP17;
      e = MEMALLOC_NEW(Equation)(MF_LOG,amp1,scaler1,-1,70);
      }
      break;
    case ET_TANH:
      {
      #define T_SCALE18 64
      #define T_AMP18 0.027f
      NxF32 scaler1 = T_SCALE18;
      NxF32 amp1 = T_AMP18;
      e = MEMALLOC_NEW(Equation)(MF_TANH,amp1,scaler1,-256,256);
      }
      break;
    case ET_SQRT:
      {
      #define T_SCALE19 256
      #define T_AMP19 0.009f
      NxF32 scaler1 = T_SCALE19;
      NxF32 amp1 = T_AMP19;
      e = MEMALLOC_NEW(Equation)(MF_SQRT,amp1,scaler1,30,256);
      }
      break;
  }

  return e;
}


Equation::Equation(EquationType type,NxF32 constant,NxF32 scaler,NxF32 minv,NxF32 maxv)
{
  mFunction = type;
  mConstant = constant;
  mScaler   = scaler;
  mMin      = minv;
  mMax      = maxv;
};

NxF32 Equation::Get(NxF32 v) const
{
  switch ( mFunction )
  {
    case MF_SQUARED:
      v = v*v*mConstant*mScaler;
      break;
    case MF_SIN:
      v = (NxF32)sin(v*mConstant)*mScaler;
      break;
    case MF_COS:
      v = (NxF32)cos(v*mConstant)*mScaler;
      break;
    case MF_TAN:
      v = (NxF32)tan(v*mConstant)*mScaler;
      break;
    case MF_SINH:
      v = (NxF32)sinh(v*mConstant)*mScaler;
      break;
    case MF_COSH:
      v = (NxF32)cosh(v*mConstant)*mScaler;
      break;
    case MF_TANH:
      v =(NxF32)tanh(v*mConstant)*mScaler;
      break;
    case MF_ASIN:
      {
      NxF32 i = v*mConstant;
      if ( i < -1 ) i = -1;
      if ( i >  1 ) i = 1;
      v = (NxF32)asin(i)*mScaler;
      }
      break;
    case MF_ACOS:
      {
      NxF32 i = v*mConstant;
      if ( i < -1 ) i = -1;
      if ( i >  1 ) i = 1;
      v = (NxF32)acos(i)*mScaler;
      }
      break;
    case MF_ATAN:
      v = (NxF32)atan(v*mConstant)*mScaler;
      break;
    case MF_EXP:
      v = (NxF32)exp(v*mConstant)*mScaler;
      break;
    case MF_LOG:
      v = (NxF32)log( fabs(v*mConstant) )*mScaler;
      break;
    case MF_SQRT:
      v = (NxF32)sqrt( fabs(v*mConstant) )*mScaler;
      break;
  }

  if ( v < mMin ) v = mMin;
  if ( v > mMax ) v = mMax;

  return v;
};


EnvelopeType Envelopes::Get(const char *str)
{
  if ( stricmp(str,"null") == 0 ) return ET_NULL;
  if ( stricmp(str,"squared1") == 0 ) return ET_SQUARED1;
  if ( stricmp(str,"squared2") == 0 ) return ET_SQUARED2;
  if ( stricmp(str,"squared3") == 0 ) return ET_SQUARED3;
  if ( stricmp(str,"atan1") == 0 ) return ET_ATAN1;
  if ( stricmp(str,"atan2") == 0 ) return ET_ATAN2;
  if ( stricmp(str,"atan3") == 0 ) return ET_ATAN3;
  if ( stricmp(str,"acos1") == 0 ) return ET_ACOS1;
  if ( stricmp(str,"acos2") == 0 ) return ET_ACOS2;
  if ( stricmp(str,"acos3") == 0 ) return ET_ACOS3;
  if ( stricmp(str,"acos4") == 0 ) return ET_ACOS4;
  if ( stricmp(str,"cos1") == 0 ) return ET_COS1;
  if ( stricmp(str,"cos2") == 0 ) return ET_COS2;
  if ( stricmp(str,"exp1") == 0 ) return ET_EXP1;
  if ( stricmp(str,"exp2") == 0 ) return ET_EXP2;
  if ( stricmp(str,"exp3") == 0 ) return ET_EXP3;
  if ( stricmp(str,"exp4") == 0 ) return ET_EXP4;
  if ( stricmp(str,"exp5") == 0 ) return ET_EXP5;
  if ( stricmp(str,"log1") == 0 ) return ET_LOG1;
  if ( stricmp(str,"log2") == 0 ) return ET_LOG2;
  if ( stricmp(str,"log3") == 0 ) return ET_LOG3;
  if ( stricmp(str,"tanh") == 0 ) return ET_TANH;
  if ( stricmp(str,"sqrt") == 0 ) return ET_SQRT;

  return ET_NULL;
}

#define B 0x100
#define BM 0xff

#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

#define MAXTERMS 32

class Perlin : public NVSHARE::Memalloc
{
public:

  Perlin(NxI32 octaves,NxF32 freq,NxF32 amp,NxI32 seed);

  void init_perlin(NxI32 n,NxF32 p);

  NxF32 Get(NxF32 x,NxF32 y)
  {
    NxF32 vec[2];
    vec[0] = x;
    vec[1] = y;
    return perlin_noise_2D(vec);
  };

  NxF32 perlin_noise_2D(NxF32 vec[2]);

  NxF32 noise1(NxF32 arg);
  NxF32 noise2(NxF32 vec[2]);
  NxF32 noise3(NxF32 vec[3]);
  void normalize2(NxF32 v[2]);
  void normalize3(NxF32 v[3]);
  void init(void);
private:
  NxI32   mOctaves;
  NxF32 mFrequency;
  NxF32 mAmplitude;
  NxI32   mSeed;

  NxI32 p[B + B + 2];
  NxF32 g3[B + B + 2][3];
  NxF32 g2[B + B + 2][2];
  NxF32 g1[B + B + 2];
  bool  mStart;
  NxF32	scale[MAXTERMS];	// scaling factor for renormalization
};

#define s_curve(t) ( t * t * (3.0f - 2.0f * t) )
#define lerp(t, a, b) ( a + t * (b - a) )

#define setup(i,b0,b1,r0,r1)\
	t = vec[i] + N;\
	b0 = ((NxI32)t) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t - (NxI32)t;\
	r1 = r0 - 1.0f;

NxF32 Perlin::noise1(NxF32 arg)
{
	NxI32 bx0, bx1;
	NxF32 rx0, rx1, sx, t, u, v, vec[1];

	vec[0] = arg;

	if (mStart)
  {
    srand(mSeed);
		mStart = false;
		init();
	}

	setup(0, bx0,bx1, rx0,rx1);

	sx = s_curve(rx0);

	u = rx0 * g1[ p[ bx0 ] ];
	v = rx1 * g1[ p[ bx1 ] ];

	return lerp(sx, u, v);
}

NxF32 Perlin::noise2(NxF32 vec[2])
{
	NxI32 bx0, bx1, by0, by1, b00, b10, b01, b11;
	NxF32 rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
	NxI32 i, j;

	if (mStart)
  {
    srand(mSeed);
		mStart = false;
		init();
	}

	setup(0,bx0,bx1,rx0,rx1);
	setup(1,by0,by1,ry0,ry1);

	i = p[bx0];
	j = p[bx1];

	b00 = p[i + by0];
	b10 = p[j + by0];
	b01 = p[i + by1];
	b11 = p[j + by1];

	sx = s_curve(rx0);
	sy = s_curve(ry0);

  #define at2(rx,ry) ( rx * q[0] + ry * q[1] )

	q = g2[b00];
	u = at2(rx0,ry0);
	q = g2[b10];
	v = at2(rx1,ry0);
	a = lerp(sx, u, v);

	q = g2[b01];
	u = at2(rx0,ry1);
	q = g2[b11];
	v = at2(rx1,ry1);
	b = lerp(sx, u, v);

	return lerp(sy, a, b);
}

NxF32 Perlin::noise3(NxF32 vec[3])
{
	NxI32 bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	NxF32 rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
	NxI32 i, j;

	if (mStart)
  {
    srand(mSeed);
		mStart = false;
		init();
	}

	setup(0, bx0,bx1, rx0,rx1);
	setup(1, by0,by1, ry0,ry1);
	setup(2, bz0,bz1, rz0,rz1);

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

	t  = s_curve(rx0);
	sy = s_curve(ry0);
	sz = s_curve(rz0);

  #define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

	q = g3[ b00 + bz0 ] ; u = at3(rx0,ry0,rz0);
	q = g3[ b10 + bz0 ] ; v = at3(rx1,ry0,rz0);
	a = lerp(t, u, v);

	q = g3[ b01 + bz0 ] ; u = at3(rx0,ry1,rz0);
	q = g3[ b11 + bz0 ] ; v = at3(rx1,ry1,rz0);
	b = lerp(t, u, v);

	c = lerp(sy, a, b);

	q = g3[ b00 + bz1 ] ; u = at3(rx0,ry0,rz1);
	q = g3[ b10 + bz1 ] ; v = at3(rx1,ry0,rz1);
	a = lerp(t, u, v);

	q = g3[ b01 + bz1 ] ; u = at3(rx0,ry1,rz1);
	q = g3[ b11 + bz1 ] ; v = at3(rx1,ry1,rz1);
	b = lerp(t, u, v);

	d = lerp(sy, a, b);

	return lerp(sz, c, d);
}

void Perlin::normalize2(NxF32 v[2])
{
	NxF32 s;

	s = (NxF32)sqrt(v[0] * v[0] + v[1] * v[1]);
  s = 1.0f/s;
	v[0] = v[0] * s;
	v[1] = v[1] * s;
}

void Perlin::normalize3(NxF32 v[3])
{
	NxF32 s;

	s = (NxF32)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  s = 1.0f/s;

	v[0] = v[0] * s;
	v[1] = v[1] * s;
	v[2] = v[2] * s;
}

void Perlin::init(void)
{
	NxI32 i, j, k;

	for (i = 0 ; i < B ; i++)
  {
		p[i] = i;
		g1[i] = (NxF32)((rand() % (B + B)) - B) / B;
		for (j = 0 ; j < 2 ; j++)
			g2[i][j] = (NxF32)((rand() % (B + B)) - B) / B;
		normalize2(g2[i]);
		for (j = 0 ; j < 3 ; j++)
			g3[i][j] = (NxF32)((rand() % (B + B)) - B) / B;
		normalize3(g3[i]);
	}

	while (--i)
  {
		k = p[i];
		p[i] = p[j = rand() % B];
		p[j] = k;
	}

	for (i = 0 ; i < B + 2 ; i++)
  {
		p[B + i] = p[i];
		g1[B + i] = g1[i];
		for (j = 0 ; j < 2 ; j++)
			g2[B + i][j] = g2[i][j];
		for (j = 0 ; j < 3 ; j++)
			g3[B + i][j] = g3[i][j];
	}

  // create renormaliztion scaling array

	for( i=0; i<MAXTERMS; i++ )
	{
		scale[i] = (NxF32)(pow(2.0f, (NxF32)i) / (pow(2.0f, (NxF32)(i+1))-1.0f));
	}

}


NxF32 Perlin::perlin_noise_2D(NxF32 vec[2])
{
  NxI32 terms    = mOctaves;
	NxF32 result = 0.0f;
  NxF32 amp = mAmplitude;

  vec[0]*=mFrequency;
  vec[1]*=mFrequency;

	for( NxI32 i=0; i<terms; i++ )
	{
		result += noise2(vec)*amp;
		vec[0] *= 2.0f;
		vec[1] *= 2.0f;
    amp*=0.5f;
	}

	if( terms >= MAXTERMS )	terms = MAXTERMS-1;

	return result;
}



Perlin::Perlin(NxI32 octaves,NxF32 freq,NxF32 amp,NxI32 seed)
{
  mOctaves = octaves;
  mFrequency = freq;
  mAmplitude = amp;
  mSeed = seed;
  mStart = true;
}

class MyPerlin4 : public Perlin4, public NVSHARE::Memalloc
{
public:

  MyPerlin4(const char *options);
  ~MyPerlin4(void);

  NxF32 Clamp(NxF32 v) const;

  void  SetClampLow(NxF32 low)
  {
    mClampLow = low;
    mClampRecip = 1.0f / (mClampHigh-mClampLow);
  };

  void  SetClampHigh(NxF32 high)
  {
    mClampHigh = high;
    mClampRecip = 1.0f / (mClampHigh-mClampLow);
  };

  NxF32 GetClampLow(void) const { return mClampLow; };
  NxF32 GetClampHigh(void) const { return mClampHigh; };

  void SetEquation1(EnvelopeType et) { mEq1 = et; };
  void SetEquation2(EnvelopeType et) { mEq2 = et; };
  void SetEquation3(EnvelopeType et) { mEq3 = et; };
  void SetEquation4(EnvelopeType et) { mEq4 = et; };

  void CreatePerlin(NxI32 pno,NxI32 octaves,NxF32 freq,NxF32 amp,NxI32 seed);

  void DelPerlin(NxI32 pno);

  NxF32 get(NxF32 x,NxF32 y);

private:
  NxF32   mClampLow;
  NxF32   mClampHigh;
  NxF32   mClampRecip;
  Perlin *mP1;
  Perlin *mP2;
  Perlin *mP3;
  Perlin *mP4;
  Envelopes *mEnvelopes;
  EnvelopeType mEq1;
  EnvelopeType mEq2;
  EnvelopeType mEq3;
  EnvelopeType mEq4;

};

MyPerlin4::MyPerlin4(const char * options)
{
  mClampLow  = 0.0f;
  mClampHigh = 1.0f;
  mClampRecip = 1;

  mP1 = 0;
  mP2 = 0;
  mP3 = 0;
  mP4 = 0;

  mEq1 = ET_NULL;
  mEq2 = ET_NULL;
  mEq3 = ET_NULL;
  mEq4 = ET_NULL;

  mEnvelopes = MEMALLOC_NEW(Envelopes);

  if ( options )
  {
    NxU32 count;
    KeyValue k;
    const char **kv = k.getKeyValues(options,count);


    NxU32 octaves1=0,octaves2=0,octaves3=0,octaves4=0;
    NxF32 freq1=0.017f,freq2=0.017f,freq3=0.017f,freq4=0.017f;
    EnvelopeType type1=ET_NULL;
    EnvelopeType type2=ET_NULL;
    EnvelopeType type3=ET_NULL;
    EnvelopeType type4=ET_NULL;
    NxF32 amp1=1,amp2=1,amp3=1,amp4=1;
    NxU32 seed1=0,seed2=0,seed3=0,seed4=0;


    for (NxU32 i=0; i<count; i++)
    {
      const char *key = kv[i*2+0];
      const char *value = kv[i*2+1];

      if ( stricmp(key,"envelope1") == 0 )
      {
        type1 = Envelopes::Get(value);
      }
      else if ( stricmp(key,"envelope2") == 0 )
      {
        type2 = Envelopes::Get(value);
      }
      else if ( stricmp(key,"envelope3") == 0 )
      {
        type3 = Envelopes::Get(value);
      }
      else if ( stricmp(key,"envelope4") == 0 )
      {
        type4 = Envelopes::Get(value);
      }
      else if ( stricmp(key,"octaves1") == 0 )
      {
        octaves1 = atoi(value);
      }
      else if ( stricmp(key,"octaves2") == 0 )
      {
        octaves2 = atoi(value);
      }
      else if ( stricmp(key,"octaves3") == 0 )
      {
        octaves3 = atoi(value);
      }
      else if ( stricmp(key,"octaves4") == 0 )
      {
        octaves4 = atoi(value);
      }
      else if ( stricmp(key,"frequency1") == 0 )
      {
        freq1 = (NxF32)atof(value);
      }
      else if ( stricmp(key,"frequency2") == 0 )
      {
        freq2 = (NxF32)atof(value);
      }
      else if ( stricmp(key,"frequency3") == 0 )
      {
        freq3 = (NxF32)atof(value);
      }
      else if ( stricmp(key,"frequency4") == 0 )
      {
        freq4 = (NxF32)atof(value);
      }
      else if ( stricmp(key,"amplitude1") == 0 )
      {
        amp1 = (NxF32)atof(value);
      }
      else if ( stricmp(key,"amplitude2") == 0 )
      {
        amp2 = (NxF32)atof(value);
      }
      else if ( stricmp(key,"amplitude3") == 0 )
      {
        amp3 = (NxF32)atof(value);
      }
      else if ( stricmp(key,"amplitude4") == 0 )
      {
        amp4 = (NxF32)atof(value);
      }
      else if ( stricmp(key,"seed1") == 0 )
      {
        seed1 = atoi(value);
      }
      else if ( stricmp(key,"seed2") == 0 )
      {
        seed2 = atoi(value);
      }
      else if ( stricmp(key,"seed3") == 0 )
      {
        seed3 = atoi(value);
      }
      else if ( stricmp(key,"seed4") == 0 )
      {
        seed4 = atoi(value);
      }
    }

    SetEquation1(type1);
    SetEquation2(type2);
    SetEquation3(type3);
    SetEquation4(type4);

    if ( octaves1 ) CreatePerlin(0,octaves1,freq1,amp1,seed1);
    if ( octaves2 ) CreatePerlin(1,octaves2,freq2,amp2,seed2);
    if ( octaves3 ) CreatePerlin(2,octaves3,freq3,amp3,seed3);
    if ( octaves4 ) CreatePerlin(3,octaves4,freq4,amp4,seed4);
  }


}

MyPerlin4::~MyPerlin4(void)
{
  delete mP1;
  delete mP2;
  delete mP3;
  delete mP4;
  delete mEnvelopes;
}

NxF32 MyPerlin4::get(NxF32 x,NxF32 y)
{
  NxF32 h1=0;
  NxF32 h2=0;
  NxF32 h3=0;
  NxF32 h4=0;

  if ( mP1 )
  {
    h1 = mP1->Get(x,y);
    h1 = mEnvelopes->Get(h1,mEq1);
  }

  if ( mP2 )
  {
    h2 = mP2->Get(x,y);
    h2 = mEnvelopes->Get(h2,mEq2);
  }

  if ( mP3 )
  {
    h3 = mP3->Get(x,y);
    h3 = mEnvelopes->Get(h3,mEq3);
  }

  if ( mP4 )
  {
    h4 = mP4->Get(x,y);
    h4 = mEnvelopes->Get(h4,mEq4);
  }

  return (h1+h2+h3+h4);
}

void MyPerlin4::CreatePerlin(NxI32 pno,NxI32 octaves,NxF32 freq,NxF32 amp,NxI32 seed)
{
  switch ( pno )
  {
    case 1:
      delete mP1;
      mP1 = MEMALLOC_NEW(Perlin)(octaves,freq,amp,seed);
      break;
    case 2:
      delete mP2;
      mP2 = MEMALLOC_NEW(Perlin)(octaves,freq,amp,seed);
      break;
    case 3:
      delete mP3;
      mP3 = MEMALLOC_NEW(Perlin)(octaves,freq,amp,seed);
      break;
    case 4:
      delete mP4;
      mP4 = MEMALLOC_NEW(Perlin)(octaves,freq,amp,seed);
      break;
  }
}


NxF32 MyPerlin4::Clamp(NxF32 v) const
{
  if ( v < mClampLow )
  {
    v = 0;
  }
  else
  {
    if ( v > mClampHigh )
    {
      v = 1;
    }
    else
    {
      v = (v-mClampLow)*mClampRecip;
    }
  }
  return v;
}

void MyPerlin4::DelPerlin(NxI32 pno)
{
  switch (pno)
  {
    case 1:
      delete mP1;
      mP1 = 0;
      break;
    case 2:
      delete mP2;
      mP2 = 0;
      break;
    case 3:
      delete mP3;
      mP3 = 0;
      break;
    case 4:
      delete mP4;
      mP4 = 0;
      break;
  }
}

}; // end of namespace

using namespace PERLIN4;

Perlin4 * createPerlin4(const char *options)
{
  MyPerlin4 *m = MEMALLOC_NEW(MyPerlin4)(options);
  return static_cast< Perlin4 * >(m);
}

void     releasePerlin4(Perlin4 *p)
{
  MyPerlin4 *m = static_cast< MyPerlin4 *>(p);
  delete m;
}
