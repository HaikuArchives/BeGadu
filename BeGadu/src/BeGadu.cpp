/*
	BeGadu.cpp - Client G*adu-G*adu for BeOS
	Code: aljen <aljen@gumisie.org>
	Homepage: http://gadu.beos.pl
*/

#include <Path.h>
#include <FindDirectory.h>
#include <File.h>
#include <Screen.h>

#include "Msg.h"
#include "BeGadu.h"
#include "Siec.h"
#include "Main.h"
#include "Osoba.h"
#include "Debug.h"
#include "Opcje.h"
#include "ProfilWizard.h"

BeGadu::BeGadu() : BApplication("application/x-vnd.BeGadu")
{
	/* inicjalizujemy :) */
	fProfil = new Profil();
	fSiec	= new Siec(fProfil, fProfil->fUserlista->fLista);
	mWindow = new MainWindow(fProfil, fSiec);
	fSiec->GotWindow(mWindow);
	
	/* pokazujemy interfejs BeGadu :) */
	mWindow->Show();
	ProfilWizard *pw = new ProfilWizard(mWindow, fProfil);
	pw->Show();
}

bool BeGadu::QuitRequested()
{
	/* zapisujemy konfiguracje */
	fProfil->fRect = mWindow->Frame();
	fProfil->Save();
	/* czyścimy po sobie */
	fSiec->GotWindow(NULL);
	if(mWindow->Lock())
		mWindow->Quit();
	fSiec->Quit();
	BApplication::QuitRequested();
	return true;
}

void BeGadu::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		/* puszczamy wiadomości od libgadu do Siec'i */
		case MAM_WIADOMOSC:
		case DODAJ_HANDLER:
		case USUN_HANDLER:
			fSiec->PostMessage(message);
			break;

		case BEGG_ABOUT:
		{
			if(mWindow)
				mWindow->PostMessage(message);
			break;
		}
		default:
			BApplication::MessageReceived(message);
			break;
	}
}

int main()
{
	BeGadu	*bgg = new BeGadu();
	bgg->Run();
	delete bgg;
	return 0;
}
