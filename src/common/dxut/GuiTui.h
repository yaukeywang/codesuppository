#ifndef GUITUI_H

#define GUITUI_H

class CDXUTDialog;
class CDXUTDialogResourceManager;

#include "cparser.h"

#define CONSOLE_LINES			100

class ConsoleMessage
{
public:
	ConsoleMessage(void)
	{
		memset( mMessage, 0, 256 );
	}

	void SetConsoleMessage(const char *msg)
	{
		strcpy(mMessage,msg);
	}

	const char * GetMsg(void) const { return mMessage; };

private:
	char mMessage[256];
};

//==================================================================================
//==================================================================================
class GuiTui : public CommandParserInterface
{
public:
	GuiTui(void *device,void *hwnd,void *instance,CDXUTDialogResourceManager *pManager);
	~GuiTui(void);

	void Render(void);

  const char *getCurrentPage(void);

	// Update the UI's state.
	void Update(void);

	bool          HasCursorFocus();
	void          HideAllGuiForTuiPages();

	bool MsgProc(void *hwnd,NxU32 msg,NxU32 wparam,NxU32 lparam);

	void OnMouseDown(NxU32 uiX, NxU32 uiY, NxU32 uiButtons);
	void OnMouseMove(NxU32 uiX, NxU32 uiY, NxU32 uiButtons);
	void OnMouseUp(NxU32 uiX, NxU32 uiY, NxU32 uiButtons);

	bool ToggleUI(void);

	void PrintText(const char *str,NxI32 x,NxI32 y,NxU32 color);

	void SetWindow(void* hwnd);


	void NotifyWindowResized( NxU32 newWidth, NxU32 newHeight );

	void NotifySceneTypeChanged( bool hardwareOn );

	void GetWindowSize( NxU32 &width, NxU32 &height );

	const char * GetConsoleMessage(NxI32 index)
	{
		index = mConsoleIndex+index;

		if ( index >= CONSOLE_LINES )
			index -= CONSOLE_LINES;
		else if ( index < 0 )
			return 0;

		return mConsole[index].GetMsg();
	}

	void AddConsoleMessage(const char *msg, ...)
	{
    char scratch[4096];
    _vsnprintf( scratch, 4095, msg, (char*)(&msg+1) );
		mConsole[mConsoleIndex].SetConsoleMessage(scratch);
		mConsoleIndex++;
		if ( mConsoleIndex == CONSOLE_LINES )
			mConsoleIndex = 0;
	}

	NxI32 GetConsoleMessageIndex( void ) const
	{
		return mConsoleIndex;
	}

  bool isConsoleActive(void ) const { return mShowConsole; };

public: //-- CommandParserInterface overrides

	virtual NxI32 CommandCallback(NxI32 token,NxI32 count,const char **arglist);

	void onLostDevice(void);
	void onResetDevice(void *device);

  bool firstChance(const char *buff);


private:

	NxI32	mWidth;
	NxI32 mHeight;
	bool mConsoleKeyDown;
  void *mNullHwnd;
  bool mShowConsole;
  bool mCapture;
  NxF32 mDtime;
  CDXUTDialogResourceManager *mDialogResourceManager; // manager for shared resources of dialogs
	NxI32                 mConsoleIndex;
	ConsoleMessage		  mConsole[CONSOLE_LINES];
};

//==================================================================================
inline void GuiTui::GetWindowSize( NxU32 &width, NxU32 &height )
{
	width  = mWidth;
	height = mHeight;
}

extern GuiTui *gGuiTui;

#endif

