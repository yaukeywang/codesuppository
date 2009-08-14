// @ZBS {
//		*MODULE_NAME DirectShow Video Capture without the Horrible Grief
//		*MASTER_FILE 1
//		+DESCRIPTION {
//			A simple C style interface around the abysmal DirectShow video capture API
//		}
//		+EXAMPLE {
//		}
//		*PORTABILITY win32
//		*SDK_DEPENDS dx8
//		*SDK_LIBS_DEBUG
//		*SDK_LIBS_RELEASE
//		*WIN32_LIBS_DEBUG strmiids.lib 
//		*WIN32_LIBS_RELEASE strmiids.lib 
//		*REQUIRED_FILES zvidcap.cpp zvidcap.h
//		*VERSION 2.0
//		+HISTORY {
//			2.0 Revised to eliminate dependency on Microsoft's deprecated APIs.
//				Thanks to Jon Blow for the new improved way.
//		}
//		+TODO {
//		}
//		*SELF_TEST yes console
//		*PUBLISH yes
// }
// OPERATING SYSTEM specific includes:
//#include <objbase.h>
//#include <initguid.h>

//#define INITGUID
//#include "windows.h"

#pragma warning(disable:4996)

#include "strmif.h"
#include "vfwmsgs.h"
#include "control.h"
#include "uuids.h"
#include "Amvideo.h"
#include "olectl.h"
#include "initguid.h"

#include "UserMemAlloc.h"

DEFINE_GUID( CLSID_Vidcap_Filter, 0x17d93618, 0xe0a3, 0x4dde, 0x9b, 0x64, 0xea, 0x50, 0xa6, 0xfe, 0xa, 0x31);
	// The GUID for our DirectShow filter which grabs the frame
// SDK includes:
// STDLIB includes:
#include "assert.h"
#include "stdio.h"
// MODULE includes:
#include "zvidcap.h"
// ZBSLIB includes:


#define vidcapMAXDEVICES (16)
char *zVidcapDevices[vidcapMAXDEVICES] = {0,};
	// A static list of all the devices as found on the last enumeration

NxI32 zVidcapNumDevices = 0;
	// This is a global list by name of all the vidcap input devices
	// This global list is returned by zVidcapGetDevices

char *zVidcapLastDevice = NULL;
	// The last device to get started

struct VidcapConnection {
	IGraphBuilder *graphBuilderIFACE;
	ICaptureGraphBuilder2 *captureGraphBuilderIFACE;
	IMediaControl *mediaControlIFACE;
	class ZVidcapFilter *filter;
	IBaseFilter *cameraFilter;
	IMediaSeeking *seekIFACE;
};
VidcapConnection vidcapConns[vidcapMAXDEVICES] = {0,};
	// Holds onto all the DirectShow interfaces we need to accesss

CRITICAL_SECTION vidcapCriticalSection;
	// A critical section is used to mutex locks on the NxF64 buffering system


// VidCap Filter and Pin overloads.
// These are custom DirectShow filters which, although ridiculously complicated
// due to the complex DirectShow paradigm, do nothing more than
// copy memory from the input pin into a system buffer!
//------------------------------------------------------------------------------------------

class ZVidcapPin : public IMemInputPin, public IPin {
	class ZVidcapFilter *receiver;
  public:
	IPin *connected_to;
	AM_MEDIA_TYPE my_media_type;
	IMemAllocator *allocator;

	ZVidcapPin(ZVidcapFilter *receiver);
	STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	HRESULT BreakConnect();
	HRESULT SetMediaType(const AM_MEDIA_TYPE *media_type);
	HRESULT CheckMediaType(const AM_MEDIA_TYPE *media_type);
	HRESULT Active();
	HRESULT Inactive();
	STDMETHODIMP QueryId(LPWSTR *Id);
	STDMETHODIMP Receive(IMediaSample *media_sample);
	STDMETHODIMP Connect(IPin * pReceivePin, const AM_MEDIA_TYPE *media_type);
	STDMETHODIMP ReceiveConnection(IPin *connector, const AM_MEDIA_TYPE *media_type);
	STDMETHODIMP Disconnect();
	STDMETHODIMP ConnectedTo(IPin **pin_return);
	STDMETHODIMP ConnectionMediaType(AM_MEDIA_TYPE *media_type_return);
	STDMETHODIMP QueryPinInfo(PIN_INFO *pin_info_return);
	STDMETHODIMP QueryDirection(PIN_DIRECTION *pin_direction_return);
	STDMETHODIMP EnumMediaTypes(IEnumMediaTypes **enumerator_return);
	STDMETHODIMP QueryAccept(const AM_MEDIA_TYPE *media_type);
	STDMETHODIMP QueryInternalConnections(IPin **pins_return, ULONG *num_pins_return);
	STDMETHODIMP EndOfStream();
	STDMETHODIMP NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, NxF64 dRate);
	STDMETHODIMP Notify(IBaseFilter * pSender, Quality q);
	STDMETHODIMP SetSink(IQualityControl * piqc);
	STDMETHODIMP GetAllocator(IMemAllocator ** ppAllocator);
	STDMETHODIMP NotifyAllocator(IMemAllocator * pAllocator, BOOL bReadOnly);
	STDMETHODIMP ReceiveMultiple (IMediaSample **pSamples, long nSamples, long *nSamplesProcessed);
	STDMETHODIMP ReceiveCanBlock();
	STDMETHODIMP BeginFlush();
	STDMETHODIMP EndFlush();
	STDMETHODIMP GetAllocatorRequirements(ALLOCATOR_PROPERTIES *pProps);
	BOOL IsConnected();
	IPin *GetConnected();
};

class ZVidcapFilter : public IBaseFilter {
  public:
	ZVidcapFilter();
	~ZVidcapFilter();

  public:
	NxI32 maxWidth, maxHeight;
	NxI32 width, height, stride, depth;
	char *incoming_image_buffers[2];
	NxI32 locks[2];
	NxI32 frameNums[2];
	NxI32 readLock;
	NxI32 frameTimings[16];
	NxU32 lastTime;
	const IUnknown *m_pUnknown;
	ZVidcapPin *input_pin;
	IFilterGraph *filter_graph;
	WCHAR *name;
	LONG reference_count;

	NxI32 getAvgFrameTimings();
	HRESULT CheckMediaType(const AM_MEDIA_TYPE *media_type);
	HRESULT SetMediaType(const AM_MEDIA_TYPE *media_type);
	virtual HRESULT Receive(IMediaSample *incoming_sample);
	virtual HRESULT BreakConnect();
	STDMETHODIMP GetClassID(CLSID *class_id_return);
	STDMETHODIMP GetState(DWORD milliseconds, FILTER_STATE *state_return);
	STDMETHODIMP SetSyncSource(IReferenceClock *clock_return);
	STDMETHODIMP GetSyncSource(IReferenceClock **clock_return);
	STDMETHODIMP Stop();
	STDMETHODIMP Pause();
	STDMETHODIMP Run(REFERENCE_TIME tStart);
	STDMETHODIMP EnumPins(IEnumPins **enumerator_return);
	STDMETHODIMP FindPin(LPCWSTR name, IPin **pin_return);
	STDMETHODIMP QueryFilterInfo(FILTER_INFO * pInfo);
	STDMETHODIMP JoinFilterGraph(IFilterGraph *graph, LPCWSTR desired_name);
	STDMETHODIMP QueryVendorInfo(LPWSTR *vendor_info_return);
	STDMETHODIMP Register();
	STDMETHODIMP Unregister();
	STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
};

