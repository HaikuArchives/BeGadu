#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <InterfaceKit.h>
#include <Screen.h>
#include <Application.h>
#include <Alert.h>
#include <String.h>
#include <libgadu.h>

#include "Main.h"
#include "Msg.h"
#include "Osoba.h"
#include "ProfilWizard.h"
#include "GfxStuff.h"

#define PROFILWIZARD_RECT BRect(0,0,350,300)
#define PROFILWIZARD_NAME "Utwórz profil..."

ProfilWizard::ProfilWizard() : BWindow(PROFILWIZARD_RECT, PROFILWIZARD_NAME, B_TITLED_WINDOW, B_NOT_ZOOMABLE | B_NOT_MOVABLE | B_NOT_RESIZABLE )
{
	fBreak = false;
	fProfil = new Profil();
	BRect r = Bounds();
	BRect przycisk = Bounds();
	przycisk.top = przycisk.bottom - 30;
	przycisk.bottom = przycisk.top + 25;
	przycisk.left = przycisk.left + 5;
	przycisk.right = przycisk.left + 60;

	fPage1 = new BView(r, "fPage1", B_FOLLOW_ALL, B_WILL_DRAW);
	fPage2 = new BView(r, "fPage2", B_FOLLOW_ALL, B_WILL_DRAW);
	fPage3 = new BView(r, "fPage3", B_FOLLOW_ALL, B_WILL_DRAW);
	fPage4 = new BView(r, "fPage4", B_FOLLOW_ALL, B_WILL_DRAW);
	fPage1->SetViewColor(100,100,100);
	fPage2->SetViewColor(100,100,100);
	fPage3->SetViewColor(100,100,100);
	fPage4->SetViewColor(100,100,100);
	r = fPage1->Bounds();
	r.left = r.left + 10;
	r.right = r.right - 10;
	r.top = r.top + 10;
	r.bottom = r.bottom - 90;
	fPBox1 = new BBox(r, "fPBox1");
	fPBox2 = new BBox(r, "fPBox2");
	fPBox3 = new BBox(r, "fPBox3");
	fPBox4 = new BBox(r, "fPBox4");
	fPBox2->SetViewColor(120,120,120);
	
	BFont *font = new BFont(be_plain_font);
	font->SetEncoding(B_ISO_8859_2);
	font->SetSize(15.0);

	r = fPBox1->Bounds();
	r.top = r.top + 5;
	r.left = r.left + 5;
	r.bottom = r.bottom - 5;
	r.right = r.right - 5;
	BTextView *tv = new BTextView(r, "tv", r, B_FOLLOW_ALL, B_WILL_DRAW);
	tv->MakeEditable(false);
	tv->MakeSelectable(false);
	tv->SetStylable(true);
	font = new BFont(be_bold_font);
	font->SetEncoding(B_ISO_8859_2);
	font->SetSize(18.0);
	rgb_color bialy = {255,255,255};
	tv->SetFontAndColor(font, B_FONT_ALL, &bialy);
	tv->Insert("Witaj w kreatorze profili\n");
	font->SetFace(B_REGULAR_FACE);
	font->SetSize(15.0);
	tv->SetFontAndColor(font);
	tv->Insert("\n\nTen kreator pomoże Ci skonfigurować swoje istniejące konto "
				"na serwerze Gadu-Gadu, lub założyć nowe.\n\n"
				"Aby kontynuować, wciśnij ");
	font->SetFace(B_BOLD_FACE);
	tv->SetFontAndColor(font);
	tv->Insert("Dalej.");
	tv->SetViewColor(120,120,120);
	fPBox1->AddChild(tv);
	fPage1->AddChild(fPBox1);
	r = Bounds();
	fAnuluj1 = new BButton( przycisk, "fAnuluj1", "Anuluj", new BMessage(GO_CANCEL));
	fPage1->AddChild(fAnuluj1);
	przycisk.OffsetBy(200, 0);
	fWroc1 = new BButton( przycisk, "fWroc1", "Wróć", new BMessage(GO_BACK1));
	fWroc1->SetEnabled(false);
	fPage1->AddChild(fWroc1);
	przycisk.OffsetBy(75, 0);
	fDalej1 = new BButton(przycisk , "fDalej1", "Dalej", new BMessage(GO_NEXT1));
	fDalej1->MakeDefault(true);
	fPage1->AddChild(fDalej1);
	AddChild(fPage1);


	r = fPBox2->Bounds();
	BStringView *sv = new BStringView(	BRect(r.left + 10, r.top + 10, r.right - 10, r.top + 30),
										"tc",
										"",
										B_FOLLOW_ALL, B_WILL_DRAW);
	font = new BFont(be_bold_font);
//	font->SetEncoding(B_ISO_8859_2);
	font->SetSize(18.0);
	sv->SetFont(font);
	sv->SetText("Co chcesz zrobić ?");
	sv->SetHighColor(255,255,255);
	fPBox2->AddChild(sv);
	font->SetSize(15.0);
	font->SetFace(B_REGULAR_FACE);
	fMam = new BRadioButton(BRect(r.left + 10, r.top + 60, r.right - 10, r.top + 80),
							"fMam",
							"Skonfigurować istniejące konto", new BMessage());
	fMam->SetValue(1);
	fMam->SetFont(font);
	fMam->SetHighColor(255,255,255);
	fNowy = new BRadioButton(BRect(r.left + 10, r.top + 90, r.right - 10, r.top + 110),
							"fNowy",
							"Założyć nowe konto (obecnie nie zaimplementowane)", new BMessage());
	fNowy->SetValue(0);
	fNowy->SetFont(font);
	fNowy->SetEnabled(false);
	fNowy->SetHighColor(255,255,255);
	fPBox2->AddChild(fMam);
	fPBox2->AddChild(fNowy);
	przycisk = Bounds();
	przycisk.top = przycisk.bottom - 30;
	przycisk.bottom = przycisk.top + 25;
	przycisk.left = przycisk.left + 5;
	przycisk.right = przycisk.left + 60;
	fAnuluj2 = new BButton( przycisk, "fAnuluj2", "Anuluj", new BMessage(GO_CANCEL));
	fPage2->AddChild(fAnuluj2);
	przycisk.OffsetBy(200,0);
	fWroc2 = new BButton( przycisk, "fWroc2", "Wróć", new BMessage(GO_BACK2));
	fPage2->AddChild(fWroc2);
	przycisk.OffsetBy(75, 0);
	fDalej2 = new BButton( przycisk, "fDalej2", "Dalej", new BMessage(GO_NEXT2));
	fPage2->AddChild(fDalej2);
	fPage2->AddChild(fPBox2);
	AddChild(fPage2);


	/* strona trzecia */
	r = fPBox3->Bounds();
	r.left = 65;
	r.top = 75;
	r.right = r.left + 220;
	r.bottom = r.top + 25;
	// nazwa profilu
	fNazwa = new BTextControl(r, "fNazwa", "Nazwa profilu: ", "Nowy", NULL);
	fNazwa->SetFont(font);
	fNazwa->SetHighColor(0,0,0);
	r.top = r.top + 30;
	r.bottom = r.top + 25;
	fNumer = new BTextControl(r, "fNumer", "Numer: ", "0", NULL);
	fNumer->SetFont(font);
	fNumer->SetHighColor(0,0,0);
	r.top = r.top + 30;
	r.bottom = r.top + 25;
	fHaslo = new BTextControl(r, "fNaslo", "Haslo ", "", NULL);
	fHaslo->TextView()->HideTyping(true);
	fHaslo->SetFont(font);
	fHaslo->SetHighColor(0,0,0);
	fPBox3->AddChild(fNazwa);
	fPBox3->AddChild(fNumer);
	fPBox3->AddChild(fHaslo);
	przycisk = Bounds();
	przycisk.top = przycisk.bottom - 30;
	przycisk.bottom = przycisk.top + 25;
	przycisk.left = przycisk.left + 5;
	przycisk.right = przycisk.left + 60;
	fAnuluj3 = new BButton( przycisk, "fAnuluj3", "Anuluj", new BMessage(GO_CANCEL));
	fPage3->AddChild(fAnuluj3);
	przycisk.OffsetBy(200,0);
	fWroc3 = new BButton( przycisk, "fWroc3", "Wróć", new BMessage(GO_BACK3));
	fPage3->AddChild(fWroc3);
	przycisk.OffsetBy(75, 0);
	fDalej3 = new BButton( przycisk, "fDalej3", "Zakoncz", new BMessage(GO_NEXT3));
	fPage3->AddChild(fDalej3);
	fPage3->AddChild(fPBox3);
	AddChild(fPage3);

	przycisk = Bounds();
	przycisk.top = przycisk.bottom - 30;
	przycisk.bottom = przycisk.top + 25;
	przycisk.left = przycisk.left + 5;
	przycisk.right = przycisk.left + 60;
	fAnuluj4 = new BButton( przycisk, "fAnuluj4", "Anuluj", new BMessage(GO_CANCEL));
	fPage4->AddChild(fAnuluj4);
	przycisk.OffsetBy(200,0);
	fWroc4 = new BButton( przycisk, "fWroc4", "Wróć", new BMessage(GO_BACK4));
	fPage4->AddChild(fWroc4);
	przycisk.OffsetBy(75, 0);
	fDalej4 = new BButton( przycisk, "fDalej4", "Zakończ", new BMessage(GO_NEXT4));
	fPage4->AddChild(fDalej4);
	fPage4->AddChild(fPBox4);
	AddChild(fPage4);
	fPage2->Hide();
	fPage3->Hide();
	fPage4->Hide();
	fDalej1->MakeDefault(true);
}

