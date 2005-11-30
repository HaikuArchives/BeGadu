/*
	SiecHandler.cpp
	Ten plik jest częscią kodu źródłowego BeGadu.
	Homepage: http://gadu.beos.pl
*/

#include <OS.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <Message.h>
#include <Alert.h>
#include <OutlineListView.h>
#include <String.h>

#include "Main.h"
#include "Msg.h"
#include "SiecHandler.h"
#include "Siec.h"
#include "Osoba.h"
#include "GaduListItem.h"

#include <libgadu.h>

static time_t 	curTime = 0;
static time_t 	pingTimer = 0;

int32 HandlerThread(void *_handler)
{
	fprintf( stderr, "HandlerThread()\n" );
	SiecHandler *handler = (SiecHandler*) _handler;
	Siec		*siec	 = handler->fSiec;
	fd_set	rd, wr;
	struct timeval tv;
	if(!(siec->fSesja = gg_login(&siec->fLoginParam)))
	{
		if(siec->fSesja)
			fprintf(stderr, "blad: %d", siec->fSesja->error);
		BMessenger(siec).SendMessage(USUN_HANDLER);
		return 0;
	}
	else
	{
		gg_change_status(siec->fSesja, siec->fStatus);
		if(siec->fWindow)
			BMessenger(siec->fWindow).SendMessage(BEGG_UPDATE_STATUS);
		time(&pingTimer);
		while(!handler->fDie)
		{
			time(&curTime);
			FD_ZERO(&rd);
			FD_ZERO(&wr);
	
			if((siec->fSesja->check & GG_CHECK_READ))
				FD_SET(siec->fSesja->fd, &rd);

			if((siec->fSesja->check & GG_CHECK_WRITE))
				FD_SET(siec->fSesja->fd, &wr);

			tv.tv_sec = 10;
			tv.tv_usec = 0;
			if(select(siec->fSesja->fd +1, &rd, &wr, NULL, &tv)< 0 && errno == EBADF)
			{
				fprintf(stderr,"HandlerThread: select() failed (%s)\n", strerror(errno));
				BMessenger(siec).SendMessage(LOGOUT);
				break;
			}
			if(handler->fDie)
				break;
			if(siec->fSesja && (FD_ISSET(siec->fSesja->fd, &rd) || FD_ISSET(siec->fSesja->fd, &wr)))
			{
				if(!(siec->fZdarzenie = gg_watch_fd(siec->fSesja)))
					perror("Zerwało połączenie :/");
				else
					handler->HandleEvent( siec->fZdarzenie );
			}
			if( Expired( pingTimer ) )
				handler->HandlePingTimeoutCallback( pingTimer );
		}
	}
	return 0;
}

SiecHandler::SiecHandler(Siec *siec, int id, int fd, int cond, void *data)
{
	fprintf( stderr, "SiecHandler::SiecHandler()\n" );
	fSiec	= siec;
	fId		= id;
	fFd		= fd;
	fCond	= cond;
	fData	= data;
}

void SiecHandler::Run()
{
	fprintf( stderr, "SiecHandler::Run()\n" );
	fDie = false;
	fThreadID = spawn_thread(HandlerThread, "handler thread", B_NORMAL_PRIORITY, this);
	resume_thread(fThreadID);
}

void SiecHandler::Stop()
{
	fprintf( stderr, "SiecHandler::Stop()\n" );
	status_t exit_value;
	fDie = true;
	wait_for_thread(fThreadID, &exit_value);
}

void SiecHandler::HandleEvent( struct gg_event *event )
{
	switch( event->type)
	{
		case GG_EVENT_NONE:
		case GG_EVENT_PONG:
		{
			// fprintf(stderr, "GG_EVENT_NONE || GG_EVENT_PONG\n");
			break;
		}
		case GG_STATE_CONNECTED:
		case GG_EVENT_CONN_SUCCESS:
		{
			HandleEventConnected( event );
			break;
		}
		case GG_EVENT_CONN_FAILED:
		{
			HandleEventConnFailed( event );
			break;
		}
		case GG_EVENT_MSG:
		{
			HandleEventMsg( event );
			break;
		}
		case GG_EVENT_USERLIST:
		{
			HandleEventUserlist( event );
			break;
		}
		case GG_EVENT_NOTIFY:
		case GG_EVENT_NOTIFY_DESCR:
		{
			HandleEventNotify( event );
	 		break;
		}
		case GG_EVENT_STATUS:
		{
			HandleEventStatus( event );
			break;
		}
		case GG_EVENT_NOTIFY60:
		{
			HandleEventNotify60( event );
	 		break;
		}
		case GG_EVENT_STATUS60:
		{
			HandleEventStatus60( event );
			break;
		}
	}
	gg_event_free( event );
}

