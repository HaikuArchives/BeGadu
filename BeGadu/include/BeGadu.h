/*
	BeGadu.h
	Code: aljen <aljen@gumisie.org>
*/

#ifndef _BE_GADU_H
#define _BE_GADU_H

#include <Application.h>
#include "Main.h"
#include "Osoba.h"
#include "Debug.h"
class Siec;

class BeGadu : public BApplication
{
	private:
		Profil		*	fProfil;
		MainWindow  *	mWindow;
		DebugWindow *	dWindow;
		Siec		*	fSiec;
	public:
		BeGadu();
		virtual bool QuitRequested();
		virtual void MessageReceived(BMessage *message);
};

#endif // _BE_GADU_H