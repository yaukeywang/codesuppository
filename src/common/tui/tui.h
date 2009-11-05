#ifndef TUI_H

#define TUI_H

#include <stdio.h>
#include "UserMemAlloc.h"
#include "stringdict.h"
#include "cparser.h"

using namespace NVSHARE;

enum  TuiType;
class TuiElement;
typedef std::vector< TuiElement * > TuiElementVector;
typedef std::vector<StringRefVector *> StringRefVectorVct;

class TuiSaveCallback
{
public:
  virtual void tuiSaveCallback(FILE *fph) = 0;
};

enum TuiType
{
	TT_DEFAULT,            // default text user interface container
	TT_PAGE,               // treated as a page
	TT_BUTTON,
	TT_SCRIPT,
	TT_CHECKBOX,
	TT_SLIDER,
	TT_COMBO,
	TT_MULTI_SELECT,
	TT_TEXT,
	TT_LAST_TUI
};

#define DFLT_BUTTON_WIDTH		150
#define DFLT_BUTTON_HEIGHT		20
#define DFLT_BUTTON_SPACING		10

#define DFLT_SLIDER_SPACING		24
#define DFLT_SLIDER_TEXT_WIDTH	80
#define DFLT_SLIDER_HEIGHT		20
#define DFLT_SLIDER_WIDTH		300

#define DFLT_COMBO_WIDTH		250
#define DFLT_COMBO_SPACING		42
#define DFLT_COMBO_HEIGHT		20
#define DFLT_COMBO_DROPDOWN		100

#define DFLT_TEXT_SPACING		24

#define DFLT_LIST_WIDTH			300

#define DFLT_LAYOUT_WIDTH		1024
#define DFLT_LAYOUT_RIGHT_MARGIN 10

#define DFLT_CHECKBOX_SPACING	22

#define DFLT_LAYOUT_LEFT_MARGIN	( DFLT_LAYOUT_WIDTH - 3*(DFLT_BUTTON_WIDTH+DFLT_BUTTON_SPACING) - DFLT_LAYOUT_RIGHT_MARGIN )

#define DFLT_CONSOLE_WIDTH	1000
#define DFLT_CONSOLE_HEIGHT 36
#define DFLT_CONSOLE_POS_X	0 
#define DFLT_CONSOLE_POS_Y	730

//==================================================================================
//==================================================================================
class TuiChoice
{
public:
	TuiChoice(const StringRef &r,const StringRef &arg,bool state)
	{
		mChoice = r;
		mArg    = arg;
		mState  = state;
	}

	const StringRef& GetChoice(void) const { return mChoice; };
	const StringRef& GetArg(void) const { return mArg; };

	bool GetState(void) const { return mState; };
	void SetState(bool state) { mState = state; };

private:

	StringRef mChoice;
	StringRef mArg;
	bool      mState;
};

typedef std::vector< TuiChoice > TuiChoiceVector;

class TextUserInterface;

//==================================================================================
//==================================================================================
class PageLayout
{
public:
  PageLayout(void)
  {
  	mButtonWidth       = DFLT_BUTTON_WIDTH;
  	mButtonHeight      = DFLT_BUTTON_HEIGHT;
  	mButtonSpacing     = DFLT_BUTTON_SPACING;

  	mSliderSpacing     = DFLT_SLIDER_SPACING;
  	mSliderTextWidth   = DFLT_SLIDER_TEXT_WIDTH;
  	mSliderHeight      = DFLT_SLIDER_HEIGHT;
  	mSliderWidth       = DFLT_SLIDER_WIDTH;
  	
	mComboWidth        = DFLT_COMBO_WIDTH;
  	mComboSpacing      = DFLT_COMBO_SPACING;
  	mComboHeight       = DFLT_COMBO_HEIGHT;
  	mComboDropdown     = DFLT_COMBO_DROPDOWN;
  	
	mTextSpacing       = DFLT_TEXT_SPACING;

  	mListWidth         = DFLT_LIST_WIDTH;
  	
	mLayoutLeftMargin  = DFLT_LAYOUT_LEFT_MARGIN;
  	mLayoutWidth       = DFLT_LAYOUT_WIDTH;
  }

