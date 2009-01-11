#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma warning(disable:4995)
#pragma warning(disable:4996)


#include "common/snippets/sutil.h"
#include "GuiTui.h"
#include "dxstdafx.h"
#include "common/snippets/log.h"

#define MAXMESSAGE 8

static ID3DXFont*              g_pFont = NULL;         // Font for drawing text
static ID3DXFont*              g_pLargeFont = NULL;    // Font for drawing text
static ID3DXSprite*            g_pTextSprite = NULL;   // Sprite for batching draw text calls
static ID3DXFont*              g_pTinyFont=0;          // Font for drawing text

#include "common/snippets/cparser.h"
#include "common/tui/tui.h"

extern HeF32 gFPS;

HeI32 gWINDOW_WIDE=1024;
HeI32 gWINDOW_TALL=768;

#define STATIC_ID_START 10000


GuiTui *gGuiTui=0;

static CDXUTDialog *console = 0;

//==================================================================================
void CALLBACK gOnUIPageEvent( UINT nEvent, HeI32 nControlID, CDXUTControl* pControl,void *pUserContext )
{
	CDXUTDialog *dialog = pControl->m_pDialog;

	void *userdata = dialog->GetUserData();
	if ( userdata )
	{
		TuiElement *page = (TuiElement *) userdata;
		TextUserInterface *tui = gTui;

		if ( page )
		{
			TuiElement *ret = page->GetElement( nControlID-1 );

			switch ( ret->GetType() )
			{
				case TT_MULTI_SELECT:
					{
						switch( nEvent )
						{
							case EVENT_LISTBOX_ITEM_DBLCLK:
							{
//								DXUTListBoxItem *pItem = ((CDXUTListBox *)pControl)->GetItem( ((CDXUTListBox *)pControl)->GetSelectedIndex( -1 ) );
								break;
							}
							case EVENT_LISTBOX_SELECTION:
							{
								CDXUTListBox *pListBox = (CDXUTListBox *)pControl;
								TuiChoiceVector &choices = ret->GetChoices();
								HeI32 count = (HeI32)choices.size();
								for (HeI32 i=0; i<count; i++)
								{
									DXUTListBoxItem *item = pListBox->GetItem(i);
									TuiChoice  &choice = choices[i];
									assert(item);
									if ( item )
									{
										if ( choice.GetState() != item->bSelected )
										{
											choice.SetState(item->bSelected);
											const char *args[2];
											args[0] = choice.GetArg().Get();

											if ( choice.GetState() )
												args[1] = "true";
											else
												args[1] = "false";

											page->ExecuteElement( nControlID-1, 2, args, tui, false );
										}
									}
								}

							}
						}
					}
					break;
				case TT_SLIDER:
					{
						CDXUTSlider *slider = (CDXUTSlider *) pControl;
						HeI32 v = slider->GetValue();
						HeF32 fv = ret->GetSliderValue(v);
						char scratch[512];
  						sprintf(scratch,"%0.3f", fv );

						const char *args[1];
						args[0] = scratch;
						page->ExecuteElement( nControlID-1, 1, args, tui, false );
					}
					break;
				case TT_COMBO:
					{
						CDXUTComboBox *combo = (CDXUTComboBox *) pControl;
						DXUTComboBoxItem *pItem = combo->GetSelectedItem();
						wchar_t *string = pItem->strText;
						char scratch[512];
						wcstombs(scratch, string, 512 );
						const char *args[1];
						args[0] = scratch;
						page->ExecuteElement( nControlID-1, 1, args,tui, false );
					
 						// now, do we need to hide any items (or show them)?
 						page->OnComboBoxChange( scratch, nControlID-1 );
					}
					break;
				case TT_BUTTON:
				case TT_PAGE:
					page->ExecuteElement( nControlID-1, 0, 0, tui, false );
					break;
				case TT_CHECKBOX:
					{
						CDXUTCheckBox *pCheck = (CDXUTCheckBox *)pControl;
						bool state = pCheck->GetChecked();
						const char *args[1];
						if ( state )
						{
							args[0] = "true";
						}
						else
						{
							args[0] = "false";
						}
						page->ExecuteElement( nControlID-1, 1, args, tui, false );

 						// now, do we need to hide any items (or show them)?
 						page->OnCheckboxChange( state, nControlID-1 );
					}
					break;
			}
			if ( tui )
			{
				if ( ret && ret->GetType() == TT_PAGE )
				{
					// tell the last page it is now exiting
					TuiElement *exitingPage = tui->GetPage();
					if ( exitingPage )
					{
						exitingPage->OnExit( exitingPage );
					}

					tui->SetPage(ret);

					// tell new page it is now loading
					TuiElement *loadingPage = tui->GetPage();
					if ( loadingPage )
					{
						loadingPage->OnLoad( loadingPage );
					}
				}
			}
		}
	}
	else
	{
		// it's for the console
		CDXUTEditBox *edit = (CDXUTEditBox *) pControl;
		switch ( nEvent )
		{
			case EVENT_EDITBOX_STRING:
				{
					LPCWSTR text = edit->GetText();
					char scratch[1024];
					WideToChar(text,scratch,1024);
					CPARSER.Parse("%s",scratch);
					edit->SetText(L"",true);
				}
				break;
			case EVENT_EDITBOX_CHANGE:
				break;
			default:
				break;
		}
	}

}

