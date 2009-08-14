//--------------------------------------------------------------------------------------
// File: DXUTgui.h
//
// Desc: 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#ifndef DXUT_GUI_H
#define DXUT_GUI_H

#include <usp10.h>
#include <dimm.h>

#include "UserMemAlloc.h"

//--------------------------------------------------------------------------------------
// Defines and macros 
//--------------------------------------------------------------------------------------
#define EVENT_BUTTON_CLICKED                0x0101
#define EVENT_COMBOBOX_SELECTION_CHANGED    0x0201
#define EVENT_RADIOBUTTON_CHANGED           0x0301
#define EVENT_CHECKBOX_CHANGED              0x0401
#define EVENT_SLIDER_VALUE_CHANGED          0x0501
#define EVENT_EDITBOX_STRING                0x0601
// EVENT_EDITBOX_CHANGE is sent when the listbox content changes
// due to user input.
#define EVENT_EDITBOX_CHANGE                0x0602
#define EVENT_LISTBOX_ITEM_DBLCLK           0x0701
// EVENT_LISTBOX_SELECTION is fired off when the selection changes in
// a single selection list box.
#define EVENT_LISTBOX_SELECTION             0x0702
#define EVENT_LISTBOX_SELECTION_END         0x0703


//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
class CDXUTDialogResourceManager;
class CDXUTControl;
class CDXUTButton;
class CDXUTStatic;
class CDXUTCheckBox;
class CDXUTRadioButton;
class CDXUTComboBox;
class CDXUTSlider;
class CDXUTEditBox;
class CDXUTIMEEditBox;
class CDXUTListBox;
class CDXUTScrollBar;
class CDXUTElement;
struct DXUTElementHolder;
struct DXUTTextureNode;
struct DXUTFontNode;
typedef VOID (CALLBACK *PCALLBACKDXUTGUIEVENT) ( UINT nEvent, NxI32 nControlID, CDXUTControl* pControl, void* pUserContext );


//--------------------------------------------------------------------------------------
// Enums for pre-defined control types
//--------------------------------------------------------------------------------------
enum DXUT_CONTROL_TYPE 
{ 
    DXUT_CONTROL_BUTTON, 
    DXUT_CONTROL_STATIC, 
    DXUT_CONTROL_CHECKBOX,
    DXUT_CONTROL_RADIOBUTTON,
    DXUT_CONTROL_COMBOBOX,
    DXUT_CONTROL_SLIDER,
    DXUT_CONTROL_EDITBOX,
    DXUT_CONTROL_IMEEDITBOX,
    DXUT_CONTROL_LISTBOX,
    DXUT_CONTROL_SCROLLBAR,
};

enum DXUT_CONTROL_STATE
{
    DXUT_STATE_NORMAL = 0,
    DXUT_STATE_DISABLED,
    DXUT_STATE_HIDDEN,
    DXUT_STATE_FOCUS,
    DXUT_STATE_MOUSEOVER,
    DXUT_STATE_PRESSED,
};

#define MAX_CONTROL_STATES 6

struct DXUTBlendColor
{
    void Init( D3DCOLOR defaultColor, D3DCOLOR disabledColor = D3DCOLOR_ARGB(200, 128, 128, 128), D3DCOLOR hiddenColor = 0 );
    void Blend( UINT iState, NxF32 fElapsedTime, NxF32 fRate = 0.7f );
    
    D3DCOLOR  States[ MAX_CONTROL_STATES ]; // Modulate colors for all possible control states
    D3DXCOLOR Current;
};


//-----------------------------------------------------------------------------
// Contains all the display tweakables for a sub-control
//-----------------------------------------------------------------------------
class CDXUTElement
{
public:
    void SetTexture( UINT iTexture, RECT* prcTexture, D3DCOLOR defaultTextureColor = D3DCOLOR_ARGB(255, 255, 255, 255) );
    void SetFont( UINT iFont, D3DCOLOR defaultFontColor = D3DCOLOR_ARGB(255, 255, 255, 255), DWORD dwTextFormat = DT_CENTER | DT_VCENTER );
    
    void Refresh();
    
    UINT iTexture;          // Index of the texture for this Element 
    UINT iFont;             // Index of the font for this Element
    DWORD dwTextFormat;     // The format argument to DrawText

    RECT rcTexture;         // Bounding rect of this element on the composite texture

    DXUTBlendColor TextureColor;
    DXUTBlendColor FontColor;
};


//-----------------------------------------------------------------------------
// All controls must be assigned to a dialog, which handles
// input and rendering for the controls.
//-----------------------------------------------------------------------------
class CDXUTDialog
{
    friend class CDXUTDialogResourceManager;

public:
    CDXUTDialog();
    ~CDXUTDialog();

    // Need to call this now
    void Init( CDXUTDialogResourceManager* pManager, bool bRegisterDialog = true );
    void Init( CDXUTDialogResourceManager* pManager, bool bRegisterDialog, LPCWSTR pszControlTextureFilename );
    void Init( CDXUTDialogResourceManager* pManager, bool bRegisterDialog, LPCWSTR szControlTextureResourceName, HMODULE hControlTextureResourceModule );

    // Windows message handler
    bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

    // Control creation
    HRESULT AddStatic( NxI32 ID, LPCWSTR strText, NxI32 x, NxI32 y, NxI32 width, NxI32 height, bool bIsDefault=false, CDXUTStatic** ppCreated=NULL );
    HRESULT AddButton( NxI32 ID, LPCWSTR strText, NxI32 x, NxI32 y, NxI32 width, NxI32 height, UINT nHotkey=0, bool bIsDefault=false, CDXUTButton** ppCreated=NULL );
    HRESULT AddCheckBox( NxI32 ID, LPCWSTR strText, NxI32 x, NxI32 y, NxI32 width, NxI32 height, bool bChecked=false, UINT nHotkey=0, bool bIsDefault=false, CDXUTCheckBox** ppCreated=NULL );
    HRESULT AddRadioButton( NxI32 ID, UINT nButtonGroup, LPCWSTR strText, NxI32 x, NxI32 y, NxI32 width, NxI32 height, bool bChecked=false, UINT nHotkey=0, bool bIsDefault=false, CDXUTRadioButton** ppCreated=NULL );
    HRESULT AddComboBox( NxI32 ID, NxI32 x, NxI32 y, NxI32 width, NxI32 height, UINT nHotKey=0, bool bIsDefault=false, CDXUTComboBox** ppCreated=NULL );
    HRESULT AddSlider( NxI32 ID, NxI32 x, NxI32 y, NxI32 width, NxI32 height, NxI32 min=0, NxI32 max=100, NxI32 value=50, bool bIsDefault=false, CDXUTSlider** ppCreated=NULL );
    HRESULT AddEditBox( NxI32 ID, LPCWSTR strText, NxI32 x, NxI32 y, NxI32 width, NxI32 height, bool bIsDefault=false, CDXUTEditBox** ppCreated=NULL );
    HRESULT AddIMEEditBox( NxI32 ID, LPCWSTR strText, NxI32 x, NxI32 y, NxI32 width, NxI32 height, bool bIsDefault=false, CDXUTIMEEditBox** ppCreated=NULL );
    HRESULT AddListBox( NxI32 ID, NxI32 x, NxI32 y, NxI32 width, NxI32 height, DWORD dwStyle=0, CDXUTListBox** ppCreated=NULL );
    HRESULT AddControl( CDXUTControl* pControl );
    HRESULT InitControl( CDXUTControl* pControl );

