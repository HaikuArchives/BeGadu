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
	/* Rozłączamy się */
	if(fStatus != GG_STATUS_NOT_AVAIL)
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
			int fd;
			message->FindInt32("fd", (int32*)&fd);
			RemoveHandler(fd);
			break;
		}
		case MAM_WIADOMOSC:
		{
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
			Login();
			break;
		}
		case LOGOUT:
		{
			Logout();
			break;
		}
		case DODAJ_OSOBE:
		{
			// do zaimplementowania
			break;
		}
		case USUN_OSOBE:
		{
			// do zaimplementowania
			break;
		}
		case OTWORZ_WIADOMOSC:
		{
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
					win->PostMessage(newmessage);
					delete newmessage;
				}
				win->Show();
			}
			break;
		}
		case WYSLIJ_WIADOMOSC:
		{
			int 			komu;
			const char *	msg;
			message->FindInt32("kto", (int32*)&komu);
			message->FindString("msg", &msg);
			SendMsg(komu, msg);
			break;
		}
		case ZAMKNIJ_WIADOMOSC:
		{
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
	if((fWindow = window))
		fWindow->PostMessage(BEGG_UPDATE_STATUS);
}

/* zwracamy wskaznik do okna jesli rozmowcy jesli z nim juz rozmawiamy */
ChatWindow * Siec::GetMesgWinForUser(uin_t kto)
{
	ChatWindow *win = NULL;
	for(int i = 0; i < fWinList->CountItems(); i++)
	{
		win = (ChatWindow*) fWinList->ItemAt(i);
		if(win->fKto != kto)
			break;
	}
	
	if(win && (win->fKto != kto))
		return win;
	
	return NULL;
}

/* zwracamy wskaznik do osoby jesli z taka rozmawiamy */
Osoba * Siec::GetOsobaForUser(uin_t kto)
{
	Osoba *osoba = NULL;
//	for(int i = 0; i < fLista->CountItems(); i++)
	for(int i = 0; i < fWindow->fProfil->fUserlista->fLista->CountItems(); i++)
	{
//		osoba = (Osoba*) fLista->ItemAt(i);
		osoba = (Osoba*) fWindow->fProfil->fUserlista->fLista->ItemAt(i);
		if(osoba->fUIN != kto)
			break;
	}

	if(osoba && (osoba->fUIN != kto))
		return osoba;

	return NULL;
}