void SiecHandler::HandleEventConnected( struct gg_event *event )
{
	fprintf(stderr,"SiecHandler::HandleEventConnected()\n");
	fprintf(stderr, "\tSprawdzam liste ludzi... ");
	if(fSiec->fWindow->fProfil->fUserlista->IsValid() == false ||  fSiec->fWindow->fProfil->fNeedImport == true)
	{
		fprintf(stderr, "Zla\n");
		fSiec->fWindow->fProfil->fUserlista->Import(fSiec->fSesja, fSiec->fWindow->fProfil->fUserlista->fLista);
		fSiec->fWindow->fProfil->fNeedImport = false;
	}
	else
	{
		fprintf(stderr, "Ok\n");
		fSiec->fWindow->fProfil->fUserlista->Send(fSiec->fSesja);
	}
	BMessenger(fSiec->fWindow).SendMessage(BEGG_UPDATE_STATUS);
}

void SiecHandler::HandleEventConnFailed( struct gg_event *event )
{
	fprintf(stderr,"SiecHandler::HandleEventConnFailed()\n");
	gg_event_free(fSiec->fZdarzenie);
	BMessenger(fSiec).SendMessage(USUN_HANDLER);
}

void SiecHandler::HandleEventMsg( struct gg_event *event )
{
	fprintf(stderr,"SiecHandler::HandleEventMsg()\n");
	BMessage *wiadomosc = new BMessage(MAM_WIADOMOSC);
	wiadomosc->AddInt32("kto", fSiec->fZdarzenie->event.msg.sender);
	wiadomosc->AddString("msg", (char*)fSiec->fZdarzenie->event.msg.message);
//	fprintf( stderr, "Od: %d\nWiadomosc: %s\n", fSiec->fZdarzenie->event.msg.sender, (char*)fSiec->fZdarzenie->event.msg.message );
	BMessenger(fSiec).SendMessage(wiadomosc);
}

void SiecHandler::HandleEventUserlist( struct gg_event *event )
{
	fprintf(stderr,"SiecHandler::HandleEventUserlist()\n");
	if(fSiec->fZdarzenie->event.userlist.type == GG_USERLIST_GET_REPLY)
	{
		if(fSiec->fZdarzenie->event.userlist.reply)
		{
			Osoba *o;
			Lista *lista = fSiec->fWindow->fProfil->fUserlista->fLista;
			Userlist *userlista = fSiec->fWindow->fProfil->fUserlista;
			if(lista->CountItems() <= 0)
			{
				for(int i = 0; lista->CountItems(); i++)
				{
					o = (Osoba*) lista->ItemAt(i);
					if(fSiec->fSesja)
						gg_remove_notify(fSiec->fSesja, o->fUIN);
				}
			}
			GaduListItem *g = NULL;
			for(int i = 0; i < fSiec->fWindow->fListaItems->CountItems(); i++)
			{
				g = (GaduListItem*) fSiec->fWindow->fListaItems->ItemAt(i);
				if(fSiec->fWindow->Lock())
				{
				if(fSiec->fWindow->fListaView->LockLooper())
				{
					fSiec->fWindow->fListaView->RemoveItem(g);
					fSiec->fWindow->fListaView->UnlockLooper();
				}
				fSiec->fWindow->Unlock();
				}
				delete g;
			}
			userlista->Set(fSiec->fZdarzenie->event.userlist.reply);
			fprintf(stderr, "wysylam liste\n");
			userlista->Send(fSiec->fSesja);
			fprintf(stderr, "wyslalem liste\n");
			BMessenger(fSiec->fWindow).SendMessage(BEGG_UPDATE_LISTY);
			BAlert *alert = new BAlert("Lista", "Lista kontaktow zostala zaladowana z servera", "OK");
			alert->Go();
		}
	}
}