    // Control retrieval
    CDXUTStatic*      GetStatic( NxI32 ID ) { return (CDXUTStatic*) GetControl( ID, DXUT_CONTROL_STATIC ); }
    CDXUTButton*      GetButton( NxI32 ID ) { return (CDXUTButton*) GetControl( ID, DXUT_CONTROL_BUTTON ); }
    CDXUTCheckBox*    GetCheckBox( NxI32 ID ) { return (CDXUTCheckBox*) GetControl( ID, DXUT_CONTROL_CHECKBOX ); }
    CDXUTRadioButton* GetRadioButton( NxI32 ID ) { return (CDXUTRadioButton*) GetControl( ID, DXUT_CONTROL_RADIOBUTTON ); }
    CDXUTComboBox*    GetComboBox( NxI32 ID ) { return (CDXUTComboBox*) GetControl( ID, DXUT_CONTROL_COMBOBOX ); }
    CDXUTSlider*      GetSlider( NxI32 ID ) { return (CDXUTSlider*) GetControl( ID, DXUT_CONTROL_SLIDER ); }
    CDXUTEditBox*     GetEditBox( NxI32 ID ) { return (CDXUTEditBox*) GetControl( ID, DXUT_CONTROL_EDITBOX ); }
    CDXUTIMEEditBox*  GetIMEEditBox( NxI32 ID ) { return (CDXUTIMEEditBox*) GetControl( ID, DXUT_CONTROL_IMEEDITBOX ); }
    CDXUTListBox*     GetListBox( NxI32 ID ) { return (CDXUTListBox*) GetControl( ID, DXUT_CONTROL_LISTBOX ); }

    CDXUTControl* GetControl( NxI32 ID );
    CDXUTControl* GetControl( NxI32 ID, UINT nControlType );
    CDXUTControl* GetControlAtPoint( POINT pt );

    bool GetControlEnabled( NxI32 ID );
    void SetControlEnabled( NxI32 ID, bool bEnabled );

    void ClearRadioButtonGroup( UINT nGroup );
    void ClearComboBox( NxI32 ID );

    // Access the default display Elements used when adding new controls
    HRESULT       SetDefaultElement( UINT nControlType, UINT iElement, CDXUTElement* pElement );
    CDXUTElement* GetDefaultElement( UINT nControlType, UINT iElement );

    // Methods called by controls
    void SendEvent( UINT nEvent, bool bTriggeredByUser, CDXUTControl* pControl );
    void RequestFocus( CDXUTControl* pControl );

    // Render helpers
    HRESULT DrawRect( RECT* pRect, D3DCOLOR color );
    HRESULT DrawPolyLine( POINT* apPoints, UINT nNumPoints, D3DCOLOR color );
    HRESULT DrawSprite( CDXUTElement* pElement, RECT* prcDest );
    HRESULT CalcTextRect( LPCWSTR strText, CDXUTElement* pElement, RECT* prcDest, NxI32 nCount = -1 );
    HRESULT DrawText( LPCWSTR strText, CDXUTElement* pElement, RECT* prcDest, bool bShadow = false, NxI32 nCount = -1 );

    // Attributes
    bool GetVisible() { return m_bVisible; }
    void SetVisible( bool bVisible ) { m_bVisible = bVisible; }
    bool GetMinimized() { return m_bMinimized; }
    void SetMinimized( bool bMinimized ) { m_bMinimized = bMinimized; }
    void SetBackgroundColors( D3DCOLOR colorAllCorners ) { SetBackgroundColors( colorAllCorners, colorAllCorners, colorAllCorners, colorAllCorners ); }
    void SetBackgroundColors( D3DCOLOR colorTopLeft, D3DCOLOR colorTopRight, D3DCOLOR colorBottomLeft, D3DCOLOR colorBottomRight );
    void EnableCaption( bool bEnable ) { m_bCaption = bEnable; }
    NxI32 GetCaptionHeight() const { return m_nCaptionHeight; }
    void SetCaptionHeight( NxI32 nHeight ) { m_nCaptionHeight = nHeight; }
    void SetCaptionText( const WCHAR *pwszText ) { StringCchCopy( m_wszCaption, sizeof(m_wszCaption)/sizeof(m_wszCaption[0]), pwszText); }
    void GetLocation( POINT &Pt ) const { Pt.x = m_x; Pt.y = m_y; }
    void SetLocation( NxI32 x, NxI32 y ) { m_x = x; m_y = y; }
    void SetSize( NxI32 width, NxI32 height ) { m_width = width; m_height = height;  }
    NxI32 GetWidth() { return m_width; }
    NxI32 GetHeight() { return m_height; }

    static void SetRefreshTime( NxF32 fTime ){ s_fTimeRefresh = fTime; }

    static CDXUTControl* GetNextControl( CDXUTControl* pControl );
    static CDXUTControl* GetPrevControl( CDXUTControl* pControl );

    void RemoveControl( NxI32 ID );
    void RemoveAllControls();

    // Sets the callback used to notify the app of control events
    void SetCallback( PCALLBACKDXUTGUIEVENT pCallback, void* pUserContext = NULL );
    void EnableNonUserEvents( bool bEnable ) { m_bNonUserEvents = bEnable; }
    void EnableKeyboardInput( bool bEnable ) { m_bKeyboardInput = bEnable; }
    void EnableMouseInput( bool bEnable ) { m_bMouseInput = bEnable; }
    bool IsKeyboardInputEnabled() const { return m_bKeyboardInput; }

    // Device state notification
    void Refresh();
    HRESULT OnRender( NxF32 fElapsedTime );

    // Shared resource access. Indexed fonts and textures are shared among
    // all the controls.
    HRESULT       SetFont( UINT index, LPCWSTR strFaceName, LONG height, LONG weight );
    DXUTFontNode* GetFont( UINT index );

    HRESULT          SetTexture( UINT index, LPCWSTR strFilename );
    HRESULT          SetTexture( UINT index, LPCWSTR strResourceName, HMODULE hResourceModule );
    DXUTTextureNode* GetTexture( UINT index );

    CDXUTDialogResourceManager* GetManager() { return m_pManager; }

    static void ClearFocus();
    void FocusDefaultControl();

    bool m_bNonUserEvents;
    bool m_bKeyboardInput;
    bool m_bMouseInput;

