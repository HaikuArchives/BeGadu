/*
	Main.cpp
	Code: aljen <aljen@gumisie.be>
	Homepage: http://begadu.sf.net
*/

#include <Alert.h>
#include <Application.h>
#include <Beep.h>
#include <Bitmap.h>
#include <DataIO.h>
#include <FindDirectory.h>
#include <ListView.h>
#include <MenuBar.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <Path.h>
#include <Point.h>
#include <PopUpMenu.h>
#include <Resources.h>
#include <Roster.h>
#include <Screen.h>
#include <ScrollView.h>
#include <String.h>
#include <StringView.h>
#include <stdlib.h>
#include <string.h>

#include "GaduListItem.h"
#include "GaduMenuItem.h"
#include "GfxStuff.h"
#include "Main.h"
#include "Msg.h"
#include "Siec.h"
#include "Opcje.h"
#include "Osoba.h"

#define MAINWINDOW_RECT BRect(50,50,300,350)
#define MAINWINDOW_NAME	"BeGadu " WERSJA


MainWindow::MainWindow(BString *profil)
 	: BWindow(MAINWINDOW_RECT, MAINWINDOW_NAME, B_TITLED_WINDOW, B_NOT_ZOOMABLE | B_ASYNCHRONOUS_CONTROLS)
{
	fprintf( stderr, "MainWindow::MainWindow()\n" );
	fProfil = new Profil();
	fProfil->Load(profil);
	SetTitle(profil->String());
	BRoster roster;
	entry_ref ref;
	BFile resfile;
	roster.FindApp("application/x-vnd.BeGadu",&ref);
	resfile.SetTo(&ref, B_READ_ONLY);
	fResources.SetTo(&resfile);
	LoadIcons();

	SetSizeLimits(250, 600, 300, 600);
	fSiec = new Siec(fProfil, fProfil->fUserlista->fLista);
	fSiec->GotWindow(this);
	fListaItems = new Lista(512);
	if(fProfil->fRect.IsValid())
		MoveTo(fProfil->fRect.left, fProfil->fRect.top);
	/* ustaw menu */
	BRect r = Bounds();
	BMenuBar *menuBar = new BMenuBar(r, "menu bar");
	BMenu *menu;
	menu = new BMenu("Menu");
	fSubMenu = new BMenu("Profile");
	BMenuItem *item = new BMenuItem("Siakis profil", new BMessage());
	fSubMenu->AddItem(item);
	item = new BMenuItem("Siakis profil inny", new BMessage());
	fSubMenu->AddItem(item);
	fProfilItem = new BMenuItem(fSubMenu, NULL);
	menu->AddItem(fProfilItem);
	menu->AddSeparatorItem();

	fSubMenu = new BMenu("Lista");
	fListaImport = new BMenuItem("Import", new BMessage(BEGG_IMPORT_LIST));
	fListaExport = new BMenuItem("Eksport", new BMessage());
	fSubMenu->AddItem(fListaImport);
	fSubMenu->AddItem(fListaExport);
	fListaMenu = new BMenuItem(fSubMenu, NULL);
	menu->AddItem(fListaMenu);
	menu->AddSeparatorItem();

	fOpcje = new BMenuItem("Ustawienia", new BMessage(BEGG_OPCJE));
	menu->AddItem(fOpcje);
	fAbout = new BMenuItem("O BeGadu..", new BMessage(BEGG_ABOUT));
	menu->AddItem(fAbout);
	menu->AddItem(new BMenuItem("Zakończ", new BMessage(B_QUIT_REQUESTED)));
	menuBar->AddItem(menu);
	AddChild(menuBar);

	r.top = menuBar->Bounds().bottom ;
//	r.bottom = r.top + 32;
//	fIconsView = new BView( BRect(r.left, r.top, r.right, r.bottom), "IconsView", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP , B_FULL_UPDATE_ON_RESIZE);
//	fIconsView->SetViewColor(100,100,100);
//	AddChild(fIconsView);

//	BBitmap *stan1 = LoadGFX("/boot/apps/BeGadu/images/1.png");
//	BBitmap *stan2 = LoadGFX("/boot/apps/BeGadu/images/2.png");
//	IconControl *iconcontrol = new IconControl(BRect(r.left, r.top + 3, r.left + 30, r.top + 30), "iconcontrol", stan1, stan2, new BMessage(B_QUIT_REQUESTED));
//	AddChild(iconcontrol);
	
//	r = Bounds();
//	r.top = fIconsView->Bounds().bottom + menuBar->Bounds().bottom;
	
	fGaduView = new BView(r, "fGaduView", B_FOLLOW_ALL  , B_FRAME_EVENTS | B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE);
	fGaduView->SetViewColor(90,90,90);
	AddChild(fGaduView);
	
	/* lista */
	r = fGaduView->Bounds();
	r.right -= B_V_SCROLL_BAR_WIDTH;
	r.bottom -= 30;

	fListaView = new BListView(r, "fListaView", B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE | B_NAVIGABLE);
	fListaView->SetViewColor(110,110,110);
	BFont *font = new BFont(be_plain_font);
	font->SetSize(15.0);
	font->SetEncoding(B_ISO_8859_2);
	fListaView->SetFont(font);
	fScrollView = new BScrollView("fScroll view", fListaView, B_FOLLOW_ALL , B_FRAME_EVENTS | B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE, false, true, B_PLAIN_BORDER);
	fGaduView->AddChild(fScrollView);
	
	/* musimy wiedzieć czy user zaznaczył/otworzyl osobę */	
	fListaView->SetSelectionMessage(new BMessage(BEGG_OSOBA_ZAZN));
	fListaView->SetInvocationMessage(new BMessage(BEGG_OSOBA_OTRO));

	/* dorabiamy status :) */
	r = fGaduView->Bounds();
	r.top = fListaView->Bounds().bottom + 5;
//	r.top = fGaduView->Bounds().bottom;
	r.left = r.left + 5;
	r.right = r.right -5;

	BMenuItem *selectstatus;
	fStatusMenu = new BPopUpMenu("change_status");
	fDostepny = new GaduMenuItem(fIkonaDostepny,"Dostępny", new BMessage(SET_AVAIL));
	fZarazWracam = new GaduMenuItem(fIkonaZarazWracam,"Zaraz wracam", new BMessage(SET_BRB));
	fNiewidoczny = new GaduMenuItem(fIkonaNiewidoczny,"Niewidoczny", new BMessage(SET_INVIS));
	fNiedostepny = new GaduMenuItem(fIkonaNiedostepny,"Niedostępny", new BMessage(SET_NOT_AVAIL));
	fZOpisem = new GaduMenuItem(fIkonaDostepnyOpis,"Z Opisem", new BMessage(SET_DESCRIPTION));	
	
	fStatusMenu->AddItem(fDostepny);
	fStatusMenu->AddItem(fZarazWracam);
	fStatusMenu->AddItem(fNiewidoczny);
	fStatusMenu->AddItem(fNiedostepny);
	fStatusMenu->AddItem(fZOpisem);
	fNiedostepny->SetMarked(true);
	
	fStatus = new BMenuField(r, "fStatus", "Status:", fStatusMenu, B_FOLLOW_LEFT | B_FOLLOW_BOTTOM, B_WILL_DRAW | B_NAVIGABLE | B_FRAME_EVENTS);
	fGaduView->AddChild(fStatus);

	add_system_beep_event( ONLINE_SOUND );
	add_system_beep_event( MESSAGE_SOUND );
	
	if(fProfil->fUserlista->IsValid())
	{
		fListaItems = fProfil->fUserlista->fLista;
		BMessenger(this).SendMessage( BEGG_UPDATE_LISTY );		
	}

	fprintf(stderr,"Profile %s loaded.\n",fProfil->fNazwaProfilu->String());

	if(fProfil->fAutoStatus != GG_STATUS_NOT_AVAIL)
	{
		if(fSiec->fSesja)
			gg_change_status(fSiec->fSesja, fProfil->fAutoStatus);
		else
			fSiec->Login(fProfil->fAutoStatus);
	}
}

