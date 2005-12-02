/*
	Siec.cpp
	Ten plik jest częscią kodu źródłowego BeGadu.
	Homepage: http://gadu.beos.pl
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <File.h>
#include <Path.h>
#include <FindDirectory.h>
#include <Message.h>
#include <String.h>
#include <OutlineListView.h>
#include "Msg.h"
#include "Network.h"
#include "Main.h"
#include "Chat.h"
#include "Person.h"
#include "NetworkHandler.h"

#ifdef DEBUG
#define DEBUG_TRACE(str) fprintf(stderr, str)
#else
#define DEBUG_TRACE(str)
#endif

extern "C"
{
#include "libgadu.h"
}

Network::Network( Profile* aProfile, List* aList ) : BLooper( "Network Loop" )
	{
	DEBUG_TRACE( "Network::Network()\n" );
	/* inicjalizacja */
	iProfile		= aProfile;
	iList			= aList;
	iWindow			= NULL;
	iHandlerList	= new BList( 256 );
	iWinList		= new BList( 512 );
	iIdent			= 0;
	iSession		= NULL;
	iStatus			= GG_STATUS_NOT_AVAIL;
	iDescription	= new BString( "" );
	
	/* czekamy na wiadomości :)) */
	Run();
	}

void Network::Quit()
	{
	DEBUG_TRACE( "Network::Quit()\n" );
	/* Rozłączamy się */
	if( ( iStatus != GG_STATUS_NOT_AVAIL ) || ( iStatus != GG_STATUS_NOT_AVAIL_DESCR ) )
		Logout();
	Lock();
	BLooper::Quit();
	}

void Network::MessageReceived( BMessage* aMessage )
	{
	switch( aMessage->what )
		{
		/*
			wiadomości otrzymane od libgadu callback
			cała obsługa jest w SiecLib.cpp, my tylko
			wywołujemy je stąd :)
		*/
		case ADD_HANDLER:
			{
			DEBUG_TRACE( "Network::MessageReceived( ADD_HANDLER )\n" );
			int fd, cond;
			void *data;
			aMessage->FindInt32( "fd", ( int32* ) &fd );
			aMessage->FindInt32( "cond", ( int32* ) &cond );
			aMessage->FindPointer( "data", &data );
			AddHandler( fd, cond, data );
			break;
			}
			
		case DEL_HANDLER:
			{
			DEBUG_TRACE( "Network::MessageReceived( DEL_HANDLER )\n" );
			int fd;
			aMessage->FindInt32( "fd", ( int32* ) &fd );
			RemoveHandler( fd );
			break;
			}
			
		case GOT_MESSAGE:
			{
			DEBUG_TRACE( "Network::MessageReceived( GOT_MESSAGE )\n" );
			int				who;
			const char *	msg;
			aMessage->FindInt32( "who", ( int32* ) &who );
			aMessage->FindString("msg", &msg );
			GotMsg( who, msg );
			break;
			}

		/*
			wiadomości otrzymane od interfejsu
			cała obsługa jest w SiecInt.cpp, my tylko
			wywołujemy je stąd :)
		*/
		
		case DO_LOGIN:
			{
			DEBUG_TRACE( "Network::MessageReceived( DO_LOGIN )\n" );
			Login();
			break;
			}
			
		case DO_LOGOUT:
			{
			DEBUG_TRACE( "Network::MessageReceived( DO_LOGOUT )\n" );
			Logout();
			break;
			}
			
		case ADD_PERSON:
			{
			DEBUG_TRACE( "Network::MessageReceived( ADD_PERSON )\n" );
			// do zaimplementowania
			break;
			}
			
		case DEL_PERSON:
			{
			DEBUG_TRACE( "Network::MessageReceived( DEL_PERSON )\n" );
			// do zaimplementowania
			break;
			}
			
		case OPEN_MESSAGE:
			{
			DEBUG_TRACE( "Network::MessageReceived( OPEN_MESSAGE )\n" );
			int	who;
			aMessage->FindInt32( "who", ( int32* ) &who );
			ChatWindow *win;
			// jeśli mamy już otwarte okno z tą osobą, przejdźmy do niego
			if( ( win = GetMesgWinForUser( who ) ) )
				{
				if( win->LockLooper() )
					{
					win->Activate();
					win->UnlockLooper();
					}
				}
			else // a jeśli nie, tworzymy nowe :)
				{
				win = new ChatWindow( this, iWindow, who );
				iWinList->AddItem( win );
				Person* person;
				if( ( person = GetPersonForUser( who ) ) )
					{
					BMessage *newmessage = new BMessage( UPDATE_STATUS );
					newmessage->AddInt32( "status", person->iStatus );
					BMessenger( win ).SendMessage( newmessage );
					delete newmessage;
					}
				win->Show();
				}
			break;
			}
			
		case SEND_MESSAGE:
			{
			DEBUG_TRACE( "Network::MessageReceived( SEND_MESSAGE )\n" );
			int 			who;
			const char *	msg;
			aMessage->FindInt32( "who", ( int32* ) &who );
			aMessage->FindString( "msg", &msg );
			SendMsg( who, msg );
			break;
			}
			
		case CLOSE_MESSAGE:
			{
			DEBUG_TRACE( "Network::MessageReceived( CLOSE_MESSAGE )\n" );
			ChatWindow	*win;
			aMessage->FindPointer( "win", ( void** ) &win );
			iWinList->RemoveItem( win );
			if( win->Lock() )
				win->Quit();
			}
			
		default:
			BLooper::MessageReceived( aMessage );
		}
	}

