#ifndef FIXED_POINT_H

#define FIXED_POINT_H

#include <float.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "common/snippets/HeSimpleTypes.h"

#define PRECISION_BITS 21

class Fixed32
{
public:

  #define PVALUE (1<<PRECISION_BITS)

  inline Fixed32(void)
  {
    mValue = 0;

  }

  inline Fixed32(const Fixed32 &s)
  {
    mValue = s.mValue;

  }

  inline Fixed32(HeI32 value)
  {
    mValue = (HeI32)(value<<PRECISION_BITS);

  }

  inline Fixed32(HeU32 value)
  {
    mValue = (HeI32)(value<<PRECISION_BITS);

  }

  inline Fixed32(HeF32 value)
  {
    mValue = (HeI32)(value*(HeF32)PVALUE);

  }

  inline Fixed32(HeF64 value)
  {
    mValue = (HeI32)(value*(HeF64)PVALUE);

  }

  inline Fixed32(const char *value)
  {
    HeF64 v = atof(value);
    mValue = (HeI32)(v*(HeF64)PVALUE);

  }

	inline Fixed32 & operator=(const char *value)
	{
    HeF64 v = atof(value);
    mValue = (HeI32)(v*(HeF64)PVALUE);

  	return *this;
	}


	inline Fixed32 & operator=(HeI32 value)
	{
    mValue = (HeI32)(value<<PRECISION_BITS);

  	return *this;
	}


	inline Fixed32 & operator=(HeU32 value)
	{
    mValue = (HeI32)(value<<PRECISION_BITS);

  	return *this;
	}

	inline Fixed32 & operator=(HeF32 value)
	{
    mValue = (HeI32)(value*(HeF32)PVALUE);
  	return *this;
	}

	inline Fixed32 & operator=(HeF64 value)
	{
    mValue = (HeI32)(value*(HeF64)PVALUE);
  	return *this;
	}


  void toString(char *dest) const
  {
    HeF64 v = (HeF64)mValue / (HeF64)PVALUE;
    sprintf(dest,"%0.24f",v);
  }

  inline HeF64 toDouble(void) const
  {
    HeF64 v = (HeF64)mValue / (HeF64)PVALUE;
    return v;
  }

  inline HeF32 toFloat(void) const
  {
    HeF32 v = (HeF32)mValue / (HeF32)PVALUE;
    return v;
  }

  inline HeI32 toInt(void) const { return mValue>>PRECISION_BITS; }

	inline bool operator<(const Fixed32 & ss2) const {  return mValue < ss2.mValue; }
	inline bool operator==(const Fixed32 & ss2) const	{ return mValue == ss2.mValue; }
	inline bool operator>(const Fixed32 & ss2) const	{ return mValue > ss2.mValue;	}
	inline bool operator>=(const Fixed32 & ss2) const	{ return mValue >= ss2.mValue;  }
	inline bool operator<=(const Fixed32 & ss2) const	{ return mValue <= ss2.mValue; }
	inline bool operator!=(const Fixed32 & ss2) const { return mValue != ss2.mValue; };
  inline void changeSign(void) { mValue = -mValue;  }
	inline Fixed32 operator-() const	{  Fixed32 ret(*this); ret.changeSign();  return ret;	}
  inline void Sub(const Fixed32 &ss2) { mValue-=ss2.mValue;  }
	inline Fixed32 operator-(const Fixed32 & ss2) const { Fixed32 ret(*this);	ret.Sub(ss2);	return ret;	}
	inline Fixed32 & operator-=(const Fixed32 & ss2)	{	Sub(ss2);	return *this;	}
  inline void Add(const Fixed32 &ss2) { mValue+=ss2.mValue;  };

  inline void getStr(HeI32 a,char *dest)
  {
    sprintf(dest,"%0.9f", (HeF64)a / (HeF64) PVALUE );
  }

  inline void debug(HeI32 a,HeI32 b,HeI32 c,const char *symbol)
  {
    char scratch[512];
    char s1[256];
    char s2[256];
    char s3[256];
    getStr(a,s1);
    getStr(b,s2);
    getStr(c,s3);
    sprintf(scratch,"%s %s %s = %s\r\n", s1, symbol, s2, s3 );
    printf("%s", scratch );
  }

  inline void Mul(const Fixed32 &ss2)
  {
    HeI32 v1 = mValue;
    HeI32 v2 = ss2.mValue;

    __asm
    {

//      push edx
//      push eax
//      push ecx

      mov  cl,PRECISION_BITS
      mov  eax,[v1]
      imul  [v2]
      shrd eax,edx,cl
      mov  [v1],eax

//      pop  ecx
//      pop  eax
//      pop  edx

    };
    mValue = v1;
  }

  inline void Div(const Fixed32 &ss2)
  {
    HeI32 v1 = mValue;
    HeI32 v2 = ss2.mValue;

    __asm
    {

      push edx
      push eax
      push ecx

      mov  cl,PRECISION_BITS
      mov  eax,[v1]
      shld edx,eax,cl
      shl  eax,cl
      idiv [v2]
      mov  [v1],eax

      pop  ecx
      pop  eax
      pop  edx

    };

     mValue = v1;
  }


	inline Fixed32 operator+(Fixed32 & ss2) const	{  Fixed32 temp(*this);	temp.Add(ss2); 	return temp;	}
	inline Fixed32 & operator+=(const Fixed32 &ss2) {	Add(ss2);	return *this;	}
	inline Fixed32 operator*(const Fixed32 & ss2) const	{  Fixed32 temp(*this);	temp.Mul(ss2); 	return temp;	}
	inline Fixed32 & operator*=(const Fixed32 & ss2)	{	Mul(ss2);	return *this;	}
	inline Fixed32 operator/(const Fixed32 & ss2) const	{	Fixed32 temp(*this);	temp.Div(ss2);	return temp;	}
	inline Fixed32 & operator/=(const Fixed32 & ss2)	{	Div(ss2);	return *this;	}

  HeI32  mValue;
};

#endif
