/*
 * ============================================================================
 *  Nazwa    : ProfilWizard z ProfilWizard.h
 *  Projekt  : BeGadu
 *  Authorzy : 
 *		Artur Wyszynski <artur.wyszynski@bellstream.pl>
 *  Opis:
 *		Klasa kreatora profili
 *  Version  : 1.2
 * ============================================================================
 */

#ifndef __BEGADU_PROFILWIZARD_H__
#define __BEGADU_PROFILWIZARD_H__

#include <Window.h>

#include <libgadu.h>

/* zewnetrzne klasy, includowane w zrodle */
class BTextControl;
class BView;
class BButton;
class BRadioButton;
class BBox;

/* Wiadomości od okna */
#define GO_NEXT1 	'gne1'
#define GO_BACK1 	'gba1'
#define GO_NEXT2 	'gne2'
#define GO_BACK2 	'gba2'
#define GO_NEXT3 	'gne3'
#define GO_BACK3 	'gba3'
#define GO_NEXT4 	'gne4'
#define GO_BACK4 	'gba4'
#define GO_CANCEL	'goca'

class ProfilWizard : public BWindow
{
	public:
		ProfilWizard();
		~ProfilWizard();
		virtual void MessageReceived(BMessage *mesg);
		virtual void Show();
		bool	fBreak;
		Profil	*fProfil;
	private:
		BView			*	fPage1;
		BView			*	fPage2;
		BView			*	fPage3;
		BView			*	fPage4;
		BBox			*	fPBox1;
		BBox			*	fPBox2;
		BBox			*	fPBox3;
		BBox			*	fPBox4;
		

		BRadioButton	*	fMam;
		BRadioButton	*	fNowy;

		BTextControl	*	fNazwa;
		BTextControl	*	fNumer;
		BTextControl	*	fHaslo;
		BTextControl	*	fEmail;

		BButton			*	fDalej1;
		BButton			*	fWroc1;
		BButton			*	fAnuluj1;
		BButton			*	fDalej2;
		BButton			*	fWroc2;
		BButton			*	fAnuluj2;
		BButton			*	fDalej3;
		BButton			*	fWroc3;
		BButton			*	fAnuluj3;
		BButton			*	fDalej4;
		BButton			*	fWroc4;
		BButton			*	fAnuluj4;
};

#endif /* __BEGADU_PROFILWIZARD_H__ */
