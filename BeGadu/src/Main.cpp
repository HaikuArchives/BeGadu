/*
	Main.cpp
	Code: aljen <aljen@gumisie.org>
	Homepage: http://gadu.beos.pl
*/

#include <stdlib.h>
#include <string.h>
#include <String.h>
#include <StringView.h>
#include <PopUpMenu.h>
#include <MenuItem.h>
#include <MenuBar.h>
#include <MenuField.h>
#include <ScrollView.h>
#include <OutlineListView.h>
#include <Application.h>
#include <Bitmap.h>
#include <Screen.h>
#include <Point.h>
#include <Alert.h>

#include "GaduListItem.h"
#include "Msg.h"
#include "Main.h"
#include "Siec.h"
#include "Opcje.h"
#include "Osoba.h"
#include "GfxStuff.h"

#define MAINWINDOW_RECT BRect(50,50,300,350)
#define MAINWINDOW_NAME	"BeGadu " WERSJA

MainWindow::MainWindow(Profil *profil, Siec *siec)
 	: BWindow(MAINWINDOW_RECT, MAINWINDOW_NAME, B_TITLED_WINDOW, B_NOT_ZOOMABLE | B_ASYNCHRONOUS_CONTROLS)
{
	SetSizeLimits(250, 600, 300, 600);
	fProfil		= profil;
	fListaItems = new Lista(512);
	fSiec		= siec;
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
	r.bottom = r.top + 32;
	fIconsView = new BView( BRect(r.left, r.top, r.right, r.bottom), "IconsView", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP , B_FULL_UPDATE_ON_RESIZE);
	fIconsView->SetViewColor(100,100,100);
	AddChild(fIconsView);

//	BBitmap *stan1 = LoadGFX("/boot/apps/BeGadu/images/1.png");
//	BBitmap *stan2 = LoadGFX("/boot/apps/BeGadu/images/2.png");
//	IconControl *iconcontrol = new IconControl(BRect(r.left, r.top + 3, r.left + 30, r.top + 30), "iconcontrol", stan1, stan2, new BMessage(B_QUIT_REQUESTED));
//	AddChild(iconcontrol);
	
	r = Bounds();
	r.top = fIconsView->Bounds().bottom + menuBar->Bounds().bottom;
	
	fGaduView = new BView(r, "fGaduView", B_FOLLOW_ALL, B_FRAME_EVENTS | B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE);
	AddChild(fGaduView);
	
	/* lista */
	r = fGaduView->Bounds();
	r.right -= B_V_SCROLL_BAR_WIDTH;
	r.bottom -= 30;

	fListaView = new BOutlineListView(r, "fListaView");
	BFont *font = new BFont(be_plain_font);
	font->SetSize(15.0);
	font->SetEncoding(B_ISO_8859_2);
	fListaView->SetFont(font);
	BScrollView *scrollView = new BScrollView("scroll view", fListaView, B_FOLLOW_ALL, B_FULL_UPDATE_ON_RESIZE, false, true);
	fGaduView->AddChild(scrollView);
	
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
	fStatusMenu = new BPopUpMenu("status");
	fDostepny = new BMenuItem("Dostępny", new BMessage(SET_AVAIL));
	fZarazWracam = new BMenuItem("Zaraz wracam", new BMessage(SET_BRB));
	fNiewidoczny = new BMenuItem("Niewidoczny", new BMessage(SET_INVIS));
	fNiedostepny = new BMenuItem("Niedostępny", new BMessage(SET_NOT_AVAIL));
	
	fStatusMenu->AddItem(fDostepny);
	fStatusMenu->AddItem(fZarazWracam);
	fStatusMenu->AddItem(fNiewidoczny);
	fStatusMenu->AddItem(fNiedostepny);
	fNiedostepny->SetMarked(true);
	
	fStatus = new BMenuField(r, "fStatus", "Status:", fStatusMenu, B_FOLLOW_LEFT | B_FOLLOW_BOTTOM, B_WILL_DRAW | B_NAVIGABLE | B_FRAME_EVENTS);
	fGaduView->AddChild(fStatus);

	if(fProfil->fUserlista->IsValid())
	{
		fListaItems = fProfil->fUserlista->fLista;
		PostMessage(BEGG_UPDATE_LISTY);
	}
}

/* mówimy aplikacji by wyszła */
bool MainWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	/* nie zamykamy okna, najpierw zapiszemy jego aktualną pozycje :) */
	return false;
}

