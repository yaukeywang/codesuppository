#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "fastipc.h"
#include "ThreadConfig.h"


#pragma warning(disable:4996 4100)

class FastIPC : public iFastIPC
{
private: // internal data structures and classes used to handle the ring buffers.
// this is part of the memory mapped file
struct FastIPCRingBuffer
{

	ErrorCode canSend(unsigned int len)
	{
		// the room available is
		ErrorCode ret = EC_OK;

		if ( len > mBufferSize ) // ok the packet will fit inside the ring buffer..
		{
			ret = EC_SEND_DATA_EXCEEDS_MAX_BUFFER;
		}
		else if ( getWriteAddress() > getReadAddress() )
		{
			unsigned int avail = (mBufferSize - getWriteAddress())+getReadAddress();
			if ( len > avail )
			{
				ret = EC_SEND_DATA_TOO_LARGE;
			}
		}
		else if ( getReadAddress() > getWriteAddress() )
		{
			unsigned int avail = getReadAddress()-getWriteAddress();
			if ( len > avail )
			{
				ret = EC_SEND_DATA_TOO_LARGE;
			}
		}

		return ret;
	}

	// Write data into this ring buffer.  If it doesn't wrap, just copy it.
	// if it wraps, then copy the first part up to the end, and roll over the rest to the beginning of the ring buffer.
	void writeData(const void *_data,unsigned int dataLen,char *baseAddress,unsigned int &writeAddress)
	{

		assert( canSend(dataLen) == EC_OK );

		const char *data = (const char *)_data;
		char *dest = &baseAddress[writeAddress];

		if ( (writeAddress+dataLen) < mBufferSize )
		{
			memcpy(dest,data,dataLen);
			if ( (writeAddress+dataLen) == mBufferSize )
			{
				writeAddress = 0;
			}
			else
			{
				writeAddress+=dataLen;
			}
		}
		else
		{
			unsigned int part1Len = (mBufferSize-writeAddress);
			unsigned int part2Len = dataLen-part1Len;
			if ( part1Len )
			{
    			memcpy(dest,data,part1Len);
    			data+=part1Len;
    		}
    		if ( part2Len )
    		{
    			dest = baseAddress;
    			memcpy(dest,data,part2Len);
    		}
			writeAddress = part2Len;
		}
	}

	void readData(unsigned int &readAddress,void *_dest,unsigned int dataLen,const char *baseAddress)
	{
		char *dest = (char *)_dest;
		const char *src = &baseAddress[readAddress];
		if ( (readAddress+dataLen) < mBufferSize )
		{
			memcpy(dest,src,dataLen);
			readAddress+=dataLen; // advance the read address
		}
		else
		{
			unsigned int part1Len = (mBufferSize-readAddress);
			unsigned int part2Len = dataLen-part1Len;
			if ( part1Len )
			{
				memcpy(dest,src,part1Len);
				dest+=part1Len;
			}
			if ( part2Len )
			{
				src = baseAddress;
				memcpy(dest,src,part2Len);
			}
			readAddress = part2Len;	// advance the read address
		}
	}

	// return pointer if this read buffer is contigous and, if so, advance the read address value.
	const void * isContiguous(unsigned int &readAddress,unsigned int dataLen,const char *baseAddress)
	{
		const void *ret = 0;
		if ( (readAddress+dataLen) <= mBufferSize )
		{
			ret = &baseAddress[readAddress];
			readAddress+=dataLen; // advance the read address
		}
		return ret;
	}

	MessageType receiveData(const void *&data,unsigned int &data_len,const char *baseAddress,char *dataBuffer)
	{
		MessageType ret = MT_NONE;
		if ( getReadAddress() != getWriteAddress() )
		{
			unsigned int readAddress = getReadAddress();
			unsigned int packetLen;
			readData(readAddress,&packetLen,sizeof(packetLen),baseAddress);
			readData(readAddress,&ret,sizeof(MessageType),baseAddress);
			data_len = packetLen - (sizeof(unsigned int)+sizeof(MessageType));
			data = isContiguous(readAddress,data_len,baseAddress);
			if ( !data )
			{
				readData(readAddress,dataBuffer,data_len,baseAddress);
				data = dataBuffer;
			}
		}
		return ret;
	}

