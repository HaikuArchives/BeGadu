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
#include <Roster.h>
#include <String.h>
#include "Msg.h"
#include "Siec.h"
#include "Main.h"
#include "Chat.h"
#include "Osoba.h"
#include "Handler.h"
extern "C" {
#include "libgadu.h"
}


Siec::Siec(Profil *profil, Lista *lista) : BLooper("Petla sieci")
{
	fprintf( stderr, "Siec::Siec()\n" );
	/* inicjalizacja */
	fProfil			= profil;
	fLista			= lista;
	fWindow			= NULL;
	fHandlerList	= new BList(256);
	fWinList		= new BList(512);
	fIdent			= 0;
	fSesja			= NULL;
	fStatus			= GG_STATUS_NOT_AVAIL;
	
	/* czekamy na wiadomości :)) */
	Run();
}

void Siec::Quit()
{
	fprintf( stderr, "Siec::Quit()\n" );
	/* Rozłączamy się */
	if((fStatus != GG_STATUS_NOT_AVAIL) || (fStatus != GG_STATUS_NOT_AVAIL_DESCR))
		Logout();
	Lock();
	BLooper::Quit();
}

void Siec::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		/*
			wiadomości otrzymane od libgadu callback
			cała obsługa jest w SiecLib.cpp, my tylko
			wywołujemy je stąd :)
		*/
		case DODAJ_HANDLER:
		{
			fprintf( stderr, "Siec::MessageReceived( DODAJ_HANDLER )\n" );
			int fd, cond;
			void *data;
			message->FindInt32("fd", (int32*)&fd);
			message->FindInt32("cond", (int32*)&cond);
			message->FindPointer("data", &data);
			AddHandler(fd,cond,data);
			break;
		}
		case USUN_HANDLER:
		{
			fprintf( stderr, "Siec::MessageReceived( USUN_HANDLER )\n" );
			int fd;
			message->FindInt32("fd", (int32*)&fd);
			RemoveHandler(fd);
			break;
		}
		case MAM_WIADOMOSC:
		{
			fprintf( stderr, "Siec::MessageReceived( MAM_WIADOMOSC )\n" );
			int				kto;
			const char *	msg;
			message->FindInt32("kto", (int32*)&kto);
			message->FindString("msg", &msg);
			GotMsg(kto, msg);
			break;
		}
		/*
			wiadomości otrzymane od interfejsu
			cała obsługa jest w SiecInt.cpp, my tylko
			wywołujemy je stąd :)
		*/
		case LOGIN:
		{
			fprintf( stderr, "Siec::MessageReceived( LOGIN )\n" );
			Login();
			break;
		}
		case LOGOUT:
		{
			fprintf( stderr, "Siec::MessageReceived( LOGOUT )\n" );
			Logout();
			break;
		}
		case DODAJ_OSOBE:
		{
			fprintf( stderr, "Siec::MessageReceived( DODAJ_OSOBE )\n" );
			// do zaimplementowania
			break;
		}
		case USUN_OSOBE:
		{
			fprintf( stderr, "Siec::MessageReceived( USUN_OSOBE )\n" );
			// do zaimplementowania
			break;
		}
		case OTWORZ_WIADOMOSC:
		{
			fprintf( stderr, "Siec::MessageReceived( OTWORZ_WIADOMOSC )\n" );
			int	kto;
			message->FindInt32("kto", (int32*)&kto);
			ChatWindow *win;
			// jeśli mamy już otwarte okno z tą osobą, przejdźmy do niego
			if( (win = GetMesgWinForUser(kto)))
				win->Activate();
			else // a jeśli nie, tworzymy nowe :)
			{
				win = new ChatWindow(this, fWindow, kto);
				fWinList->AddItem(win);
				Osoba	*osoba;
				if((osoba = GetOsobaForUser(kto)))
				{
					BMessage *newmessage = new BMessage(BEGG_UPDATE_STATUS);
					newmessage->AddInt32("status", osoba->fStatus);
					BMessenger(win).SendMessage(newmessage);
					delete newmessage;
				}
				win->Show();
			}
			break;
		}
		case WYSLIJ_WIADOMOSC:
		{
			fprintf( stderr, "Siec::MessageReceived( WYSLIJ_WIADOMOSC )\n" );
			int 			komu;
			const char *	msg;
			message->FindInt32("kto", (int32*)&komu);
			message->FindString("msg", &msg);
			SendMsg(komu, msg);
			break;
		}
		case ZAMKNIJ_WIADOMOSC:
		{
			fprintf( stderr, "Siec::MessageReceived( ZAMKNIJ_WIADOMOSC )\n" );
			ChatWindow	*win;
			message->FindPointer("win",(void**)&win);
			fWinList->RemoveItem(win);
			if(win->Lock())
				win->Quit();
		}
		default:
			BLooper::MessageReceived(message);
	}
}