inline HRESULT vidcapGetInterface(IUnknown *unknown, void **result) {
	*result = unknown;
	unknown->AddRef();
	return NOERROR;
}

class ZVidcapMediaTypeEnumerator : public IEnumMediaTypes {
	NxI32 index;
	ZVidcapPin *my_pin;
	LONG version;
	LONG reference_count;
  public:
	ZVidcapMediaTypeEnumerator(ZVidcapPin *my_pin, ZVidcapMediaTypeEnumerator *enumerator_to_copy);
	virtual ~ZVidcapMediaTypeEnumerator();
	STDMETHODIMP QueryInterface(REFIID riid, void **result);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP Next(ULONG num_media_types, AM_MEDIA_TYPE **media_types_return, ULONG *num_fetched_return);
	STDMETHODIMP Skip(ULONG numMediaTypes);
	STDMETHODIMP Reset();
	STDMETHODIMP Clone(IEnumMediaTypes **enumerator_return);
};

ZVidcapPin::ZVidcapPin(ZVidcapFilter *_receiver) {
	receiver = _receiver;
	connected_to = NULL;
	allocator = NULL;
}

STDMETHODIMP ZVidcapPin::QueryInterface(REFIID riid, void **result) {
	if (riid == IID_IMemInputPin) {
		return vidcapGetInterface((IMemInputPin *)this, result);
	}

	if (riid == IID_IPin) {
		return vidcapGetInterface((IPin *)this, result);
	}

	if (riid == IID_IUnknown) {
		return vidcapGetInterface((IUnknown *)(IPin *)this, result);
	}

	*result = NULL;
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) ZVidcapPin::AddRef() {  
	return receiver->AddRef();	
};								  

STDMETHODIMP_(ULONG) ZVidcapPin::Release() {  
	return receiver->Release(); 
};


STDMETHODIMP ZVidcapPin::Receive(IMediaSample *pSample) {
	HRESULT hr = receiver->Receive(pSample);
	return hr;
}

HRESULT ZVidcapPin::BreakConnect() {
	HRESULT hr = receiver->BreakConnect();
	return hr;
}

STDMETHODIMP ZVidcapPin::QueryId(LPWSTR *Id) {
	LPCWSTR src = receiver->name;
	LPWSTR *result = Id;		

	DWORD name_len = sizeof(WCHAR) * (lstrlenW(src) + 1);

	LPWSTR dest = (LPWSTR)CoTaskMemAlloc(name_len);
	if (dest == NULL) return E_OUTOFMEMORY;

	*result = dest;
	CopyMemory(dest, src, name_len);

	return NOERROR;
}

HRESULT ZVidcapPin::CheckMediaType(const AM_MEDIA_TYPE *media_type) {
	return receiver->CheckMediaType(media_type);
}

HRESULT ZVidcapPin::Active() {
	return TRUE;
}

HRESULT ZVidcapPin::Inactive() {
	return FALSE;
}

HRESULT ZVidcapPin::SetMediaType(const AM_MEDIA_TYPE *media_type) {
	return receiver->SetMediaType(media_type);
}

STDMETHODIMP ZVidcapPin::Connect(IPin * /* receive_pin */, const AM_MEDIA_TYPE * /* media_type */) 
{
	return E_NOTIMPL;
}

STDMETHODIMP ZVidcapPin::ReceiveConnection(IPin *connector_pin, const AM_MEDIA_TYPE *media_type) 
{
	// Respond to a connection that an output pin is making.
	if (connected_to) return VFW_E_ALREADY_CONNECTED;
	// See whether media type is OK.
	HRESULT result = CheckMediaType(media_type);
	if (result != NOERROR) {
		BreakConnect();

		if (SUCCEEDED(result) || (result == E_FAIL) ||
			(result == E_INVALIDARG)) {
			result = VFW_E_TYPE_NOT_ACCEPTED;
		}

		return result;
	}

	// Complete the connection.

	connected_to = connector_pin;
	connected_to->AddRef();
	result = SetMediaType(media_type);

	if (!SUCCEEDED(result)) {
		connected_to->Release();
		connected_to = NULL;

		BreakConnect();
		return result;
	}

	return NOERROR;
}

STDMETHODIMP ZVidcapPin::Disconnect() {
	if (!connected_to) return S_FALSE;

	HRESULT result = BreakConnect();
	if (FAILED(result)) return result;

	connected_to->Release();
	connected_to = NULL;

	return S_OK;
}

STDMETHODIMP ZVidcapPin::ConnectedTo(IPin **pin_return) {
	IPin *pin = connected_to;
	*pin_return = pin;

	if (pin == NULL) return VFW_E_NOT_CONNECTED;

	pin->AddRef();
	return S_OK;
}

STDMETHODIMP ZVidcapPin::ConnectionMediaType(AM_MEDIA_TYPE *media_type) {
	if (IsConnected()) {
		AM_MEDIA_TYPE *dest = media_type;
		AM_MEDIA_TYPE *source = &my_media_type;

		assert(source != dest);

		*dest = *source;
		if (source->cbFormat != 0) {
			assert(source->pbFormat != NULL);
			dest->pbFormat = (PBYTE)CoTaskMemAlloc(source->cbFormat);
			if (dest->pbFormat == NULL) {
				dest->cbFormat = 0;
				assert( 0 );
				return VFW_E_NOT_CONNECTED;
			}
			else {
				CopyMemory((PVOID)dest->pbFormat, (PVOID)source->pbFormat, dest->cbFormat);
			}
		}

		if (dest->pUnk != NULL) dest->pUnk->AddRef();
		return S_OK;
	}
	else {
		memset(media_type, 0, sizeof(*media_type));
		return VFW_E_NOT_CONNECTED;
	}
}

STDMETHODIMP ZVidcapPin::QueryPinInfo(PIN_INFO *pin_info_return) {
	pin_info_return->pFilter = receiver;
	if (receiver) receiver->AddRef();

	lstrcpynW( pin_info_return->achName, receiver->name, sizeof(pin_info_return->achName) / sizeof(WCHAR));

	pin_info_return->dir = PINDIR_INPUT;

	return NOERROR;
}

STDMETHODIMP ZVidcapPin::QueryDirection(PIN_DIRECTION *direction_return) {
	*direction_return = PINDIR_INPUT;
	return NOERROR;
}

STDMETHODIMP ZVidcapPin::QueryAccept(const AM_MEDIA_TYPE *media_type) {
	HRESULT result = CheckMediaType(media_type);
	if (FAILED(result)) return S_FALSE;

	return result;
}

STDMETHODIMP ZVidcapPin::EnumMediaTypes(IEnumMediaTypes **enumerator_return) {
	*enumerator_return = MEMALLOC_NEW(ZVidcapMediaTypeEnumerator)(this, NULL);
	return S_OK;
}

