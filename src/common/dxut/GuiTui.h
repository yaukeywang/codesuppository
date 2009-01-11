#ifndef GUITUI_H

#define GUITUI_H

class CDXUTDialog;
class CDXUTDialogResourceManager;

#include "common/snippets/cparser.h"

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

	bool MsgProc(void *hwnd,HeU32 msg,HeU32 wparam,HeU32 lparam);

	void OnMouseDown(HeU32 uiX, HeU32 uiY, HeU32 uiButtons);
	void OnMouseMove(HeU32 uiX, HeU32 uiY, HeU32 uiButtons);
	void OnMouseUp(HeU32 uiX, HeU32 uiY, HeU32 uiButtons);

	bool ToggleUI(void);

	void PrintText(const char *str,HeI32 x,HeI32 y,HeU32 color);

	void SetWindow(void* hwnd);


	void NotifyWindowResized( HeU32 newWidth, HeU32 newHeight );

	void NotifySceneTypeChanged( bool hardwareOn );

	void GetWindowSize( HeU32 &width, HeU32 &height );

	const char * GetConsoleMessage(HeI32 index)
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

	HeI32 GetConsoleMessageIndex( void ) const
	{
		return mConsoleIndex;
	}

  bool isConsoleActive(void ) const { return mShowConsole; };

public: //-- CommandParserInterface overrides

	virtual HeI32 CommandCallback(HeI32 token,HeI32 count,const char **arglist);

	void onLostDevice(void);
	void onResetDevice(void *device);

  bool firstChance(const char *buff);


private:

	HeI32	mWidth;
	HeI32 mHeight;
	bool mConsoleKeyDown;
  void *mNullHwnd;
  bool mShowConsole;
  bool mCapture;
  HeF32 mDtime;
  CDXUTDialogResourceManager *mDialogResourceManager; // manager for shared resources of dialogs
	HeI32                 mConsoleIndex;
	ConsoleMessage		  mConsole[CONSOLE_LINES];
};

//==================================================================================
inline void GuiTui::GetWindowSize( HeU32 &width, HeU32 &height )
{
	width  = mWidth;
	height = mHeight;
}

extern GuiTui *gGuiTui;

#endif

