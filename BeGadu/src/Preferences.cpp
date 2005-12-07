/*
	Opcje.cpp
	Ten plik jest częscią kodu źródłowego BeGadu.
	Homepage: http://begadu.sf.net
*/
#include <Alert.h>
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
#include "PreferencesItem.h"
#include "PreferencesList.h"
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
	iBox = 1;
	BRect r = Bounds();

	/* making a default background */
	iView = new BView( r, "background view", B_FOLLOW_ALL, B_WILL_DRAW );
//	iView->SetViewUIColor( B_UI_PANEL_BACKGROUND_COLOR );
	iView->SetViewColor( 255, 255, 255 );
	AddChild( iView );

	r = Bounds();
	r.right = r.left + 90;

	iPreferencesList = new PreferencesList( r, "Preferences List", this );
	iPreferencesList->SetViewUIColor( B_UI_DOCUMENT_BACKGROUND_COLOR );
	
	PreferencesItem *pi1 = new PreferencesItem( new BString( _T( "General" ) ), GetBitmap( "config_general" ) );
	iPreferencesList->AddItem( pi1 );

	PreferencesItem *pi2 = new PreferencesItem( new BString( _T( "IM" ) ), GetBitmap( "config_im" ) );
	iPreferencesList->AddItem( pi2 );

	PreferencesItem *pi3 = new PreferencesItem( new BString( _T( "Sounds" ) ), GetBitmap( "config_sounds" ) );
	iPreferencesList->AddItem( pi3 );

	PreferencesItem *pi4 = new PreferencesItem( new BString( _T( "Catalog" ) ), GetBitmap( "config_catalog" ) );
	iPreferencesList->AddItem( pi4 );

	PreferencesItem *pi5 = new PreferencesItem( new BString( _T( "Notify" ) ), GetBitmap( "config_notify" ) );
	iPreferencesList->AddItem( pi5 );

	PreferencesItem *pi6 = new PreferencesItem( new BString( _T( "Theme" ) ), GetBitmap( "config_theme" ) );
	iPreferencesList->AddItem( pi6 );

	PreferencesItem *pi7 = new PreferencesItem( new BString( _T( "Connection" ) ), GetBitmap( "config_connection" ) );
	iPreferencesList->AddItem( pi7 );
	
	iPreferencesList->Select( 0 );
	iView->AddChild( iPreferencesList );
	
	r = Bounds();
	r.left = 95;
	r.top = r.top + 5;
	r.right = r.right - 5;
	r.bottom = r.bottom - 40;
	
	iBoxGeneral = new BBox( r, "Box General" );
	iBoxGeneral->SetLabel( _T( "General" ) );

	iBoxIM = new BBox( r, "Box IM" );
	iBoxIM->SetLabel( _T( "IM" ) );
	iBoxIM->Hide();

	iBoxSounds = new BBox( r, "Box Sounds" );
	iBoxSounds->SetLabel( _T( "Sounds" ) );
	iBoxSounds->Hide();

	iBoxCatalog = new BBox( r, "Box Catalog" );
	iBoxCatalog->SetLabel( _T( "Catalog" ) );
	iBoxCatalog->Hide();

	iBoxNotify = new BBox( r, "Box Notify" );
	iBoxNotify->SetLabel( _T( "Notify" ) );
	iBoxNotify->Hide();

	iBoxTheme = new BBox( r, "Box Theme" );
	iBoxTheme->SetLabel( _T( "Theme" ) );
	iBoxTheme->Hide();

	iBoxConnection = new BBox( r, "Box Connection" );
	iBoxConnection->SetLabel( _T( "Connection" ) );
	iBoxConnection->Hide();
	
	iView->AddChild( iBoxGeneral );
	iView->AddChild( iBoxIM );
	iView->AddChild( iBoxSounds );
	iView->AddChild( iBoxCatalog );
	iView->AddChild( iBoxNotify );
	iView->AddChild( iBoxTheme );
	iView->AddChild( iBoxConnection );

	r = iBoxGeneral->Bounds();
	r.left = 10;
	r.top = 20;
	r.right = r.right - 10;
	r.bottom = r.top + 120;
	
	iBoxGeneralProfile = new BBox( r, "Profile Box" );
	iBoxGeneralProfile->SetLabel( _T( "Profile" ) );
	
	BFont *font = new BFont( be_plain_font );
	font->SetSize( 12.0 );

	r = iBoxGeneralProfile->Bounds();
	r.left = 10;
	r.top = 20;
	r.right = r.right - 10;
	r.bottom = r.top + 25;
	iProfileNameControl = new BTextControl( r,
											"iProfileNameControl",
											_T( "Profile name:"),
											"",
											NULL );
	iProfileNameControl->SetDivider( 100 );
	iProfileNameControl->SetAlignment( B_ALIGN_RIGHT, B_ALIGN_LEFT );
	iProfileNameControl->SetFont( font );
	
	iBoxGeneralProfile->AddChild( iProfileNameControl );
	
	r = iBoxGeneralProfile->Bounds();
	r.left = 10;
	r.top = iProfileNameControl->Bounds().bottom + 35;
	r.bottom = r.top + 25;
	r.right = r.right - 130;
	iProfilePasswordCheckBox = new BCheckBox( r,
											  "iProfilePasswordCheckBox",
											  _T( "Access to the profile with a password" ),
											  new BMessage( PROFILE_PASS_CHECK_BOX ) );
	iProfilePasswordCheckBox->SetFont( font );
	iBoxGeneralProfile->AddChild( iProfilePasswordCheckBox );

	r = iBoxGeneralProfile->Bounds();
	r.left = iProfilePasswordCheckBox->Bounds().right + 15;
	r.top = iProfileNameControl->Bounds().bottom + 37;
	r.right = r.right - 5;
	r.bottom = r.top + 25;
	r.bottom = r.top + 25;
	r.right = r.right - 10;
	iProfilePasswordControl	= new BTextControl( r,
												"iProfilePasswordControl",
												NULL,
												"",
												NULL );
	iProfilePasswordControl->TextView()->HideTyping( true );
	iProfilePasswordControl->SetFont( font );
	iBoxGeneralProfile->AddChild( iProfilePasswordControl );

	r = iBoxGeneralProfile->Bounds();
	r.left = 150;
	r.top = iProfileNameControl->Bounds().bottom + 70;
	r.right = r.right - 5;
	r.bottom = r.top + 25;
	r.bottom = r.top + 25;
	r.right = r.right - 10;
	iProfilePasswordRepeatControl = new BTextControl( r,
													  "iProfilePasswordRepeatControl",
													  _T( "Repeat password:" ),
													  "",
													  NULL );
	iProfilePasswordRepeatControl->TextView()->HideTyping( true );
	iProfilePasswordRepeatControl->SetDivider( 105 );
	iProfilePasswordRepeatControl->SetAlignment( B_ALIGN_RIGHT, B_ALIGN_LEFT );
	iProfilePasswordRepeatControl->SetFont( font );
	iBoxGeneralProfile->AddChild( iProfilePasswordRepeatControl );

	iBoxGeneral->AddChild( iBoxGeneralProfile );

	r = iBoxGeneral->Bounds();
	r.left = 10;
	r.top = iBoxGeneralProfile->Bounds().bottom + 30;
	r.right = r.right - 10;
	r.bottom = r.top + 120;
	
	iBoxGeneralAccount = new BBox( r, "Account Box" );
	iBoxGeneralAccount->SetLabel( _T( "Your account" ) );
	
	r = iBoxGeneralAccount->Bounds();
	r.left = 10;
	r.top = 20;
	r.right = r.left + 220;
	r.bottom = r.top + 25;
	iProfileUINControl = new BTextControl( r,
										   "iProfileUINControl",
										   _T( "Number:" ),
										   "0",
										   NULL );
	iProfileUINControl->SetDivider( 60 );
	iProfileUINControl->SetAlignment( B_ALIGN_RIGHT, B_ALIGN_LEFT );
	iProfileUINControl->SetFont( font );
	iBoxGeneralAccount->AddChild( iProfileUINControl );

	r = iBoxGeneralAccount->Bounds();
	r.left = r.right - 130;
	r.top = 17;
	r.right = r.right - 5 ;
	r.bottom = r.top + 20;
	iProfileDeleteAccount = new BButton( r,
										 "Delete Button",
										 _T( "Delete account" ),
										 new BMessage( PROFILE_DELETE_ACCOUNT ) );
	iBoxGeneralAccount->AddChild( iProfileDeleteAccount );

	r = iBoxGeneralAccount->Bounds();
	r.left = 10;
	r.top = 50;
	r.right = r.left + 220;
	r.bottom = r.top + 25;
	iProfileUINPasswordControl = new BTextControl( r,
												   "iProfileUINPasswordControl",
												   _T( "Password:" ),
												   "",
												   NULL );
	iProfileUINPasswordControl->SetDivider( 60 );
	iProfileUINPasswordControl->SetAlignment( B_ALIGN_RIGHT, B_ALIGN_LEFT );
	iProfileUINPasswordControl->SetFont( font );
	iProfileUINPasswordControl->TextView()->HideTyping( true );
	iBoxGeneralAccount->AddChild( iProfileUINPasswordControl );

	r = iBoxGeneralAccount->Bounds();
	r.left = r.right - 130;
	r.top = 47;
	r.right = r.right - 5 ;
	r.bottom = r.top + 20;
	iProfileChangePassword = new BButton( r,
										 "Change pass Button",
										 _T( "Change" ),
										 new BMessage( PROFILE_CHANGE_PASSWORD ) );
	iBoxGeneralAccount->AddChild( iProfileChangePassword );
	
	r = iBoxGeneralAccount->Bounds();
	r.left = 70;
	r.top = 80;
	r.right = r.left + 220;
	r.bottom = r.top + 25;
	iProfileRememberPasswordCheckBox = new BCheckBox( r,
												  	  "iProfileRememberPasswordCheckBox",
											  		  _T( "Remember password" ),
											  		  NULL );
	iProfileRememberPasswordCheckBox->SetFont( font );
	iBoxGeneralAccount->AddChild( iProfileRememberPasswordCheckBox );


	iBoxGeneral->AddChild( iBoxGeneralAccount );


	/* Ok & Cancel buttons */
	BButton *button;

	r = Bounds();
	r.left = r.right - 180;
	r.right = r.left + 80;
	r.top = r.bottom - 30;
	r.bottom = r.bottom - 10;
    button = new BButton( r, "ok button", _T( "Ok" ), new BMessage( PREFERENCES_OK ) );
    button->MakeDefault( true );
    iView->AddChild( button );

	r = Bounds();
	r.left = r.right - 90;
	r.right = r.left + 80;
	r.top = r.bottom - 30;
	r.bottom = r.bottom - 10;
    button = new BButton( r, "cancel button", _T( "Cancel" ), new BMessage( PREFERENCES_CANCEL ) );
    iView->AddChild( button );
    
    LoadConfig();
	}