STDMETHODIMP ZVidcapPin::EndOfStream(void) {
	return S_OK;
}

STDMETHODIMP ZVidcapPin::SetSink(IQualityControl * /* piqc */) 
{
	return NOERROR;
}

STDMETHODIMP ZVidcapPin::Notify(IBaseFilter *sender, Quality quality) {
	UNREFERENCED_PARAMETER(quality);
	UNREFERENCED_PARAMETER(sender);
	return E_NOTIMPL;
}

STDMETHODIMP ZVidcapPin::NewSegment(REFERENCE_TIME /* tStart */, REFERENCE_TIME /* tStop */, NxF64 /* dRate */) 
{
	return S_OK;
}

STDMETHODIMP ZVidcapPin::BeginFlush() {
	return S_OK;
}

STDMETHODIMP ZVidcapPin::EndFlush(void) {
	return S_OK;
}

STDMETHODIMP ZVidcapPin::ReceiveCanBlock() {
	return S_FALSE;
}

STDMETHODIMP ZVidcapPin::ReceiveMultiple(IMediaSample **samples, long num_samples, long *num_processed_return) {
	HRESULT result = S_OK;
	NxI32 num_processed = 0;

	while (num_samples-- > 0) {
		 result = Receive(samples[num_processed]);

		 if (result != S_OK) break;

		 num_processed++;
	}

	*num_processed_return = num_processed;
	return result;
}

STDMETHODIMP ZVidcapPin::GetAllocatorRequirements(ALLOCATOR_PROPERTIES * /* pProps */) 
{
	return E_NOTIMPL;
}

STDMETHODIMP ZVidcapPin::NotifyAllocator(IMemAllocator *pAllocator, BOOL /* bReadOnly */) 
{
	if( allocator ) allocator->Release();
	allocator = NULL;
	allocator = pAllocator;
	pAllocator->AddRef();
	return NOERROR;
}

STDMETHODIMP ZVidcapPin::GetAllocator(IMemAllocator **allocator_return) {
	if (allocator == NULL) {
		HRESULT result = CoCreateInstance(CLSID_MemoryAllocator, 0, CLSCTX_INPROC_SERVER,IID_IMemAllocator, (void **)&allocator);
		if (FAILED(result)) return result;
	}

	assert(allocator != NULL);
	*allocator_return = allocator;
	allocator->AddRef();
	return NOERROR;
}

STDMETHODIMP ZVidcapPin::QueryInternalConnections(IPin ** /* pins_return */, ULONG * /* num_pins_return */) 
{
	return E_NOTIMPL;
}

BOOL ZVidcapPin::IsConnected() {
	return (connected_to != NULL); 
}

IPin *ZVidcapPin::GetConnected() {
	return connected_to;
}

ZVidcapMediaTypeEnumerator::ZVidcapMediaTypeEnumerator(ZVidcapPin *pPin, ZVidcapMediaTypeEnumerator *pEnumMediaTypes) {
	assert(pPin != NULL);

	reference_count = 1;
	my_pin = pPin;
	my_pin->AddRef();

	if (pEnumMediaTypes != NULL) {
		// This is a copy of another enumerator.
		index = pEnumMediaTypes->index;
		version = pEnumMediaTypes->version;
		return;
	}

	// This is a new enumerator, not a copy.
	index = 0;
	version = 0;
}

ZVidcapMediaTypeEnumerator::~ZVidcapMediaTypeEnumerator() {
	my_pin->Release();
}


STDMETHODIMP ZVidcapMediaTypeEnumerator::QueryInterface(REFIID riid, void **result) {
	if (riid == IID_IEnumMediaTypes || riid == IID_IUnknown) {
		return vidcapGetInterface((IEnumMediaTypes *) this, result);
	}
	else {
		*result = NULL;
		return E_NOINTERFACE;
	}
}

STDMETHODIMP_(ULONG) ZVidcapMediaTypeEnumerator::AddRef() {
	return InterlockedIncrement(&reference_count);
}

STDMETHODIMP_(ULONG) ZVidcapMediaTypeEnumerator::Release() {
	ULONG cRef = InterlockedDecrement(&reference_count);
	if (cRef == 0) {
		delete this;
	}
	return cRef;
}

STDMETHODIMP ZVidcapMediaTypeEnumerator::Clone(IEnumMediaTypes **enumerator_return) {
	HRESULT result = NOERROR;

	*enumerator_return = MEMALLOC_NEW(ZVidcapMediaTypeEnumerator)(my_pin, this);
	if (*enumerator_return == NULL) result =  E_OUTOFMEMORY;

	return result;
}

STDMETHODIMP ZVidcapMediaTypeEnumerator::Next(ULONG /* num_media_types */, AM_MEDIA_TYPE ** /* media_types_return */, ULONG *num_fetched_return) 
{
	// We don't have to enumerate media types; only output
	// pins need to do this.
	*num_fetched_return = 0;
	return S_FALSE;
}

STDMETHODIMP ZVidcapMediaTypeEnumerator::Skip(ULONG num_to_skip) {
	// Since we know we only have one media type, this is
	// real simple.
	const NxI32 NUM_TYPES = 1;
	ULONG types_left = NUM_TYPES - index;
	if (num_to_skip > types_left) return S_FALSE;

	index += num_to_skip;
	return S_OK;
}

STDMETHODIMP ZVidcapMediaTypeEnumerator::Reset() {
	index = 0;
	version = 0;
	return NOERROR;
}


class ZVidcapPinEnumerator : public IEnumPins {
  public:
	ZVidcapPinEnumerator(ZVidcapFilter *receiver, ZVidcapPinEnumerator *enumerator_to_copy);
	virtual ~ZVidcapPinEnumerator();

	NxI32 index;
	ZVidcapFilter *receiver;
	LONG reference_count;

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **result);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IEnumPins
	STDMETHODIMP Next(ULONG num_pins, IPin **pins_return, ULONG *num_fetched_return);

	STDMETHODIMP Skip(ULONG num_to_skip);
	STDMETHODIMP Reset();
	STDMETHODIMP Clone(IEnumPins **result);
	STDMETHODIMP Refresh();
};

#pragma warning(disable:4355)
ZVidcapFilter::ZVidcapFilter() : m_pUnknown( reinterpret_cast<IUnknown *>(this) ) {
	reference_count = 1;
	name = NULL;
	filter_graph = NULL;
	input_pin = NULL;

	maxWidth = 320;
	maxHeight = 240;

	width = 0;
	height = 0;
	stride = 0;

	incoming_image_buffers[0] = 0;
	incoming_image_buffers[1] = 0;
	locks[0] = 0;
	locks[1] = 0;
	frameNums[0] = 0;
	frameNums[1] = 0;
	readLock = -1;

	memset( frameTimings, 0, sizeof(frameTimings) );
	lastTime = 0;

	input_pin = MEMALLOC_NEW(ZVidcapPin)(this);
}

ZVidcapFilter::~ZVidcapFilter() {
	if (incoming_image_buffers[0]) delete [] incoming_image_buffers[0];
	if (incoming_image_buffers[1]) delete [] incoming_image_buffers[1];
}