	  void * GetUserData(void) const { return mUserData; };
	  void   SetUserData(void *userData) { mUserData = userData; };



private:
    void *mUserData;

    NxI32 m_nDefaultControlID;

    static NxF64 s_fTimeRefresh;
    NxF64 m_fTimeLastRefresh;

    // Initialize default Elements
    void InitDefaultElements();

    // Windows message handlers
    void OnMouseMove( POINT pt );
    void OnMouseUp( POINT pt );

    void SetNextDialog( CDXUTDialog* pNextDialog );

    // Control events
    bool OnCycleFocus( bool bForward );

    static CDXUTControl* s_pControlFocus;        // The control which has focus
    static CDXUTControl* s_pControlPressed;      // The control currently pressed

    CDXUTControl* m_pControlMouseOver;           // The control which is hovered over

    bool m_bVisible;
    bool m_bCaption;
    bool m_bMinimized;
    bool m_bDrag;
    WCHAR m_wszCaption[256];

    NxI32 m_x;
    NxI32 m_y;
    NxI32 m_width;
    NxI32 m_height;
    NxI32 m_nCaptionHeight;

    D3DCOLOR m_colorTopLeft;
    D3DCOLOR m_colorTopRight;
    D3DCOLOR m_colorBottomLeft;
    D3DCOLOR m_colorBottomRight;

    CDXUTDialogResourceManager* m_pManager;
    PCALLBACKDXUTGUIEVENT m_pCallbackEvent;
    void* m_pCallbackEventUserContext;

    CGrowableArray< NxI32 > m_Textures;   // Index into m_TextureCache;
    CGrowableArray< NxI32 > m_Fonts;      // Index into m_FontCache;

    CGrowableArray< CDXUTControl* > m_Controls;
    CGrowableArray< DXUTElementHolder* > m_DefaultElements;

    CDXUTElement m_CapElement;  // Element for the caption

    CDXUTDialog* m_pNextDialog;
    CDXUTDialog* m_pPrevDialog;
};


//--------------------------------------------------------------------------------------
// Structs for shared resources
//--------------------------------------------------------------------------------------
struct DXUTTextureNode
{
    bool bFileSource;  // True if this texture is loaded from a file. False if from resource.
    HMODULE hResourceModule;
    NxI32 nResourceID;   // Resource ID. If 0, string-based ID is used and stored in strFilename.
    WCHAR strFilename[MAX_PATH];
    IDirect3DTexture9* pTexture;
    DWORD dwWidth;
    DWORD dwHeight;
};

struct DXUTFontNode
{
    WCHAR strFace[MAX_PATH];
    ID3DXFont* pFont;
    LONG  nHeight;
    LONG  nWeight;
};


//-----------------------------------------------------------------------------
// Manages shared resources of dialogs
//-----------------------------------------------------------------------------
class CDXUTDialogResourceManager
{
public:
    CDXUTDialogResourceManager();
    ~CDXUTDialogResourceManager();

    HRESULT     OnCreateDevice( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT     OnResetDevice();
    void        OnLostDevice();
    void        OnDestroyDevice();
    bool        MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    
    NxI32 AddFont( LPCWSTR strFaceName, LONG height, LONG weight );
    NxI32 AddTexture( LPCWSTR strFilename );
    NxI32 AddTexture( LPCWSTR strResourceName, HMODULE hResourceModule );

    DXUTFontNode*     GetFontNode( NxI32 iIndex )     { return m_FontCache.GetAt( iIndex ); };
    DXUTTextureNode*  GetTextureNode( NxI32 iIndex )  { return m_TextureCache.GetAt( iIndex ); };
    IDirect3DDevice9* GetD3DDevice()                { return m_pd3dDevice; }

    bool RegisterDialog( CDXUTDialog *pDialog );
    void UnregisterDialog( CDXUTDialog *pDialog );
    void EnableKeyboardInputForAllDialogs();

    // Shared between all dialogs
    IDirect3DStateBlock9* m_pStateBlock;
    ID3DXSprite*          m_pSprite;          // Sprite used for drawing

    CGrowableArray< CDXUTDialog* > m_Dialogs;            // Dialogs registered

protected:
    CGrowableArray< DXUTTextureNode* > m_TextureCache;   // Shared textures
    CGrowableArray< DXUTFontNode* > m_FontCache;         // Shared fonts

    IDirect3DDevice9* m_pd3dDevice;

    // Resource creation helpers
    HRESULT CreateFont( UINT index );
    HRESULT CreateTexture( UINT index );
};


//-----------------------------------------------------------------------------
// Base class for controls
//-----------------------------------------------------------------------------
class CDXUTControl
{
public:
    CDXUTControl( CDXUTDialog *pDialog = NULL );
    virtual ~CDXUTControl();

    virtual HRESULT OnInit() { return S_OK; }
    virtual void Refresh();
    virtual void Render( IDirect3DDevice9* pd3dDevice, NxF32 fElapsedTime ) { };

    // Windows message handler
    virtual bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam ) { return false; }

    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam ) { return false; }
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam ) { return false; }

    virtual bool CanHaveFocus() { return false; }
    virtual void OnFocusIn() { m_bHasFocus = true; }
    virtual void OnFocusOut() { m_bHasFocus = false; }
    virtual void OnMouseEnter() { m_bMouseOver = true; }
    virtual void OnMouseLeave() { m_bMouseOver = false; }
    virtual void OnHotkey() {}

    virtual BOOL ContainsPoint( POINT pt ) { return PtInRect( &m_rcBoundingBox, pt ); }

    virtual void SetEnabled( bool bEnabled ) { m_bEnabled = bEnabled; }
    virtual bool GetEnabled() { return m_bEnabled; }
    virtual void SetVisible( bool bVisible ) { m_bVisible = bVisible; }
    virtual bool GetVisible() { return m_bVisible; }

    UINT GetType() const { return m_Type; }

    NxI32  GetID() const { return m_ID; }
    void SetID( NxI32 ID ) { m_ID = ID; }

    void SetLocation( NxI32 x, NxI32 y ) { m_x = x; m_y = y; UpdateRects(); }
    void SetSize( NxI32 width, NxI32 height ) { m_width = width; m_height = height; UpdateRects(); }

    void SetHotkey( UINT nHotkey ) { m_nHotkey = nHotkey; }
    UINT GetHotkey() { return m_nHotkey; }

    void SetUserData( void *pUserData ) { m_pUserData = pUserData; }
    void *GetUserData() const { return m_pUserData; }

    virtual void SetTextColor( D3DCOLOR Color );
    CDXUTElement* GetElement( UINT iElement ) { return m_Elements.GetAt( iElement ); }
    HRESULT SetElement( UINT iElement, CDXUTElement* pElement);

    bool m_bVisible;                // Shown/hidden flag
    bool m_bMouseOver;              // Mouse pointer is above control
    bool m_bHasFocus;               // Control has input focus
    bool m_bIsDefault;              // Is the default control

    // Size, scale, and positioning members
    NxI32 m_x, m_y;
    NxI32 m_width, m_height;

    // These members are set by the container
    CDXUTDialog* m_pDialog;    // Parent container
    UINT m_Index;              // Index within the control list

    CGrowableArray< CDXUTElement* > m_Elements;  // All display elements