  void Modify( NxU32 newScreenWidth, NxU32 newScreenHeight );

  NxI32	mButtonWidth;
  NxI32	mButtonHeight;
  NxI32	mButtonSpacing;
  NxI32	mLayoutLeftMargin;
  NxI32	mLayoutWidth;
  NxI32	mSliderSpacing;
  NxI32	mSliderTextWidth;
  NxI32	mSliderHeight;
  NxI32	mSliderWidth;
  NxI32	mComboWidth;
  NxI32	mComboSpacing;
  NxI32	mComboHeight;
  NxI32	mComboDropdown;
  NxI32	mListWidth;
  NxI32	mTextSpacing;
};

//==================================================================================
//==================================================================================
class TuiElement : public NVSHARE::Memalloc
{
public:
	TuiElement(const StringRef &key,TuiType type,NxI32 count,const char **commands);
	~TuiElement(void);

	TuiType           GetType(void) const;
	void              SetType(TuiType type);
	void              SetTokens(NxI32 count,const char **args);
	void              SetDescription(NxI32 count,const char **args);
	void              SetText(const char *text);
	void              SetTextColor(NxU32 color );
	void              Add(TuiElement *tui);
	void              SetName(const StringRef &name);
	void              SetKey(const StringRef &key);
	void              Reset(void);
	void              End(void);
	void              Indent(NxI32 indent);
	void              ShowHelp(NxI32 indent,bool detailed,NxI32 index,bool showcount);
	bool              ShowHelp(const char *str);

	TuiElement *      Command(const StringRef &c,NxI32 count,const char **arglist,TextUserInterface *tui);

	bool              NeedSpace(const char *foo);

	const char *      GetCommand(const StringRef &c,NxI32 count,const char **arglist,TuiElement *parent,TextUserInterface *tui,bool toggleOk);

	void              Execute(NxI32 count,const char **arglist,TuiElement *parent,TextUserInterface *tui,bool toggleOk);
	void              SetArg(NxI32 count,const char *c);

	const char *      GetText( void ) const;
	NxU32          GetTextColor( void ) const;
	TuiElementVector& GetElements(void);
	void *            GetUserPtr(void);
	void *            GetUserPtr2(void);
	void              SetUserPtr(void *ptr);
	void              SetUserPtr2(void *ptr);
	const StringRef&  GetKey(void) const;
	const StringRef&  GetName(void) const;
	TuiElement *      ExecuteElement(NxI32 index,NxI32 count,const char **arglist,TextUserInterface *tui,bool toggleOk=true);
	TuiElement *      GetElement(NxI32 index);

	void              AddArg(NxI32 count,const char **arglist);
	void              AddChoice(NxI32 count,const char **arglist);
	void              AddChoices(NxI32 count,const char **arglist);
	void              AddFiles(NxI32 count,const char **arglist);
	void              AddTechniques(const char *fxfile); // add all of the 'tehcniques' in a FX file
	void              SetMinMax(NxF32 fmin,NxF32 fmax) { mMin = fmin; mMax = fmax; };
	void              AddActors(void);

	TuiChoiceVector& GetChoices(void) { return mChoices; };

	const char *      ArgumentLookup(const char *name);
	const char *      GetToggleArg(NxI32 count,bool toggleOk); // locate this argument, toggle the result and return.
	bool		      GetArg(NxI32 count, char *arg) const;
	bool		      GetArg_Actual(NxI32 count, char *arg) const;

 	bool              GetState(void);
 
 	NxI32			      GetSliderTick(NxF32 v) const; // convert into a normalized tick value 0-1000
 	NxF32             GetSliderValue(NxI32 tick) const; // convert integer tick (0-1000) into floating point value.
 
 	void              Save(FILE *fph);

