/*
	Opcje.cpp
	Ten plik jest częscią kodu źródłowego BeGadu.
	Homepage: http://begadu.sf.net
*/
#include <stdio.h>
#include <stdlib.h>
#include <OS.h>
#include <Application.h>
#include <Roster.h>
#include <Path.h>
#include <View.h>
#include <String.h>
#include <TextControl.h>
#include <Button.h>
#include <Bitmap.h>
#include <Screen.h>
#include <TabView.h>
#include <CheckBox.h>
#include <Box.h>
#include <OutlineListView.h>
#include <ListView.h>
#include <ListItem.h>
#include "Msg.h"
#include "Preferences.h"
#include "PreferencesTab.h"
#include "Main.h"
#include "GfxStuff.h"
#include "Network.h"
#include "Person.h"

#ifdef ZETA
#include <locale/Locale.h>
#else
#define _T(str) (str)
#endif

#define PREFERENCES_NAME "Preferences"

Preferences::Preferences( Profile* aProfile, MainWindow* aWindow, BRect aRect, BResources* aRes ) : BWindow( aRect, PREFERENCES_NAME, B_FLOATING_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_NOT_MOVABLE | B_ASYNCHRONOUS_CONTROLS )
	{

#ifdef ZETA
	app_info appinfo;
	be_app->GetAppInfo( &appinfo );
	BPath localization;
	BEntry entryloc( &appinfo.ref, true );
	entryloc.GetPath( &localization );
	localization.GetParent( &localization );
	localization.Append( "Language/Dictionaries/BeGadu" );
	BString localization_string;
	if( localization.InitCheck() != B_OK )
		localization_string.SetTo( "Language/Dictionaries/BeGadu" );
	else
		localization_string.SetTo( localization.Path() );
	be_locale.LoadLanguageFile( localization_string.String() );
#endif

	SetTitle( _T( PREFERENCES_NAME ) );

	iProfile = aProfile;
	iWindow = aWindow;
	iResources = aRes;
	BRect r = Bounds();

	/* making a default background */
	BView *someView;
	someView = new BView( r, "some view", B_FOLLOW_ALL, B_WILL_DRAW );
	someView->SetViewColor( 60, 60, 60 );
	AddChild( someView );

	r = Bounds();
	r.bottom = r.top + 60;
	iPreferencesTab = new PreferencesTab( r, "Preferences Tab", iResources );
	AddChild( iPreferencesTab );
	
/*
	r = Bounds();
	r.left = 20;
	r.top = 100;
	r.right = r.left + 250;
	r.bottom = r.top + 25;
	iNumberControl = new BTextControl( r, "iNumberControl", _T("Number GG:"), "0", NULL, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE );
	AddChild( iNumberControl );
	r.left = 20;
	r.right = r.left + 250;
	r.top = 130;
	r.bottom = r.top + 25;
	iPasswordControl = new BTextControl( r, "iPasswordControl", _T("Password:"), "", NULL, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE );
	AddChild( iPasswordControl );
	r = Bounds();
	BButton *przycisk;
    przycisk = new BButton( BRect( r.left + 360, r.bottom - 30,
    							   r.left + 440, r.bottom - 5),
    							   "ok button", _T("Ok"),
    							   new BMessage( PREFERENCES_OK ) );
    przycisk->MakeDefault(true);
    AddChild(przycisk);
    
    przycisk = new BButton( BRect( r.left + 270, r.bottom - 30,
    							   r.left + 350, r.bottom - 5),
    							   "cancel button", _T("Cancel"),
    							   new BMessage( PREFERENCES_CANCEL ) );
    AddChild(przycisk);

    if( iNumberControl->LockLooper() )
    	{
		BString a;
		a << ( int32 ) iProfile->iNumber;
        iNumberControl->SetText( a.String() );
        iPasswordControl->SetText( iProfile->iPassword->String() );
        iNumberControl->UnlockLooper();
    	}    
*/
	}

void Preferences::MessageReceived( BMessage* aMessage )
	{
	switch( aMessage->what )
		{
		/* anuluj zmiany */
		case PREFERENCES_CANCEL:
			{
			BMessenger( this ).SendMessage( B_QUIT_REQUESTED );
			break;
			}
		
		case PREFERENCES_OK:
			{
			if( iNumberControl->LockLooper())
				{
				iProfile->iNumber = atoi( iNumberControl->Text() );
				iNumberControl->UnlockLooper();
				}
			if( iPasswordControl->LockLooper())
				{
				iProfile->iPassword->SetTo( iPasswordControl->Text() );
				iPasswordControl->UnlockLooper();
				}
			BMessenger( this ).SendMessage( B_QUIT_REQUESTED );
			break;
			}
			
		default:
			BWindow::MessageReceived( aMessage );
		}
	}

void Preferences::Show()
	{
    BWindow::Show();
	}

BBitmap* Preferences::GetBitmap( const char* aName )
	{
	BBitmap 	*bitmap = NULL;
	size_t 		len = 0;
	status_t 	error;	

	const void *data = iResources->LoadResource( 'BBMP', aName, &len );

	BMemoryIO stream(data, len);
	
	BMessage archive;
	error = archive.Unflatten(&stream);
	if (error != B_OK)
		return NULL;
	bitmap = new BBitmap(&archive);
	if(!bitmap)
		return NULL;

	if(bitmap->InitCheck() != B_OK)
	{
		delete bitmap;
		return NULL;
	}
	
	return bitmap;
}
