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
#include "ProfilWizard.h"

extern "C" _EXPORT BView* instantiate_deskbar_item();

BView *instantiate_deskbar_item( void )
{
	return new BGDeskbar();
}

BGDeskbar::BGDeskbar() : BView( BRect( 0, 0, 15, 15),
								"BGDeskbar",
								B_FOLLOW_LEFT | B_FOLLOW_TOP,
								B_WILL_DRAW | B_PULSE_NEEDED )
{
	fprintf( stderr, "BGDeskbar()\n" );
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
}

BGDeskbar::BGDeskbar( BMessage *aMessage ) : BView( aMessage )
{
	fprintf( stderr, "BGDeskbar( aMessage )\n" );
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
	Initialize();
}

BGDeskbar::~BGDeskbar()
{
	fprintf( stderr, "BGDeskbar::~BGdeskbar()\n" );
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
		iIconAvailDescr = NULL;
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
	iIcon = NULL;
}

void BGDeskbar::Initialize()
{
	fprintf( stderr, "BGDeskbar::Initialize()\n" );
	BRoster roster;
	entry_ref ref;
	BFile resfile;
	if( !roster.IsRunning( "application/x-vnd.BeGadu" ) )
	{
		be_roster->Launch( "application/x-vnd.BeGadu" );
	}
	if( roster.IsRunning( "application/x-vnd.BeGadu" ) )
	{
		fprintf( stderr, "BGDeskbar: Jest BeGadu :D\n" );
		roster.FindApp( "application/x-vnd.BeGadu", &ref );
		resfile.SetTo( &ref, B_READ_ONLY );	
		fResources.SetTo( &resfile );
		iIconAvail = GetBitmap( "Avail.png" );
		iIconBusy = GetBitmap( "Busy.png" );
		iIconInvisible = GetBitmap( "Invisible.png" );
		iIconNotAvail = GetBitmap( "NotAvail.png" );
		iIconAvailDescr = GetBitmap( "AvailDescr.png" );
		iIconBusyDescr = GetBitmap( "BusyDescr.png" );
		iIconInvisibleDescr = GetBitmap( "InvisibleDescr.png" );
		iIconNotAvailDescr = GetBitmap( "NotAvail.png" );
		iIconNewMessage = GetBitmap( "NewMessage.png" );
		iIconQuit = GetBitmap( "Quit.png" );
		iIcon = iIconNotAvail;
	}
	else
		fprintf(stderr, "BGDeskbar: Nie ma BeGadu :(\n" );
}

BGDeskbar *BGDeskbar::Instantiate( BMessage *aData )
{
	if( !validate_instantiation( aData, "BGDeskbar" ) )
		return NULL;
	return ( new BGDeskbar( aData ) );
}

status_t BGDeskbar::Archive( BMessage *aData, bool aDeep = true) const
{
	BView::Archive( aData, aDeep );
	aData->AddString( "add_on", "application/x-vnd.BeGadu" );
	aData->AddString( "add-on", "application/x-vnd.BeGadu" );
	aData->AddString( "class", "BGDeskbar" );
	return B_NO_ERROR;
}

void BGDeskbar::MessageReceived( BMessage *aMessage )
{
	switch( aMessage->what )
	{
		case SET_AVAIL:
		case SET_BRB:
		case SET_INVIS:
		case SET_NOT_AVAIL:
		case SET_DESCRIPTION:
		case BEGG_ABOUT:
		case REMOVE_FROM_DESKBAR:
		{
			BMessenger messenger( "application/x-vnd.BeGadu" );
			messenger.SendMessage( aMessage );
			fprintf( stderr, "BGDeskbar::MessageReceived() and forwarded to BeGadu\n" );
			break;
		}
		/* handling messages */
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
	unsigned long buttons;
	if( LockLooper() )
	{
		GetMouse( &aWhere, &buttons, false );
		UnlockLooper();
	}
	if( buttons & B_PRIMARY_MOUSE_BUTTON )
	{
//		if( iWindow->LockLooper() )
//		{
//			if( iWindow->IsHidden() )
//				iWindow->Show();
//			else
//				iWindow->Activate();
//			iWindow->UnlockLooper();
//		}
	}
	else if( buttons & B_SECONDARY_MOUSE_BUTTON )
	{
//		GaduMenuItem *selectedItem = (GaduMenuItem*) iMenu->Go( ConvertToScreen( aWhere ) );
//		if( selectedItem )
//		{
//			BLooper *looper;
//			BHandler *target = selectedItem->Target( &looper );
//			BMessenger(target).SendMessage( selectedItem->Message() );
//		}
	iMenu = new BPopUpMenu( "BGDeskbarMenu", true, true );
	GaduMenuItem *availItem = new GaduMenuItem( iIconAvail, "Dostepny", new BMessage( SET_AVAIL ) );
	iMenu->AddItem( availItem );
	GaduMenuItem *busyItem = new GaduMenuItem( iIconBusy, "Zaraz wracam", new BMessage( SET_BRB ) );
	iMenu->AddItem( busyItem );
	GaduMenuItem *invisibleItem = new GaduMenuItem( iIconInvisible, "Niewidoczny", new BMessage( SET_INVIS ) );
	iMenu->AddItem( invisibleItem );
	GaduMenuItem *notavailItem = new GaduMenuItem( iIconNotAvail, "Niedostepny", new BMessage( SET_NOT_AVAIL ) );
	iMenu->AddItem( notavailItem );
	GaduMenuItem *descrItem = new GaduMenuItem( iIconAvailDescr, "Zmien opis", new BMessage( SET_DESCRIPTION ) );
	iMenu->AddItem( descrItem );
	iMenu->AddSeparatorItem();
	GaduMenuItem *aboutItem = new GaduMenuItem( iIconAvail, "O BeGadu..", new BMessage( BEGG_ABOUT ) );
	iMenu->AddItem( aboutItem );
	GaduMenuItem *quitItem = new GaduMenuItem( iIconQuit, "Zakoncz", new BMessage( REMOVE_FROM_DESKBAR ) );
	iMenu->AddItem( quitItem );
	iMenu->SetTargetForItems( this );
	iMenu->Go( ConvertToScreen( aWhere ), true, true, Bounds() , true );
	delete iMenu;
	}
}

void BGDeskbar::AttachedToWindow()
{
	/* temporary empty */
	BView::AttachedToWindow();
}

void BGDeskbar::DetachedFromWindow()
{
	/* temporary empty */
	BView::DetachedFromWindow();
}

void BGDeskbar::Remove()
{
}

BBitmap *BGDeskbar::GetBitmap(const char *name)
{
	BBitmap 	*bitmap = NULL;
	size_t 		len = 0;
	status_t 	error;	

	const void *data = fResources.LoadResource('BBMP', name, &len);

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