	ErrorCode	receiveAcknowledge(const char *baseAddress)
	{
		ErrorCode ret = EC_NO_RECEIVE_PENDING;
		assert(getReadAddress()!=getWriteAddress());
		if ( getReadAddress() != getWriteAddress() )
		{
			unsigned int readAddress = getReadAddress();
			unsigned int packetLen;
			readData(readAddress,&packetLen,sizeof(packetLen),baseAddress);
			if ( (packetLen+getReadAddress()) < mBufferSize )
			{
				setReadAddress( getReadAddress()+packetLen );
			}
			else
			{
				setReadAddress(packetLen - (mBufferSize-getReadAddress())); // new read address is the remainder..
			}
			//printf("Read: %d Write: %d\r\n", getReadAddress(), getWriteAddress() );
			ret = EC_OK;
		}
		return ret;
	}

	unsigned int getReadAddress(void) const
	{
		assert( mReadAddress < mBufferSize );
		return mReadAddress;
	}

	void setReadAddress(unsigned int adr)
	{
		//printf("Read Address: %d : %d\r\n", mReadAddress, adr );
		assert( mReadAddress < mBufferSize );
		mReadAddress = adr;
		assert( mReadAddress < mBufferSize );
	}

	unsigned int getWriteAddress(void) const
	{
		assert( mWriteAddress < mBufferSize );
		return mWriteAddress;
	}

	void setWriteAddress(unsigned int adr)
	{
		//printf("Write Address: %d %d\r\n", mWriteAddress, adr );
		assert( mWriteAddress < mBufferSize );
		mWriteAddress = adr;
		assert( mWriteAddress < mBufferSize );
	}

	unsigned int	mBufferSize;
	unsigned int	mWriteAddress;
	unsigned int	mReadAddress;
};

// this is part of the memory mapped file.
struct FastIPCHeader
{
	bool				mIsServer;
	bool				mIsClient;
	FastIPCRingBuffer	mServerRingBuffer;
	FastIPCRingBuffer	mClientRingBuffer;
};

// this class handles reading and writing to a ring buffer in the memory mapped file.
class RingBuffer
{
public:
	RingBuffer(void)
	{
		mIsWriter = false;
		mBaseAddress = NULL;
		mRingBuffer  = NULL;
		mReadBuffer = NULL;
	}
	~RingBuffer(void)
	{
		::free(mReadBuffer);
	}

	void init(bool isWriter,FastIPCRingBuffer *rb,char *baseAddress)
	{
		mIsWriter = isWriter;
		mRingBuffer = rb;
		mBaseAddress = baseAddress;
		mReadBuffer = (char *)::malloc( rb->mBufferSize );
	}


	ErrorCode		 sendData(MessageType type,const void *data,unsigned int dataLen)
	{
		ErrorCode ret = EC_FAIL;

		assert(mIsWriter);

		unsigned int packetLen = sizeof(unsigned int)+sizeof(MessageType)+dataLen;
		ret = mRingBuffer->canSend(packetLen);
		if ( ret == EC_OK )
		{
			// We store into the ring buffer the following data per packet.
			// <unsigned int> total size of the packet including all components.
			// <unsigned int> The message type enum
			// <packetData> The raw packet data.
			unsigned int writeAddress = mRingBuffer->getWriteAddress();
			mRingBuffer->writeData(&packetLen,sizeof(packetLen),mBaseAddress,writeAddress);
			mRingBuffer->writeData(&type,sizeof(MessageType),mBaseAddress,writeAddress);
			mRingBuffer->writeData(data,dataLen,mBaseAddress,writeAddress);
			mRingBuffer->setWriteAddress(writeAddress);
		}

		return ret;
	}

	MessageType receiveData(const void *&data,unsigned int &data_len)
	{
		MessageType ret = MT_NONE;

		assert(!mIsWriter);
		ret = mRingBuffer->receiveData(data,data_len,mBaseAddress,mReadBuffer);

		return ret;
	}

	ErrorCode			receiveAcknowledge(void) // acknowledge that we have processed the incmoing message and can advance the read buffer.
	{
		assert(!mIsWriter);
		return mRingBuffer->receiveAcknowledge(mBaseAddress);
	}