//==================================================================================
GuiTui::GuiTui(void *device,void *hwnd,void *instance,CDXUTDialogResourceManager *pManager) :
	mConsoleKeyDown( false )
{
	mConsoleIndex = 0;
	mShowConsole = false;
	mCapture     = false;
	mDtime       = 1.0f / 60.0f;
	mDialogResourceManager = pManager;
	if ( device )
	{
		IDirect3DDevice9 *pd3dDevice = (IDirect3DDevice9 *) device;
		D3DXCreateSprite( pd3dDevice, &g_pTextSprite );

//    D3DXCreateFont(pd3dDevice,  9, 0, 0,       1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Courier",     &g_pTinyFont );

 		D3DXCreateFont(pd3dDevice,  7, 0, 0,       1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Courier",     &g_pTinyFont );

		D3DXCreateFont(pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial",       &g_pFont );
		D3DXCreateFont(pd3dDevice, 26, 0, 0,       1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial Black", &g_pLargeFont );

	  mWidth = 1024;
	  mHeight = 768;

		gGuiTui = this;
	}
  CPARSER.AddFallback(this);
}

//==================================================================================
GuiTui::~GuiTui(void)
{
	SAFE_RELEASE(g_pFont);
	SAFE_RELEASE(g_pLargeFont);
	SAFE_RELEASE(g_pTextSprite);
	SAFE_RELEASE(g_pTinyFont);
}

//==================================================================================
HeI32 GuiTui::CommandCallback(HeI32 token,HeI32 count,const char **arglist)
{
	HeI32 ret = 0;


	return ret;
}

bool GuiTui::firstChance(const char *buff)
{
  bool ret = false;

  return ret;
}

//==================================================================================
bool GuiTui::HasCursorFocus()
{
	return true;
}

//==================================================================================
void GuiTui::HideAllGuiForTuiPages()
{
}

//==================================================================================
bool GuiTui::MsgProc(void *hwnd,HeU32 msg,HeU32 wparam,HeU32 lparam)
{
	bool ret = false;

	TextUserInterface *tui = gTui;

	if ( tui && tui->GetShowGraphics() )
	{
		TuiElement *page = tui->GetPage();
		if ( page )
		{
			#if defined(WIN32)
			CDXUTDialog *dialog = (CDXUTDialog *)page->GetUserPtr();
			if ( dialog )
			{
				ret = dialog->MsgProc( (HWND) hwnd, (UINT) msg,(WPARAM) wparam, (LPARAM) lparam );
			}
			#endif
		}
	}

	if ( console )
	{
		bool ignore = false;

		if ( msg == WM_KEYDOWN )
		{
			if ( (wparam == 126 || wparam == 96 || wparam == 9 ) )
			{
				ignore = true;

				if ( !mConsoleKeyDown )
				{
					mConsoleKeyDown = true;
					if ( mShowConsole )
					{
						mShowConsole = false;
					}
					else
					{
						mShowConsole = true;

						// indicate I handled this msg.
						ret = true;
					}
				}
			}
		}
		else if ( msg == WM_KEYUP )
		{
			if ( (wparam == 126 || wparam == 96 || wparam == 9 ) )
			{
				ignore = true;
				mConsoleKeyDown = false;
			}
		}


		// if the user didn't hit one of the "special" keys, put key hit in the console
		if ( !ignore && mShowConsole )
		{
 			ret = console->MsgProc( (HWND) hwnd, (UINT) msg,(WPARAM) wparam, (LPARAM) lparam );
		}
	}

	return ret;
}

//==================================================================================
void GuiTui::OnMouseDown(HeU32 uiX, HeU32 uiY, HeU32 uiButtons)
{
}

//==================================================================================
void GuiTui::OnMouseMove(HeU32 uiX, HeU32 uiY, HeU32 uiButtons)
{
}

//==================================================================================
void GuiTui::OnMouseUp(HeU32 uiX, HeU32 uiY, HeU32 uiButtons)
{
}

//==================================================================================
//*** The DirectX GUI version
//==================================================================================

const char *GuiTui::getCurrentPage(void)
{
  const char *ret="";

  if ( gTui )
  {
    TuiElement *page = gTui->GetPage();
    if ( page )
    {
      ret = page->GetName().Get();
    }
  }

  return ret;
}

void GuiTui::Render(void)
{
	HeI32 fontSize = 16;

  TextUserInterface *tui = gTui;


	if (gGuiTui == 0 )
		return;

	if ( mCapture )
		return;

  if ( gLog )
  {
    const char *msg = gLog->GetLogMessage();
    if ( msg )
    {
			AddConsoleMessage(msg);
    }
  }

	{

		if ( console == 0 )
		{
			console = MEMALLOC_NEW(CDXUTDialog);
			console->Init(mDialogResourceManager);
			console->SetCallback( gOnUIPageEvent );
			console->SetUserData(0);
			console->AddEditBox( 1, L"", 0, gWINDOW_TALL-26*2, 1000, 36, true, 0 );
		}

 		if ( mShowConsole )
 		{

 			CDXUTEditBox *edit = console->GetEditBox(1);

 			if ( edit )
 			{
			  edit->SetFocused();
 			}

			console->OnRender( mDtime );

  		if ( g_pTinyFont && g_pTextSprite )
 			{
 		  	CDXUTTextHelper txtHelper( g_pTinyFont, g_pTextSprite, fontSize );

 				txtHelper.Begin();
   			txtHelper.SetForegroundColor( 0xFFFFFF00 );

				HeI32 maxPos		= mHeight - 2*fontSize - 2;
				HeI32 addAmt		= fontSize;
				HeI32 menuHeight  = fontSize;
				HeI32 startPos	= maxPos - addAmt * CONSOLE_LINES - menuHeight;

				// ok..print console text!
 				for (HeI32 i=0; i<CONSOLE_LINES; i++)
 				{
 					const char *msg = GetConsoleMessage(i);
					if ( !msg )
						break;
   				wchar_t wfoo[1024];
 	  			CharToWide(msg,wfoo,1024);
					txtHelper.SetInsertionPos( 0, startPos );
					startPos += addAmt;
   				txtHelper.DrawTextLine(wfoo);
 				}
 		  	txtHelper.End();
 			}
 		}
 		else if ( tui->GetShowGraphics() )
 		{
  		if ( g_pTinyFont && g_pTextSprite )
 			{
	  		CDXUTTextHelper txtHelper( g_pTinyFont, g_pTextSprite, fontSize );

 				txtHelper.Begin();
 				txtHelper.SetForegroundColor( 0xFFD0D0D0 );

   				// ok..print console text!

				HeI32 maxPos		= mHeight - fontSize;
				HeI32 addAmt		= fontSize;
				HeI32 startPos	= maxPos - addAmt * 5;

 				for (HeI32 i=(CONSOLE_LINES-5); i<CONSOLE_LINES; i++)
 				{
 					const char *msg = GetConsoleMessage(i);
					if ( !msg )
						break;
  				wchar_t wfoo[1024];
	  			CharToWide(msg,wfoo,1024);
					txtHelper.SetInsertionPos( 0, startPos );
 					txtHelper.DrawTextLine(wfoo);
					startPos += addAmt;
 				}
	  		txtHelper.End();
 			}
 		}


		if ( tui && tui->GetShowGraphics() && !mShowConsole )
		{
			TuiElement *page = tui->GetPage();

			if ( page )
			{
				CDXUTDialog *dialog = (CDXUTDialog *)page->GetUserPtr();

 				bool dialogExists = ( dialog != 0 );

 				// k, if the dialog doesn't exist or the page needs updating, go in
 				if ( !dialogExists || page->GetNeedsUpdating() )
 				{
 					// first, let it figure out what is now visible
 					page->DetermineVisibleElements( 0, -1 );

 					// if the dialog does not exist, have it determine rescripted items
 					if ( !dialogExists )
 					{
 						page->DetermineRescriptedItems( 0, -1 );
 					}
 					else if ( page->GetNeedsUpdating() )
 					{
 						dialogExists = false;
 						page->SetUserPtr( 0 );
 						page->SetNeedsUpdating( false );
 						delete dialog;
 					}
 				}

 				if ( dialogExists == false )
				{
					dialog = MEMALLOC_NEW(CDXUTDialog);
			    dialog->Init(mDialogResourceManager);
					dialog->SetCallback( gOnUIPageEvent );

					dialog->SetUserData(page);

					const PageLayout &pl   = page->getPageLayout();

					HeI32 bwidth             = pl.mButtonWidth;
					HeI32 lmargin            = pl.mLayoutLeftMargin;
					HeI32 layout_width       = pl.mLayoutWidth;
					HeI32 button_spacing     = pl.mButtonSpacing;
					HeI32 bhit               = pl.mButtonHeight;
					HeI32 checkbox_spacing   = DFLT_CHECKBOX_SPACING;

					HeI32 text_spacing       = pl.mTextSpacing;
					HeI32 slider_spacing     = pl.mSliderSpacing;
					HeI32 slider_text_width  = pl.mSliderTextWidth;
					HeI32 slider_height      = pl.mSliderHeight;
					HeI32 slider_width       = pl.mSliderWidth;

					HeI32 combo_width        = pl.mComboWidth;
					HeI32 combo_spacing      = pl.mComboSpacing;
					HeI32 combo_height       = pl.mComboHeight;
					HeI32 combo_dropdown     = pl.mComboDropdown;

					HeI32 list_width         = pl.mListWidth;

					HeI32 xloc     = lmargin;
					HeI32 yloc     = 10;
					HeI32 id       = 0;
					HeI32 staticId = STATIC_ID_START;

					page->SetUserPtr(dialog);

					TuiElementVector &elements = page->GetElements();
					TuiElementVector::iterator i;

					for (i=elements.begin(); i!=elements.end(); ++i)
					{
						++id;

						TuiElement *tui = (*i);
 						if ( tui && tui->IsVisible() )
						{
							switch ( tui->GetType() )
							{
								case TT_TEXT:
								{
									wchar_t scratch[512];
									mbstowcs(scratch, tui->GetText(), 512 );
									if ( xloc != lmargin )
									{
										xloc = lmargin;
										yloc+= text_spacing;
									}
									dialog->AddStatic( staticId, scratch, xloc, yloc, 500, 20, 0 );

									// k, now make sure it is justified to the left
									CDXUTControl *ctrl = dialog->GetControl( staticId );
									if ( ctrl && ( ctrl->GetType() == DXUT_CONTROL_STATIC ) )
									{
										// set color
										ctrl->SetUserData( (void *)tui->GetTextColor() );

										CDXUTElement *element = ctrl->GetElement( 0 );
										if ( element )
										{
											// make sure it is left-justified, and vertically centered
											element->dwTextFormat = (DT_LEFT | DT_VCENTER);
										}
									}

									// move next element down
									yloc += text_spacing;

									// increment static id
									++staticId;
								}
								break;

								case TT_CHECKBOX:
									{
										wchar_t scratch[512];
										mbstowcs(scratch, tui->GetName().Get(), 512 );
										dialog->AddCheckBox( id, scratch, xloc, yloc, bwidth, bhit, tui->GetState(), 0);
										xloc+=(bwidth+10);
										if ( xloc > (layout_width-(bwidth+10)) )
										{
											xloc = lmargin;
											yloc += checkbox_spacing;
										}
									}
									break;
								case TT_SLIDER:
									{
										if ( xloc != lmargin )
										{
											xloc = lmargin;
											yloc += slider_spacing;
										}
										wchar_t scratch[512];
										mbstowcs(scratch, tui->GetName().Get(), 512 );
										dialog->AddStatic(0, scratch, xloc, yloc, slider_text_width, slider_height, 0 );
										dialog->AddSlider(id, xloc+slider_text_width+10, yloc, slider_width, slider_height, 0, 1000, 0, false, 0);
										dialog->AddStatic(id+1000, L"10mhz", xloc+slider_text_width+10+slider_width+10, yloc, slider_text_width, slider_height, 0 );
										xloc = lmargin;
										yloc += slider_spacing;
									}
									break;
								case TT_COMBO:
									{
										if ( xloc != lmargin )
										{
											xloc = lmargin;
											yloc += combo_spacing;
										}
										wchar_t scratch[512];
										mbstowcs(scratch, tui->GetName().Get(), 512 );
										dialog->AddStatic(0, scratch, xloc, yloc, combo_width, combo_height, 0 );
										CDXUTComboBox *pCombo;
										dialog->AddComboBox(id, xloc, yloc+combo_height, combo_width, combo_height, 0, false, &pCombo);
										if ( pCombo )
										{
											// ok..populate the combo box now..
											TuiChoiceVector &choices = tui->GetChoices();
											TuiChoiceVector::iterator i;
											HeI32 index = 0;
											for (i=choices.begin(); i!=choices.end(); ++i)
											{
												wchar_t scratch[512];
												mbstowcs(scratch, (*i).GetChoice().Get(), 512 );
												pCombo->AddItem( scratch, (LPVOID) index );
												index++;
											}

											pCombo->SetDropHeight(combo_dropdown);

										}
										xloc = lmargin;
										yloc += combo_spacing;
									}
									break;
								case TT_MULTI_SELECT:
									{
										if ( xloc != lmargin )
										{
											xloc = lmargin;
											yloc+=30;
										}
										TuiChoiceVector &choices = tui->GetChoices();

										HeI32 list_height = choices.size()*15;

										if ( (yloc+list_height) > 700 )
										{
											list_height = 700-yloc;
										}

										wchar_t scratch[512];
										mbstowcs(scratch, tui->GetName().Get(), 512 );
										dialog->AddStatic(0, scratch, xloc, yloc, 180, 20, 0 );
										dialog->AddListBox(id, xloc, yloc+26, list_width, list_height, CDXUTListBox::MULTISELECTION );
										CDXUTListBox *pListBox = dialog->GetListBox(id);
										if(pListBox)
										{
											// ok..populate the combo box now..
											TuiChoiceVector::iterator i;
											HeI32 index = 0;
											for (i=choices.begin(); i!=choices.end(); ++i)
											{
												wchar_t scratch[512];
												mbstowcs(scratch, (*i).GetChoice().Get(), 512 );
												pListBox->AddItem( scratch, (LPVOID) index );
												index++;
											}
										}
										xloc = lmargin;
										yloc+=list_height;
									}
									break;
								case TT_BUTTON:
								case TT_PAGE:
									{
										wchar_t scratch[512];
										mbstowcs(scratch, tui->GetName().Get(), 512 );
										dialog->AddButton( id, scratch, xloc, yloc, bwidth, 20, 0);
										xloc += (bwidth+button_spacing);

										if ( xloc > (layout_width-(bwidth+button_spacing)) )
										{
											xloc = lmargin;
											yloc+=24;
										}
									}
									break;
							}
						}
					}
				}

				// SRM : do this only when dialog exists
 				if ( dialogExists )
				{
					// static's that are just static's have id's of 10000+
					HeI32 id = STATIC_ID_START;

					CDXUTControl *ctrl = dialog->GetControl( id++ );
					while ( ctrl )
					{
						if ( ctrl->GetType() == DXUT_CONTROL_STATIC )
						{
							CDXUTElement *element = ctrl->GetElement( 0 );
							void *userData        = ctrl->GetUserData();
							if ( element && userData )
							{
								//element->FontColor.Current = D3DCOLOR_ARGB(255, 255, 255, 0);
								element->FontColor.Current = (HeU32)( userData );
							}
						}
						ctrl = dialog->GetControl( id++ );
					}
				}

				dialog->OnRender(mDtime);

			}
		}

		{

      if ( tui /*&& tui->GetShowGraphics()*/ )
			{

				CDXUTTextHelper txtHelper( g_pFont, g_pTextSprite, 15 );

				// Output statistics
				txtHelper.Begin();
				txtHelper.SetInsertionPos( 5, 2 );
				txtHelper.SetForegroundColor( D3DXCOLOR( 0.8f, 0.8f, 0.8f, 0.8f ) );
				txtHelper.DrawTextLine( DXUTGetFrameStats() );
				wchar_t scratch[512];
				const char *stats = 0;

				if (stats )
				{
					mbstowcs(scratch, stats, 512);
					txtHelper.DrawTextLine( scratch );
				}

				if ( gTui->GetShowGraphics() )
				{
  				txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
					wchar_t temp[512];
          StringCchPrintf(temp,512, L"FPS: %3d", (HeI32)gFPS);
  				txtHelper.DrawTextLine(temp);
  				txtHelper.DrawTextLine( L"Press F1 to toggle UI" );
  				{
	  				txtHelper.DrawTextLine( L"Press ESC to exit" );
	  				txtHelper.DrawTextLine( L"Press TAB for Console" );
						txtHelper.DrawTextLine( L"Press 0-9 to control camera speed in 3d" );
            txtHelper.DrawTextLine( L"Press P to take a screen capture" );
					}

  			}

				txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

				txtHelper.End();
			}
		}

	}
}


void GuiTui::PrintText(const char *str,HeI32 x,HeI32 y,HeU32 color)
{
  CDXUTTextHelper txtHelper( g_pTinyFont, g_pTextSprite, 15 );

	txtHelper.Begin();
	txtHelper.SetInsertionPos( x, y );
	txtHelper.SetForegroundColor( D3DXCOLOR( 0.8f, 0.8f, 0.8f, 0.8f ) );

	wchar_t scratch[512];
	mbstowcs(scratch, str, 512);
	txtHelper.DrawTextLine( scratch );

	txtHelper.End();
}

//==================================================================================
// Update the UI's state.
//==================================================================================
void GuiTui::Update(void)
{
	TextUserInterface *tui = gTui;
	if(!tui)
		return;

	TuiElement *page       = tui->GetPage();
	if(!page)
		return;

#if defined(WIN32)
	CDXUTDialog *dialog    = (CDXUTDialog *)page->GetUserPtr();
	if(!dialog)
		return;
#endif

	// SRM : k, here we ask the globals if anything changed
	// and if so, we may need to update some elements!!!


	HeI32 id   = 0;
	TuiElementVector &elements = page->GetElements();
	TuiElementVector::iterator i;
	for (i=elements.begin(); i!=elements.end(); ++i)
	{
		id++;
		TuiElement *tui = (*i);
		if ( tui )
		{
			switch ( tui->GetType() )
			{
				case TT_CHECKBOX:
				{
					bool state = tui->GetState();
					CDXUTCheckBox *checkbox = dialog->GetCheckBox(id);
					if ( checkbox )
					{
						checkbox->SetChecked( state );
					}
					break;
				}
				case TT_SLIDER:
				{
					char actualVal[128], displayVal[128];
					tui->GetArg( 0, displayVal );
					tui->GetArg_Actual( 0, actualVal );
					CDXUTSlider *pslider = dialog->GetSlider(id);
					CDXUTStatic *label  = dialog->GetStatic(id+1000);
					if ( pslider && label )
					{
						HeF32 v = (HeF32) atof(actualVal);
						HeI32 slider = tui->GetSliderTick(v); // convert the slider value into a tick point 0-1000

						wchar_t wvalue[512];
						CharToWide( displayVal, wvalue, 512 );
						label->SetText( wvalue );
						pslider->SetValue( slider );
					}
					break;
				}
				case TT_COMBO:
				{
					char value[128];
					tui->GetArg( 0, value );
					CDXUTComboBox *combo = dialog->GetComboBox(id);
					if(combo && !combo->IsOpened())
					{
						bool found = false;
						size_t len = strlen(value);
						wchar_t wvalue[512];
						CharToWide(value,wvalue,512);
						//combo->SetSelectedByText(wvalue);
						UINT numItems = combo->GetNumItems();
						for(UINT index=0; index<numItems; index++)
						{
#pragma warning(disable:4996)
							DXUTComboBoxItem *item = combo->GetItem(index);
							if(item && !wcsnicmp(wvalue, item->strText, len))
							{
								combo->SetSelectedByIndex(index);
								found = true;
								break;
							}
						}
						//if(!found) combo->SetSelectedByIndex(0); // select none?
					}
					break;
				}
				case TT_MULTI_SELECT:
				{
//					CDXUTListBox *listbox = dialog->GetListBox(id);
//					assert(listbox);
					break;
				}
				case TT_BUTTON:
				case TT_PAGE:
				{
					break;
				}
			}
		}
	}
}

//==================================================================================
void GuiTui::SetWindow(void* hwnd)
{
}

//==================================================================================
bool GuiTui::ToggleUI(void)
{
	bool ret = false;
	TextUserInterface *tui = gTui;
	if ( tui ) 
		ret = tui->ToggleShowGraphics();

	return ret;
}

//==================================================================================
void GuiTui::NotifyWindowResized( HeU32 newWidth, HeU32 newHeight )
{
	mWidth	= newWidth;
	mHeight	= newHeight;

	// update our pages
	TextUserInterface *tui = gTui;
	if ( tui )
	{
		tui->NotifyAllPagesOfNewDimensions( newWidth, newHeight );
	}

	if ( console )
	{
		CDXUTEditBox *edit = console->GetEditBox( 1 );
		if ( edit )
		{
			// modify the console's position as well
			edit->SetLocation( 0, newHeight - DFLT_CONSOLE_HEIGHT );
			edit->SetSize( newWidth, DFLT_CONSOLE_HEIGHT );
		}
	}
}

//==================================================================================
void GuiTui::NotifySceneTypeChanged( bool hardwareOn )
{
	// k, now we need to go through and items that need to be disabled when
	// they need to know about the scene type turning on/off hardware
	TextUserInterface *tui = gTui;
	if ( tui )
	{
		// in addition, let it know so it can go through
		// and enable/disable appropriate items for visibility
		tui->NotifyHardwareStatus( hardwareOn );
	
		if ( hardwareOn )
		{
			// k, now we need to make sure that it is checked appropriately
			TuiElement *page = tui->GetElementOfTypeWithName( TT_PAGE, "Physics" );
			if ( page )
			{
				HeI32 index = page->GetElementIndexOfTypeWithName( TT_CHECKBOX, "Always Use 6DOF" );
				if ( index >= 0 )
				{
					const char *args[1] = { "true" };
					page->ExecuteElement( index, 1, args, tui, false );
				}
			}
		}
	}
}

//==================================================================================
void PageLayout::Modify( HeU32 newScreenWidth, HeU32 newScreenHeight )
{
	// k, we are being told what the new screen width and height is, so we might need
	// to adjust ourselves
	mLayoutWidth		= newScreenWidth;
    mLayoutLeftMargin	= newScreenWidth - 3*(DFLT_BUTTON_WIDTH+DFLT_BUTTON_SPACING) - DFLT_LAYOUT_RIGHT_MARGIN;
}

void GuiTui::onLostDevice(void)
{
	if ( g_pFont )      g_pFont->OnLostDevice();
	if ( g_pLargeFont ) g_pLargeFont->OnLostDevice();
	if ( g_pTinyFont )  g_pTinyFont->OnLostDevice();
	if ( g_pTextSprite ) g_pTextSprite->OnLostDevice();
}

void GuiTui::onResetDevice(void *device)
{
	if ( g_pFont )      g_pFont->OnResetDevice();
	if ( g_pLargeFont ) g_pLargeFont->OnResetDevice();
	if ( g_pTinyFont )  g_pTinyFont->OnResetDevice();
	if ( g_pTextSprite ) g_pTextSprite->OnResetDevice();
}