/* mówimy aplikacji by wyszła */
bool MainWindow::QuitRequested()
{
	fprintf( stderr, "MainWindow::QuitRequested()\n" );
	/* saving profile settings */
	fProfil->SetRect(Frame());
	fProfil->Save();
	/* cleaning up ;D */
	fSiec->GotWindow(NULL);
	fSiec->Quit();
	BMessenger(be_app).SendMessage(B_QUIT_REQUESTED);		
	return true;
}

void MainWindow::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case BEGG_OPCJE:
		{
			fprintf( stderr, "MainWindow::MessageReceived( BEGG_OPCJE )\n" );
			BScreen *screen = new BScreen(this);
			display_mode tryb;
			screen->GetMode(&tryb);
			// teraz centrujemy okienko
			int32 szerokosc = 600;
			int32 wysokosc = 400; 
			// obliczamy srodek ekranu /2  - 1/2 szerokosci okna
			int32 x_wind = tryb.timing.h_display/2 - (szerokosc/2);
			// obliczamy srodek ekranu /2 - 1/2 wysokosci okna
			int32 y_wind = tryb.timing.v_display/2 - (wysokosc/2);
			int32 new_szerokosc = x_wind + szerokosc;	// x 2
			int32 new_wysokosc = y_wind + wysokosc;		// x 2
			Opcje	*okno;
			okno = new Opcje(fProfil, this, BRect(x_wind,y_wind,new_szerokosc,new_wysokosc), &fResources);
			okno->Show();
			break;
		}
		case BEGG_ABOUT:
		{
			fprintf( stderr, "MainWindow::MessageReceived( BEGG_ABOUT )\n" );
//			AboutWindow *about;
//			about = new AboutWindow();
//			about->Show();
			break;
		}
		case BEGG_OSOBA_ZAZN:
		{
			fprintf( stderr, "MainWindow::MessageReceived( BEGG_OSOBA_ZAZN )\n" );
			if(fListaView->CurrentSelection() < 0)
				break;
			Osoba *osoba = NULL;
			GaduListItem *kto = (GaduListItem*) fListaView->ItemAt(fListaView->CurrentSelection());
			for(int i = 0; i < fProfil->fUserlista->fLista->CountItems(); i++)
			{
				osoba = (Osoba*) fProfil->fUserlista->fLista->ItemAt(i);
				if(!osoba->fDisplay->Compare( kto->fNazwa->String() ) )
					break;
			}
			break;
		}
		case BEGG_OSOBA_OTRO:
		{
			fprintf( stderr, "MainWindow::MessageReceived( BEGG_OSOBA_OTRO )\n" );
			if(fListaView->CurrentSelection() < 0 )
				break;
			Osoba *osoba = NULL;
			GaduListItem *kto = (GaduListItem*) fListaView->ItemAt(fListaView->CurrentSelection());
			for(int i = 0; i < fProfil->fUserlista->fLista->CountItems(); i++)
			{
				osoba = (Osoba*) fProfil->fUserlista->fLista->ItemAt(i);
				if(!osoba->fDisplay->Compare(kto->fNazwa->String()))
					break;
			}
			if(osoba->fUIN == fProfil->fNumer)
				break;
			BMessage *wiadomosc;
			wiadomosc = new BMessage(OTWORZ_WIADOMOSC);
			wiadomosc->AddInt32("kto", osoba->fUIN);
			BMessenger(fSiec).SendMessage(wiadomosc);		
			delete wiadomosc;
			break;
		}

		case SET_AVAIL:
		{
			fprintf( stderr, "MainWindow::MessageReceived( SET_AVAIL )\n" );
			if(fSiec->fSesja)
			{
				gg_change_status(fSiec->fSesja, GG_STATUS_AVAIL);
				fSiec->fStatus = GG_STATUS_AVAIL;
				BMessenger(this).SendMessage(BEGG_UPDATE_LISTY);
			}
			else
				fSiec->Login(GG_STATUS_AVAIL);
			break;
		}
		case SET_BRB:
		{
			fprintf( stderr, "MainWindow::MessageReceived( SET_BRB )\n" );
			if(fSiec->fSesja)
			{
				gg_change_status(fSiec->fSesja, GG_STATUS_BUSY);
				fSiec->fStatus = GG_STATUS_BUSY;
				BMessenger(this).SendMessage(BEGG_UPDATE_LISTY);
			}
			else
				fSiec->Login(GG_STATUS_BUSY);
			break;
		}
		case SET_INVIS:
		{
			fprintf( stderr, "MainWindow::MessageReceived( SET_INVIS )\n" );
			if(fSiec->fSesja)
			{
				gg_change_status(fSiec->fSesja, GG_STATUS_INVISIBLE);
				fSiec->fStatus = GG_STATUS_INVISIBLE;
				BMessenger(this).SendMessage(BEGG_UPDATE_LISTY);
			}
			else
				fSiec->Login(GG_STATUS_INVISIBLE);
			break;
		}
		case SET_NOT_AVAIL:
		{
			fprintf( stderr, "MainWindow::MessageReceived( SET_NOT_AVAIL )\n" );
			if(fSiec->fSesja)
			{
				fSiec->Logout();
			}
			break;
		}
		case BEGG_UPDATE_STATUS:
		{
			fprintf( stderr, "MainWindow::MessageReceived( BEGG_UPDATE_STATUS )\n" );
			switch(fSiec->GetStatus())
			{
				case GG_STATUS_NOT_AVAIL:
				{
					fNiedostepny->SetMarked(true);
					break;
				}
				case GG_STATUS_INVISIBLE:
				{
					fNiewidoczny->SetMarked(true);
					break;
				}
				case GG_STATUS_BUSY:
				{
					fZarazWracam->SetMarked(true);
					break;
				}
				case GG_STATUS_AVAIL:
				{
					fDostepny->SetMarked(true);
					break;
				}
				case GG_STATUS_AVAIL_DESCR:
				case GG_STATUS_BUSY_DESCR:
				case GG_STATUS_INVISIBLE_DESCR:
				case GG_STATUS_NOT_AVAIL_DESCR:
				{
					fZOpisem->SetMarked(true);
					break;			
				}
			}
			break;
		}
		case BEGG_UPDATE_LISTY:
		{
			fprintf( stderr, "MainWindow::MessageReceived( BEGG_UPDATE_LISTY )\n" );
			Lista *listaDostepnych = new Lista(512);
			Lista *listaNiedostepnych = new Lista(512);
			
			GaduListItem *g = NULL;
			Osoba	 *o = NULL;
			for(int i = 0; i < fProfil->fUserlista->fLista->CountItems(); i++)
			{
				o = (Osoba*) fProfil->fUserlista->fLista->ItemAt(i);
				
				if(o->fUIN == fProfil->fNumer)
				{
					g = new GaduListItem(o->fDisplay, fSiec->GetStatus(), fProfil->fOpis, &fResources);
					if( fSiec->GetStatus() == GG_STATUS_NOT_AVAIL || fSiec->GetStatus() == GG_STATUS_NOT_AVAIL_DESCR ||
						fSiec->GetStatus() == GG_STATUS_INVISIBLE || fSiec->GetStatus() == GG_STATUS_INVISIBLE_DESCR )
					{
						listaNiedostepnych->AddItem(g);
					}
					else
					{
						listaDostepnych->AddItem(g);
					}
				}
				else
				{
					if( fSiec->GetStatus() == GG_STATUS_NOT_AVAIL || fSiec->GetStatus() == GG_STATUS_NOT_AVAIL_DESCR )
					{
						BString *pusty = new BString();
						pusty->SetTo("");
						g = new GaduListItem(o->fDisplay, o->fStatus, pusty, &fResources);
					}
					else
						g = new GaduListItem(o->fDisplay, o->fStatus, o->fOpis, &fResources);
					
					
					
					if( o->fStatus == GG_STATUS_NOT_AVAIL || o->fStatus == GG_STATUS_NOT_AVAIL_DESCR )
					{
						listaNiedostepnych->AddItem(g);
					}
					else
					{
						listaDostepnych->AddItem(g);
					}
				}
			}
			if( fListaView->CountItems() > 0 )
			{
				GaduListItem *a = NULL;
				for( int i = 0; i < fListaView->CountItems(); i++ )
				{
					a = (GaduListItem*) fListaView->ItemAt( i );
					delete a;
				}
				fListaView->MakeEmpty();
			}
			listaDostepnych->SortItems(SortUsers);
			fListaView->AddList(listaDostepnych);
			listaNiedostepnych->SortItems(SortUsers);
			fListaView->AddList(listaNiedostepnych);
			fListaView->Invalidate();
			delete listaDostepnych;
			delete listaNiedostepnych;
			break;
		}
		case BEGG_IMPORT_LIST:
		{
			fprintf( stderr, "MainWindow::MessageReceived( BEGG_IMPORT_LIST )\n" );
			if(fSiec->fSesja)
				fProfil->fUserlista->Import(fSiec->fSesja, fProfil->fUserlista->fLista);
			else
			{
				BAlert *alert = new BAlert("Lista", "Musisz być połączony by zaimportować listę", "Ok");
				alert->Go();
			}
			break;
		}

		default:
			BWindow::MessageReceived(message);
			break;
	}
}