	bool				mIsWriter;
	char				*mBaseAddress; // base address of the ring buffer.
	FastIPCRingBuffer	*mRingBuffer;
	char				*mReadBuffer;  // a scratch read buffer..only if needed...
};

public:
	FastIPC(const char *mappingObject,
			unsigned int serverRingBufferSize,
			unsigned int clientRingBufferSize,
			iFastIPC::ConnectionType connectionType,
			iFastIPC::ErrorCode &errorCode,
			bool useSendMutex,
			bool useReceiveMutex,
			bool singleProcessCommunications,
			bool allowLongMessages)
	{
		mSendThreadID = 0;
		mReceiveThreadID = 0;
		mIsSingleProcess = singleProcessCommunications;
		errorCode = EC_MAPFILE_CREATE;
		unsigned int mapSize = clientRingBufferSize+serverRingBufferSize+sizeof(FastIPCHeader);
		mIsServer = false;
		mHeader = NULL;
		mSendMutex = useSendMutex ? THREAD_CONFIG::tc_createThreadMutex() : NULL;
		mReceiveMutex = useReceiveMutex ? THREAD_CONFIG::tc_createThreadMutex() : NULL;
		if ( mIsSingleProcess )
		{
			mHeader = (FastIPCHeader *)::malloc(mapSize);
		}
		else
		{
			mMemoryMappedFile = THREAD_CONFIG::tc_createMemoryMappedFile(mappingObject,mapSize);
			if ( mMemoryMappedFile )
			{
				mHeader = (FastIPCHeader *)mMemoryMappedFile->getBaseAddress();
			}
    	}

		if ( mHeader )
		{
			char *serverBaseAddress = (char *)(mHeader+1);
			char *clientBaseAddress = serverBaseAddress+serverRingBufferSize;

			errorCode = EC_OK;
			switch ( connectionType )
			{
				case CT_CLIENT: 					// start up as a client, will succeed even if the server has not yet been found.
				case CT_CLIENT_REQUIRE_SERVER:   // start up as a client, but only if the server already exists.
					if ( mHeader->mIsClient ) // if there is already a client, we can't create another one.
					{
						errorCode = EC_CLIENT_ALREADY_EXISTS;
					}
					else if ( mHeader->mIsServer) // if there is already as server, make sure the buffer sizes match
					{
						if ( mHeader->mClientRingBuffer.mBufferSize != clientRingBufferSize ||
							 mHeader->mServerRingBuffer.mBufferSize != serverRingBufferSize )
						{
							errorCode = EC_BUFFER_MISSMATCH;
						}
					}
					else if ( connectionType == CT_CLIENT_REQUIRE_SERVER ) // if we require a server to already be present, then we fail.
					{
						errorCode = EC_SERVER_NOT_FOUND;
					}
					// if everything passes, set up the client
					if ( errorCode == EC_OK )
					{
						mHeader->mIsClient = true; // snarf the fact that we are the client.
						mHeader->mClientRingBuffer.mBufferSize = clientRingBufferSize;
						mServerRingBuffer.init(false,&mHeader->mServerRingBuffer,serverBaseAddress);
						mClientRingBuffer.init(true,&mHeader->mClientRingBuffer,clientBaseAddress);
						mReader = &mServerRingBuffer;
						mWriter = &mClientRingBuffer;
						if ( !mHeader->mIsServer )
						{
							mHeader->mServerRingBuffer.mBufferSize = serverRingBufferSize;
						}
					}
					break;
				case CT_SERVER: 					// will start up as a server, will fail if an existing server is already open.
					if ( mHeader->mIsServer ) // if there is already a client, we can't create another one.
					{
						errorCode = EC_SERVER_ALREADY_EXISTS;
					}
					else if ( mHeader->mIsClient) // if there is already as client, make sure the buffer sizes match
					{
						if ( mHeader->mClientRingBuffer.mBufferSize != clientRingBufferSize ||
							 mHeader->mServerRingBuffer.mBufferSize != serverRingBufferSize )
						{
							errorCode = EC_BUFFER_MISSMATCH;
						}
					}
					if ( errorCode == EC_OK )
					{
						mHeader->mIsServer = true; // snarf the fact that we are the client.
						mHeader->mClientRingBuffer.mBufferSize = clientRingBufferSize;
						mServerRingBuffer.init(true,&mHeader->mServerRingBuffer,serverBaseAddress);
						mClientRingBuffer.init(false,&mHeader->mClientRingBuffer,clientBaseAddress);
						mWriter = &mServerRingBuffer;
						mReader = &mClientRingBuffer;
						if ( !mHeader->mIsClient )
						{
							mHeader->mServerRingBuffer.mBufferSize = serverRingBufferSize;
						}
						mIsServer = true;
					}
					break;
				case CT_CLIENT_OR_SERVER:  		// connect as either a client or server, don't care who is created first.
					if ( mHeader->mIsClient && mHeader->mIsServer ) // if there is already both an active client and server, we fail
					{
						errorCode = EC_CLIENT_SERVER_ALREADY_EXISTS;
					}
					else if ( mHeader->mIsServer || mHeader->mIsClient )
					{

						if ( mHeader->mClientRingBuffer.mBufferSize != clientRingBufferSize ||
							 mHeader->mServerRingBuffer.mBufferSize != serverRingBufferSize )
						{
							errorCode = EC_BUFFER_MISSMATCH;
						}
					}
					if ( errorCode == EC_OK )
					{
						if ( mHeader->mIsServer )
						{
  								mHeader->mIsClient = true; // set the fact that we are the client.
      							mWriter = &mServerRingBuffer;
      							mReader = &mClientRingBuffer;
  								mHeader->mServerRingBuffer.mBufferSize = clientRingBufferSize;
						}
						else
						{
  								mHeader->mIsServer = true; // snarf the fact that we are the client.
      							mWriter = &mServerRingBuffer;
      							mReader = &mClientRingBuffer;
  								mHeader->mClientRingBuffer.mBufferSize = clientRingBufferSize;
  								mIsServer = true;
  							}
  							mHeader->mClientRingBuffer.mBufferSize = clientRingBufferSize;
  							mServerRingBuffer.init(false,&mHeader->mServerRingBuffer,serverBaseAddress);
  							mClientRingBuffer.init(true,&mHeader->mClientRingBuffer,clientBaseAddress);
					}
					break;

			}
		}
		else
		{
			errorCode = EC_MAPFILE_VIEW;
		}
	}