void MainWindow::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case BEGG_OPCJE:
		{
			BScreen *screen = new BScreen(NULL);
			display_mode tryb;
			screen->GetMode(&tryb);
			// teraz centrujemy okienko
			int32 szerokosc = 450;
			int32 wysokosc = 400; 
			// obliczamy srodek ekranu /2  - 1/2 szerokosci okna
			int32 x_wind = tryb.timing.h_display/2 - (szerokosc/2);
			// obliczamy srodek ekranu /2 - 1/2 wysokosci okna
			int32 y_wind = tryb.timing.v_display/2 - (wysokosc/2);
			int32 new_szerokosc = x_wind + szerokosc;	// x 2
			int32 new_wysokosc = y_wind + wysokosc;		// x 2
			Opcje	*okno;
			okno = new Opcje(fProfil, this, BRect(x_wind,y_wind,new_szerokosc,new_wysokosc));
			okno->Show();
			break;
		}
		case BEGG_ABOUT:
		{
//			AboutWindow *about;
//			about = new AboutWindow();
//			about->Show();
			break;
		}
		case BEGG_OSOBA_ZAZN:
		{
			/* do implementacji :) */
			break;
		}
		case BEGG_OSOBA_OTRO:
		{
			if(fListaView->CurrentSelection() < 0 )
				break;
				
			GaduListItem *kto;
			Osoba *osoba;
			kto = (GaduListItem*) fListaView->ItemAt(fListaView->CurrentSelection());
			for(int i = 0; i < fProfil->fUserlista->fLista->CountItems(); i++)
			{
				osoba = (Osoba*) fProfil->fUserlista->fLista->ItemAt(i);
				fprintf(stderr, "kto->fNazwa = %s, osoba->fDisplay = %s\n", kto->fNazwa, osoba->fDisplay);
				if(!strcmp(osoba->fDisplay, kto->fNazwa))
				{
					fprintf(stderr, "sa rowne :)\n");
					break;
				}
			}
			BMessage *wiadomosc;
			wiadomosc = new BMessage(OTWORZ_WIADOMOSC);
			wiadomosc->AddInt32("kto", osoba->fUIN);
			fSiec->PostMessage(wiadomosc);
			delete wiadomosc;
			break;
		}

		case SET_AVAIL:
		{
			if(fSiec->fSesja)
				gg_change_status(fSiec->fSesja, GG_STATUS_AVAIL);
			else
				fSiec->Login(GG_STATUS_AVAIL);
			break;
		}
		case SET_BRB:
		{
			if(fSiec->fSesja)
				gg_change_status(fSiec->fSesja, GG_STATUS_BUSY);
			else
				fSiec->Login(GG_STATUS_BUSY);
			break;
		}
		case SET_INVIS:
		{
			if(fSiec->fSesja)
				gg_change_status(fSiec->fSesja, GG_STATUS_INVISIBLE);
			else
				fSiec->Login(GG_STATUS_INVISIBLE);
			break;
		}
		case SET_NOT_AVAIL:
		{
			if(fSiec->fSesja)
			{
				fSiec->Logout();
			}
			break;
		}
		case BEGG_UPDATE_STATUS:
		{
			switch(fSiec->GetStatus())
			{
				case GG_STATUS_NOT_AVAIL:
				case GG_STATUS_NOT_AVAIL_DESCR:
				{
					fNiedostepny->SetMarked(true);
					break;
				}
				case GG_STATUS_INVISIBLE:
				case GG_STATUS_INVISIBLE_DESCR:
				{
					fNiewidoczny->SetMarked(true);
					break;
				}
				case GG_STATUS_BUSY:
				case GG_STATUS_BUSY_DESCR:
				{
					fZarazWracam->SetMarked(true);
					break;
				}
				case GG_STATUS_AVAIL:
				case GG_STATUS_AVAIL_DESCR:
				{
					fDostepny->SetMarked(true);
					break;
				}
			}
			break;
		}
		case BEGG_UPDATE_LISTY:
		{
			fListaItems = new Lista(512);
			GaduListItem *g = NULL;
			Osoba	 *o = NULL;
			for(int i = 0; i < fProfil->fUserlista->fLista->CountItems(); i++)
			{
				o = (Osoba*) fProfil->fUserlista->fLista->ItemAt(i);
				g = new GaduListItem(o->fDisplay, o->fStatus);
				fListaItems->AddItem(g);
			}
			fListaView->AddList(fListaItems);
			fListaView->Invalidate();
			
			break;
		}
		case BEGG_IMPORT_LIST:
		{
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