void SiecHandler::HandleEventNotify( struct gg_event *event )
{
	fprintf(stderr,"SiecHandler::HandleEventNotify()\n");
 	struct gg_notify_reply *n;
 	if(fSiec->fZdarzenie->type == GG_EVENT_NOTIFY)
 		n = fSiec->fZdarzenie->event.notify;
 	else
 		n = fSiec->fZdarzenie->event.notify_descr.notify;
	
	for(; n->uin; n++)
	{
		Lista *lista = fSiec->fWindow->fProfil->fUserlista->fLista;
		Userlist *userlista = fSiec->fWindow->fProfil->fUserlista;
 		Osoba *o;
 		if(!(o = userlista->Find(n->uin)))
 		{
 			break;
 		}
 		for(int i = 0; i < lista->CountItems(); i++)
 		{
 			o = (Osoba*) lista->ItemAt(i);
 			if(o->fUIN == n->uin)
 				break;
 		}
 		o->fStatus = n->status;
	}
	BMessenger(fSiec->fWindow).SendMessage(BEGG_UPDATE_LISTY);
}

void SiecHandler::HandleEventNotify60( struct gg_event *event )
{
	fprintf(stderr,"SiecHandler::HandleEventNotify60()\n");
	Lista *lista = fSiec->fWindow->fProfil->fUserlista->fLista;
	Userlist *userlista = fSiec->fWindow->fProfil->fUserlista;
	for(int i = 0; fSiec->fZdarzenie->event.notify60[i].uin; i++)
	{
 		Osoba *o = NULL;
 		if(!(o = userlista->Find(fSiec->fZdarzenie->event.notify60[i].uin)))
 			break;
 		for(int a = 0; a < lista->CountItems(); a++)
 		{
 			o = (Osoba*) lista->ItemAt(a);
 			if(o->fUIN == fSiec->fZdarzenie->event.notify60[i].uin)
 				break;
 		}
 		o->fStatus = fSiec->fZdarzenie->event.notify60[i].status;
 		o->fOpis->SetTo(fSiec->fZdarzenie->event.notify60[i].descr);
	}
	BMessenger(fSiec->fWindow).SendMessage(BEGG_UPDATE_LISTY);
}

void SiecHandler::HandleEventStatus( struct gg_event *event )
{
	fprintf(stderr,"SiecHandler::HandleEventStatus()\n");
	Lista *lista = fSiec->fWindow->fProfil->fUserlista->fLista;
	Userlist *userlista = fSiec->fWindow->fProfil->fUserlista;
 	Osoba *o;
 	if(!(o = userlista->Find(fSiec->fZdarzenie->event.status.uin)))
 		return;
 	for(int i = 0; i < lista->CountItems(); i++)
 	{
 		o = (Osoba*) lista->ItemAt(i);
 		if(o->fUIN == fSiec->fZdarzenie->event.status.uin)
			return;
 	}
 	o->fStatus = fSiec->fZdarzenie->event.status.status;
 	o->fOpis->SetTo(fSiec->fZdarzenie->event.status.descr);
	BMessenger(fSiec->fWindow).SendMessage(BEGG_UPDATE_LISTY);
}

void SiecHandler::HandleEventStatus60( struct gg_event *event )
{
	fprintf(stderr,"SiecHandler::HandleEventStatus60()\n");
	Lista *lista = fSiec->fWindow->fProfil->fUserlista->fLista;
	Userlist *userlista = fSiec->fWindow->fProfil->fUserlista;
 	char *descr;
 	Osoba *o;
 	if(!(o = userlista->Find(fSiec->fZdarzenie->event.status60.uin)))
 	{
 		return;
 	}
 	for(int i = 0; i < lista->CountItems(); i++)
 	{
 		o = (Osoba*) lista->ItemAt(i);
 		if(o->fUIN == fSiec->fZdarzenie->event.status60.uin)
 			return;
 	}
 	o->fStatus = fSiec->fZdarzenie->event.status60.status;
 	o->fOpis->SetTo(fSiec->fZdarzenie->event.status60.descr);
	BMessenger(fSiec->fWindow).SendMessage(BEGG_UPDATE_LISTY);
}

static int Expired(time_t timer)
{
//	fprintf( stderr, "timer = %d curTime = %d\n", timer, curTime );
	if (timer && curTime >= timer)
		return 1;
	return 0;
}

static void Rearm(time_t *timer, int seconds)
{
	time(timer);
	*timer += seconds;
}

void SiecHandler::HandlePingTimeoutCallback( time_t &pingTimer )
{
	fprintf(stderr,"SiecHandler::HandlePingTimeoutCallback( %d )\n", pingTimer );
	gg_ping( fSiec->fSesja );
	Rearm( &pingTimer, 60 );
}