void Preferences::MessageReceived( BMessage* aMessage )
	{
	switch( aMessage->what )
		{
		case PROFILE_PASS_CHECK_BOX:
			{
			if( iProfilePasswordCheckBox->Value() == 0 )
				{
				iProfilePasswordControl->SetEnabled( false );
				iProfilePasswordRepeatControl->SetEnabled( false );
				}
			else
				{
				iProfilePasswordControl->SetEnabled( true );
				iProfilePasswordRepeatControl->SetEnabled( true );
				}
			break;
			}
			
		case PREFERENCES_SWITCH:
			{
			int32 current;
			aMessage->FindInt32( "SwitchTo", &current );
			switch( iBox )
				{
				case 1:
					{
					if( current != iBox )
						{
						iBoxGeneral->Hide();
						}
					break;
					}
				case 2:
					{
					if( current != iBox )
						{
						iBoxIM->Hide();
						}
					break;
					}
				case 3:
					{
					if( current != iBox )
						{
						iBoxSounds->Hide();
						}
					break;
					}
				case 4:
					{
					if( current != iBox )
						{
						iBoxCatalog->Hide();
						}
					break;
					}
				case 5:
					{
					if( current != iBox )
						{
						iBoxNotify->Hide();
						}
					break;
					}
				case 6:
					{
					if( current != iBox )
						{
						iBoxTheme->Hide();
						}
					break;
					}
				case 7:
					{
					if( current != iBox )
						{
						iBoxConnection->Hide();
						}
					break;
					}
				}
			switch( current )
				{
				case 1:
					{
					if( current != iBox )
						{
						iBoxGeneral->Show();
						}
					break;
					}
				case 2:
					{
					if( current != iBox )
						{
						iBoxIM->Show();
						}
					break;
					}
				case 3:
					{
					if( current != iBox )
						{
						iBoxSounds->Show();
						}
					break;
					}
				case 4:
					{
					if( current != iBox )
						{
						iBoxCatalog->Show();
						}
					break;
					}
				case 5:
					{
					if( current != iBox )
						{
						iBoxNotify->Show();
						}
					break;
					}
				case 6:
					{
					if( current != iBox )
						{
						iBoxTheme->Show();
						}
					break;
					}
				case 7:
					{
					if( current != iBox )
						{
						iBoxConnection->Show();
						}
					break;
					}
				}
				iBox = current;
			break;
			}
			
		/* anuluj zmiany */
		case PREFERENCES_CANCEL:
			{
			BMessenger( this ).SendMessage( B_QUIT_REQUESTED );
			break;
			}
		
		case PREFERENCES_OK:
			{
			if( iProfilePasswordCheckBox->Value() == 1 )
				{
				BString p1;
				BString p2;
				if( iProfilePasswordControl->LockLooper() )
					{
					p1.SetTo( iProfilePasswordControl->Text() );
					iProfilePasswordControl->UnlockLooper();
					}
				if( iProfilePasswordRepeatControl->LockLooper() )
					{
					p2.SetTo( iProfilePasswordRepeatControl->Text() );
					iProfilePasswordRepeatControl->UnlockLooper();
					}
				if( p1.Compare( p2 ) != 0 )
					{
					BAlert* alert = new BAlert( "BeGadu",
												"Hasło do profilu musi takie samo w obydwu polach",
												"Wróć" );
					alert->Go();
					break;
					}
				else
					{
					iProfile->SetProfilePassword( p1.String() );
					}
				}
			else
				{
				if( iProfilePasswordControl->LockLooper() )
					{
					iProfilePasswordControl->SetText( "" );
					iProfilePasswordControl->UnlockLooper();
					}
				if( iProfilePasswordRepeatControl->LockLooper() )
					{
					iProfilePasswordRepeatControl->SetText( "" );
					iProfilePasswordRepeatControl->UnlockLooper();
					}
				}
			
			if( iProfileNameControl->LockLooper() )
				{
				iProfile->SetProfileName( iProfileNameControl->Text() );
				iProfileNameControl->UnlockLooper();
				}
			if( iProfileUINControl->LockLooper() )
				{
				iProfile->SetUIN( atoi( iProfileUINControl->Text() ) );
				iProfileUINControl->UnlockLooper();
				}
			if( iProfileUINPasswordControl->LockLooper())
				{
				iProfile->SetPassword( iProfileUINPasswordControl->Text() );
				iProfileUINPasswordControl->UnlockLooper();
				}
			iProfile->Save();
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

void Preferences::LoadConfig()
	{
	/* loading configuration for general box */

	/* loading profile name */
	if( iProfileNameControl->LockLooper() )
		{
		iProfileNameControl->SetText( iProfile->GetProfileName() );
		iProfileNameControl->UnlockLooper();
		}
	
	/* loading profile password */
	if( strcmp( iProfile->GetProfilePassword(), "" ) == 0 )
		{
		if( iProfilePasswordCheckBox->LockLooper() )
			{
			iProfilePasswordCheckBox->SetValue( 0 );
			iProfilePasswordCheckBox->UnlockLooper();
			}
		BMessenger( this ).SendMessage( new BMessage( PROFILE_PASS_CHECK_BOX ) );
		}
	else
		{
		if( iProfilePasswordCheckBox->LockLooper() )
			{
			iProfilePasswordCheckBox->SetValue( 1 );
			iProfilePasswordCheckBox->UnlockLooper();
			}
		if( iProfilePasswordControl->LockLooper() )
			{
			iProfilePasswordControl->SetText( iProfile->GetProfilePassword() );
			iProfilePasswordControl->UnlockLooper();
			}
		if( iProfilePasswordRepeatControl->LockLooper() )
			{
			iProfilePasswordRepeatControl->SetText( iProfile->GetProfilePassword() );
			iProfilePasswordRepeatControl->UnlockLooper();
			}
		BMessenger( this ).SendMessage( new BMessage( PROFILE_PASS_CHECK_BOX ) );
		}
		
	/* loading account number */
    if( iProfileUINControl->LockLooper() )
    	{
		BString a;
		a << ( int32 ) iProfile->GetUIN();
        iProfileUINControl->SetText( a.String() );
        iProfileUINControl->UnlockLooper();
    	}
    	
    /* loading account password */
    if( iProfileUINPasswordControl->LockLooper() )
    	{
        iProfileUINPasswordControl->SetText( iProfile->GetPassword() );
        iProfileUINPasswordControl->UnlockLooper();
    	}
    
    /* are we remember account password ? */
    if( iProfile->GetRememberPassword() )
    	{
	    if( iProfileRememberPasswordCheckBox->LockLooper() )
    		{
    		iProfileRememberPasswordCheckBox->SetValue( 1 );
    		iProfileRememberPasswordCheckBox->UnlockLooper();
    		}
    	}
	}

void Preferences::SaveConfig()
	{
	}
