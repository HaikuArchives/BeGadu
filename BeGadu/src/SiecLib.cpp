/*
	SiecLib.cpp
	Ten plik jest częscią kodu źródłowego BeGadu.
	Homepage: http://gadu.beos.pl
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

#include <Alert.h>
#include <Message.h>
#include <String.h>

#include "Msg.h"
#include "Siec.h"
#include "Main.h"
#include "Chat.h"
#include "SiecHandler.h"
#include "Osoba.h"

extern "C" {
#include "libgadu.h"
}

void Siec::GotMsg(uin_t odkogo, const char *msg)
{
	/* sprawdzamy czy mamy aktualnie otwarte okno dla tej osoby :) */
	ChatWindow	*win = NULL;
	for( int i = 0; i < fWinList->CountItems(); i++)
	{
		win = (ChatWindow*) fWinList->ItemAt(i);
		if( win->fKto == odkogo)
			break;
	}
	
	if( win && ( win->fKto == odkogo ) )
		win->Activate();
	else
	{
		win = new ChatWindow(this, fWindow, odkogo);
		fWinList->AddItem(win);
		win->Show();
	}
	/* i pokazujemy je :P */
	BMessage	*	wiadomosc = new BMessage(POKAZ_WIADOMOSC);
	wiadomosc->AddString("msg", msg);
	win->PostMessage(wiadomosc);
	delete wiadomosc;
}

void Siec::AddHandler(int fd, int cond, void *data)
{
	fprintf(stderr, "AddHandler (fd=%d, cond=%d)\n", fd, cond );
	SiecHandler * handler = new SiecHandler(this, fId, fd, cond, data);
	fHandlerList->AddItem(handler);
	handler->Run();
}

void Siec::RemoveHandler(int fd)
{
	fprintf( stderr, "RemoveHandler (fd=%d)\n", fd );
	
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
