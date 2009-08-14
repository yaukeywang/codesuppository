// @ZBS {
//		*MODULE_OWNER_NAME zvidcap
// }

#ifndef ZVIDCAP_H
#define ZVIDCAP_H

#include "UserMemAlloc.h"

// This is a fascade around the incredibly poorly designed
// DirectShow API for Video Capture.
//
// This API is not intended to be all things to all people 
// (which is exactly the problem with the DirectShow API).
// Instead it should do most of what you want with minimal hassle
// If nothing else, it is a good starting place for more 
// complicated solutions.
//
// This code is MEMALLOC_FREE with no strings attached and no warranty.
// Author: Jon Blow & Zack Booth Simpson.  25 March 2003.  www.mine-control.com

extern char *zVidcapLastDevice;
	// The name of the last device started

char **zVidcapGetDevices( NxI32 *count=0 );
	// Retrives a list of video capture devices.
	// The list is terminated by a NULL pointer
	// Or you may fetch the list size from optional count pointer
	// Pass the these pointers to the functions below
	// A special device called the "Simulacrum" is always added to the list
	// so that the simulator may be explicitly statred if desired.

NxI32 zVidcapStartDevice( char *deviceName=(char*)0, NxI32 maxWidth=320, NxI32 maxHeight=240 );
	// Starts the specified device, the first one if none specified
NxI32 zVidcapStartAVI( char *filename, NxI32 maxWidth, NxI32 maxHeight );
	// Start an AVI file as if it were a live video source

void zVidcapShutdownDevice( char *deviceName=(char*)0 );
	// DirectShow is very fussy about not shutting down devices
	// properly.  In NT it will survive but in early OSs it often keeps
	// a lock on the camera which means you have to reboot to use the camera again!

void zVidcapShutdownAll();
	// Clears all

char *zVidcapLockNewest( char *deviceName=(char*)0, NxI32 *frameNumber=(NxI32*)0 );
	// Locks the newest frame available.
	// If deviceName is null it uses the last device you started
	// If you specify an (optional) frame number, it will
	// return NULL if there is no frame available newer than frameNumber 
	// in which case you need not unlock it.

void zVidcapUnlock( char *deviceName=(char*)0 );
	// Unlocks the buffer.  Be sure to call this as soon as you are
	// done processing the frame.

NxI32 zVidcapGetAvgFrameTimeInMils( char *deviceName=(char*)0 );
	// Get the avg speed at which frames are arriving from this device

void zVidcapGetBitmapDesc( char *deviceName, NxI32 &w, NxI32 &h, NxI32 &d );
	// Fetches the bitmap description.
	// Pass NULL to deviceName for last device
	// w = width in pixels, h = height in pixels, d = depth in bytes
	// Note that the image will be converted from 
	// bottom to top ordering and also from BGR to RGB automatically

NxI32 zVidcapShowFilterPropertyPageModalDialog( char *deviceName=(char*)0 );
	// Shows a modal dialog box for the device's filter properties
	// Returns 1 if the vidcap system has changed its size
	// in which case, if you do any high-level buffering of the dimensions,
	// you should update it.

NxI32 zVidcapShowPinPropertyPageModalDialog( char *deviceName=(char*)0 );
	// Shows a modal dialog box for the device's output pin properties
	// Returns 1 if the vidcap system has changed its size
	// in which case, if you do any high-level buffering of the dimensions,
	// you should update it.

void zVidcapAVISeek();
	// Used to seek an avi source

#endif