	~FastIPC(void)
	{
		if ( mHeader )
		{
			if ( mIsServer ) // on clean exit, release the server buffers..
			{
				mHeader->mIsServer = false;		// kills the server.
			}
			else
			{
				mHeader->mIsClient = false;	// kills the client
			}
			if ( mIsSingleProcess )
			{
				::free(mHeader);
			}
			else if ( mMemoryMappedFile )
			{
				THREAD_CONFIG::tc_releaseMemoryMappedFile(mMemoryMappedFile);
			}
		}
	}

	virtual ErrorCode 		 sendMessage(bool bufferIfFull,const char *fmt,...)  // send a message to the other process using a printf style format.
	{
   		char wbuff[8192];
   		wbuff[8191] = 0;
   		_vsnprintf(wbuff,8191, fmt, (char *)(&fmt+1));
   		unsigned int len = (unsigned int )strlen(wbuff);
   		return sendData(MT_ASCIIZ,wbuff,len+1,bufferIfFull);
	}

	virtual const char * receiveMessage(void) 		// recieve an ASCII message from the other process.
	{
		const char *ret = NULL;

		const void *data;
		unsigned int data_len;
		MessageType type = receiveData(data,data_len);
		if ( type == MT_ASCIIZ )
		{
			ret = (const char *)data;
		}

		return ret;
	}

