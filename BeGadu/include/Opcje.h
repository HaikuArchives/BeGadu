/*
 * ============================================================================
 *  Nazwa    : Opcje z Opcje.h
 *  Projekt  : BeGadu
 *  Authorzy : 
 *		Artur Wyszynski <artur.wyszynski@bellstream.pl>
 *  Opis:
 *		Klasa okna z opcjami
 *  Version  : 1.2
 * ============================================================================
 */

#ifndef __BEGADU_OPCJE_H__
#define __BEGADU_OPCJE_H__

#include <Window.h>
#include <Resources.h>

/* zewnetrzne klasy, includowane w zrodle */
class Profil;
class BTextControl;
class BCheckBox;
class MainWindow;
class Siec;
class Osoba;
class BBitmap;
class BitmapView;

class Opcje : public BWindow
{
	public:
		Opcje(Profil *profil, MainWindow *window, BRect rect, BResources *res);
		virtual void MessageReceived(BMessage *message);
		virtual void Show();
		BBitmap	*GetBitmap(const char *name);
		
		Profil				*	fProfil;
		MainWindow			*	fWindow;
		BBitmap				*	fLogo;
		BitmapView			*	fLogoView;
		BTextControl		*	fNumerControl;
		BTextControl		*	fHasloControl;
		BResources			*	fResources;
};

#endif /* __BEGADU_OPCJE_H__ */