protected:
    virtual void UpdateRects();

    NxI32  m_ID;                 // ID number
    DXUT_CONTROL_TYPE m_Type;  // Control type, set once in constructor
    UINT m_nHotkey;            // Virtual key code for this control's hotkey
    void *m_pUserData;         // Data associated with this control that is set by user.

    bool m_bEnabled;           // Enabled/disabled flag

    RECT m_rcBoundingBox;      // Rectangle defining the active region of the control
};


//-----------------------------------------------------------------------------
// Contains all the display information for a given control type
//-----------------------------------------------------------------------------
struct DXUTElementHolder
{
    UINT nControlType;
    UINT iElement;

    CDXUTElement Element;
};


//-----------------------------------------------------------------------------
// Static control
//-----------------------------------------------------------------------------
class CDXUTStatic : public CDXUTControl
{
public:
    CDXUTStatic( CDXUTDialog *pDialog = NULL );

    virtual void Render( IDirect3DDevice9* pd3dDevice, NxF32 fElapsedTime );
    virtual BOOL ContainsPoint( POINT pt ) { return false; }

    HRESULT GetTextCopy( LPWSTR strDest, UINT bufferCount );
    LPCWSTR GetText() { return m_strText; }
    HRESULT SetText( LPCWSTR strText );


protected:
    WCHAR m_strText[MAX_PATH];      // Window text  
};


//-----------------------------------------------------------------------------
// Button control
//-----------------------------------------------------------------------------
class CDXUTButton : public CDXUTStatic
{
public:
    CDXUTButton( CDXUTDialog *pDialog = NULL );

    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual void OnHotkey() { if( m_pDialog->IsKeyboardInputEnabled() ) m_pDialog->RequestFocus( this ); m_pDialog->SendEvent( EVENT_BUTTON_CLICKED, true, this ); }
    
    virtual BOOL ContainsPoint( POINT pt ) { return PtInRect( &m_rcBoundingBox, pt ); }
    virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }

    virtual void Render( IDirect3DDevice9* pd3dDevice, NxF32 fElapsedTime );

protected:
    bool m_bPressed;
};


//-----------------------------------------------------------------------------
// CheckBox control
//-----------------------------------------------------------------------------
class CDXUTCheckBox : public CDXUTButton
{
public:
    CDXUTCheckBox( CDXUTDialog *pDialog = NULL );

    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual void OnHotkey() { if( m_pDialog->IsKeyboardInputEnabled() ) m_pDialog->RequestFocus( this ); SetCheckedInternal( !m_bChecked, true ); }

    virtual BOOL ContainsPoint( POINT pt ); 
    virtual void UpdateRects(); 

    virtual void Render( IDirect3DDevice9* pd3dDevice, NxF32 fElapsedTime );

    bool GetChecked() { return m_bChecked; }
    void SetChecked( bool bChecked ) { SetCheckedInternal( bChecked, false ); }
    
protected:
    virtual void SetCheckedInternal( bool bChecked, bool bFromInput );

    bool m_bChecked;
    RECT m_rcButton;
    RECT m_rcText;
};


//-----------------------------------------------------------------------------
// RadioButton control
//-----------------------------------------------------------------------------
class CDXUTRadioButton : public CDXUTCheckBox
{
public:
    CDXUTRadioButton( CDXUTDialog *pDialog = NULL );

    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual void OnHotkey() { if( m_pDialog->IsKeyboardInputEnabled() ) m_pDialog->RequestFocus( this ); SetCheckedInternal( true, true, true ); }
    
    void SetChecked( bool bChecked, bool bClearGroup=true ) { SetCheckedInternal( bChecked, bClearGroup, false ); }
    void SetButtonGroup( UINT nButtonGroup ) { m_nButtonGroup = nButtonGroup; }
    UINT GetButtonGroup() { return m_nButtonGroup; }
    
protected:
    virtual void SetCheckedInternal( bool bChecked, bool bClearGroup, bool bFromInput );
    UINT m_nButtonGroup;
};


//-----------------------------------------------------------------------------
// Scrollbar control
//-----------------------------------------------------------------------------
class CDXUTScrollBar : public CDXUTControl
{
public:
    CDXUTScrollBar( CDXUTDialog *pDialog = NULL );
    virtual ~CDXUTScrollBar();

    virtual bool    HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool    HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual bool    MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

    virtual void    Render( IDirect3DDevice9* pd3dDevice, NxF32 fElapsedTime );
    virtual void    UpdateRects();

    void SetTrackRange( NxI32 nStart, NxI32 nEnd );
    NxI32 GetTrackPos() { return m_nPosition; }
    void SetTrackPos( NxI32 nPosition ) { m_nPosition = nPosition; Cap(); UpdateThumbRect(); }
    NxI32 GetPageSize() { return m_nPageSize; }
    void SetPageSize( NxI32 nPageSize ) { m_nPageSize = nPageSize; Cap(); UpdateThumbRect(); }

    void Scroll( NxI32 nDelta );    // Scroll by nDelta items (plus or minus)
    void ShowItem( NxI32 nIndex );  // Ensure that item nIndex is displayed, scroll if necessary

protected:
    // ARROWSTATE indicates the state of the arrow buttons.
    // CLEAR            No arrow is down.
    // CLICKED_UP       Up arrow is clicked.
    // CLICKED_DOWN     Down arrow is clicked.
    // HELD_UP          Up arrow is held down for sustained period.
    // HELD_DOWN        Down arrow is held down for sustained period.
    enum ARROWSTATE { CLEAR, CLICKED_UP, CLICKED_DOWN, HELD_UP, HELD_DOWN };

    void UpdateThumbRect();
    void Cap();  // Clips position at boundaries. Ensures it stays within legal range.

    bool m_bShowThumb;
    bool m_bDrag;
    RECT m_rcUpButton;
    RECT m_rcDownButton;
    RECT m_rcTrack;
    RECT m_rcThumb;
    NxI32 m_nPosition;  // Position of the first displayed item
    NxI32 m_nPageSize;  // How many items are displayable in one page
    NxI32 m_nStart;     // First item
    NxI32 m_nEnd;       // The index after the last item
    POINT m_LastMouse;// Last mouse position
    ARROWSTATE m_Arrow; // State of the arrows
    NxF64 m_dArrowTS;  // Timestamp of last arrow event.
};


//-----------------------------------------------------------------------------
// ListBox control
//-----------------------------------------------------------------------------
struct DXUTListBoxItem
{
    WCHAR strText[256];
    void*  pData;

