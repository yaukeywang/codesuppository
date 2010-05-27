#ifndef FAST_IPC_H

#define FAST_IPC_H

// This class implements high speed inter-process communication using memory mapped files.
// The initial implementation just supports Windows but the code could be modified
// easily enough to support other operating systems.

class iFastIPC
{
public:
	enum MessageType
	{
		MT_NONE,		// indicates no incoming message.
		MT_ASCIIZ,		// the message is an zero byte terminated ASCII string.
		MT_DATA,		// The message is an arbitrary blob of data.
		MT_MULTI_PART,	// The message is part of a multi-part message.
		MT_APP = 100,	// Starting at 100 the application can use any message type value they wish to enumerate different message types.
	};

	enum ConnectionType
	{
		CT_CLIENT, 					// start up as a client, will succeed even if the server has not yet been found.
		CT_CLIENT_REQUIRE_SERVER,   // start up as a client, but only if the server already exists.
		CT_SERVER, 					// will start up as a server, will fail if an existing server is already open.
		CT_CLIENT_OR_SERVER,  		// connect as either a client or server, don't care who is created first.
		CT_LAST
	};

	enum ErrorCode
	{
		EC_OK,   					// no error.
		EC_FAIL,					// generic failure.
		EC_SERVER_ALREADY_EXISTS, 	// couldn't create a server, because the server already exists.
		EC_CLIENT_ALREADY_EXISTS, 	// couldn't create a client, because an existing client is already registered.
		EC_CLIENT_SERVER_ALREADY_EXISTS, // both the client and server channels are already used
		EC_SERVER_NOT_FOUND,		// client opened with a required server, which was not found.
		EC_BUFFER_MISSMATCH,      	// the reserved buffers for client/server do not match up.
		EC_MAPFILE_CREATE,          // failed to create the shared memory map file.
		EC_MAPFILE_VIEW,			// failed to map the memory view of he
		// communications errors.
		EC_SEND_DATA_EXCEEDS_MAX_BUFFER, // trying to send more data than can even fit in the sednd buffe.
		EC_SEND_DATA_TOO_LARGE,		// the data we tried to send exceeds the available room int the output ring buffer.
		EC_SEND_BUFFER_FULL,        // the send buffer is completely full.
		EC_SEND_FROM_WRONG_THREAD,  // Tried to do a send from a different thread
		EC_RECEIVE_FROM_WRONG_THREAD, // Tried to do a recieve from a different thread
		EC_NO_RECEIVE_PENDING,		// tried to acknowledge a receive but none was pending.
	};



	virtual ErrorCode 		sendMessage(bool bufferIfFull,const char *fmt,...) = 0; // send a message to the other process using a printf style format.
	virtual const char * 	receiveMessage(void) = 0;		// recieve an ASCII message from the other process.

	virtual void			pumpPendingSends(void) = 0;
	virtual void			pumpPendingReceives(void) = 0;

	// Send a blog of binary data to the other process.
	virtual ErrorCode		sendData(MessageType type,const void *data,unsigned int data_len,bool bufferIfFull) = 0;
	virtual MessageType 	receiveData(const void *&data,unsigned int &data_len) = 0;

	virtual	ErrorCode			receiveAcknowledge(void) = 0; // acknowledge that we have processed the incmoing message and can advance the read buffer.


	virtual bool isServer(void) const = 0; // returns true if we are opened as a server.

	virtual bool haveConnection(void) const = 0;

protected:
	virtual ~iFastIPC(void)
	{

	}

};

iFastIPC * createFastIPC(iFastIPC::ErrorCode &errorCode,		// error code return if creation fails
						 iFastIPC::ConnectionType connectionType=iFastIPC::CT_CLIENT_OR_SERVER, // how to establish the connection
						 const char *mappingObject="Global\\FastIPC",		// Name of communications channel
						 unsigned int serverRingBufferSize=32,	// buffer size for outgoing server messages.
						 unsigned int clientRingBufferSize=32, // buffer size for incoming client messages.
						 bool	useSendMutex=false,					// allow multiple threads to do sends, sets up a mutex if this is the case.
						 bool	useReceiveMutex=false,				// allows multiple threads to do receives, sets up a mutex if this is true.
						 bool	singleProcessCommunication=false,    // true if you are not communication between seperate processes, but instead seperate threads in a single process.
						 bool allowLongMessages=true);	// allows messages larger than the ring buffer size; will break them up into pieces.  Has performance penalties due to excessive memory copying.

void	  releaseFastIPC(iFastIPC *f);

#endif