 	void              SetSaveOk(bool saveok) { mSaveOk = saveok; };
	void			  SetExecuteOk(bool executeok) { mExecuteOk = executeok; };
 	void              SetRecip(bool state) { mRecip = state; };
 
 	void              Refresh(void);

 	void              SetNeedsUpdating( bool yesNo );
 	bool              GetNeedsUpdating( void ) const;

 	void              AddOnCheckShow( NxI32 count, const char **name );
 	void              AddOnCheckRescript( NxI32 count, const char **script );
 	const StringRefVectorVct& GetOnCheckShow( void ) const;
 	const StringRefVectorVct& GetOnCheckRescript( void ) const;
 	void              SetVisible( bool visible );
 	bool              IsVisible( void ) const;

 	void              OnCheckboxChange( bool state, NxI32 id );
 	void              OnComboBoxChange( const char *selectedItem, NxI32 id );

 	void              DetermineVisibleElements( const char *, NxI32 );
 	void              DetermineRescriptedItems( const char *, NxI32 );

	void			  AddExecuteOnLoad( const char **, NxI32 );
	void			  AddExecuteOnExit( const char **, NxI32 );
	void			  OnLoad( TuiElement *loadingPage );
	void			  OnExit( TuiElement *exitingPage );

	NxI32				  GetElementIndexOfTypeWithName( TuiType type, const char *name );
	TuiElement *	  GetElementOfTypeWithName( TuiType type, const char *name );

	bool			  NotifyHardwareStatus( bool hwOn );

	PageLayout& getPageLayout(void) { return mPageLayout; };

	void SetInt(bool state) { mInt = state; };
	bool GetInt(void) const { return mInt; };

	void				SetSWEnabled( bool state ) { mSWEnabled = state; }
	void				SetHWEnabled( bool state ) { mHWEnabled = state; }
	bool				GetSWEnabled( void ) const { return mSWEnabled; }
	bool				GetHWEnabled( void ) const { return mHWEnabled; }

	TuiElement *		GetPreviousPage( void );

	bool ChildNeedsUpdating(void)
	{
  		bool ret = false;

   		TuiElementVector::const_iterator i;
   		for (i=mElements.begin(); i!=mElements.end(); i++)
   		{
   			TuiElement *te = (*i);
   			if ( te->GetType() != TT_PAGE )
   			{
    			if ( te->GetNeedsUpdating() )
    			{
    				te->SetNeedsUpdating(false);
    				ret = true;
    			}
    		}
		}

		return ret;
	}

	bool HasElement(TuiElement *t) const
	{
  		bool ret = false;
  		TuiElementVector::const_iterator i;
  		for (i=mElements.begin(); i!=mElements.end(); i++)
  		{
  			if ( t == (*i) )
  			{
  				ret = true;
  				break;
  			}
  		}
  		return ret;
	}

  void ExecuteCommands(void);

private:

	void			  GetOnLoadOnExitCommand( char *buff, NxI32 buffLen, const StringRef &command );

	TuiType          mType;     // type of text user interface element

	StringRef        mKey;      // key name
	StringRef        mName;     // descriptive name
	StringRef        mText;
	StringRefVector  mTokens;
	StringRefVector  mArgs;     // default arguments.
	StringRefVector  mDescription; // the detailed usage description.
	StringRefVector  mCommands; // the shorthand command or commands.
	TuiElementVector mElements; // sub-elements in this element
	void             *mUserPtr; // user data field
	void             *mUserPtr2; //-- temporary until Ag gui is working
	TuiChoiceVector   mChoices; // choices for combo boxes or list boxes
	TuiChoiceVector   mOriginalChoices; // original choices prior to reading file spec!
	NxF32             mMin;
	NxF32             mMax;
	bool              mSaveOk;
	bool              mExecuteOk;
	NxI32               mSaveFrame;
	bool              mRecip;
	StringRefVector   mFileSpecs;
	NxU32          mTextColor;
	bool              mInt;
	bool              mHasActors;