void Network::GotWindow( MainWindow* aWindow )
	{
	DEBUG_TRACE( "Network::GotWindow()\n" );
	iWindow = aWindow;
//	if( ( iWindow = aWindow ) ) // or =
//		BMessenger( iWindow ).SendMessage( UPDATE_STATUS );
	}

/* zwracamy wskaznik do okna jesli rozmowcy jesli z nim juz rozmawiamy */
ChatWindow* Network::GetMesgWinForUser( uin_t aWho )
	{
	DEBUG_TRACE( "Network::GotMesgWinForUser()\n" );
	ChatWindow *win = NULL;
	for( int i = 0; i < iWinList->CountItems(); i++ )
		{
		win = ( ChatWindow* ) iWinList->ItemAt( i );
		if( win->iWho == aWho )
			break;
		}
	
	if( win && (win->iWho == aWho ) )
		return win;
	
	return NULL;
	}

/* zwracamy wskaznik do osoby jesli z taka rozmawiamy */
Person* Network::GetPersonForUser( uin_t aWho )
	{
	DEBUG_TRACE( "Network::GotPersonForUser()\n" );
	Person* person = NULL;
	for( int i = 0; i < iWindow->GetProfile()->GetUserlist()->GetList()->CountItems(); i++ )
		{
		person = ( Person* ) iWindow->GetProfile()->GetUserlist()->GetList()->ItemAt( i );
		if( person->iUIN == aWho )
			break;
		}
	if( person && ( person->iUIN == aWho ) )
		return person;

	return NULL;
	}

void Network::Login()
	{
	DEBUG_TRACE( "Network::Login()\n" );
	/* ustawiamy status na "Łączenie" */
	iStatus = BEGG_CONNECTING;
//	if( iWindow )
//		BMessenger( iWindow ).SendMessage( UPDATE_STATUS );
	/* ustawiamy pola potrzebne do połączenia z gg */
	memset( &iLoginParam, 0, sizeof( iLoginParam ) );
	iLoginParam.uin = iProfile->iNumber;
	iLoginParam.password = ( char* ) iProfile->iPassword->String();
	iLoginParam.async = 1;
	iLoginParam.status = iProfile->AutoStatus();
//	gg_debug_level = ~0;
	BMessenger( this ).SendMessage( ADD_HANDLER );
	if( iWindow )
		BMessenger( iWindow ).SendMessage( UPDATE_STATUS );
	}

void Network::Login( int status )
	{
	DEBUG_TRACE( "Network::Login(status)\n");
	/* ustawiamy status na "Łączenie" */
	iStatus = status;
//	if( iWindow )
//		BMessenger( iWindow ).SendMessage( UPDATE_STATUS );
	/* ustawiamy pola potrzebne do połączenia z gg */
	memset( &iLoginParam, 0, sizeof( iLoginParam ) );
	iLoginParam.uin = iProfile->iNumber;
	iLoginParam.password = ( char* ) iProfile->iPassword->String();
	iLoginParam.async = 1;
	iLoginParam.status = iStatus;
//	gg_debug_level = ~0;
	BMessenger( this ).SendMessage( ADD_HANDLER );
	if( iWindow )
	 	BMessenger( iWindow ).SendMessage( UPDATE_STATUS );
	}

void Network::Login( int aStatus, BString *aDescription )
	{
	DEBUG_TRACE( "Network::Login(status, description)\n");
	/* ustawiamy status na "Łączenie" */
	iStatus = aStatus;
	iDescription = aDescription;
//	if( iWindow )
//		BMessenger( iWindow ).SendMessage( UPDATE_STATUS );
	/* ustawiamy pola potrzebne do połączenia z gg */
	memset( &iLoginParam, 0, sizeof( iLoginParam ) );
	iLoginParam.uin = iProfile->iNumber;
	iLoginParam.password = ( char* ) iProfile->iPassword->String();
	iLoginParam.async = 1;
	iLoginParam.status = iStatus;
	iLoginParam.status_descr = ( char* ) iDescription->String();
//	gg_debug_level = ~0;
	BMessenger( this ).SendMessage( ADD_HANDLER );
	if( iWindow )
	 	BMessenger( iWindow ).SendMessage( UPDATE_STATUS );
	}