void Siec::GotWindow(MainWindow *window)
{
	fprintf( stderr, "Siec::GotWindow(%p)\n", window );
	if((fWindow = window))
		BMessenger(fWindow).SendMessage(BEGG_UPDATE_STATUS);
}

/* zwracamy wskaznik do okna jesli rozmowcy jesli z nim juz rozmawiamy */
ChatWindow * Siec::GetMesgWinForUser(uin_t kto)
{
	fprintf( stderr, "Siec::GotMesgWinForUser(%d)\n", kto );
	ChatWindow *win = NULL;
	for(int i = 0; i < fWinList->CountItems(); i++)
	{
		win = (ChatWindow*) fWinList->ItemAt(i);
		if(win->fKto == kto)
			break;
	}
	
	if(win && (win->fKto == kto))
		return win;
	
	return NULL;
}

/* zwracamy wskaznik do osoby jesli z taka rozmawiamy */
Osoba * Siec::GetOsobaForUser(uin_t kto)
{
	fprintf( stderr, "Siec::GotOsobaForUser(%d)\n", kto );
	Osoba *osoba = NULL;
	for(int i = 0; i < fWindow->fProfil->fUserlista->fLista->CountItems(); i++)
	{
		osoba = (Osoba*) fWindow->fProfil->fUserlista->fLista->ItemAt(i);
		if(osoba->fUIN == kto)
			break;
	}
	if(osoba && (osoba->fUIN == kto))
		return osoba;

	return NULL;
}

void Siec::Login()
{
	fprintf( stderr, "Siec::Login()\n" );
	/* ustawiamy status na "Łączenie" */
	fStatus = BEGG_CONNECTING;
	if(fWindow)
		BMessenger(fWindow).SendMessage(BEGG_UPDATE_STATUS);
	/* ustawiamy pola potrzebne do połączenia z gg */
	memset(&fLoginParam, 0, sizeof(fLoginParam));
	fLoginParam.uin = fProfil->fNumer;
	fLoginParam.password = (char*)fProfil->fHaslo->String();
	fLoginParam.async = 1;
	fLoginParam.status = fProfil->fAutoStatus;
//	gg_debug_level = ~0;
	BMessenger(this).SendMessage(DODAJ_HANDLER);
	if(fWindow)
		BMessenger(fWindow).SendMessage(BEGG_UPDATE_STATUS);
}

void Siec::Login(int status)
{
	fprintf( stderr, "Siec::Login(%d)\n", status );
	/* ustawiamy status na "Łączenie" */
	fStatus = status;
	if(fWindow)
		BMessenger(fWindow).SendMessage(BEGG_UPDATE_STATUS);
	/* ustawiamy pola potrzebne do połączenia z gg */
	memset(&fLoginParam, 0, sizeof(fLoginParam));
	fLoginParam.uin = fProfil->fNumer;
	fLoginParam.password = (char*)fProfil->fHaslo->String();
	fLoginParam.async = 1;
	fLoginParam.status = fStatus;
//	gg_debug_level = ~0;
	BMessenger(this).SendMessage(DODAJ_HANDLER);
	if(fWindow)
		BMessenger(fWindow).SendMessage(BEGG_UPDATE_STATUS);
}