NxI32 ZVidcapFilter::getAvgFrameTimings() {
	NxI32 i;
	NxU32 sum = 0;
	for( i=0; i<sizeof(frameTimings)/sizeof(frameTimings[0]); i++ ) {
		sum += frameTimings[i];
	}
	return NxI32( sum / (unsigned)i );
}

STDMETHODIMP ZVidcapFilter::QueryInterface(REFIID riid, void **result) {
	if (riid == IID_IBaseFilter) {
		return vidcapGetInterface((IBaseFilter *) this, result);
	}
	else if (riid == IID_IMediaFilter) {
		return vidcapGetInterface((IMediaFilter *) this, result);
	}
	else if (riid == IID_IPersist) {
		return vidcapGetInterface((IPersist *) this, result);
	}
	else if (riid == IID_IAMovieSetup) {
		return vidcapGetInterface((IAMovieSetup *) this, result);
	}
	else if (riid == IID_IUnknown) {
		return vidcapGetInterface((IUnknown *) this, result);
	}
	else {
		*result = NULL;
		return E_NOINTERFACE;
	}
}

STDMETHODIMP_(ULONG) ZVidcapFilter::AddRef() {	
	return InterlockedIncrement(&reference_count);
};								  

STDMETHODIMP_(ULONG) ZVidcapFilter::Release() {  
	ULONG cRef = InterlockedDecrement(&reference_count);
	if (cRef == 0) {
		delete this;
	}

	return cRef;
};

HRESULT ZVidcapFilter::CheckMediaType(const AM_MEDIA_TYPE *media_type) {
	HRESULT   hr = E_FAIL;
	
	if (media_type->formattype != FORMAT_VideoInfo ) {
		return E_INVALIDARG;
	}

	if( media_type->formattype == FORMAT_VideoInfo ) {
		VIDEOINFOHEADER *pVih = reinterpret_cast<VIDEOINFOHEADER*>(media_type->pbFormat);
		if( pVih->bmiHeader.biWidth > maxWidth || pVih->bmiHeader.biHeight > maxHeight ) {
			return S_FALSE;
		}
	}
	
	if(
		IsEqualGUID(media_type->majortype, MEDIATYPE_Video)
		&& (
			IsEqualGUID(media_type->subtype, MEDIASUBTYPE_RGB24)
			|| IsEqualGUID(media_type->subtype, MEDIASUBTYPE_RGB8)
		)
	) {
		hr = S_OK;
	}
	
	return hr;
}

HRESULT ZVidcapFilter::SetMediaType(const AM_MEDIA_TYPE *media_type) {
	VIDEOINFO *pviBmp;
	pviBmp = (VIDEOINFO *)media_type->pbFormat;
	width  = pviBmp->bmiHeader.biWidth;
	height = abs(pviBmp->bmiHeader.biHeight);
	depth = pviBmp->bmiHeader.biBitCount / 8;
	stride = (width * depth + 3) & ~(3);


	if( incoming_image_buffers[0] ) {
		delete incoming_image_buffers[0];
	}
	if( incoming_image_buffers[1] ) {
		delete incoming_image_buffers[1];
	}
	incoming_image_buffers[0] = MEMALLOC_NEW_ARRAY(char,width * height * depth)[width * height * depth];
	incoming_image_buffers[1] = MEMALLOC_NEW_ARRAY(char,width * height * depth)[width * height * depth];

	locks[0] = 0;
	locks[1] = 0;
	frameNums[0] = 0;
	frameNums[1] = 0;
	readLock = -1;

	return S_OK;
}

HRESULT ZVidcapFilter::BreakConnect() {
	if (input_pin->IsConnected() == FALSE) {
		return S_FALSE;
	}

	return NOERROR;
}

HRESULT ZVidcapFilter::Receive(IMediaSample *incoming_sample) {
	BYTE  *input_image;

	incoming_sample->GetPointer(&input_image);

	// DECIDE on which buffer.	Pick the oldest one that isn't locked
	EnterCriticalSection( &vidcapCriticalSection );

	NxI32 which = 0;
	if( !locks[0] && !locks[1] ) {
		// Both are unlocked, pick the older
		which = frameNums[0] < frameNums[1] ? 0 : 1;
	}
	else if( locks[0] && locks[1] ) {
		// Both locked, do nothing
		LeaveCriticalSection( &vidcapCriticalSection );
		return NULL;
	}
	else {
		// One is unlocked, other locked.  Pick the unlocked one.
		which = !locks[0] ? 0 : 1;
	}

	NxI32 nextFrameNum = max( frameNums[0], frameNums[1] ) + 1;

	char *dest_line_start = incoming_image_buffers[which];
	locks[which] = 1;

	LeaveCriticalSection( &vidcapCriticalSection );

	NxI32 numTimingSlots = sizeof(frameTimings) / sizeof(frameTimings[0]);
	NxU32 now = timeGetTime();
	frameTimings[nextFrameNum % numTimingSlots] = now - lastTime;
	lastTime = now;

	BYTE *input_line_start = input_image + (height-1)*stride;
	NxI32 bpp = depth;

	if( bpp == 1 ) {
		for (NxI32 j = 0; j < height; j++) {
			BYTE *src = input_line_start;
			char *dest = dest_line_start;
			memcpy( dest, src, width );
			input_line_start -= stride;
			dest_line_start += width * bpp;
		}
	}
	else if( bpp == 2 ) {
		for (NxI32 j = 0; j < height; j++) {
			BYTE *src = input_line_start;
			char *dest = dest_line_start;
			memcpy( dest, src, width*2 );
			input_line_start -= stride;
			dest_line_start += width * bpp;
		}
	}
	else if( bpp == 3 ) {
		for (NxI32 j = 0; j < height; j++) {
			BYTE *src = input_line_start;
			char *dest = dest_line_start;
			NxI32 i;
			for (i = 0; i < width; i++) {
				dest[0] = src[2];
				dest[1] = src[1];
				dest[2] = src[0];
			
				src += 3;
				dest += bpp;
			}

			input_line_start -= stride;
			dest_line_start += width * bpp;
		}
	}

	EnterCriticalSection( &vidcapCriticalSection );
	locks[which] = 0;
	frameNums[which] = nextFrameNum;
	LeaveCriticalSection( &vidcapCriticalSection );
		
	return NOERROR;
}

STDMETHODIMP ZVidcapFilter::JoinFilterGraph(IFilterGraph *graph, LPCWSTR desired_name) {
	filter_graph = graph;

	if (name) {
		delete[] name;
		name = NULL;
	}

	if (desired_name) {
		DWORD len = lstrlenW(desired_name)+1;
		name = MEMALLOC_NEW_ARRAY(WCHAR,len)[len];
		assert(name);

		if (name) memcpy(name, desired_name, len * sizeof(WCHAR));
	}

	return NOERROR;
}

STDMETHODIMP ZVidcapFilter::QueryVendorInfo(LPWSTR *vendor_info_return) {
	UNREFERENCED_PARAMETER(vendor_info_return);
	return E_NOTIMPL;
}

STDMETHODIMP ZVidcapFilter::Register() {
	return S_FALSE;
}