    RECT  rcActive;
    bool  bSelected;
};

class CDXUTListBox : public CDXUTControl
{
public:
    CDXUTListBox( CDXUTDialog *pDialog = NULL );
    virtual ~CDXUTListBox();

    virtual HRESULT OnInit() { return m_pDialog->InitControl( &m_ScrollBar ); }
    virtual bool    CanHaveFocus() { return (m_bVisible && m_bEnabled); }
    virtual bool    HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool    HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual bool    MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

    virtual void    Render( IDirect3DDevice9* pd3dDevice, NxF32 fElapsedTime );
    virtual void    UpdateRects();

    DWORD GetStyle() const { return m_dwStyle; }
    NxI32 GetSize() const { return m_Items.GetSize(); }
    void SetStyle( DWORD dwStyle ) { m_dwStyle = dwStyle; }
    NxI32  GetScrollBarWidth() const { return m_nSBWidth; }
    void SetScrollBarWidth( NxI32 nWidth ) { m_nSBWidth = nWidth; UpdateRects(); }
    void SetBorder( NxI32 nBorder, NxI32 nMargin ) { m_nBorder = nBorder; m_nMargin = nMargin; }
    HRESULT AddItem( const WCHAR *wszText, void *pData );
    HRESULT InsertItem( NxI32 nIndex, const WCHAR *wszText, void *pData );
    void RemoveItem( NxI32 nIndex );
    void RemoveItemByText( WCHAR *wszText );
    void RemoveItemByData( void *pData );
    void RemoveAllItems();

    DXUTListBoxItem *GetItem( NxI32 nIndex );
    NxI32 GetSelectedIndex( NxI32 nPreviousSelected = -1 );
    DXUTListBoxItem *GetSelectedItem( NxI32 nPreviousSelected = -1 ) { return GetItem( GetSelectedIndex( nPreviousSelected ) ); }
    void SelectItem( NxI32 nNewIndex );

    enum STYLE { MULTISELECTION = 1 };

protected:
    RECT m_rcText;      // Text rendering bound
    RECT m_rcSelection; // Selection box bound
    CDXUTScrollBar m_ScrollBar;
    NxI32 m_nSBWidth;
    NxI32 m_nBorder;
    NxI32 m_nMargin;
    NxI32 m_nTextHeight;  // Height of a single line of text
    DWORD m_dwStyle;    // List box style
    NxI32 m_nSelected;    // Index of the selected item for single selection list box
    NxI32 m_nSelStart;    // Index of the item where selection starts (for handling multi-selection)
    bool m_bDrag;       // Whether the user is dragging the mouse to select

    CGrowableArray< DXUTListBoxItem* > m_Items;
};


//-----------------------------------------------------------------------------
// ComboBox control
//-----------------------------------------------------------------------------
struct DXUTComboBoxItem
{
    WCHAR strText[256];
    void*  pData;

    RECT  rcActive;
    bool  bVisible;
};


class CDXUTComboBox : public CDXUTButton
{
public:
    CDXUTComboBox( CDXUTDialog *pDialog = NULL );
    virtual ~CDXUTComboBox();

    virtual void SetTextColor( D3DCOLOR Color );
    virtual HRESULT OnInit() { return m_pDialog->InitControl( &m_ScrollBar ); }

    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual void OnHotkey();

    virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
    virtual void OnFocusOut();
    virtual void Render( IDirect3DDevice9* pd3dDevice, NxF32 fElapsedTime );

    virtual void UpdateRects();

    HRESULT AddItem( const WCHAR* strText, void* pData );
    void    RemoveAllItems();
    void    RemoveItem( UINT index );
    bool    ContainsItem( const WCHAR* strText, UINT iStart=0 );
    NxI32     FindItem( const WCHAR* strText, UINT iStart=0 );
    void*   GetItemData( const WCHAR* strText );
    void*   GetItemData( NxI32 nIndex );
    void    SetDropHeight( UINT nHeight ) { m_nDropHeight = nHeight; UpdateRects(); }
    NxI32     GetScrollBarWidth() const { return m_nSBWidth; }
    void    SetScrollBarWidth( NxI32 nWidth ) { m_nSBWidth = nWidth; UpdateRects(); }

    void*   GetSelectedData();
    DXUTComboBoxItem* GetSelectedItem();

    UINT    GetNumItems() { return m_Items.GetSize(); }
    DXUTComboBoxItem* GetItem( UINT index ) { return m_Items.GetAt( index ); }

    HRESULT SetSelectedByIndex( UINT index );
    HRESULT SetSelectedByText( const WCHAR* strText );
    HRESULT SetSelectedByData( void* pData );

		bool IsOpened(void) { return m_bOpened; }

protected:
    NxI32     m_iSelected;
    NxI32     m_iFocused;
    NxI32     m_nDropHeight;
    CDXUTScrollBar m_ScrollBar;
    NxI32     m_nSBWidth;

    bool    m_bOpened;

    RECT m_rcText;
    RECT m_rcButton;
    RECT m_rcDropdown;
    RECT m_rcDropdownText;


    CGrowableArray< DXUTComboBoxItem* > m_Items;
};


//-----------------------------------------------------------------------------
// Slider control
//-----------------------------------------------------------------------------
class CDXUTSlider : public CDXUTControl
{
public:
    CDXUTSlider( CDXUTDialog *pDialog = NULL );

    virtual BOOL ContainsPoint( POINT pt ); 
    virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    
    virtual void UpdateRects(); 

    virtual void Render( IDirect3DDevice9* pd3dDevice, NxF32 fElapsedTime );

    void SetValue( NxI32 nValue ) { SetValueInternal( nValue, false ); }
    NxI32  GetValue() const { return m_nValue; };

    void GetRange( NxI32 &nMin, NxI32 &nMax ) const { nMin = m_nMin; nMax = m_nMax; }
    void SetRange( NxI32 nMin, NxI32 nMax );

protected:
    void SetValueInternal( NxI32 nValue, bool bFromInput );
    NxI32  ValueFromPos( NxI32 x ); 

    NxI32 m_nValue;

    NxI32 m_nMin;
    NxI32 m_nMax;

    NxI32 m_nDragX;      // Mouse position at start of drag
    NxI32 m_nDragOffset; // Drag offset from the center of the button
    NxI32 m_nButtonX;

    bool m_bPressed;
    RECT m_rcButton;
};


//-----------------------------------------------------------------------------
// CUniBuffer class for the edit control
//-----------------------------------------------------------------------------
class CUniBuffer
{
public:
    CUniBuffer( NxI32 nInitialSize = 1 );
    ~CUniBuffer();

    static void Initialize();
    static void Uninitialize();

