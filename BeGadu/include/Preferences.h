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

#ifndef __BEGADU_PREFERENCES_H__
#define __BEGADU_PREFERENCES_H__

#include <Window.h>
#include <Resources.h>

/* zewnetrzne klasy, includowane w zrodle */
class BBox;
class Profile;
class BTextControl;
class BCheckBox;
class MainWindow;
class Network;
class Person;
class BBitmap;
class BitmapView;
class PreferencesList;

class Preferences : public BWindow
	{
	public:
		Preferences( Profile* aProfile, MainWindow* aWindow, BRect aRect, BResources* aRes );
		virtual void MessageReceived( BMessage* aMessage );
		virtual void Show();
		
	private:
		BBitmap* GetBitmap( const char* aName );
		void SwitchTo( int iBox );
		void LoadConfig();
		void SaveConfig();
		
	private:
		BView				*	iView;
		PreferencesList		*	iPreferencesList;
		BBox				*	iBoxGeneral;
		BBox				*	iBoxGeneralProfile;
		BTextControl		*	iProfileNameControl;
		BCheckBox			*	iProfilePasswordCheckBox;
		BTextControl		*	iProfilePasswordControl;
		BTextControl		*	iProfilePasswordRepeatControl;
		BTextControl		*	iProfileUINControl;
		BTextControl		*	iProfileUINPasswordControl;
		BButton				*	iProfileDeleteAccount;
		BButton				*	iProfileChangePassword;
		BCheckBox			*	iProfileRememberPasswordCheckBox;
		
		BBox				*	iBoxGeneralAccount;
		BBox				*	iBoxIM;
		BBox				*	iBoxSounds;
		BBox				*	iBoxCatalog;
		BBox				*	iBoxNotify;
		BBox				*	iBoxTheme;
		BBox				*	iBoxConnection;
		
		int						iBox;
		
		Profile				*	iProfile;
		MainWindow			*	iWindow;
		BResources			*	iResources;
	};

#endif /* __BEGADU_PREFERENCES_H__ */