STDMETHODIMP ZVidcapFilter::Unregister() {
	return S_FALSE;
}

STDMETHODIMP ZVidcapFilter::SetSyncSource(IReferenceClock * /* clock_return */) 
{
	return NOERROR;
}

STDMETHODIMP ZVidcapFilter::GetSyncSource(IReferenceClock **clock_return) 
{
	*clock_return = NULL;
	return NOERROR;
}

STDMETHODIMP ZVidcapFilter::Stop() {
	// For some reason we might want to tell the receiver to stop,
	// or something.
	HRESULT hr = NOERROR;
	return hr;
}

STDMETHODIMP ZVidcapFilter::Pause() {
	return S_OK;
}

STDMETHODIMP ZVidcapFilter::Run(REFERENCE_TIME /* start_timex */) 
{
	return S_OK;
}

STDMETHODIMP ZVidcapFilter::GetClassID(CLSID *class_id_return) 
{
	*class_id_return = CLSID_Vidcap_Filter;
	return NOERROR;
}

STDMETHODIMP ZVidcapFilter::QueryFilterInfo(FILTER_INFO * pInfo) 
{
	if (name) {
		lstrcpynW(pInfo->achName, name, sizeof(pInfo->achName)/sizeof(WCHAR));
	}
	else {
		pInfo->achName[0] = L'\0';
	}

	pInfo->pGraph = filter_graph;
	if (filter_graph) filter_graph->AddRef();

	return NOERROR;
}

STDMETHODIMP ZVidcapFilter::FindPin(LPCWSTR Id, IPin **pin_return) {
	ZVidcapPin *pPin = input_pin;

	if (lstrcmpW(name, Id) == 0) {	// The pin's name is actually my name.
		*pin_return = pPin;
		pPin->AddRef();
		return S_OK;
	}

	*pin_return = NULL;
	return VFW_E_NOT_FOUND;
}

STDMETHODIMP ZVidcapFilter::GetState(DWORD /* milliseconds */, FILTER_STATE *state_return)
{
	*state_return = State_Running;
	return S_OK;
}

STDMETHODIMP ZVidcapFilter::EnumPins(IEnumPins **ppEnum) {
	*ppEnum = MEMALLOC_NEW(ZVidcapPinEnumerator)(this, NULL);
	if (*ppEnum == NULL) return E_OUTOFMEMORY;
	return NOERROR;
}

ZVidcapPinEnumerator::ZVidcapPinEnumerator(ZVidcapFilter *_receiver, ZVidcapPinEnumerator *to_copy) {
	reference_count = 1;
	index = 0;

	receiver = _receiver;
	receiver->AddRef();

	if (to_copy) index = to_copy->index;
}

ZVidcapPinEnumerator::~ZVidcapPinEnumerator() {
	receiver->Release();
}

STDMETHODIMP ZVidcapPinEnumerator::QueryInterface(REFIID riid, void **result) {
	if (riid == IID_IEnumPins || riid == IID_IUnknown) {
		return vidcapGetInterface((IEnumPins *)this, result);
	}
	else {
		*result = NULL;
		return E_NOINTERFACE;
	}
}

STDMETHODIMP_(ULONG) ZVidcapPinEnumerator::AddRef() {
	return InterlockedIncrement(&reference_count);
}

STDMETHODIMP_(ULONG) ZVidcapPinEnumerator::Release() {
	ULONG cRef = InterlockedDecrement(&reference_count);
	if (cRef == 0) delete this;  // I thought this was not valid C++,
	return cRef;				 // but it is how COM works.
}

STDMETHODIMP ZVidcapPinEnumerator::Clone(IEnumPins **result) {
	*result = MEMALLOC_NEW(ZVidcapPinEnumerator)(receiver, this);
	if (*result == NULL) return E_OUTOFMEMORY;

	return NOERROR;
}

STDMETHODIMP ZVidcapPinEnumerator::Next(ULONG /* num_pins */, IPin **pins_return, ULONG *num_fetched_return) 
{
	if (num_fetched_return != NULL) *num_fetched_return = 0;

	const NxI32 NUM_PINS = 1;
	NxI32 pins_left = NUM_PINS - index;
	if (pins_left <= 0) return S_FALSE;

	assert(index == 0);
	pins_return[0] = receiver->input_pin;
	receiver->input_pin->AddRef();
	index++;
	*num_fetched_return = 1;

	return NOERROR;
}

STDMETHODIMP ZVidcapPinEnumerator::Skip(ULONG num_to_skip) {
	// Since we know we only have one pin, this is
	// real simple.

	const NxI32 NUM_PINS = 1;
	ULONG pins_left = NUM_PINS - index;
	if(num_to_skip > pins_left) return S_FALSE;

	index += num_to_skip;
	return S_OK;
}

STDMETHODIMP ZVidcapPinEnumerator::Reset() {
	index = 0;
	return S_OK;
}

STDMETHODIMP ZVidcapPinEnumerator::Refresh() {
	return Reset();
}


// VidCap Internal Interfaces
//------------------------------------------------------------------------------------------

NxI32 zVidcapTraverseAndBindDevices( char *bindTo, void **boundFilter ) 
{

  // INSTANCIATE a class enumerator on vidcap inputs
  IEnumMoniker *pEnumMoniker=0;


	// CLEAR the device list
	for( NxI32 i=0; i<zVidcapNumDevices; i++ ) {
		if( zVidcapDevices[i] ) {
			MEMALLOC_FREE( zVidcapDevices[i] );
			zVidcapDevices[i] = 0;
		}
	}
	zVidcapNumDevices = 0;

	// SETUP Microsoft COM
	CoInitialize(NULL);

	// INIT local
	HRESULT hr = 0;
	if( boundFilter ) *boundFilter = NULL;
	NxI32 success = 0;

	// ASSUME success if we are merely enumerating and not binding
	if( !bindTo ) success = 1;

	// INSTANCIATE a device enumerator
	ICreateDevEnum *pDeviceEnumerator=0;
	hr = CoCreateInstance( CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pDeviceEnumerator );
	if( hr != S_OK ) goto error;


	hr = pDeviceEnumerator->CreateClassEnumerator( CLSID_VideoInputDeviceCategory, &pEnumMoniker, 0 );
	if( hr != S_OK ) goto error;

	{ // ENUMARATE the devices and potentially bind to them
		pEnumMoniker->Reset();

		ULONG cFetched;
		IMoniker *pMoniker = NULL;
		while( hr = pEnumMoniker->Next(1, &pMoniker, &cFetched), hr==S_OK ) {
			IPropertyBag *pBag;
			char name[256] = {0,};

			hr = pMoniker->BindToStorage( 0, 0, IID_IPropertyBag, (void **)&pBag );
			if( hr == S_OK ) {
				VARIANT var;
				var.vt = VT_BSTR;
				hr = pBag->Read( L"FriendlyName", &var, NULL );
				if( hr == S_OK ) {
					WideCharToMultiByte( CP_ACP, 0, var.bstrVal, -1, name, 80, NULL, NULL );
					zVidcapDevices[zVidcapNumDevices++] = strdup( name );
					assert( zVidcapNumDevices < vidcapMAXDEVICES );
					SysFreeString( var.bstrVal );
				}
				hr = pBag->Release();
				if( bindTo && !*boundFilter ) {
					// If we are being asked to bind to a filter and we haven't already...
					if( !stricmp( bindTo, name ) ) {
						hr = pMoniker->BindToObject( 0, 0, IID_IBaseFilter, boundFilter );
						success = hr == S_OK ? 1 : 0;
					}
				}
			}
			pMoniker->Release();
		}

	}

error:
	if( pDeviceEnumerator )
		pDeviceEnumerator->Release();
	if( pEnumMoniker )
		pEnumMoniker->Release();
	return success;
}

