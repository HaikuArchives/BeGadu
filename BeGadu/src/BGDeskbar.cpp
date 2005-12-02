/*
 * ============================================================================
 *  Name     : BGDeskbar from BGDeskbar.h
 *  Part of  : BeGadu
 *  Authors  : 
 *		Artur Wyszynski <artur.wyszynski@bellstream.pl>
 *  Implementation notes:
 *		BGDeskbar replicant
 *  Version  : 1.2
 * ============================================================================
 */

#include <Alert.h>
#include <Bitmap.h>
#include <Deskbar.h>
#include <FindDirectory.h>
#include <InterfaceDefs.h>
#include <Menu.h>
#include <MenuItem.h>
#include <Message.h>
#include <Path.h>
#include <PopUpMenu.h>
#include <Resources.h>
#include <Roster.h>
#include <Screen.h>
#include <String.h>
#include <stdio.h>

#include "BGDeskbar.h"
#include "Debug.h"
#include "GaduMenuItem.h"
#include "GfxStuff.h"
#include "Main.h"
#include "Msg.h"
#include "ProfileWizard.h"

#ifdef ZETA
#include <locale/Locale.h>
#else
#define _T(str) (str)
#endif

#ifdef DEBUG
#define DEBUG_TRACE(str) fprintf(stderr, str)
#else
#define DEBUG_TRACE(str)
#endif

extern "C" _EXPORT BView* instantiate_deskbar_item();

BView *instantiate_deskbar_item( void )
	{
	DEBUG_TRACE( "instantiate_deskbar_item()\n" );
	return new BGDeskbar();
	}

BGDeskbar::BGDeskbar() : BView( BRect( 0, 0, 15, 15),
								"BGDeskbar",
								B_FOLLOW_LEFT | B_FOLLOW_TOP,
								B_WILL_DRAW | B_PULSE_NEEDED )
	{
	DEBUG_TRACE( "BGDeskbar()\n" );

#ifdef ZETA
	BPath localization( "/boot/apps/Internet/BeGadu/Language/Dictionaries/BeGadu" );
	if( localization.InitCheck() == B_OK )
		be_locale.LoadLanguageFile( localization.Path() );
#endif

	iIcon = NULL;
	iIconAvail = NULL;
	iIconAvailDescr = NULL;
	iIconBusy = NULL;
	iIconBusyDescr = NULL;
	iIconInvisible = NULL;
	iIconInvisibleDescr = NULL;
	iIconNotAvail = NULL;
	iIconNotAvailDescr = NULL;
	iIconQuit = NULL;
	iIconNewMessage = NULL;
	iMenu = NULL;
	}

BGDeskbar::BGDeskbar( BMessage *aMessage ) : BView( aMessage )
	{
	DEBUG_TRACE( "BGDeskbar( aMessage )\n" );
	
#ifdef ZETA
	BPath localization( "/boot/apps/Internet/BeGadu/Translations/BeGadu" );
	if( localization.InitCheck() == B_OK )
		be_locale.LoadLanguageFile( localization.Path() );
#endif

	iIcon = NULL;
	iIconAvail = NULL;
	iIconAvailDescr = NULL;
	iIconBusy = NULL;
	iIconBusyDescr = NULL;
	iIconInvisible = NULL;
	iIconInvisibleDescr = NULL;
	iIconNotAvail = NULL;
	iIconNotAvailDescr = NULL;
	iIconQuit = NULL;
	iIconNewMessage = NULL;
	iMenu = NULL;
	Initialize();
	}

BGDeskbar::~BGDeskbar()
	{
	DEBUG_TRACE( "BGDeskbar::~BGdeskbar()\n" );
	if( iIconAvail )
		{
		delete iIconAvail;
		iIconAvail = NULL;
		}
	if( iIconAvailDescr )
		{
		delete iIconAvailDescr;
		iIconAvailDescr = NULL;
		}
	if( iIconBusy )
		{
		delete iIconBusy;
		iIconBusy = NULL;
		}
	if( iIconBusyDescr )
		{
		delete iIconBusyDescr;
		iIconBusyDescr = NULL;
		}
	if( iIconInvisible )
		{
		delete iIconInvisible;
		iIconInvisible = NULL;
		}
	if( iIconInvisibleDescr )
		{
		delete iIconInvisibleDescr;
		iIconInvisibleDescr = NULL;
		}
	if( iIconNotAvail )
		{
		delete iIconNotAvail;
		iIconNotAvail = NULL;
		}
	if( iIconNotAvailDescr )
		{
		delete iIconNotAvailDescr;
		iIconNotAvailDescr = NULL;
		}
	if( iIconQuit )
		{
		delete iIconQuit;
		iIconQuit = NULL;
		}
	if( iIconNewMessage )
		{
		delete iIconNewMessage;
		iIconNewMessage = NULL;
		}
	if( iIcon )
		{
		iIcon = NULL;
		}
	if( iMenu )
		{
		delete iMenu;
		iMenu = NULL;
		}
	}