    NxI32  GetBufferSize() { return m_nBufferSize; }
    bool SetBufferSize( NxI32 nSize );
    NxI32  GetTextSize()  { return lstrlenW( m_pwszBuffer ); }
    const WCHAR* GetBuffer() { return m_pwszBuffer; }
    const WCHAR& operator[]( NxI32 n ) const { return m_pwszBuffer[n]; }
    WCHAR& operator[]( NxI32 n );
    DXUTFontNode* GetFontNode() { return m_pFontNode; }
    void SetFontNode( DXUTFontNode *pFontNode ) { m_pFontNode = pFontNode; }
    void Clear();

    bool InsertChar( NxI32 nIndex, WCHAR wChar ); // Inserts the char at specified index. If nIndex == -1, insert to the end.
    bool RemoveChar( NxI32 nIndex );  // Removes the char at specified index. If nIndex == -1, remove the last char.
    bool InsertString( NxI32 nIndex, const WCHAR *pStr, NxI32 nCount = -1 );  // Inserts the first nCount characters of the string pStr at specified index.  If nCount == -1, the entire string is inserted. If nIndex == -1, insert to the end.
    bool SetText( LPCWSTR wszText );

    // Uniscribe
    HRESULT CPtoX( NxI32 nCP, BOOL bTrail, NxI32 *pX );
    HRESULT XtoCP( NxI32 nX, NxI32 *pCP, NxI32 *pnTrail );
    void GetPriorItemPos( NxI32 nCP, NxI32 *pPrior );
    void GetNextItemPos( NxI32 nCP, NxI32 *pPrior );

private:
    HRESULT Analyse();      // Uniscribe -- Analyse() analyses the string in the buffer

    WCHAR* m_pwszBuffer;    // Buffer to hold text
    NxI32    m_nBufferSize;   // Size of the buffer allocated, in characters

    // Uniscribe-specific
    DXUTFontNode* m_pFontNode;          // Font node for the font that this buffer uses
    bool m_bAnalyseRequired;            // True if the string has changed since last analysis.
    SCRIPT_STRING_ANALYSIS m_Analysis;  // Analysis for the current string

private:
    // Empty implementation of the Uniscribe API
    static HRESULT WINAPI Dummy_ScriptApplyDigitSubstitution( const SCRIPT_DIGITSUBSTITUTE*, SCRIPT_CONTROL*, SCRIPT_STATE* ) { return E_NOTIMPL; }
    static HRESULT WINAPI Dummy_ScriptStringAnalyse( HDC, const void *, NxI32, NxI32, NxI32, DWORD, NxI32, SCRIPT_CONTROL*, SCRIPT_STATE*, const NxI32*, SCRIPT_TABDEF*, const BYTE*, SCRIPT_STRING_ANALYSIS* ) { return E_NOTIMPL; }
    static HRESULT WINAPI Dummy_ScriptStringCPtoX( SCRIPT_STRING_ANALYSIS, NxI32, BOOL, NxI32* ) { return E_NOTIMPL; }
    static HRESULT WINAPI Dummy_ScriptStringXtoCP( SCRIPT_STRING_ANALYSIS, NxI32, NxI32*, NxI32* ) { return E_NOTIMPL; }
    static HRESULT WINAPI Dummy_ScriptStringFree( SCRIPT_STRING_ANALYSIS* ) { return E_NOTIMPL; }
    static const SCRIPT_LOGATTR* WINAPI Dummy_ScriptString_pLogAttr( SCRIPT_STRING_ANALYSIS ) { return NULL; }
    static const NxI32* WINAPI Dummy_ScriptString_pcOutChars( SCRIPT_STRING_ANALYSIS ) { return NULL; }

    // Function pointers
    static HRESULT (WINAPI *_ScriptApplyDigitSubstitution)( const SCRIPT_DIGITSUBSTITUTE*, SCRIPT_CONTROL*, SCRIPT_STATE* );
    static HRESULT (WINAPI *_ScriptStringAnalyse)( HDC, const void *, NxI32, NxI32, NxI32, DWORD, NxI32, SCRIPT_CONTROL*, SCRIPT_STATE*, const NxI32*, SCRIPT_TABDEF*, const BYTE*, SCRIPT_STRING_ANALYSIS* );
    static HRESULT (WINAPI *_ScriptStringCPtoX)( SCRIPT_STRING_ANALYSIS, NxI32, BOOL, NxI32* );
    static HRESULT (WINAPI *_ScriptStringXtoCP)( SCRIPT_STRING_ANALYSIS, NxI32, NxI32*, NxI32* );
    static HRESULT (WINAPI *_ScriptStringFree)( SCRIPT_STRING_ANALYSIS* );
    static const SCRIPT_LOGATTR* (WINAPI *_ScriptString_pLogAttr)( SCRIPT_STRING_ANALYSIS );
    static const NxI32* (WINAPI *_ScriptString_pcOutChars)( SCRIPT_STRING_ANALYSIS );

    static HINSTANCE s_hDll;  // Uniscribe DLL handle

};

//-----------------------------------------------------------------------------
// EditBox control
//-----------------------------------------------------------------------------
class CDXUTEditBox : public CDXUTControl
{
public:
    CDXUTEditBox( CDXUTDialog *pDialog = NULL );
    virtual ~CDXUTEditBox();

    virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual void UpdateRects();
    virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }
    virtual void Render( IDirect3DDevice9* pd3dDevice, NxF32 fElapsedTime );
    virtual void OnFocusIn();

    void SetText( LPCWSTR wszText, bool bSelected = false );
    LPCWSTR GetText() { return m_Buffer.GetBuffer(); }
    NxI32 GetTextLength() { return m_Buffer.GetTextSize(); }  // Returns text length in chars excluding NULL.
    HRESULT GetTextCopy( LPWSTR strDest, UINT bufferCount );
    void ClearText();
    virtual void SetTextColor( D3DCOLOR Color ) { m_TextColor = Color; }  // Text color
    void SetSelectedTextColor( D3DCOLOR Color ) { m_SelTextColor = Color; }  // Selected text color
    void SetSelectedBackColor( D3DCOLOR Color ) { m_SelBkColor = Color; }  // Selected background color
    void SetCaretColor( D3DCOLOR Color ) { m_CaretColor = Color; }  // Caret color
    void SetBorderWidth( NxI32 nBorder ) { m_nBorder = nBorder; UpdateRects(); }  // Border of the window
    void SetSpacing( NxI32 nSpacing ) { m_nSpacing = nSpacing; UpdateRects(); }
    void ParseFloatArray( NxF32 *pNumbers, NxI32 nCount );
    void SetTextFloatArray( const NxF32 *pNumbers, NxI32 nCount );
		void SetFocused(void)
		{
		  if( !m_bHasFocus )
    		m_pDialog->RequestFocus( this );
		}

