#ifndef HE_FOUNDATION_NXBitField
#define HE_FOUNDATION_NXBitField

#include "../snippets/He.h"

/**
 \brief BitField class, for efficient storage of flags, and sub-byte width enums.

 Bits can hypothetically be changed by changing the integer type of the flags member var.
 Previously this used to be a template class but it was too painful to maintain for what is probably zero benefit.
*/
class HeBitField
	{
	public:
	/**
	this could be bool, but an integer type is more efficient.  In any case, Flag variables
	should either be 1 or 0.
	*/
	typedef HeU32 IntType;	//currently I hardcode this to U32, it used to be templatized.  If we need more sizes later we can try to put the template back, its in the repository.

	typedef HeU32 Flag;
	typedef HeU32 Field;
	typedef HeU32 Mask;
	typedef HeU32 Shift;

	/**
	\brief Refrences an individual flag.
	*/

#pragma warning( push )
#pragma warning( disable : 4512 )
	class FlagRef
	{
		public:
		HE_INLINE FlagRef(HeBitField & x, HeU32 index) : bitField(x), bitIndex(index)	
    {	
    }
		HE_INLINE const FlagRef & operator=(Flag f)
		{
			bitField.setFlag(bitIndex, f);
			return *this;
		}
		HE_INLINE operator Flag()
		{
			return bitField.getFlag(bitIndex);
		}
		private:

		HeBitField & bitField;
		HeU32 bitIndex;
	};
#pragma warning( pop )


	//construction and assignment:
	//! default constructor leaves uninitialized.
	HE_INLINE HeBitField() { }
	HE_INLINE HeBitField(IntType);
	HE_INLINE HeBitField(const HeBitField &);
	HE_INLINE operator IntType()	{	return bitField;	}

	HE_INLINE const HeBitField & operator=(const HeBitField &);
	HE_INLINE const HeBitField & operator=(IntType);
	
	//!manipulating a single bit using a bit index.  The smallest bitIndex is 0.
	HE_INLINE void setFlag(HeU32 bitIndex, Flag value);
	HE_INLINE void raiseFlag(HeU32 bitIndex);
	HE_INLINE void lowerFlag(HeU32 bitIndex);
	HE_INLINE Flag getFlag(HeU32 bitIndex) const;


	//manipulating a single bit, using a single bit bit-mask
	HE_INLINE void setFlagMask(Mask mask, Flag value);
	HE_INLINE void raiseFlagMask(Mask mask);
	HE_INLINE void lowerFlagMask(Mask mask);

	HE_INLINE bool getFlagMask(Mask mask) const;


	/**
	manipulating a set of bits: 
	shift is the lsb of the field
	mask is the value of all the raised flags in the field.

	Example: if the bits 4,5,6 of the bit field are being used,
	then shift is 4 and mask is (1<<4)|(1<<5)|(1<<6) == 1110000b = 112

	\param shift
	\param mask
	*/
	HE_INLINE Field getField(Shift shift, Mask mask) const;
	HE_INLINE void setField(Shift shift, Mask mask, Field field);
	HE_INLINE void clearField(Mask mask);

	//! more operators
	HE_INLINE FlagRef operator[](HeU32 bitIndex);
	//!statics

	static HE_INLINE Mask rangeToDenseMask(HeU32 lowIndex, HeU32 highIndex);
	static HE_INLINE Shift maskToShift(Mask mask); 


	IntType bitField;
	};

typedef HeBitField HeBitField32;



HE_INLINE HeBitField::HeBitField(IntType v)
	{
	bitField = v;
	}


HE_INLINE HeBitField::HeBitField(const HeBitField & r)
	{
	bitField = r.bitField;
	}


HE_INLINE void HeBitField::setFlag(HeU32 bitIndex, Flag value)
	{
	if (value)
		raiseFlag(bitIndex);
	else
		lowerFlag(bitIndex);
	}


HE_INLINE void HeBitField::raiseFlag(HeU32 bitIndex)
	{
	bitField |= (1 << bitIndex);
	}


HE_INLINE void HeBitField::lowerFlag(HeU32 bitIndex)
	{
	bitField &= ~(1 << bitIndex);
	}


HE_INLINE HeBitField::Flag HeBitField::getFlag(HeU32 bitIndex) const
	{
	return (bitField & (1 << bitIndex)) >> bitIndex;
	}



HE_INLINE void HeBitField::setFlagMask(Mask mask, Flag value)
	{
	if (value)
		raiseFlagMask(mask);
	else
		lowerFlagMask(mask);
	}


HE_INLINE void HeBitField::raiseFlagMask(Mask mask)
	{
	bitField |= mask;
	}


HE_INLINE void HeBitField::lowerFlagMask(Mask mask)
	{
	bitField &= ~mask;
	}


//HE_INLINE HeBitField::Flag HeBitField::getFlagMask(Mask mask) const
HE_INLINE bool HeBitField::getFlagMask(Mask mask) const
	{
	return (bitField & mask) != 0;
	}


HE_INLINE HeBitField::Field HeBitField::getField(Shift shift, Mask mask) const
	{
	return ((bitField & mask) >>  shift);
	}


HE_INLINE void HeBitField::setField(Shift shift, Mask mask, Field field)
	{
	clearField(mask);
	bitField |= field << shift;
	}


HE_INLINE void  HeBitField::clearField(Mask mask)
	{
	bitField &= ~mask;
	}


HE_INLINE HeBitField::FlagRef HeBitField::operator[](HeU32 bitIndex)
	{
	return FlagRef(*this, bitIndex);
	}


HE_INLINE const HeBitField & HeBitField::operator=(const HeBitField &x)
	{
	bitField = x.bitField;
	return *this;
	}


HE_INLINE const HeBitField & HeBitField::operator=(IntType x)
	{
	bitField = x;
	return *this;
	}

#endif