void BGDeskbar::Initialize()
	{
	DEBUG_TRACE( "BGDeskbar::Initialize()\n" );
	BRoster roster;
	entry_ref ref;
	BFile resfile;
	if( roster.IsRunning( "application/x-vnd.BeGadu" ) )
		{
		fprintf( stderr, _T("BGDeskbar: Found running BeGadu...\n") );
		roster.FindApp( "application/x-vnd.BeGadu", &ref );
		resfile.SetTo( &ref, B_READ_ONLY );	
		iResources.SetTo( &resfile );
		iIconAvail = GetBitmap( "online" );
		iIconBusy = GetBitmap( "busy" );
		iIconInvisible = GetBitmap( "invisible" );
		iIconNotAvail = GetBitmap( "offline" );
		iIconAvailDescr = GetBitmap( "online_d" );
		iIconBusyDescr = GetBitmap( "busy_d" );
		iIconInvisibleDescr = GetBitmap( "invisible_d" );
		iIconNotAvailDescr = GetBitmap( "offline_d" );
		iIconNewMessage = GetBitmap( "message" );
		iIconQuit = GetBitmap( "quit" );
		iIcon = iIconNotAvail;
		}
	else
		{
		fprintf(stderr, _T("BGDeskbar: BeGadu not runned, launching it...\n") );
		be_roster->Launch( "application/x-vnd.BeGadu" );
		}
	iMenu = new BPopUpMenu( "BGDeskbarMenu", true, true );
	GaduMenuItem *availItem = new GaduMenuItem( iIconAvail, _T("Availble"), new BMessage( SET_AVAIL ) );
	iMenu->AddItem( availItem );
	GaduMenuItem *busyItem = new GaduMenuItem( iIconBusy, _T("Be right back"), new BMessage( SET_BRB ) );
	iMenu->AddItem( busyItem );
	GaduMenuItem *invisibleItem = new GaduMenuItem( iIconInvisible, _T("Invisible"), new BMessage( SET_INVIS ) );
	iMenu->AddItem( invisibleItem );
	GaduMenuItem *notavailItem = new GaduMenuItem( iIconNotAvail, _T("Not availble"), new BMessage( SET_NOT_AVAIL ) );
	iMenu->AddItem( notavailItem );
	GaduMenuItem *descrItem = new GaduMenuItem( iIconAvailDescr, _T("Change description"), new BMessage( CHANGE_DESCRIPTION ) );
	iMenu->AddItem( descrItem );
	iMenu->AddSeparatorItem();
	GaduMenuItem *aboutItem = new GaduMenuItem( iIconAvail, _T("About BeGadu"), new BMessage( BEGG_ABOUT ) );
	iMenu->AddItem( aboutItem );
	GaduMenuItem *quitItem = new GaduMenuItem( iIconQuit, _T("Exit"), new BMessage( BEGG_QUIT ) );
	iMenu->AddItem( quitItem );
	}

BArchivable* BGDeskbar::Instantiate( BMessage *aData )
	{
	if( !validate_instantiation( aData, "BGDeskbar" ) )
		{
		return NULL;
		}
	return ( new BGDeskbar( aData ) );
	}

status_t BGDeskbar::Archive( BMessage *aData, bool aDeep = true) const
	{
	BView::Archive( aData, aDeep );
	aData->AddString( "add_on", "application/x-vnd.BeGadu" );
	return B_NO_ERROR;
	}

