/*
 * ============================================================================
 *  Nazwa    : Main z Main.h
 *  Projekt  : BeGadu
 *  Authorzy : 
 *		Artur Wyszynski <artur.wyszynski@bellstream.pl>
 *  Opis:
 *		Glowne okno programu
 *  Version  : 1.2
 * ============================================================================
 */

#ifndef __BEGADU_MAINWINDOW_H__
#define __BEGADU_MAINWINDOW_H__

#include <Window.h>
#include <Resources.h>

/* zewnetrzne klasy, includowane w zrodle */
class Siec;
class Opcje;
class Lista;
class BStringView;
class BMenuItem;
class BMenu;
class BBitmap;
class BListItem;
class BListView;
class Profil;
class BPopUpMenu;
class BMenuField;
class GaduListItem;
class GaduMenuItem;
class BScrollView;

class MainWindow : public BWindow
{
	public:
		MainWindow(BString *profil);
		virtual bool QuitRequested();
		virtual void MessageReceived(BMessage *message);
		void SetStatus(char *status);
		void SetProfil( BString *profil );
		static int SortUsers(const void *, const void *);
		void LoadIcons();
		BBitmap	*GetBitmap(const char *name);

		/* do gg */
		Profil				*	fProfil;
		Siec				*	fSiec;

		/* do interfejsu */
		BResources				fResources;
		BView				*	fGaduView;
		BScrollView			*	fScrollView;
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
		GaduMenuItem		*	fDostepny;
		GaduMenuItem		*	fZarazWracam;
		GaduMenuItem		*	fNiewidoczny;
		GaduMenuItem		*	fNiedostepny;
		GaduMenuItem		*	fZOpisem;
		BBitmap				*	fIkonaDostepny;
		BBitmap				*	fIkonaZarazWracam;
		BBitmap				*	fIkonaNiewidoczny;
		BBitmap				*	fIkonaNiedostepny;
		BBitmap				*	fIkonaDostepnyOpis;
		BBitmap				*	fIkonaZarazWracamOpis;
		BBitmap				*	fIkonaNiewidocznyOpis;
		BBitmap				*	fIkonaNiedostepnyOpis;
};

#endif /* __BEGADU_MAINWINDOW_H__ */