VidcapConnection *zVidcapGetConnectionFromName( char *deviceName ) {
	if( !deviceName ) {
		deviceName = zVidcapLastDevice;
	}
	if( !deviceName ) {
		return NULL;
	}

	for( NxI32 i=0; i<zVidcapNumDevices; i++ ) {
		if( !stricmp(zVidcapDevices[i],deviceName) ) {
			return &vidcapConns[i];
		}
	}
	return NULL;
}

void zVidcapRecursiveDestoryGraph( IFilterGraph *filterGraph, IBaseFilter *pf ) {
	IPin *pP, *pTo;
	ULONG u;
	IEnumPins *pins = NULL;
	PIN_INFO pininfo;
	HRESULT hr = pf->EnumPins(&pins);
	pins->Reset();
	while (hr == NOERROR) {
		hr = pins->Next(1, &pP, &u);
		if (hr == S_OK && pP) {
			pP->ConnectedTo(&pTo);
			if (pTo) {
				hr = pTo->QueryPinInfo(&pininfo);
				if (hr == NOERROR) {
					if (pininfo.dir == PINDIR_INPUT) {
						zVidcapRecursiveDestoryGraph(filterGraph,pininfo.pFilter);
						filterGraph->Disconnect(pTo);
						filterGraph->Disconnect(pP);
						filterGraph->RemoveFilter(pininfo.pFilter);
					}
					pininfo.pFilter->Release();
				}
				pTo->Release();
			}
			pP->Release();
		}
	}
	if (pins) {
		pins->Release();
	}
}


// vidcap public interfaces
//--------------------------------------------------------------

char **zVidcapGetDevices( NxI32 *count ) {
	zVidcapTraverseAndBindDevices( NULL, NULL );
	if( count ) {
		*count = zVidcapNumDevices;
	}
	return zVidcapDevices;
}

NxI32 zVidcapStartDevice( char *deviceName, NxI32 maxWidth, NxI32 maxHeight ) {
	InitializeCriticalSection( &vidcapCriticalSection );

	if( !deviceName ) {
		char **devs = zVidcapGetDevices();
		if( devs[0] ) {
			deviceName = devs[0];
		}
		else {
			return 0;
		}
	}

	HRESULT hr;

	VidcapConnection *conn = zVidcapGetConnectionFromName( deviceName );
	if( !conn ) return 0;

	// CREATE the filter graph
	hr = CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&conn->graphBuilderIFACE );
	if( !SUCCEEDED(hr) ) {
		zVidcapShutdownDevice( deviceName );
		return 0;
	}
	assert( conn->graphBuilderIFACE );

	// CREATE the graph builder
	hr = CoCreateInstance( CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC, IID_ICaptureGraphBuilder2, (void **)&conn->captureGraphBuilderIFACE );
	if( !SUCCEEDED(hr) ) {
		zVidcapShutdownDevice( deviceName );
		return 0;
	}
	assert( conn->captureGraphBuilderIFACE );

	// FIND the media control	 
	hr = conn->graphBuilderIFACE->QueryInterface( IID_IMediaControl,(LPVOID *) &conn->mediaControlIFACE );
	if( !SUCCEEDED(hr) ) {
		zVidcapShutdownDevice( deviceName );
		return 0;
	}
	assert( conn->mediaControlIFACE );

	// TELL the graph builder which filter graph it is working on
	hr = conn->captureGraphBuilderIFACE->SetFiltergraph( conn->graphBuilderIFACE );
	if( !SUCCEEDED(hr) ) {
		zVidcapShutdownDevice( deviceName );
		return 0;
	}

	// BIND to the requested camera
	zVidcapTraverseAndBindDevices( deviceName, (void**)&conn->cameraFilter );

	// ADD capture filter to our graph.
	hr = conn->graphBuilderIFACE->AddFilter( conn->cameraFilter, L"Video Capture" );
	if( !SUCCEEDED(hr) ) {
		zVidcapShutdownDevice( deviceName );
		return 0;
	}

	// CREATE an instance of our capture filter which just grabs the data into memory
	conn->filter = MEMALLOC_NEW(ZVidcapFilter)();
	conn->filter->maxWidth = maxWidth;
	conn->filter->maxHeight = maxHeight;

	// ATTACH our capture filter to the filterGraph
	hr = conn->graphBuilderIFACE->AddFilter( conn->filter, L"MEMORYRENDERER");
	if( !SUCCEEDED(hr) ) {
		zVidcapShutdownDevice( deviceName );
		return 0;
	}

	// CONNECT the video source to our capture filter (automatically creating intermediate filters?)
	hr = conn->captureGraphBuilderIFACE->RenderStream( &PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, conn->cameraFilter, NULL, conn->filter );
	if( !SUCCEEDED(hr) ) {
		zVidcapShutdownDevice( deviceName );
		return 0;
	}

	// START capturing video
	hr = conn->mediaControlIFACE->Run();
	if( !SUCCEEDED(hr) ) {
		zVidcapShutdownDevice( deviceName );
		return 0;
	}

	zVidcapLastDevice = strdup( deviceName );

	return 1;
}

