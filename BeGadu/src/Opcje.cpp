/*
	Opcje.cpp
	Ten plik jest częscią kodu źródłowego BeGadu.
	Homepage: http://gadu.beos.pl
*/

#include <stdio.h>
#include <stdlib.h>
#include <View.h>
#include <TextControl.h>
#include <Button.h>
#include <Bitmap.h>
#include <Screen.h>
#include <TabView.h>
#include <CheckBox.h>
#include <Box.h>
#include <OutlineListView.h>
#include <ListView.h>
#include <ListItem.h>

#include "Msg.h"
#include "Opcje.h"
#include "Main.h"
#include "GfxStuff.h"
#include "Siec.h"
#include "Osoba.h"

#define OPCJE_NAME "Ustawienia"

Opcje::Opcje(Profil *profil, MainWindow *window, BRect rect) : BWindow(rect, OPCJE_NAME, B_FLOATING_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_NOT_MOVABLE | B_ASYNCHRONOUS_CONTROLS)
{
	fProfil	= profil;
	fWindow = window;
	BRect r = Bounds();

	fLogo	= LoadGFX("Logo.png");

	fLogoView = new BitmapView( BRect( r.left + 25, r.top,
									   r.left + 125, r.top + 100 ),
									   fLogo );
	AddChild(fLogoView);

    BButton *przycisk;
    przycisk = new BButton( BRect( r.left + 360, r.bottom - 30,
    							   r.left + 440, r.bottom - 5),
    							   "przycisk ok", "Ok",
    							   new BMessage(OPCJE_OK));
    przycisk->MakeDefault(true);
    AddChild(przycisk);
    
    przycisk = new BButton( BRect( r.left + 270, r.bottom - 30,
    							   r.left + 350, r.bottom - 5),
    							   "przycisk anuluj", "Anuluj",
    							   new BMessage(OPCJE_ANULUJ));

    AddChild(przycisk);

	fNumerControl = new BTextControl(BRect(10,10,175,25), "fNumerControl", "Numer GG:", "0", NULL);
	fHasloControl = new BTextControl(BRect(10,40,175,25), "fHasloControl", "Haslo:", "", NULL);
	BView *siakisView = new BView(BRect(100,100,350,200), "siakisView", B_FOLLOW_ALL, B_WILL_DRAW|B_FULL_UPDATE_ON_RESIZE);
	AddChild(siakisView);
	siakisView->AddChild(fNumerControl);
	siakisView->AddChild(fHasloControl);	


	/* pobieramy aktualna konfiguracje */

    if( fNumerControl->LockLooper() )
    {
    	int numer = fProfil->fNumer;
		char *a = (char*) calloc(22,1);
		sprintf(a, "%d", numer);
        fNumerControl->SetText(a);
        fNumerControl->UnlockLooper();
    }    
	if( fHasloControl->LockLooper() )
	{
		char *b = (char*) calloc(20,1);
		sprintf(b, "%s", fProfil->fHaslo);
		fHasloControl->SetText(b);
		fHasloControl->UnlockLooper();
	}
}

void Opcje::MessageReceived(BMessage *mesg)
{
	switch(mesg->what)
	{
		/* anuluj zmiany */
		case OPCJE_ANULUJ:
		{
			PostMessage(B_QUIT_REQUESTED);
			break;
		}
		case OPCJE_OK:
		{
			if(fNumerControl->LockLooper())
			{
				fProfil->fNumer = atoi(fNumerControl->Text());
				fNumerControl->UnlockLooper();
			}
			if(fHasloControl->LockLooper())
			{
				fProfil->fHaslo = (char*)strdup(fHasloControl->Text());
				fHasloControl->UnlockLooper();
			}
			fprintf(stderr, "login: %d\n", fProfil->fNumer);
			fprintf(stderr, "haslo: %s\n", fProfil->fHaslo);
			fprintf(stderr, "haslo: %s\n", fProfil->fHaslo);
			fprintf(stderr, "profil_name: %s\n", fProfil->fNazwaProfilu);
			PostMessage(B_QUIT_REQUESTED);
			break;
		}
		case BEGG_ABOUT:
		{
			fWindow->PostMessage(BEGG_ABOUT);
			break;
		}
		default:
			BWindow::MessageReceived(mesg);
	}
}

void Opcje::Show()
{
    BWindow::Show();
}