void Network::Logout()
	{
	DEBUG_TRACE( "Network::Logout()\n" );
	/* poprostu sie wylogowujemy */
	if( iSession )
		{
		if( iStatus != GG_STATUS_NOT_AVAIL || iStatus != GG_STATUS_NOT_AVAIL_DESCR )
			iStatus = GG_STATUS_NOT_AVAIL;
		gg_logoff( iSession );
		gg_free_session( iSession );
		iSession = NULL;
		/* zatrzymujemy wszystkie handlery */
		NetworkHandler *handler;
		for( int i = iHandlerList->CountItems(); i > 0; i-- )
			{
			handler = ( NetworkHandler* ) iHandlerList->ItemAt( i - 1 );
			RemoveHandler( handler->iFd );
			}
		/* uaktualniamy statusy ludzi z listy */
		if( iWindow )
			{
			Person* p = NULL;
			for( int i = 0; i < iWindow->ListItems()->CountItems(); i++ )
				{
				p = ( Person* ) iWindow->ListItems()->ItemAt( i );
				p->iStatus = GG_STATUS_NOT_AVAIL;
			}
		
			/* uaktualniamy liste */
			if( iWindow->ListView()->LockLooper() )
				{
				iWindow->ListView()->MakeEmpty();
		 		iWindow->ListView()->UnlockLooper();
		 		}
			BMessenger( iWindow ).SendMessage( UPDATE_LIST );
			}
		/* uaktualniamy status */
		if( iWindow )
			BMessenger( iWindow ).SendMessage( UPDATE_STATUS );
		}
	}

/* wysyłamy wiadomość */
void Network::SendMsg( uin_t aWho, const char* aMessage )
	{
	DEBUG_TRACE( "Network::SendMsg()\n" );
	if( iSession )
		{
		if( gg_send_message( iSession, GG_CLASS_CHAT, aWho, ( unsigned char* ) aMessage ) == -1 )
			{	
			gg_free_session( iSession );
			perror( "Connection lost." );
			}
//		else
//			fprintf(stderr,"Wysłałem wiadomość o treści %s do %d\n", komu, wiadomosc);
		}
	}

void Network::GotMsg( uin_t aWho, const char* aMessage )
	{
	DEBUG_TRACE( "Network::GotMsg()\n" );
	/* sprawdzamy czy mamy aktualnie otwarte okno dla tej osoby :) */
	ChatWindow* win = NULL;
	for( int i = 0; i < iWinList->CountItems(); i++ )
		{
		win = ( ChatWindow* ) iWinList->ItemAt( i );
		if( win->iWho == aWho )
			break;
		}
	if( win && ( win->iWho == aWho ) )
		{
//		win->Activate();
		}
	else
		{
		win = new ChatWindow( this, iWindow, aWho );
		iWinList->AddItem( win );
		win->Show();
		}
	/* i pokazujemy je :P */
	BMessage* wiadomosc = new BMessage( SHOW_MESSAGE );
	wiadomosc->AddString( "msg", aMessage );
	BMessenger( win ).SendMessage( wiadomosc );
	delete wiadomosc;
	}

void Network::AddHandler( int fd, int cond, void* data )
	{
	DEBUG_TRACE( "Network::AddHandler()\n" );
	NetworkHandler* handler;
	handler = new NetworkHandler( this, iId, fd, cond, data );
	iHandlerList->AddItem( handler );
	handler->Run();
	}

void Network::RemoveHandler( int fd )
	{
	DEBUG_TRACE( "Network::RemoveHandler()\n" );
	NetworkHandler* handler = NULL;
	for( int i = 0; i < iHandlerList->CountItems(); i++ )
		{
		handler = ( NetworkHandler* ) iHandlerList->ItemAt( i );
		if( handler->iFd == fd )
			break;
		}
	if( !handler || ( handler->iFd != fd ) )
		return;
	handler->Stop();
	iHandlerList->RemoveItem( handler );
	delete handler;
	}

void Network::SetStatus( int aStatus )
	{
	iStatus = aStatus;
	}

void Network::SetDescription( BString *aDescription )
	{
	iDescription = aDescription;
	}

struct gg_session* Network::Session() const
	{
	return iSession;
	}