NxI32 zVidcapStartAVI( char *filename, NxI32 maxWidth, NxI32 maxHeight ) {
	InitializeCriticalSection( &vidcapCriticalSection );

	HRESULT hr;

	VidcapConnection *conn = &vidcapConns[0];
	strcpy( zVidcapDevices[0], filename );

	// CREATE the filter graph
	hr = CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&conn->graphBuilderIFACE );
	if( !SUCCEEDED(hr) ) {
		zVidcapShutdownDevice( filename );
		return 0;
	}
	assert( conn->graphBuilderIFACE );

	// CREATE the graph builder
	hr = CoCreateInstance( CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC, IID_ICaptureGraphBuilder2, (void **)&conn->captureGraphBuilderIFACE );
	if( !SUCCEEDED(hr) ) {
		zVidcapShutdownDevice( filename );
		return 0;
	}
	assert( conn->captureGraphBuilderIFACE );

	// FIND the media control	 
	hr = conn->graphBuilderIFACE->QueryInterface( IID_IMediaControl,(LPVOID *) &conn->mediaControlIFACE );
	if( !SUCCEEDED(hr) ) {
		zVidcapShutdownDevice( filename );
		return 0;
	}
	assert( conn->mediaControlIFACE );

	// TELL the graph builder which filter graph it is working on
	hr = conn->captureGraphBuilderIFACE->SetFiltergraph( conn->graphBuilderIFACE );
	if( !SUCCEEDED(hr) ) {
		zVidcapShutdownDevice( filename );
		return 0;
	}

	// CREATE a source filter from the avi file
	wchar_t wideFilename[256];
	MultiByteToWideChar( CP_ACP, 0, filename, strlen(filename)+1, wideFilename, 256 );
	hr = conn->graphBuilderIFACE->AddSourceFilter( wideFilename/*L"d:\\transfer\\ants.avi"*/, L"Source Filter", &conn->cameraFilter );
	if( !SUCCEEDED(hr) ) 
  {
		zVidcapShutdownDevice( filename );
		return 0;
	}

	// CREATE an instance of our capture filter which just grabs the data into memory
	conn->filter = MEMALLOC_NEW(ZVidcapFilter)();
	conn->filter->maxWidth = maxWidth;
	conn->filter->maxHeight = maxHeight;

	// ATTACH our capture filter to the filterGraph
	hr = conn->graphBuilderIFACE->AddFilter( conn->filter, L"MEMORYRENDERER" );
	if( !SUCCEEDED(hr) ) {
		zVidcapShutdownDevice( filename );
		return 0;
	}

	// CONNECT the video source to our capture filter (automatically creating intermediate filters?)
	hr = conn->captureGraphBuilderIFACE->RenderStream( NULL, &MEDIATYPE_Video, conn->cameraFilter, NULL, conn->filter );

	if( !SUCCEEDED(hr) ) {
		zVidcapShutdownDevice( filename );
		return 0;
	}

	// START capturing video
	hr = conn->mediaControlIFACE->Run();
	if( !SUCCEEDED(hr) ) {
		zVidcapShutdownDevice( filename );
		return 0;
	}
/*
	hr = conn->captureGraphBuilderIFACE->FindInterface(
		NULL,
		NULL,
		conn->filter,
		IID_IMediaSeeking,
		(void **)&conn->seekIFACE
	);
	if( !SUCCEEDED(hr) ) {
		zVidcapShutdownDevice( filename );
		return 0;
	}
*/
	zVidcapLastDevice = strdup( filename );

	return 1;
}

void zVidcapAVISeek() 
{
	VidcapConnection *conn = &vidcapConns[0];

	LONGLONG current = -1;
	if( conn->seekIFACE ) 
  {
		conn->seekIFACE->GetCurrentPosition( &current );
	}
}

void zVidcapShutdownDevice( char *deviceName ) {
	VidcapConnection *conn = zVidcapGetConnectionFromName( deviceName );
	if( conn ) {
		if( conn->mediaControlIFACE ) {
			conn->mediaControlIFACE->Stop();
			conn->mediaControlIFACE->Release();
			conn->mediaControlIFACE = NULL;
		}

		if( conn->filter ) {
			conn->filter->Release();
			conn->filter = NULL;
		}

		if( conn->cameraFilter ) {
			zVidcapRecursiveDestoryGraph( conn->graphBuilderIFACE, conn->cameraFilter );
			conn->cameraFilter->Release();
			conn->cameraFilter = NULL;
		}

		if( conn->captureGraphBuilderIFACE ) {
			conn->captureGraphBuilderIFACE->Release();
			conn->captureGraphBuilderIFACE= NULL;
		}

		if( conn->graphBuilderIFACE ) {
			conn->graphBuilderIFACE->Release();
			conn->graphBuilderIFACE = NULL;
		}

		if( conn->seekIFACE ) {
			conn->seekIFACE->Release();
			conn->seekIFACE = NULL;
		}

		if( zVidcapLastDevice && !strcmp(zVidcapLastDevice,deviceName) ) {
			MEMALLOC_FREE( zVidcapLastDevice );
			zVidcapLastDevice = 0;
		}
	}
}

void zVidcapShutdownAll() {
	for( NxI32 i=0; i<zVidcapNumDevices; i++ ) {
		zVidcapShutdownDevice( zVidcapDevices[i] );
	}
}

void zVidcapGetBitmapDesc( char *deviceName, NxI32 &w, NxI32 &h, NxI32 &d ) {
	w = 0;
	h = 0;
	d = 0;
	VidcapConnection *conn = zVidcapGetConnectionFromName( deviceName );
	if( conn ) {
		if( conn->filter ) {
			w = conn->filter->width;
			h = conn->filter->height;
			d = conn->filter->depth;
		}
	}
}

NxI32 zVidcapShowFilterPropertyPageModalDialog( char *deviceName ) {
	HRESULT hr = 0;
	NxI32 w0, h0, d0, w1, h1, d1;
	VidcapConnection *conn = zVidcapGetConnectionFromName( deviceName );
	if( conn ) {
		// Don't do anything if we don't have a media control
		if( !conn->mediaControlIFACE ) return 0;

		// STORE old, see if it changed.
		zVidcapGetBitmapDesc( deviceName, w0, h0, d0 );

		// FETCH the camera filter's IID_ISpecifyPropertyPages interface
		ISpecifyPropertyPages *pispp = NULL;
		hr = conn->captureGraphBuilderIFACE->FindInterface( NULL, NULL, conn->cameraFilter, IID_ISpecifyPropertyPages, (void **)&pispp );
		if( !SUCCEEDED(hr) ) {
			return 0;
		}

		// STOP the media
		hr = conn->mediaControlIFACE->Stop();

		// FETCH the property page
		CAUUID caGUID;
		hr = pispp->GetPages(&caGUID);
		if( !SUCCEEDED(hr) ) {
			pispp->Release();
			hr = conn->mediaControlIFACE->Run();
			return 0;
		}
		pispp->Release();

		// CREATE the modal dialog box
		hr = OleCreatePropertyFrame(
			NULL, 0, 0, L"Filter",	1, (IUnknown **)&conn->cameraFilter,
			caGUID.cElems, caGUID.pElems, 0, 0, NULL
		);

		// RESUME the media
		hr = conn->mediaControlIFACE->Run();

		// CHECK if the format changed
		zVidcapGetBitmapDesc( deviceName, w1, h1, d1 );

		if( w0!=w1 || h0!=h1 || d0!=d1 ) {
			return 1;
		}
	}
	return 0;
}

