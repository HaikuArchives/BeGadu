/*
	Opcje.cpp
	Ten plik jest częscią kodu źródłowego BeGadu.
	Homepage: http://begadu.sf.net
*/

#include <stdio.h>
#include <stdlib.h>
#include <View.h>
#include <String.h>
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

Opcje::Opcje(Profil *profil, MainWindow *window, BRect rect, BResources *res) : BWindow(rect, OPCJE_NAME, B_FLOATING_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_NOT_MOVABLE | B_ASYNCHRONOUS_CONTROLS)
{
	fProfil	= profil;
	fWindow = window;
	fResources = res;
	BRect r = Bounds();

	fLogoView = new BitmapView( BRect( r.left , r.top,
									   r.left + 600, r.top + 150 ),
									   "logo", res );
	AddChild(fLogoView);
	
	r.left = 20;
	r.top = 100;
	r.right = r.left + 250;
	r.bottom = r.top + 25;
	fNumerControl = new BTextControl(r, "fNumerControl", "Numer GG:", "0", NULL, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE);
	AddChild(fNumerControl);
	r.left = 20;
	r.right = r.left + 250;
	r.top = 130;
	r.bottom = r.top + 25;
	fHasloControl = new BTextControl(r, "fHasloControl", "Haslo:", "", NULL, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE);
	AddChild(fHasloControl);
	r = Bounds();
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

    if( fNumerControl->LockLooper() )
    {
		BString a;
		a << (int32)fProfil->fNumer;
        fNumerControl->SetText(a.String());
        fHasloControl->SetText(fProfil->fHaslo->String());
		fprintf(stderr,"numer: %s\nhaslo: %s\n", a.String(), fProfil->fHaslo->String());
        fNumerControl->UnlockLooper();
    }    

}

void Opcje::MessageReceived(BMessage *mesg)
{
	switch(mesg->what)
	{
		/* anuluj zmiany */
		case OPCJE_ANULUJ:
		{
			BMessenger(this).SendMessage(B_QUIT_REQUESTED);
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
				fProfil->fHaslo->SetTo(fHasloControl->Text());
				fHasloControl->UnlockLooper();
			}
			BMessenger(this).SendMessage(B_QUIT_REQUESTED);
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

BBitmap *Opcje::GetBitmap(const char *name)
{
	BBitmap 	*bitmap = NULL;
	size_t 		len = 0;
	status_t 	error;	

	const void *data = fResources->LoadResource('BBMP', name, &len);

	BMemoryIO stream(data, len);
	
	BMessage archive;
	error = archive.Unflatten(&stream);
	if (error != B_OK)
		return NULL;
	bitmap = new BBitmap(&archive);
	if(!bitmap)
		return NULL;

	if(bitmap->InitCheck() != B_OK)
	{
		delete bitmap;
		return NULL;
	}
	
	return bitmap;
}
