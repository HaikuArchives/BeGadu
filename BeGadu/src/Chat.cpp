/*
	Chat.cpp
	Code: aljen <aljen@gumisie.org>
	Homepage: http://gadu.beos.pl
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <TextControl.h>
#include <ScrollView.h>
#include <TextView.h>
#include <Screen.h>
#include <String.h>
#include "Msg.h"
#include "Chat.h"
#include "Siec.h"
#include "Main.h"
#include "Osoba.h"
extern "C" {
#include "libgadu.h"
}

#define CHATWIN_RECT BRect(100,100,500,400)
#define CHATWIN_NAME "Rozmowa z "

ChatWindow::ChatWindow(Siec *siec, MainWindow *window, uin_t kto) : BWindow(CHATWIN_RECT, CHATWIN_NAME, B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_ZOOMABLE)
{
	fSiec 	= siec;
	fWindow	= window;
	fKto	= kto;
	SetSizeLimits(300,2000,200,2000);
	
	/* 
		poprawiamy tytuł, sprawdzamy czy dany numer mamy na liscie, jesli tak to dajemy
		fDisplay
	*/
	Osoba *osoba;
	BString tytul = Title();
	BString *os;
	for(int i = 0; i < fWindow->fProfil->fUserlista->fLista->CountItems(); i++)
	{
		osoba = (Osoba*) fWindow->fProfil->fUserlista->fLista->ItemAt(i);
		if(kto == osoba->fUIN)
		{
			os = osoba->fDisplay;
			break;
		}
		else
		{
			os = new BString();
			os->SetTo("[Nieznajomy]");
		}
	}
	tytul.Append(os->String());
	SetTitle(tytul.String());
	
	/* robimy defaultowe tło */
	BRect r = Bounds();
	BView *siakisView;
	siakisView = new BView(r, "siakis view", B_FOLLOW_ALL, B_WILL_DRAW);
	siakisView->SetViewColor(60,60,60);
	AddChild(siakisView);
	
	/* 'rozmowa' na bazie BTextView */
	r = siakisView->Bounds();
	r.InsetBy(10,10);
	r.right -= B_V_SCROLL_BAR_WIDTH;
	r.bottom -= 25;
	BRect tekstRect = BRect(5,5,r.Width() - 5, r.Height() - 5);
	fRozmowa = new BTextView(r, "rozmowa view", tekstRect, B_FOLLOW_ALL, B_WILL_DRAW);
	fRozmowa->MakeEditable(false);
	fRozmowa->SetStylable(true);
	BFont *font = new BFont(be_plain_font);
	font->SetSize(15.0);
	font->SetEncoding(B_ISO_8859_2);
	fRozmowa->SetFontAndColor(font);
	fScrollView = new BScrollView("scroll view", fRozmowa, B_FOLLOW_ALL, 0, false, true);
	siakisView->AddChild(fScrollView);
	fRozmowa->SetViewColor(70,70,70);	
	/* pole do pisania treści wiadomości */
	r = siakisView->Bounds();
	r.InsetBy(10,10);
	r.top = r.bottom - 15;
	fPowiedzControl = new BTextControl(r, "powiedz control", "Powiedz:", NULL, new BMessage(BEGG_WYSLIJ), B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM);
	fPowiedzControl->MakeFocus(true);
	float width, height;
	fPowiedzControl->GetPreferredSize(&width, &height);
	fPowiedzControl->SetDivider(width/2);
	fPowiedzControl->SetFont(font);
	siakisView->AddChild(fPowiedzControl);
}