ProfilWizard::~ProfilWizard()
{
}

void ProfilWizard::MessageReceived(BMessage *mesg)
{
	switch(mesg->what)
	{
		case GO_CANCEL:
		{
			BAlert *alert = new BAlert("Wizard", "Przykro mi ale musisz miec jakis profil, sprobuj jeszcze raz :P", "Ehh :/");
			alert->Go();
			delete alert;
			break;
		}
		case GO_NEXT1:
		{
			fPage1->Hide();
			fPage2->Show();
			fDalej2->MakeDefault(true);
			break;
		}
		case GO_BACK1:
		{
			break;
		}
		case GO_NEXT2:
		{
			fPage2->Hide();
			fPage3->Show();
			fDalej3->MakeDefault(true);
			break;
		}
		case GO_BACK2:
		{
			fPage2->Hide();
			fPage1->Show();
			break;
		}
		case GO_NEXT3:
		{
			if(fNazwa->LockLooper())
			{
				fProfil->fNazwaProfilu->SetTo(fNazwa->Text());
				fNazwa->UnlockLooper();
			}
			if(fNumer->LockLooper())
			{
				fProfil->SetUIN(atoi(fNumer->Text()));
				fNumer->UnlockLooper();
			}
			if(fHaslo->LockLooper())
			{
				fProfil->fHaslo->SetTo(fHaslo->Text());
				fHaslo->UnlockLooper();
			}
			fProfil->Save();
			BMessage *mesg = new BMessage(PROFILE_CREATED);
			mesg->AddString("ProfileName", *fProfil->fNazwaProfilu);
			BMessenger(be_app).SendMessage(mesg);
			delete mesg;
			BWindow::Quit();
			break;
		}
		case GO_BACK3:
		{
			fPage3->Hide();
			fPage2->Show();
			break;
		}
		case GO_NEXT4:
		{
			break;
		}
		case GO_BACK4:
		{
			fPage4->Hide();
			fPage3->Show();
			break;
		}
		default:
			BWindow::MessageReceived(mesg);
			break;
	}
}

void ProfilWizard::Show()
{
	BScreen *screen = new BScreen(this);
	display_mode tryb;
	screen->GetMode(&tryb);
	// teraz centrujemy okienko
	int32 szerokosc = (int32)(PROFILWIZARD_RECT.right - PROFILWIZARD_RECT.left);
	int32 wysokosc = (int32)(PROFILWIZARD_RECT.bottom - PROFILWIZARD_RECT.top); 
	// obliczamy srodek ekranu /2  - 1/2 szerokosci okna
	int32 x_wind = tryb.timing.h_display/2 - (szerokosc/2);
	// obliczamy srodek ekranu /2 - 1/2 wysokosci okna
	int32 y_wind = tryb.timing.v_display/2 - (wysokosc/2);
	MoveTo(x_wind, y_wind);
	BWindow::Show();
}