NxI32 zVidcapShowPinPropertyPageModalDialog( char *deviceName ) {
	HRESULT hr = 0;
	NxI32 w0, h0, d0, w1, h1, d1;
	VidcapConnection *conn = zVidcapGetConnectionFromName( deviceName );
	if( conn ) {
		// Don't do anything if we don't have a media control
		if( !conn->mediaControlIFACE ) return 0;

		// STORE old, see if it changed.
		zVidcapGetBitmapDesc( deviceName, w0, h0, d0 );

		// FETCH the output pin on the camera filter
		IPin *cameraPin = 0;
		hr = conn->captureGraphBuilderIFACE->FindPin( conn->cameraFilter, PINDIR_OUTPUT, &PIN_CATEGORY_CAPTURE, NULL, FALSE, 0, &cameraPin );
		if( !SUCCEEDED(hr) ) {
			return 0;
		}

		// FETCH the pin's IID_ISpecifyPropertyPages interface
		ISpecifyPropertyPages *pispp = NULL;
		hr = cameraPin->QueryInterface( IID_ISpecifyPropertyPages, (void **)&pispp );
		if( !SUCCEEDED(hr) ) {
			cameraPin->Release();
			return 0;
		}

		// STOP the media
		hr = conn->mediaControlIFACE->Stop();

		// FETCH the property page
		CAUUID caGUID;
		hr = pispp->GetPages(&caGUID);
		if( !SUCCEEDED(hr) ) {
			cameraPin->Release();
			pispp->Release();
			hr = conn->mediaControlIFACE->Run();
			return 0;
		}
		pispp->Release();

		// CREATE the modal dialog
		hr = OleCreatePropertyFrame(
			NULL, 0, 0, L"Filter",	1, (IUnknown **)&cameraPin,
			caGUID.cElems, caGUID.pElems, 0, 0, NULL
		);
		cameraPin->Release();

		// RESUME the media
		hr = conn->mediaControlIFACE->Run();

		// CHECK if the format changed
		zVidcapGetBitmapDesc( deviceName, w1, h1, d1 );
		if( w0!=w1 || h0!=h1 || d0!=d1 ) {
			return 1;
		}
	}

	return 0;
}

char *zVidcapLockNewest( char *deviceName, NxI32 *frameNumber ) {
	VidcapConnection *conn = zVidcapGetConnectionFromName( deviceName );
	if( conn ) {
		EnterCriticalSection( &vidcapCriticalSection );

		// WHICH one is newer and unlocked?
		NxI32 which = 0;
		if( !conn->filter->locks[0] && !conn->filter->locks[1] ) {
			// Both are unlocked, pick the newer
			which = conn->filter->frameNums[0] > conn->filter->frameNums[1] ? 0 : 1;
		}
		else if( conn->filter->locks[0] && conn->filter->locks[1] ) {
			// Both locked, do nothing
			LeaveCriticalSection( &vidcapCriticalSection );
			conn->filter->readLock = -1;
			return NULL;
		}
		else {
			// One is unlocked, other locked.  Pick the unlocked one.
			which = !conn->filter->locks[0] ? 0 : 1;
		}

		if( frameNumber ) {
			if( *frameNumber >= conn->filter->frameNums[which] ) {
				// This isn't any newer than the one we already have
				LeaveCriticalSection( &vidcapCriticalSection );
				conn->filter->readLock = -1;
				return NULL;
			}
			else {
				*frameNumber = conn->filter->frameNums[which];
			}
		}

		conn->filter->locks[which] = 1;
		conn->filter->readLock = which;
		LeaveCriticalSection( &vidcapCriticalSection );

		return conn->filter->incoming_image_buffers[which];
	}
	return NULL;
}

void zVidcapUnlock( char *deviceName ) {
	VidcapConnection *conn = zVidcapGetConnectionFromName( deviceName );
	if( conn ) {
		EnterCriticalSection( &vidcapCriticalSection );
		if( conn->filter->readLock != -1 ) {
			conn->filter->locks[ conn->filter->readLock ] = 0;
		}
		LeaveCriticalSection( &vidcapCriticalSection );
	}
}

NxI32 zVidcapGetAvgFrameTimeInMils( char *deviceName ) {
	VidcapConnection *conn = zVidcapGetConnectionFromName( deviceName );
	if( conn ) {
		return conn->filter->getAvgFrameTimings();
	}
	return 0;
}


//--------------------------------------------------------------------------------
// Test code shows the capture image in old-fashioned ASCII text!

//#define SELF_TEST
#ifdef SELF_TEST

#include "conio.h"

void clrscr() {
	HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
	COORD coordScreen = { 0, 0 };
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo( hConsole, &csbi );
	DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	DWORD cCharsWritten;
	FillConsoleOutputCharacter( hConsole, (TCHAR) ' ', dwConSize, coordScreen, &cCharsWritten );
	GetConsoleScreenBufferInfo( hConsole, &csbi );
	FillConsoleOutputAttribute( hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten );
	SetConsoleCursorPosition( hConsole, coordScreen );
}

NxI32 menu( char **items, NxI32 count ) {
	printf( "Select one:\n" );
	for( NxI32 i=0; i<count; i++ ) {
		printf( "%2d. %s\n", i+1, items[i] );
	}
	scanf( "%d", &i );
	return i;
}

void main() {
	char *mainMenu[] = { "Open AVI File", "Select Camera", "View Camera in ASCII text", "Pin Options", "Filter Options", "Quit" };

	NxI32 numCameras = 0;
	char **cameras = zVidcapGetDevices( &numCameras );

	char *camera = 0;

	NxI32 w, h, d;
	zVidcapGetBitmapDesc( camera, w, h, d );
	NxI32 lastFrame = 0;

	while( 1 ) {
		switch( menu( mainMenu, sizeof(mainMenu)/sizeof(mainMenu[0]) ) ) {
			case 1: {
				printf( "Enter filename:\n" );
				char filename[256];
				scanf( "%s", filename );
				camera = strdup( filename );
				zVidcapShutdownAll();
				NxI32 ok = zVidcapStartAVI( camera, 320, 240 );
				zVidcapGetBitmapDesc( camera, w, h, d );
				lastFrame = 0;
				break;
			}

			case 2: {
				NxI32 i = menu( cameras, numCameras );
				camera = strdup( cameras[i-1] );
				zVidcapShutdownAll();
				NxI32 ok = zVidcapStartDevice( camera );
				zVidcapGetBitmapDesc( camera, w, h, d );
				lastFrame = 0;
				break;
			}

			case 3: {
				while( !kbhit() ) {
					// LOCK the newest frame
					char *cap = zVidcapLockNewest( 0, &lastFrame );
					if( cap ) {
						#define W (78)
						#define H (24)
						static char disp[(W+1)*H];
						char *dst = disp;
						clrscr();
						for( NxI32 y=0; y<H; y++ ) {
							NxU8 *src = (NxU8 *)( cap + d*w*(h*y/H) );
							for( NxI32 x=0; x<W; x++ ) {
								NxU8 *_src = src + d * (x * w / W);
								*dst++ = " .-:;/>)|I!%H*&#"[ ((*_src) >> 5) & 15 ];
							}
							*dst++ = '\n';
						}
						printf( "%s", disp );

						// UNLOCK
						zVidcapUnlock( NULL );

						NxI32 mils = zVidcapGetAvgFrameTimeInMils();

						printf( "w=%d h=%d d=%d lastFrame=%d FPS=%2.1f\n", w, h, d, lastFrame, 1000.f / (NxF32)mils );
					}

					Sleep( 10 );
						// Sleeping helps reduce flicker
				}
				break;
			}

			case 4: {
				zVidcapShowPinPropertyPageModalDialog( camera );
				zVidcapGetBitmapDesc( camera, w, h, d );
				lastFrame = 0;
				break;
			}

			case 5: {
				zVidcapShowFilterPropertyPageModalDialog( camera );
				zVidcapGetBitmapDesc( camera, w, h, d );
				lastFrame = 0;
				break;
			}

			case 6: {
				goto done;
			}
		}
	}

	done:
	zVidcapShutdownAll();
}

#endif
