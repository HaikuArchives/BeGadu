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

#include "Main.h"
#include "Msg.h"
#include "SiecHandler.h"
#include "Siec.h"
#include "Osoba.h"
#include "GaduListItem.h"

extern "C"{
#include "libgadu.h"
}

int32 HandlerThread(void *_handler)
{
	SiecHandler *handler = (SiecHandler*) _handler;
	Siec		*siec	 = handler->fSiec;
	fd_set	rd, wr;
	struct timeval tv;
	if(!(siec->fSesja = gg_login(&siec->fLoginParam)))
	{
		fprintf(stderr, "blad: %d", siec->fSesja->error);
		siec->PostMessage(USUN_HANDLER);
	}
	else
	{
		gg_change_status(siec->fSesja, siec->fStatus);
		if(siec->fWindow)
			siec->fWindow->PostMessage(BEGG_UPDATE_STATUS);
		if(siec->fWindow->fProfil->fUserlista->IsValid() == false)
			siec->fWindow->fProfil->fUserlista->Import(siec->fSesja, siec->fWindow->fProfil->fUserlista->fLista);
		else
			siec->fWindow->fProfil->fUserlista->Send(siec->fSesja);

		while(!handler->fDie)
		{
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
				siec->PostMessage(LOGOUT);
				break;
			}
			if(handler->fDie)
				break;
			if(siec->fSesja && (FD_ISSET(siec->fSesja->fd, &rd) || FD_ISSET(siec->fSesja->fd, &wr)))
			{
				if(!(siec->fZdarzenie = gg_watch_fd(siec->fSesja)))
					perror("Zerwało połączenie :/");
				else
				{
					switch(siec->fZdarzenie->type)
					{
						case GG_EVENT_NONE:
						case GG_EVENT_PONG:
						{
							fprintf(stderr, "GG_EVENT_NONE || GG_EVENT_PONG\n");
							break;
						}
						case GG_STATE_CONNECTED:
						case GG_EVENT_CONN_SUCCESS:
						{
							fprintf(stderr,"Połączyłem się :D\n");
							break;
						}
						case GG_EVENT_CONN_FAILED:
						{
							fprintf(stderr, "Nie mogłem się podpiąc :/\n");
							gg_event_free(siec->fZdarzenie);
							siec->PostMessage(USUN_HANDLER);
							break;
						}
						case GG_EVENT_MSG:
						{
							fprintf(stderr, "mam mesga\n");
							BMessage *wiadomosc = new BMessage(MAM_WIADOMOSC);
							wiadomosc->AddInt32("kto", siec->fZdarzenie->event.msg.sender);
							wiadomosc->AddString("msg", (char*)siec->fZdarzenie->event.msg.message);
							siec->PostMessage(wiadomosc);
							break;
						}

						case GG_EVENT_USERLIST:
						{
							if(siec->fZdarzenie->event.userlist.type == GG_USERLIST_GET_REPLY)
							{
								if(siec->fZdarzenie->event.userlist.reply)
								{
									Osoba *o;
									Lista *lista = siec->fWindow->fProfil->fUserlista->fLista;
									Userlist *userlista = siec->fWindow->fProfil->fUserlista;
									if(lista->CountItems() <= 0)
									{
										for(int i = 0; lista->CountItems(); i++)
										{
											o = (Osoba*) lista->ItemAt(i);
											if(siec->fSesja)
												gg_remove_notify(siec->fSesja, o->fUIN);
										}
									}
									GaduListItem *g = NULL;
									for(int i = 0; i < siec->fWindow->fListaItems->CountItems(); i++)
									{
										g = (GaduListItem*) siec->fWindow->fListaItems->ItemAt(i);
										if(siec->fWindow->fListaView->LockLooper())
										{
											siec->fWindow->fListaView->RemoveItem(g);
											siec->fWindow->fListaView->UnlockLooper();
										}
										delete g;
									}
									siec->fWindow->fListaItems->MakeEmpty();
									userlista->Set(siec->fZdarzenie->event.userlist.reply);
									fprintf(stderr, "wysylam liste\n");
									userlista->Send(siec->fSesja);
									fprintf(stderr, "wyslalem liste\n");
									siec->fWindow->PostMessage(BEGG_UPDATE_LISTY);
									BAlert *alert = new BAlert("Lista", "Lista kontaktow zostala zaladowana z servera", "OK");
									alert->Go();
								}
										
							}
							break;
						 }
						 
						 case GG_EVENT_NOTIFY:
						 case GG_EVENT_NOTIFY_DESCR:
						 {
						 	struct gg_notify_reply *n;
						 	if(siec->fZdarzenie->type == GG_EVENT_NOTIFY)
						 		n = siec->fZdarzenie->event.notify;
						 	else
						 		n = siec->fZdarzenie->event.notify_descr.notify;
							
							for(; n->uin; n++)
							{
								Lista *lista = siec->fWindow->fProfil->fUserlista->fLista;
								Userlist *userlista = siec->fWindow->fProfil->fUserlista;
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
				 			if(siec->fWindow->fListaView->LockLooper())
				 			{
					 			siec->fWindow->fListaView->MakeEmpty();
					 			siec->fWindow->fListaView->UnlockLooper();
					 		}
							siec->fWindow->PostMessage(BEGG_UPDATE_LISTY);
					 		break;
						 }
						 case GG_EVENT_STATUS:
						 {
							Lista *lista = siec->fWindow->fProfil->fUserlista->fLista;
							Userlist *userlista = siec->fWindow->fProfil->fUserlista;
						 	Osoba *o;
						 	if(!(o = userlista->Find(siec->fZdarzenie->event.status.uin)))
						 		break;
						 	for(int i = 0; i < lista->CountItems(); i++)
						 	{
						 		o = (Osoba*) lista->ItemAt(i);
						 		if(o->fUIN == siec->fZdarzenie->event.status.uin)
						 			break;
						 	}
						 	o->fStatus = siec->fZdarzenie->event.status.status;
						 	if( (o->fStatus == GG_STATUS_NOT_AVAIL_DESCR) || (o->fStatus == GG_STATUS_INVISIBLE_DESCR) || (o->fStatus == GG_STATUS_BUSY_DESCR) || (o->fStatus == GG_STATUS_AVAIL_DESCR) )
						 	{
						 		o->fOpis = (char*) calloc(70 + 2, 1);
							 	sprintf(o->fOpis, "%s", siec->fZdarzenie->event.status.descr);
						 	}
						 	else
						 		o->fOpis = "";
							if(siec->fWindow->fListaView->LockLooper())
							{
						 		siec->fWindow->fListaView->MakeEmpty();
						 		siec->fWindow->PostMessage(BEGG_UPDATE_LISTY);
						 		siec->fWindow->fListaView->UnlockLooper();
						 	}
						 	break;
						 }
						 
						 case GG_EVENT_NOTIFY60:
						 {
							Lista *lista = siec->fWindow->fProfil->fUserlista->fLista;
							Userlist *userlista = siec->fWindow->fProfil->fUserlista;
							for(int i = 0; siec->fZdarzenie->event.notify60[i].uin; i++)
							{
						 		Osoba *o = NULL;
						 		if(!(o = userlista->Find(siec->fZdarzenie->event.notify60[i].uin)))
						 			break;
						 		for(int a = 0; a < lista->CountItems(); a++)
						 		{
						 			o = (Osoba*) lista->ItemAt(a);
						 			if(o->fUIN == siec->fZdarzenie->event.notify60[i].uin)
						 				break;
						 		}
						 		o->fStatus = siec->fZdarzenie->event.notify60[i].status;
							 	if( (o->fStatus == GG_STATUS_NOT_AVAIL_DESCR) || (o->fStatus == GG_STATUS_INVISIBLE_DESCR) || (o->fStatus == GG_STATUS_BUSY_DESCR) || (o->fStatus == GG_STATUS_AVAIL_DESCR) )
							 	{
							 		o->fOpis = (char*) calloc(70 + 2, 1);
								 	sprintf(o->fOpis, "%s", siec->fZdarzenie->event.notify60[i].descr);
							 	}
							 	else
							 		o->fOpis = "";
							}
				 			if(siec->fWindow->fListaView->LockLooper())
				 			{
					 			siec->fWindow->fListaView->MakeEmpty();
					 			siec->fWindow->fListaView->UnlockLooper();
					 		}
							siec->fWindow->PostMessage(BEGG_UPDATE_LISTY);
					 		break;
						 }
						 
						 case GG_EVENT_STATUS60:
						 {
							Lista *lista = siec->fWindow->fProfil->fUserlista->fLista;
							Userlist *userlista = siec->fWindow->fProfil->fUserlista;
						 	Osoba *o;
						 	if(!(o = userlista->Find(siec->fZdarzenie->event.status60.uin)))
						 	{
						 		break;
						 	}
						 	for(int i = 0; i < lista->CountItems(); i++)
						 	{
						 		o = (Osoba*) lista->ItemAt(i);
						 		if(o->fUIN == siec->fZdarzenie->event.status60.uin)
						 			break;
						 	}
						 	o->fStatus = siec->fZdarzenie->event.status60.status;
						 	if( (o->fStatus == GG_STATUS_NOT_AVAIL_DESCR) || (o->fStatus == GG_STATUS_INVISIBLE_DESCR) || (o->fStatus == GG_STATUS_BUSY_DESCR) || (o->fStatus == GG_STATUS_AVAIL_DESCR) )
						 	{
						 		o->fOpis = (char*) calloc(70 + 2, 1);
							 	sprintf(o->fOpis, "%s", siec->fZdarzenie->event.status60.descr);
							}
							else
								o->fOpis = "";

					 		if(siec->fWindow->fListaView->LockLooper())
					 		{
						 		siec->fWindow->fListaView->MakeEmpty();
						 		siec->fWindow->fListaView->UnlockLooper();
						 	}
							siec->fWindow->PostMessage(BEGG_UPDATE_LISTY);
						 	break;
						 }
					}
					gg_event_free(siec->fZdarzenie);
				}
			}
		}
	}
	return 0;
}

SiecHandler::SiecHandler(Siec *siec, int id, int fd, int cond, void *data)
{
	fSiec	= siec;
	fId		= id;
	fFd		= fd;
	fCond	= cond;
	fData	= data;
}

void SiecHandler::Run()
{
	fDie = false;
	fThreadID = spawn_thread(HandlerThread, "handler thread", B_LOW_PRIORITY, this);
	resume_thread(fThreadID);
}

void SiecHandler::Stop()
{
	status_t exit_value;
	fDie = true;
	wait_for_thread(fThreadID, &exit_value);
}