 	StringRefVectorVct mOnCheckShow;
 	StringRefVectorVct mOnCheckRescript;
	StringRefVector	  mOnLoadCommands;
	StringRefVector   mOnExitCommands;
 	bool              mNeedsUpdating;
 	bool              mVisible;

	bool			  mHWEnabled;
	bool			  mSWEnabled;

//***
//***
  PageLayout        mPageLayout;

};

//==================================================================================
inline void TuiElement::SetNeedsUpdating( bool yesNo )
{
	mNeedsUpdating = yesNo;
}

//==================================================================================
inline bool TuiElement::GetNeedsUpdating( void ) const
{
	return mNeedsUpdating;
}

//==================================================================================
inline const StringRefVectorVct& TuiElement::GetOnCheckShow( void ) const
{
	return mOnCheckShow;
}

//==================================================================================
inline const StringRefVectorVct& TuiElement::GetOnCheckRescript( void ) const
{
	return mOnCheckRescript;
}

//==================================================================================
inline void TuiElement::SetVisible( bool visible )
{
	mVisible = visible;
}

//==================================================================================
inline bool TuiElement::IsVisible( void ) const
{
	return mVisible;
}

//==================================================================================
inline const char *TuiElement::GetText( void ) const
{
	return mText.Get();
}

//==================================================================================
inline NxU32 TuiElement::GetTextColor( void ) const
{
	return mTextColor;
}

typedef std::map<StringRef, TuiElement*> TuiElementMap;

//==================================================================================
//==================================================================================
class TextUserInterface : public CommandParserInterface, public NVSHARE::Memalloc
{
public:
	TextUserInterface(const char *startup);
	~TextUserInterface(void);

	virtual NxI32 CommandCallback(NxI32 token,NxI32 count,const char **arglist);
	virtual NxI32 CommandFallback(NxI32 count,const char **arglist);

	TuiElement * Locate(const StringRef &name) const; // locate a text user interface element by name.
	NxI32 SlashCommand(const char *key,NxI32 count,const char **arglist);

	TuiElement * GetPage(void) { return mPage; }
	void SetPage(TuiElement *page) { mPage = page; };

	void HelpAll(void);

	bool GetShowGraphics(void) const { return mShowGraphics; };
	void SetShowGraphics(bool state) { mShowGraphics = state; };
	bool ToggleShowGraphics(void)
	{
		if ( mShowGraphics )
			mShowGraphics = false;
		else
			mShowGraphics = true;
		return mShowGraphics;
	};


	// 'look up' this argument by name field.
	const char * ArgumentLookup(const char *name);

	void Refresh(void);

	bool IsTuiLoad(void) const { return mTuiLoad; };

	void setLook(const NxF32 *eye,const NxF32 *look)
	{
	  mEyePos[0] = eye[0];
	  mEyePos[1] = eye[1];
	  mEyePos[2] = eye[2];

	  mLookAt[0] = look[0];
	  mLookAt[1] = look[1];
	  mLookAt[2] = look[2];
	}

	void NotifyAllPagesOfNewDimensions( NxU32 width, NxU32 height );
	TuiElement *GetElementOfTypeWithName( TuiType type, const char *name );

	void NotifyHardwareStatus( bool hwOn );

  void setTuiSaveCallback(TuiSaveCallback *callback) { mCallback = callback; };

private:

	void Begin(TuiType type,NxI32 count,const char **arglist);
	void End(void);

	bool          mShowGraphics;
	TuiElement   *mPage;       // current active user interface page.
	TuiElement   *mCurrent;
	TuiElementMap mTui; // complete text user interface specification.

	NxF32         mEyePos[3]; //
	NxF32         mLookAt[3];

	FILE          *mFph;
	FILE          *mFileOpen;

	bool          mTuiLoad;
	StringRef     mSaveName;
	NxI32           mSlashDepth;

  TuiSaveCallback *mCallback;

};

extern TextUserInterface *gTui;

#endif



