#ifndef __BEGADU_PREFERENCES_TAB_H__
#define __BEGADU_PREFERENCES_TAB_H__

#include <View.h>
#include <Resources.h>

class BBitmap;
class BView;

class PreferencesTab : public BView
	{
	public:
		PreferencesTab( BRect aFrame, const char *aName, BResources *aResources );
		~PreferencesTab();
		virtual void Draw( BRect aRect );
		virtual void AttachedToWindow();
		virtual void MouseMoved( BPoint aPoint, uint32 aTransit, const BMessage *aMessage );
		virtual void MouseDown( BPoint aPoint );
		virtual void MouseUp( BPoint aPoint );
		virtual void MessageReceived( BMessage *aMessage );

	private:
		BBitmap		*	GetBitmap( const char *aName );

	private:
		BMessage	*	iMessage;
		/*
			notify
			theme
		*/
		
		BBitmap 	*	iIconGeneral;
		BBitmap		*	iIconIM;
		BBitmap 	*	iIconSounds;
		BBitmap 	*	iIconCatalog;
		BBitmap 	*	iIconNotify;
		BBitmap 	*	iIconTheme;
		BBitmap		*	iIconConnection;
		
		bool			iIsDown;
		bool 			iIsOver;
		int				iItemOver;
		int				iItemOverOld;
		int				iItemOverNew;
		int 			iItemSelected;
		int				iItemSelectedOld;

		BResources	*	iResources;
	};

#endif /* __BEGADU_PREFERENCES_TAB_H__ */
