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
#include "StackBar.h"
#include "StackPage.h"

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

	fSBar = new StackBar( BRect( r.left, r.top + 100,
								 r.left+150, r.bottom ),
						  "opcje", B_FOLLOW_LEFT | B_FOLLOW_TOP_BOTTOM,
						  B_FULL_UPDATE_ON_RESIZE | B_WILL_DRAW | B_FRAME_EVENTS | 
						  B_NAVIGABLE_JUMP | B_NAVIGABLE, B_FANCY_BORDER );
	AddChild(fSBar);
	
	fProfilPageView = new BView(fSBar->Bounds(), "ProfilPageView", B_FOLLOW_ALL, B_WILL_DRAW);

	BBitmap *profilbmp = LoadGFX("Profil.png");
	BitmapView *profilbmpvw = new BitmapView(BRect(fProfilPageView->Bounds().left + 10, fProfilPageView->Bounds().top + 10, fProfilPageView->Bounds().left + 60, fProfilPageView->Bounds().top + 60), profilbmp);
	fProfilPageView->AddChild(profilbmpvw);
	fProfilPage = new StackPage("Profil");
	fProfilView = new BView( BRect( r.left + 150, r.top,
									r.right, r.bottom - 35 ),
							 "ProfilView", B_FOLLOW_ALL, B_WILL_DRAW);
//	fProfilView->SetViewColor(255,0,255);
	fOpcjePageView = new BView(fSBar->Bounds(), "OpcjePageView", B_FOLLOW_ALL, B_WILL_DRAW);
	fOpcjePage = new StackPage("Opcje");
	fSBar->AddPage(fProfilPageView, fProfilPage);
	fSBar->AddPage(fOpcjePageView, fOpcjePage);
	AddChild(fProfilView);

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

	


	/* pobieramy aktualna konfiguracje */
/*
    if( fNumerControl->LockLooper() )
    {
    	int numer = fProfil->fNumer;
		char *a;
		sprintf(a, "%d", numer);
        fNumerControl->SetText(a);
        fHasloControl->SetText(fProfil->fHaslo);
        fNumerControl->UnlockLooper();
    }    
*/
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
/*			if(fNumerControl->LockLooper())
			{
				fProfil->fNumer = atoi(fNumerControl->Text());
				fNumerControl->UnlockLooper();
			}
			if(fHasloControl->LockLooper())
			{
				fProfil->fHaslo = (char*)strdup(fHasloControl->Text());
				fHasloControl->UnlockLooper();
			}
			PostMessage(B_QUIT_REQUESTED);
			*/
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