void ChatWindow::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case POKAZ_WIADOMOSC:
		{
			const char *msg;
			message->FindString("msg", &msg);
			time_t _teraz = time(NULL);
			struct tm *teraz = localtime(&_teraz);
			BString str;
			BString str2;
			char *string;
			Osoba *osoba;
			for(int i = 0; i < fWindow->fProfil->fUserlista->fLista->CountItems(); i++)
			{
				osoba = (Osoba*) fWindow->fProfil->fUserlista->fLista->ItemAt(i);
				if(fKto == osoba->fUIN)
				{
					str.SetTo(osoba->fDisplay->String());
					break;
				}
				else
					str << (int32)fKto;
			}
			BFont *font = new BFont(be_plain_font);
			font->SetSize(16.0);
			font->SetEncoding(B_ISO_8859_2);
			rgb_color zolty = {255,255,0,0};
			rgb_color czerwony = {255,0,0,0};
			rgb_color bialy = {255,255,255,0};
			string = (char*)calloc(strlen("[00:00] "), 1);
			sprintf(string, "[%02d:%02d] ", teraz->tm_hour, teraz->tm_min);
			str2.SetTo(string);
			free(string);
//			str2 << "[" << (int32)now->tm_hour << ":" << now->tm_min << "] ";
			fRozmowa->SetFontAndColor(fRozmowa->TextLength(), fRozmowa->TextLength() + str2.Length(), font, B_FONT_ALL, &zolty);
			fRozmowa->Insert(fRozmowa->TextLength(), str2.String(), str2.Length());
			str.Append(": ");

			fRozmowa->SetFontAndColor(fRozmowa->TextLength(), fRozmowa->TextLength() + str.Length(), font, B_FONT_ALL, &czerwony);
			fRozmowa->Insert(fRozmowa->TextLength(), str.String(), str.Length());

			str2.SetTo(msg);
			str2.Append("\n");
			fRozmowa->SetFontAndColor(fRozmowa->TextLength(), fRozmowa->TextLength() + str2.Length(), font, B_FONT_ALL, &bialy);
			fRozmowa->Insert(fRozmowa->TextLength(), str2.String(), str2.Length());
			BScrollBar * scrollBar = fScrollView->ScrollBar(B_VERTICAL);
			if(scrollBar->LockLooper())
			{
				float max,min;
				scrollBar->GetRange(&min, &max);
				scrollBar->SetValue(max);
				scrollBar->UnlockLooper();
			}
			break;			
		}
		case BEGG_WYSLIJ:
		{
			if(fPowiedzControl->LockLooper())
			{
				if(!*fPowiedzControl->Text())
				{
					/* nic nie wysyłamy */
					fPowiedzControl->UnlockLooper();
					break;
				}
				
				/* najpierw dodajemy wiadomośc lokalnie do rozmowy */
				time_t _teraz = time(NULL);
				struct tm * teraz = localtime(&_teraz);
				BString str;
				BString str2;
				char *string;
				int id = fSiec->GetIdent();

				BFont *font = new BFont(be_plain_font);
				font->SetSize(16.0);
				font->SetEncoding(B_ISO_8859_2);
				rgb_color zolty = {255,255,0,0};
				rgb_color zielony = {0,255,0,0};
				rgb_color bialy = {255,255,255,0};
				string = (char*)calloc(strlen("[00:00] "), 1);
				sprintf(string, "[%02d:%02d] ", teraz->tm_hour, teraz->tm_min);
				str2.SetTo(string);
				free(string);
				fRozmowa->SetFontAndColor(fRozmowa->TextLength(), fRozmowa->TextLength() + str2.Length(), font, B_FONT_ALL, &zolty);
				fRozmowa->Insert(fRozmowa->TextLength(), str2.String(), str2.Length());

				str.SetTo(fWindow->fProfil->fNazwaProfilu->String());
				str.Append(": ");
				fRozmowa->SetFontAndColor(fRozmowa->TextLength(), fRozmowa->TextLength() + str.Length(), font, B_FONT_ALL, &zielony);
				fRozmowa->Insert(fRozmowa->TextLength(), str.String(), str.Length());

				str2.SetTo(fPowiedzControl->Text());
				str2.Append("\n");
				fRozmowa->SetFontAndColor(fRozmowa->TextLength(), fRozmowa->TextLength() + str2.Length(), font, B_FONT_ALL, &bialy);
				fRozmowa->Insert(fRozmowa->TextLength(), str2.String(), str2.Length());

//				string = (char*)calloc(strlen(" [00:00]\n%s\n") + 1 + fWindow->fProfil->fNazwaProfilu->Length() + strlen(fPowiedzControl->Text()), 1);
//				sprintf(string, "%s [%02d:%02d]\n%s\n", fWindow->fProfil->fNazwaProfilu->String(), teraz->tm_hour, teraz->tm_min, fPowiedzControl->Text());
//				fRozmowa->Insert(fRozmowa->TextLength(), string, strlen(string));
//				free(string);
				
				/* przewiń na dół */
				BScrollBar * scrollBar = fScrollView->ScrollBar(B_VERTICAL);
				if(scrollBar->LockLooper())
				{
					float max,min;
					scrollBar->GetRange(&min, &max);
					scrollBar->SetValue(max);
					scrollBar->UnlockLooper();
				}
				
				/* no i w końcu wysyłamy :) */
				BMessage *nowawiadomosc;
				nowawiadomosc = new BMessage(WYSLIJ_WIADOMOSC);
				nowawiadomosc->AddInt32("kto", fKto);
				nowawiadomosc->AddString("msg",fPowiedzControl->Text());
				fSiec->PostMessage(nowawiadomosc);
				
				/* wyczyśćmy pole tekstu */
				fPowiedzControl->SetText(NULL);
				fPowiedzControl->UnlockLooper();
			}
			break;
		}
		case BEGG_ABOUT:
		{
			fWindow->PostMessage(BEGG_ABOUT);
			break;
		}
		default:
			BWindow::MessageReceived(message);
			break;
	}
}

bool ChatWindow::QuitRequested()
{
	BMessage *mesg = new BMessage(ZAMKNIJ_WIADOMOSC);
	mesg->AddPointer("win",this);
	fSiec->PostMessage(mesg);
	delete mesg;
	return false;
}

void ChatWindow::FrameResized(float width, float height)
{
	BWindow::FrameResized(width,height);
	BRect	r = fRozmowa->Bounds();
	r.InsetBy(10,10);
	fRozmowa->SetTextRect(r);
	BScrollBar *scrollBar = fScrollView->ScrollBar(B_VERTICAL);
	if(scrollBar->LockLooper())
	{
		float min, max;
		scrollBar->GetRange(&min,&max);
		scrollBar->SetValue(max);
		scrollBar->UnlockLooper();
	}
}

void ChatWindow::Show()
{
	/* wyciągamy okno na wierzch */
	BPoint point;
	point.x = fWindow->Frame().left + 
				(fWindow->Frame().Width() - Frame().Width())/2;
	point.y = fWindow->Frame().top +
				(fWindow->Frame().Height() - Frame().Height())/2;
	BScreen _screen;
	BRect screen = _screen.Frame();
	point.x = MIN(point.x, screen.right - Frame().Width());
	point.x = MAX(point.x, screen.left);
	point.y = MIN(point.y, screen.bottom - Frame().Height());
	point.y = MAX(point.y, screen.top);
	MoveTo(point);
	fPowiedzControl->MakeFocus();
	BWindow::Show();
}

void ChatWindow::WindowActivated(bool activated)
{
	if(activated)
	{
		fPowiedzControl->MakeFocus(true);
	}
	BWindow::WindowActivated(activated);
}