void Siec::Logout()
{
	fprintf( stderr, "Siec::Logout()\n" );
	/* poprostu sie wylogowujemy */
	if(fSesja)
	{
		fStatus = GG_STATUS_NOT_AVAIL;
		gg_logoff(fSesja);
		gg_free_session(fSesja);
		fSesja = NULL;
		/* zatrzymujemy wszystkie handlery */
		SiecHandler *handler;
		for(int i=fHandlerList->CountItems(); i>0; i--)
		{
			handler = (SiecHandler*) fHandlerList->ItemAt(i-1);
			RemoveHandler(handler->fFd);
		}
		/* uaktualniamy statusy ludzi z listy */
		if(fWindow)
		{
			Osoba *osoba = NULL;
			for(int i = 0; i < fWindow->fListaItems->CountItems(); i++)
			{
				osoba = (Osoba*) fWindow->fListaItems->ItemAt(i);
				osoba->fStatus = GG_STATUS_NOT_AVAIL;
			}
		
			/* uaktualniamy liste */
			if(fWindow->fListaView->LockLooper())
			{
				fWindow->fListaView->MakeEmpty();
		 		fWindow->fListaView->UnlockLooper();
		 	}
			BMessenger(fWindow).SendMessage(BEGG_UPDATE_LISTY);
		}
		
		/* uaktualniamy status */
		if(fWindow)
			BMessenger(fWindow).SendMessage(BEGG_UPDATE_STATUS);
	}
}

/* wysyłamy wiadomość */
void Siec::SendMsg(uin_t komu, const char *wiadomosc)
{
	fprintf( stderr, "Siec::SendMsg()\n" );
	if(fSesja)
	{
		if(gg_send_message(fSesja, GG_CLASS_CHAT, komu, (unsigned char*)wiadomosc) == -1)
		{	
			gg_free_session(fSesja);
			perror("polaczenie zerwane");
		}
//		else
//			fprintf(stderr,"Wysłałem wiadomość o treści %s do %d\n", komu, wiadomosc);
	}
}

void Siec::GotMsg(uin_t odkogo, const char *msg)
{
	fprintf( stderr, "Siec::GotMsg()\n" );
	/* sprawdzamy czy mamy aktualnie otwarte okno dla tej osoby :) */
	ChatWindow	*win = NULL;
	for( int i = 0; i < fWinList->CountItems(); i++)
	{
		win = (ChatWindow*) fWinList->ItemAt(i);
		if( win->fKto == odkogo)
			break;
	}
	if( win && ( win->fKto == odkogo ) )
	{
//		win->Activate();
	}
	else
	{
		win = new ChatWindow(this, fWindow, odkogo);
		fWinList->AddItem(win);
		win->Show();
	}
	/* i pokazujemy je :P */
	BMessage	*	wiadomosc = new BMessage(POKAZ_WIADOMOSC);
	wiadomosc->AddString("msg", msg);
	BMessenger(win).SendMessage(wiadomosc);
	delete wiadomosc;
}

void Siec::AddHandler(int fd, int cond, void *data)
{
	fprintf(stderr, "Siec::AddHandler(fd=%d, cond=%d)\n", fd, cond );
	SiecHandler * handler = new SiecHandler(this, fId, fd, cond, data);
	fHandlerList->AddItem(handler);
	handler->Run();
}

void Siec::RemoveHandler(int fd)
{
	fprintf( stderr, "Siec::RemoveHandler(fd=%d)\n", fd );
	SiecHandler *handler = NULL;
	for(int i=0; i<fHandlerList->CountItems();i++)
	{
		handler = (SiecHandler*) fHandlerList->ItemAt(i);
		if(handler->fFd == fd)
			break;
	}
	if(!handler || handler->fFd != fd)
		return;
	handler->Stop();
	fHandlerList->RemoveItem(handler);
	delete handler;
}
