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
#include "ProfilWizard.h"

class BeGadu : public BApplication
{
	private:
		MainWindow  	*	mWindow;
		DebugWindow 	*	dWindow;
		bool				fFirstRun;
		BString			*	fLastProfile;
	public:
		BeGadu();
		virtual bool QuitRequested();
		virtual void MessageReceived(BMessage *message);
};

#endif // _BE_GADU_H
