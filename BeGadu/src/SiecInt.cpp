/*
	SiecInt.cpp
	Ten plik jest częscią kodu źródłowego BeGadu.
	Homepage: http://gadu.beos.pl
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <Message.h>
#include <errno.h>
#include <OutlineListView.h>

#include "Msg.h"
#include "Siec.h"
#include "Main.h"
#include "Osoba.h"
#include "SiecHandler.h"

extern "C" {
#include "libgadu.h"
}

void Siec::Login()
{
	/* ustawiamy status na "Łączenie" */
	fStatus = BEGG_CONNECTING;
	if(fWindow)
		fWindow->PostMessage(BEGG_UPDATE_STATUS);
	/* ustawiamy pola potrzebne do połączenia z gg */
	memset(&fLoginParam, 0, sizeof(fLoginParam));
	fLoginParam.uin = fProfil->fNumer;
	fLoginParam.password = (char*)fProfil->fHaslo->String();
	fLoginParam.async = 1;
	fLoginParam.status = fProfil->fAutoStatus;
	gg_debug_level = ~0;
	PostMessage(DODAJ_HANDLER);
}

void Siec::Login(int status)
{
	/* ustawiamy status na "Łączenie" */
	fStatus = status;
	if(fWindow)
		fWindow->PostMessage(BEGG_UPDATE_STATUS);
	/* ustawiamy pola potrzebne do połączenia z gg */
	memset(&fLoginParam, 0, sizeof(fLoginParam));
	fLoginParam.uin = fProfil->fNumer;
	fLoginParam.password = (char*)fProfil->fHaslo->String();
	fLoginParam.async = 1;
	fLoginParam.status = fStatus;
	gg_debug_level = ~0;
	PostMessage(DODAJ_HANDLER);
	if(fWindow)
		fWindow->PostMessage(BEGG_UPDATE_STATUS);
}

void Siec::Logout()
{
	/* poprostu sie wylogowujemy */
	if(fSesja)
	{
		fStatus = GG_STATUS_NOT_AVAIL;
		gg_logoff(fSesja);
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
//				free(osoba->fOpis);
			}
		
			/* uaktualniamy liste */
			if(fWindow->fListaView->LockLooper())
			{
				fWindow->fListaView->MakeEmpty();
		 		fWindow->fListaView->UnlockLooper();
		 	}
			fWindow->PostMessage(BEGG_UPDATE_LISTY);
		}
		
		/* uaktualniamy status */
		if(fWindow)
			fWindow->PostMessage(BEGG_UPDATE_STATUS);
		fSesja = NULL;
	}
}

/* wysyłamy wiadomość */
void Siec::SendMsg(uin_t komu, const char *wiadomosc)
{
	if(fSesja)
	{
		if(gg_send_message(fSesja, GG_CLASS_MSG, komu, (unsigned char*)wiadomosc) == -1)
		{	
			gg_free_session(fSesja);
			perror("polaczenie zerwane");
		}
//		else
//			fprintf(stderr,"Wysłałem wiadomość o treści %s do %d\n", komu, wiadomosc);
	}
}
