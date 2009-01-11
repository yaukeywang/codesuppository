#ifndef HE_STREAM
#define HE_STREAM

#include "../snippets/He.h"

class HeStream
	{
	public:
	/**
	\brief Empty constructor.
	*/

								HeStream()				{}
	/**
	\brief Virtual destructor.
	*/

	virtual						~HeStream()				{}

	// Loading API
	
	/**
	\brief Called to read a single unsigned byte(8 bits)

	\return Byte read.
	*/
	virtual		HeU8			readByte()								const	= 0;
	
	/**
	\brief Called to read a single unsigned word(16 bits)

	\return Word read.
	*/
	virtual		HeU16			readWord()								const	= 0;
	
	/**
	\brief Called to read a single unsigned dword(32 bits)

	\return DWord read.
	*/
	virtual		HeU32			readDword()								const	= 0;
	
	/**
	\brief Called to read a single precision floating point value(32 bits)

	\return Floating point value read.
	*/
	virtual		HeF32			readFloat()								const	= 0;
	
	/**
	\brief Called to read a double precision floating point value(64 bits)

	\return Floating point value read.
	*/
	virtual		HeF64			readDouble()							const	= 0;
	
	/**
	\brief Called to read a number of bytes.

	\param[out] buffer Buffer to read bytes into, must be at least size bytes in size.
	\param[in] size The size of the buffer in bytes.
	*/
	virtual		void			readBuffer(void* buffer, HeU32 size)	const	= 0;

	// Saving API
	
	/**
	\brief Called to write a single unsigned byte to the stream(8 bits).

	\param b Byte to store.
	\return Reference to the current HeStream object.
	*/
	virtual		HeStream&		storeByte(HeU8 b)								= 0;
	
	/**
	\brief Called to write a single unsigned word to the stream(16 bits).
	
	\param w World to store.
	\return Reference to the current HeStream object.
	*/
	virtual		HeStream&		storeWord(HeU16 w)								= 0;
	
	/**
	\brief Called to write a single unsigned dword to the stream(32 bits).

	\param d DWord to store.
	\return Reference to the current HeStream object.
	*/
	virtual		HeStream&		storeDword(HeU32 d)								= 0;

	/**
	\brief Called to write a single precision floating point value to the stream(32 bits).

	\param f floating point value to store.
	\return Reference to the current HeStream object.
	*/
	virtual		HeStream&		storeFloat(HeF32 f)								= 0;

	/**
	\brief Called to write a double precision floating point value to the stream(64 bits).

	\param f floating point value to store.
	\return Reference to the current HeStream object.
	*/
	virtual		HeStream&		storeDouble(HeF64 f)							= 0;

	/**
	\brief Called to write an array of bytes to the stream.

	\param[in] buffer Array of bytes, size bytes in size.
	\param[in] size Size, in bytes of buffer.
	\return Reference to the current HeStream object.
	*/
	virtual		HeStream&		storeBuffer(const void* buffer, HeU32 size)		= 0;


	/**
	\brief Store a signed byte(wrapper for the unsigned version).

	\param b Byte to store.
	\return Reference to the current HeStream object.
	*/
	HE_INLINE	HeStream&		storeByte(HeI8 b)		{ return storeByte(HeU8(b));	}

	/**
	\brief Store a signed word(wrapper for the unsigned version).

	\param w Word to store.
	\return Reference to the current HeStream object.
	*/
	HE_INLINE	HeStream&		storeWord(HeI16 w)		{ return storeWord(HeU16(w));	}

	/**
	\brief Store a signed dword(wrapper for the unsigned version).

	\param d DWord to store.
	\return Reference to the current HeStream object.
	*/
	HE_INLINE	HeStream&		storeDword(HeI32 d)		{ return storeDword(HeU32(d));	}
	};

 /** @} */
#endif
