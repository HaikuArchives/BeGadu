/*
	Opcje.h
	Ten plik jest częscią kodu źródłowego BeGadu.
	Homepage: http://gadu.beos.pl
*/

#ifndef _OPCJE_H
#define _OPCJE_H

#include <Window.h>

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
		Opcje(Profil *profil, MainWindow *window, BRect rect);
		virtual void MessageReceived(BMessage *message);
		virtual void Show();
		
		Profil				*	fProfil;
		MainWindow			*	fWindow;
		BBitmap				*	fLogo;
		BitmapView			*	fLogoView;
		BTextControl		*	fNumerControl;
		BTextControl		*	fHasloControl;
};

#endif // _OPCJE_H