protected:
    void PlaceCaret( NxI32 nCP );
    void DeleteSelectionText();
    void ResetCaretBlink();
    void CopyToClipboard();
    void PasteFromClipboard();

    CUniBuffer m_Buffer;     // Buffer to hold text
    NxI32      m_nBorder;      // Border of the window
    NxI32      m_nSpacing;     // Spacing between the text and the edge of border
    RECT     m_rcText;       // Bounding rectangle for the text
    RECT     m_rcRender[9];  // Convenient rectangles for rendering elements
    NxF64   m_dfBlink;      // Caret blink time in milliseconds
    NxF64   m_dfLastBlink;  // Last timestamp of caret blink
    bool     m_bCaretOn;     // Flag to indicate whether caret is currently visible
    NxI32      m_nCaret;       // Caret position, in characters
    bool     m_bInsertMode;  // If true, control is in insert mode. Else, overwrite mode.
    NxI32      m_nSelStart;    // Starting position of the selection. The caret marks the end.
    NxI32      m_nFirstVisible;// First visible character in the edit control
    D3DCOLOR m_TextColor;    // Text color
    D3DCOLOR m_SelTextColor; // Selected text color
    D3DCOLOR m_SelBkColor;   // Selected background color
    D3DCOLOR m_CaretColor;   // Caret color

    // Mouse-specific
    bool m_bMouseDrag;       // True to indicate drag in progress

    // Static
    static bool s_bHideCaret;   // If true, we don't render the caret.
};


//-----------------------------------------------------------------------------
// IME-enabled EditBox control
//-----------------------------------------------------------------------------
#define MAX_CANDLIST 10
#define MAX_COMPSTRING_SIZE 256

class CDXUTIMEEditBox : public CDXUTEditBox
{
protected:
    // Empty implementation of the IMM32 API
    static INPUTCONTEXT* WINAPI Dummy_ImmLockIMC( HIMC ) { return NULL; }
    static BOOL WINAPI Dummy_ImmUnlockIMC( HIMC ) { return FALSE; }
    static LPVOID WINAPI Dummy_ImmLockIMCC( HIMCC ) { return NULL; }
    static BOOL WINAPI Dummy_ImmUnlockIMCC( HIMCC ) { return FALSE; }
    static BOOL WINAPI Dummy_ImmDisableTextFrameService( DWORD ) { return TRUE; }
    static LONG WINAPI Dummy_ImmGetCompositionStringW( HIMC, DWORD, LPVOID, DWORD ) { return IMM_ERROR_GENERAL; }
    static DWORD WINAPI Dummy_ImmGetCandidateListW( HIMC, DWORD, LPCANDIDATELIST, DWORD ) { return 0; }
    static HIMC WINAPI Dummy_ImmGetContext( HWND ) { return NULL; }
    static BOOL WINAPI Dummy_ImmReleaseContext( HWND, HIMC ) { return FALSE; }
    static HIMC WINAPI Dummy_ImmAssociateContext( HWND, HIMC ) { return NULL; }
    static BOOL WINAPI Dummy_ImmGetOpenStatus( HIMC ) { return 0; }
    static BOOL WINAPI Dummy_ImmSetOpenStatus( HIMC, BOOL ) { return 0; }
    static BOOL WINAPI Dummy_ImmGetConversionStatus( HIMC, LPDWORD, LPDWORD ) { return 0; }
    static HWND WINAPI Dummy_ImmGetDefaultIMEWnd( HWND ) { return NULL; }
    static UINT WINAPI Dummy_ImmGetIMEFileNameA( HKL, LPSTR, UINT ) { return 0; }
    static UINT WINAPI Dummy_ImmGetVirtualKey( HWND ) { return 0; }
    static BOOL WINAPI Dummy_ImmNotifyIME( HIMC, DWORD, DWORD, DWORD ) { return FALSE; }
    static BOOL WINAPI Dummy_ImmSetConversionStatus( HIMC, DWORD, DWORD ) { return FALSE; }
    static BOOL WINAPI Dummy_ImmSimulateHotKey( HWND, DWORD ) { return FALSE; }
    static BOOL WINAPI Dummy_ImmIsIME( HKL ) { return FALSE; }

    // Traditional Chinese IME
    static UINT WINAPI Dummy_GetReadingString( HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT ) { return 0; }
    static BOOL WINAPI Dummy_ShowReadingWindow( HIMC, BOOL ) { return FALSE; }

    // Verion library imports
    static BOOL APIENTRY Dummy_VerQueryValueA( const LPVOID, LPSTR, LPVOID *, PUINT ) { return 0; }
    static BOOL APIENTRY Dummy_GetFileVersionInfoA( LPSTR, DWORD, DWORD, LPVOID ) { return 0; }
    static DWORD APIENTRY Dummy_GetFileVersionInfoSizeA( LPSTR, LPDWORD ) { return 0; }

    // Function pointers: IMM32
    static INPUTCONTEXT* (WINAPI * _ImmLockIMC)( HIMC );
    static BOOL (WINAPI * _ImmUnlockIMC)( HIMC );
    static LPVOID (WINAPI * _ImmLockIMCC)( HIMCC );
    static BOOL (WINAPI * _ImmUnlockIMCC)( HIMCC );
    static BOOL (WINAPI * _ImmDisableTextFrameService)( DWORD );
    static LONG (WINAPI * _ImmGetCompositionStringW)( HIMC, DWORD, LPVOID, DWORD );
    static DWORD (WINAPI * _ImmGetCandidateListW)( HIMC, DWORD, LPCANDIDATELIST, DWORD );
    static HIMC (WINAPI * _ImmGetContext)( HWND );
    static BOOL (WINAPI * _ImmReleaseContext)( HWND, HIMC );
    static HIMC (WINAPI * _ImmAssociateContext)( HWND, HIMC );
    static BOOL (WINAPI * _ImmGetOpenStatus)( HIMC );
    static BOOL (WINAPI * _ImmSetOpenStatus)( HIMC, BOOL );
    static BOOL (WINAPI * _ImmGetConversionStatus)( HIMC, LPDWORD, LPDWORD );
    static HWND (WINAPI * _ImmGetDefaultIMEWnd)( HWND );
    static UINT (WINAPI * _ImmGetIMEFileNameA)( HKL, LPSTR, UINT );
    static UINT (WINAPI * _ImmGetVirtualKey)( HWND );
    static BOOL (WINAPI * _ImmNotifyIME)( HIMC, DWORD, DWORD, DWORD );
    static BOOL (WINAPI * _ImmSetConversionStatus)( HIMC, DWORD, DWORD );
    static BOOL (WINAPI * _ImmSimulateHotKey)( HWND, DWORD );
    static BOOL (WINAPI * _ImmIsIME)( HKL );

    // Function pointers: Traditional Chinese IME
    static UINT (WINAPI * _GetReadingString)( HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT );
    static BOOL (WINAPI * _ShowReadingWindow)( HIMC, BOOL );