int MainWindow::SortUsers(const void *left, const void *right)
{
//	fprintf( stderr, "MainWindow::SortUsers()\n" );
	const GaduListItem 	**firstItem((const GaduListItem **) left),
						**secondItem((const GaduListItem **) right);
	BString users[2];
	
	users[0] = (*firstItem)->fNazwa->String();
	users[1] = (*secondItem)->fNazwa->String();
	return users[0].ICompare(users[1]);
}

void MainWindow::LoadIcons()
{
	fprintf( stderr, "MainWindow::LoadIcons()\n" );
	fIkonaDostepny 			= GetBitmap( "Avail.png" );
	fIkonaZarazWracam 		= GetBitmap( "Busy.png" );
	fIkonaNiewidoczny 		= GetBitmap( "Invisible.png" );
	fIkonaNiedostepny 		= GetBitmap( "NotAvail.png" );
	fIkonaDostepnyOpis 		= GetBitmap( "AvailDescr.png" );
	fIkonaZarazWracamOpis 	= GetBitmap( "BusyDescr.png" );
	fIkonaNiewidocznyOpis 	= GetBitmap( "InvisibleDescr.png" );
	fIkonaNiedostepnyOpis 	= GetBitmap( "NotAvailDescr.png" );
}

void MainWindow::SetProfil( BString *profil)
{
	fprintf( stderr, "MainWindow::SetProfil()\n" );
	fProfil->Load(profil);
	SetTitle(fProfil->fNazwaProfilu->String());
	if(fProfil->fUserlista->IsValid())
	{
		fListaItems = fProfil->fUserlista->fLista;
		BMessenger(this).SendMessage(BEGG_UPDATE_LISTY);
	}
/*
	if( fProfil->fAutoStatus != GG_STATUS_NOT_AVAIL )
	{
		if(fSiec->fSesja)
			gg_change_status(fSiec->fSesja, fProfil->fAutoStatus);
		else
			fSiec->Login(fProfil->fAutoStatus);
	}
*/
}

BBitmap *MainWindow::GetBitmap(const char *name)
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