void BGDeskbar::MessageReceived( BMessage *aMessage )
	{
	switch( aMessage->what )
		{
		case BGDESKBAR_CHSTATE:
			{
			fprintf( stderr, "BGDeskbar::MessageReceived( BGDESKBAR_CHSTATE );\n" );
			int16 status;
			if( aMessage->FindInt16( "iStatus", &status ) != B_OK )
				{
				fprintf( stderr, _T("Can't find 'iStatus' in message...\n") );
				break;
				}
			else
				{
				switch( status )
					{
					case GG_STATUS_AVAIL:
						{
						iIcon = iIconAvail;
						break;
						}

					case GG_STATUS_BUSY:
						{
						iIcon = iIconBusy;
						break;
						}

					case GG_STATUS_INVISIBLE:
						{
						iIcon = iIconInvisible;
						break;
						}

					case GG_STATUS_NOT_AVAIL:
						{
						iIcon = iIconNotAvail;
						break;
						}

					case GG_STATUS_AVAIL_DESCR:
						{
						iIcon = iIconAvailDescr;
						break;
						}

					case GG_STATUS_BUSY_DESCR:
						{
						iIcon = iIconBusyDescr;
						break;
						}

					case GG_STATUS_INVISIBLE_DESCR:
						{
						iIcon = iIconInvisibleDescr;
						break;
						}

					case GG_STATUS_NOT_AVAIL_DESCR:
						{
						iIcon = iIconNotAvailDescr;
						break;
						}
					}
				}
			break;
			}
		default:
			BView::MessageReceived( aMessage );
		}
	}


void BGDeskbar::Draw( BRect aRect )
	{
	SetHighColor( Parent()->ViewColor() );
	FillRect( Bounds() );
	SetDrawingMode( B_OP_OVER );

	if( iIcon )
		{
		SetDrawingMode( B_OP_ALPHA );
		SetLowColor( 0, 0, 0, 0 );
		SetViewColor( B_TRANSPARENT_32_BIT );
		DrawBitmap( iIcon, BPoint( 0, 0 ) );
		}
	else
		{
		SetHighColor( 110, 110, 110 );
		FillRect( Bounds() );
		}
	}

void BGDeskbar::Pulse()
	{
	/* temporary... */
	Invalidate();
	}

void BGDeskbar::MouseDown( BPoint aWhere )
	{
	DEBUG_TRACE( "BGDeskbar: MouseDown( aWhere )\n" );
	unsigned long buttons;
	if( LockLooper() )
		{
		GetMouse( &aWhere, &buttons, false );
		UnlockLooper();
		}
	if( buttons & B_PRIMARY_MOUSE_BUTTON )
		{
		DEBUG_TRACE( "BGDeskbar: MouseDown( B_PRIMARY_MOUSE_BUTTON )\n" );
		BMessenger( "application/x-vnd.BeGadu" ).SendMessage( new BMessage( SHOW_MAIN_WINDOW ) );
		}
	else if( buttons & B_SECONDARY_MOUSE_BUTTON )
		{
		DEBUG_TRACE( "BGDeskbar: MouseDown( B_SECONDARY_MOUSE_BUTTON )\n" );
		GaduMenuItem *selectedItem = (GaduMenuItem*) iMenu->Go( ConvertToScreen( aWhere ), false, true );
		if( selectedItem )
			{
			BMessenger( "application/x-vnd.BeGadu" ).SendMessage( selectedItem->Message() );
			}
		}
	}

void BGDeskbar::AttachedToWindow()
	{
	DEBUG_TRACE( "BGDeskbar::AttachedToWindow()\n" );
	snooze( 500*100 );
	BMessage* message = new BMessage( ADD_MESSENGER );
	message->AddMessenger( "messenger", BMessenger( this ) );
	BMessenger( "application/x-vnd.BeGadu" ).SendMessage( message );
	delete message;
	BView::AttachedToWindow();
	}

void BGDeskbar::DetachedFromWindow()
	{
	DEBUG_TRACE( "BGDeskbar::DetachedFromWindow()\n" );
	BView::DetachedFromWindow();
	}

void BGDeskbar::Remove()
	{
	}

BBitmap *BGDeskbar::GetBitmap( const char* aName )
	{
	BBitmap 	*bitmap = NULL;
	size_t 		len = 0;
	status_t 	error;	

	const void *data = iResources.LoadResource( 'BBMP', aName, &len );

	BMemoryIO stream( data, len );
	
	BMessage archive;
	error = archive.Unflatten( &stream );
	if( error != B_OK )
		return NULL;
	bitmap = new BBitmap( &archive );
	if( !bitmap )
		return NULL;

	if( bitmap->InitCheck() != B_OK )
		{
		delete bitmap;
		return NULL;
		}
	
	return bitmap;
	}
