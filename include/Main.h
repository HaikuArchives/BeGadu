/*
	Main.h
	Code: aljen <aljen@gumisie.org>
	Based on Yahoo Messenger by titer
	Homepage: http://gadu.beos.pl
*/

#ifndef _MAIN_WINDOW_H
#define _MAIN_WINDOW_H

#include <Window.h>
#include <ListItem.h>
#include <ListView.h>

class Siec;
class Opcje;
class Lista;
class BStringView;
class BMenu;
class BMenuItem;
class BBitmap;
class Profil;
class BPopUpMenu;
class BMenuField;
class GaduListItem;

class MainWindow : public BWindow
{
	public:
		MainWindow(Profil *profil, Siec *siec);
		virtual bool QuitRequested();
		virtual void MessageReceived(BMessage *message);
		void UstawStatus(char *status);

		/* do gg */
		Profil				*	fProfil;
		Siec				*	fSiec;

		/* do interfejsu */
		BView				*	fGaduView;
		BListView			*	fListaView;
		GaduListItem		*	fListaItem;
		Lista				*	fListaItems;
		
		BMenu				*	fSubMenu;
		BMenuItem			*	fProfilItem;

		BMenuItem			*	fListaMenu;
		BMenuItem			*	fListaImport;
		BMenuItem			*	fListaExport;

		BMenuItem			*	fDodajOsobe;
		BMenuItem			*	fUsunOsobe;
		BMenuItem			*	fKatalog;
		BMenuItem			*	fAbout;
		BMenuItem			*	fOpcje;
		BView				*	fIconsView;

		BMenuField			*	fStatus;
		BPopUpMenu			*	fStatusMenu;
		BMenuItem			*	fDostepny;
		BMenuItem			*	fZarazWracam;
		BMenuItem			*	fNiewidoczny;
		BMenuItem			*	fNiedostepny;
};

#endif // _MAIN_WINDOW_H