	// Send a blob of binary data to the other process.
	virtual ErrorCode		 sendData(MessageType type,const void *data,unsigned int data_len,bool bufferIfFull)
	{
		ErrorCode ret = EC_OK;

		if ( mSendMutex )
		{
			mSendMutex->lock();
		}
		else if ( mSendThreadID == 0 )
		{
			mSendThreadID = THREAD_CONFIG::tc_getCurrentThreadId();
		}
		else if ( mSendThreadID != THREAD_CONFIG::tc_getCurrentThreadId() )
		{
			assert(0);
			ret =  EC_SEND_FROM_WRONG_THREAD;
		}
		if ( ret == EC_OK )
		{
    		assert(mWriter);
    		if ( mWriter )
    		{
    			ret = mWriter->sendData(type,data,data_len);
    		}
    	}
    	if ( mSendMutex )
    	{
    		mSendMutex->unlock();
    	}

		return ret;
	}

	virtual MessageType receiveData(const void *&data,unsigned int &data_len)
	{
		MessageType ret = MT_NONE;
		data = NULL;
		data_len = 0;

		if ( mReceiveMutex )
		{
			mReceiveMutex->lock();
		}
		else if ( mReceiveThreadID == 0 )
		{
			mReceiveThreadID = THREAD_CONFIG::tc_getCurrentThreadId();
		}
		else if ( mReceiveThreadID != THREAD_CONFIG::tc_getCurrentThreadId() )
		{
			assert(0);
		}
   		assert(mReader);
   		if ( mReader )
   		{
   			ret = mReader->receiveData(data,data_len);
    	}

    	if ( mReceiveMutex )
    	{
    		mReceiveMutex->unlock();
    	}

		return ret;
	}

	virtual bool isServer(void) const
	{
		return mIsServer;
	}

	virtual	ErrorCode			receiveAcknowledge(void) // acknowledge that we have processed the incmoing message and can advance the read buffer.
	{
		ErrorCode ret = EC_OK;


		if ( mReceiveMutex )
		{
			mReceiveMutex->lock();
		}
		else if ( mReceiveThreadID == 0 )
		{
			mReceiveThreadID = THREAD_CONFIG::tc_getCurrentThreadId();
		}
		else if ( mReceiveThreadID != THREAD_CONFIG::tc_getCurrentThreadId() )
		{
			assert(0);
			ret = EC_RECEIVE_FROM_WRONG_THREAD;
		}
		if ( ret == EC_OK )
		{
    		assert(mReader);
    		if ( mReader )
    		{
    			ret = mReader->receiveAcknowledge();
    		}
    	}


		return ret;
	}

	virtual void			pumpPendingSends(void)
	{
	}

	virtual void			pumpPendingReceives(void)
	{
	}

	virtual bool haveConnection(void) const 
	{
		bool ret = false;
		if ( mHeader )
		{
			ret = mIsServer ? mHeader->mIsClient : mHeader->mIsServer;
		}
		return ret;
	}

private:
	unsigned int	mSendThreadID;
	unsigned int	mReceiveThreadID;
	bool				mIsServer;
	bool				mIsSingleProcess;
	FastIPCHeader		*mHeader;
	unsigned int		mRingBufferSize;
	RingBuffer			mServerRingBuffer;
	RingBuffer			mClientRingBuffer;
	RingBuffer			*mWriter;
	RingBuffer			*mReader;
	THREAD_CONFIG::ThreadMutex	*mReceiveMutex;
	THREAD_CONFIG::ThreadMutex	*mSendMutex;
	THREAD_CONFIG::MemoryMappedFile *mMemoryMappedFile;
};



iFastIPC * createFastIPC(iFastIPC::ErrorCode &errorCode,
						iFastIPC::ConnectionType connectionType,
						 const char *mappingObject,
						unsigned int serverRingBufferSize,
						unsigned int clientRingBufferSize,
						bool useSendMutex,
						bool useReceiveMutex,
						bool	singleProcessCommunication,    // true if you are not communication between seperate processes, but instead seperate threads in a single process.
						bool allowLongMessages)
{
	FastIPC *ret = new FastIPC(mappingObject,serverRingBufferSize,clientRingBufferSize,connectionType,errorCode,useSendMutex,useReceiveMutex,singleProcessCommunication,allowLongMessages);
	if ( errorCode != iFastIPC::EC_OK )
	{
		delete ret;
		ret = NULL;
	}
	return static_cast< iFastIPC * >(ret);
}

void	  releaseFastIPC(iFastIPC *f)
{
	FastIPC *fip = static_cast< FastIPC *>(f);
	delete fip;
}