    // Function pointers: Verion library imports
    static BOOL (APIENTRY * _VerQueryValueA)( const LPVOID, LPSTR, LPVOID *, PUINT );
    static BOOL (APIENTRY * _GetFileVersionInfoA)( LPSTR, DWORD, DWORD, LPVOID );
    static DWORD (APIENTRY * _GetFileVersionInfoSizeA)( LPSTR, LPDWORD );

public:
    CDXUTIMEEditBox( CDXUTDialog *pDialog = NULL );
    virtual ~CDXUTIMEEditBox();

    static void Initialize();
    static void Uninitialize();

    static  HRESULT StaticOnCreateDevice();
    static  bool StaticMsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

    static  void EnableImeSystem( bool bEnable );

    virtual void Render( IDirect3DDevice9* pd3dDevice, NxF32 fElapsedTime );
    virtual bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual bool HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
    virtual void UpdateRects();
    virtual void OnFocusIn();
    virtual void OnFocusOut();

    void PumpMessage();

    virtual void RenderCandidateReadingWindow( IDirect3DDevice9* pd3dDevice, NxF32 fElapsedTime, bool bReading );
    virtual void RenderComposition( IDirect3DDevice9* pd3dDevice, NxF32 fElapsedTime );
    virtual void RenderIndicator( IDirect3DDevice9* pd3dDevice, NxF32 fElapsedTime );

protected:
    static WORD GetLanguage() { return LOWORD( s_hklCurrent ); }
    static WORD GetPrimaryLanguage() { return PRIMARYLANGID( LOWORD( s_hklCurrent ) ); }
    static WORD GetSubLanguage() { return SUBLANGID( LOWORD( s_hklCurrent ) ); }
    static void SendKey( BYTE nVirtKey );
    static DWORD GetImeId( UINT uIndex = 0 );
    static void CheckInputLocale();
    static void CheckToggleState();
    static void SetupImeApi();
    static void ResetCompositionString();
    void TruncateCompString( bool bUseBackSpace = true, NxI32 iNewStrLen = 0 );
    void FinalizeString( bool bSend );
    static void GetReadingWindowOrientation( DWORD dwId );
    static void GetPrivateReadingString();

    void SendCompString();

protected:
    enum { INDICATOR_NON_IME, INDICATOR_CHS, INDICATOR_CHT, INDICATOR_KOREAN, INDICATOR_JAPANESE };
    enum IMESTATE { IMEUI_STATE_OFF, IMEUI_STATE_ON, IMEUI_STATE_ENGLISH };

    struct CCandList
    {
        WCHAR awszCandidate[MAX_CANDLIST][256];
        CUniBuffer HoriCand; // Candidate list string (for horizontal candidate window)
        NxI32   nFirstSelected; // First character position of the selected string in HoriCand
        NxI32   nHoriSelectedLen; // Length of the selected string in HoriCand
        DWORD dwCount;       // Number of valid entries in the candidate list
        DWORD dwSelection;   // Currently selected candidate entry relative to page top
        DWORD dwPageSize;
        NxI32   nReadingError; // Index of the error character
        bool  bShowWindow;   // Whether the candidate list window is visible
        RECT  rcCandidate;   // Candidate rectangle computed and filled each time before rendered
    };

    struct CInputLocale
    {
        HKL   m_hKL;            // Keyboard layout
        WCHAR m_wszLangAbb[3];  // Language abbreviation
        WCHAR m_wszLang[64];    // Localized language name
    };

    // Application-wide data
    static HINSTANCE s_hDllImm32;         // IMM32 DLL handle
    static HINSTANCE s_hDllVer;           // Version DLL handle
    static HIMC      s_hImcDef;           // Default input context

    static HKL     s_hklCurrent;          // Current keyboard layout of the process
    static bool    s_bVerticalCand;       // Indicates that the candidates are listed vertically
    static LPWSTR  s_wszCurrIndicator;    // Points to an indicator string that corresponds to current input locale
    static WCHAR   s_aszIndicator[5][3];  // String to draw to indicate current input locale
    static bool    s_bInsertOnType;       // Insert the character as soon as a key is pressed (Korean behavior)
    static HINSTANCE s_hDllIme;           // Instance handle of the current IME module
    static IMESTATE  s_ImeState;          // IME global state
    static bool    s_bEnableImeSystem;    // Whether the IME system is active
    static POINT   s_ptCompString;        // Composition string position. Updated every frame.
    static NxI32     s_nCompCaret;          // Caret position of the composition string
    static NxI32     s_nFirstTargetConv;    // Index of the first target converted char in comp string.  If none, -1.
    static CUniBuffer s_CompString;       // Buffer to hold the composition string (we fix its length)
    static BYTE    s_abCompStringAttr[MAX_COMPSTRING_SIZE];
    static DWORD   s_adwCompStringClause[MAX_COMPSTRING_SIZE];
    static WCHAR   s_wszReadingString[32];// Used only with horizontal reading window (why?)
    static CCandList s_CandList;          // Data relevant to the candidate list
    static bool    s_bShowReadingWindow;  // Indicates whether reading window is visible
    static bool    s_bHorizontalReading;  // Indicates whether the reading window is vertical or horizontal
    static bool    s_bChineseIME;
    static CGrowableArray< CInputLocale > s_Locale; // Array of loaded keyboard layout on system

    // Color of various IME elements
    D3DCOLOR       m_ReadingColor;        // Reading string color
    D3DCOLOR       m_ReadingWinColor;     // Reading window color
    D3DCOLOR       m_ReadingSelColor;     // Selected character in reading string
    D3DCOLOR       m_ReadingSelBkColor;   // Background color for selected char in reading str
    D3DCOLOR       m_CandidateColor;      // Candidate string color
    D3DCOLOR       m_CandidateWinColor;   // Candidate window color
    D3DCOLOR       m_CandidateSelColor;   // Selected candidate string color
    D3DCOLOR       m_CandidateSelBkColor; // Selected candidate background color
    D3DCOLOR       m_CompColor;           // Composition string color
    D3DCOLOR       m_CompWinColor;        // Composition string window color
    D3DCOLOR       m_CompCaretColor;      // Composition string caret color
    D3DCOLOR       m_CompTargetColor;     // Composition string target converted color
    D3DCOLOR       m_CompTargetBkColor;   // Composition string target converted background
    D3DCOLOR       m_CompTargetNonColor;  // Composition string target non-converted color
    D3DCOLOR       m_CompTargetNonBkColor;// Composition string target non-converted background
    D3DCOLOR       m_IndicatorImeColor;   // Indicator text color for IME
    D3DCOLOR       m_IndicatorEngColor;   // Indicator text color for English
    D3DCOLOR       m_IndicatorBkColor;    // Indicator text background color

    // Edit-control-specific data
    NxI32            m_nIndicatorWidth;     // Width of the indicator symbol
    RECT           m_rcIndicator;         // Rectangle for drawing the indicator button

#if defined(DEBUG) || defined(_DEBUG)
    static bool    m_bIMEStaticMsgProcCalled;
#endif
};



#endif // DXUT_GUI_H
