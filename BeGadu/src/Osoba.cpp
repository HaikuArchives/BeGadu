/*
	Osoba.cpp
	Ten plik jest częscią kodu źródłowego BeGadu.
	Homepage: http://gadu.beos.pl
*/

#include <stdlib.h>
#include <string.h>
#include <String.h>
#include <Path.h>
#include <File.h>
#include <FindDirectory.h>
#include <Alert.h>
#include <StorageKit.h>
#include "Osoba.h"
extern "C" {
#include "xmalloc.h"
#include "dynstuff.h"
#include "libgadu.h"
#include "strlcpy.h"
}

#ifndef PATH_MAX
#  define PATH_MAX _POSIX_PATH_MAX
#endif

Osoba::Osoba()
{
	fImie = new BString("");
	fNazwisko = new BString("");
	fNick = new BString("");
	fDisplay = new BString("");
	fTelefon = new BString("");
	fEmail = new BString("");
	fOpis = new BString("");
	fLastDescr = new BString("");
}

Profil::Profil()
{
	fRect.left = 50;
	fRect.top = 50;
	fRect.right = 300;
	fRect.bottom = 350;
	fImie = new BString();
	fImie->SetTo("");
	fNazwisko = new BString();
	fNazwisko->SetTo("");
	fNick = new BString();
	fNick->SetTo("");
	fTelefon = new BString();
	fTelefon->SetTo("");
	fEmail = new BString();
	fEmail->SetTo("");
	fNazwaProfilu = new BString();
	fNazwaProfilu->SetTo("Nowy");
	fNumer = 0;
	fHaslo = new BString();
	fHaslo->SetTo("");
	fUserlista = new Userlist();
	fAutoStatus = GG_STATUS_AVAIL;
	fNeedImport = true;
}

void Profil::SetUIN(uin_t number)
{
	fNumer = number;
}

void Profil::SetPass(BString *pass)
{
	fHaslo = pass;
}

void Profil::SetName(BString *name)
{
	fNazwaProfilu = name;
}

void Profil::SetRect(BRect rect)
{
	fRect = rect;
}

void Profil::Load(BString *profile)
{
	BPath path;
	BMessage *cfgmesg = new BMessage();
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	path.Append("BeGadu/Profiles");
	path.Append(profile->String());
	path.Append("Profil");
	BFile file(path.Path(), B_READ_ONLY);
	if(file.InitCheck() == B_OK)
	{
		cfgmesg->Unflatten(&file);
		file.Unset();
	}
	if( cfgmesg->FindRect("fRect", &fRect) != B_OK )
		fRect =  BRect(50,50,300,350);
	if( cfgmesg->FindInt32("fNumer", (int32*)&fNumer) != B_OK )
		fNumer = 0;
	if( cfgmesg->FindString("fHaslo", fHaslo) != B_OK )
		fHaslo->SetTo("");
	if( cfgmesg->FindInt32("fAutoStatus", (int32*)&fAutoStatus) != B_OK )
		fAutoStatus = GG_STATUS_AVAIL;
	if( cfgmesg->FindString("fImie", fImie) != B_OK )
		fImie->SetTo("");
	if( cfgmesg->FindString("fNazwisko", fNazwisko) != B_OK )
		fNazwisko->SetTo("");
	if( cfgmesg->FindString("fNick", fNick) != B_OK )
		fNick->SetTo("");
	if( cfgmesg->FindString("fNazwaProfilu", fNazwaProfilu) != B_OK )
		fNazwaProfilu->SetTo("");
	if( cfgmesg->FindString("fTelefon", fTelefon) != B_OK )
		fTelefon->SetTo("");
	if( cfgmesg->FindString("fEmail", fEmail) != B_OK )
		fEmail->SetTo("");
	if( cfgmesg->FindBool("fNeedImport", &fNeedImport) != B_OK )
		fNeedImport = true;
	delete cfgmesg;
	fprintf(stderr, "Loading profile from: %s\n", path.Path());
	fUserlista->Read(fNazwaProfilu);
}

void Profil::Save()
{
	BMessage *cfgmesg = new BMessage();
	cfgmesg->AddRect("fRect", fRect );
	cfgmesg->AddInt32("fNumer", fNumer);
	cfgmesg->AddString("fHaslo", *fHaslo);
	cfgmesg->AddInt32("fAutoStatus", fAutoStatus);
	cfgmesg->AddString("fImie", *fImie);
	cfgmesg->AddString("fNazwisko", *fNazwisko);
	cfgmesg->AddString("fNick", *fNick);
	cfgmesg->AddString("fNazwaProfilu", *fNazwaProfilu);
	cfgmesg->AddString("fTelefon", *fTelefon);
	cfgmesg->AddString("fEmail", *fEmail);
	cfgmesg->AddBool("fNeedImport", fNeedImport);
	BPath path;
	BEntry entry;
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	path.Append("BeGadu/Profiles");
	BDirectory * profil = new BDirectory(path.Path());
	if(profil->FindEntry(fNazwaProfilu->String(), &entry) != B_OK)
	{
		path.Append(fNazwaProfilu->String());
		profil->CreateDirectory(path.Path(), profil);
	}
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	path.Append("BeGadu/Profiles");
	path.Append(fNazwaProfilu->String());
	path.Append("Profil");
	BFile file(path.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
	if(file.InitCheck() == B_OK)
	{
		cfgmesg->Flatten(&file);
		file.Unset();
	}
	delete cfgmesg;
	fprintf(stderr, "Saving profile to: %s\n", path.Path());
	fUserlista->Write(fNazwaProfilu);
}


/* Konstruktor userlisty */
Userlist::Userlist()
{	
	/* sadze, że nikt nie ma >512 osób na liscie :P */
	fIsValid = false;
	fLista = new Lista(512);
}

/* Wczytuje userliste z pliku w formacie eksportu z servera */
/* Kod w większości ze źródeł ekg/userlist.[c,h] */
int Userlist::Read(BString *name)
{

	BPath 		path;
	FILE	*	plik;
	char	*	buf;
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	path.Append("BeGadu/Profiles");
	path.Append(name->String());
	path.Append("Userlist");
	
	if(!(plik = fopen(path.Path(), "r")))
		return -1;
	Osoba *osoba = NULL;

	while(buf = ReadLine(plik))
	{
		osoba = new Osoba();
		char **entry, *uin;
		int i, count;
		
		if(buf[0] == '#' || (buf[0] == '/' && buf[1] == '/'))
		{
			xfree(buf);
			continue;
		}
		entry = array_make(buf, ";", 9, 0, 0);
		if((count = array_count(entry)) < 7)
		{
			array_free(entry);
			continue;
		}
		uin = entry[6];
		
		if((strcmp(uin, "") && !(osoba->fUIN = atoi(uin))))
		{
			array_free(entry);
			xfree(buf);
			continue;
		}
		osoba->fImie->SetTo(entry[0]);
		osoba->fNazwisko->SetTo(entry[1]);
		osoba->fNick->SetTo(entry[2]);
		osoba->fDisplay->SetTo(entry[3]);
		osoba->fTelefon->SetTo(entry[4]);
		osoba->fStatus = GG_STATUS_NOT_AVAIL;
		/* mamy mejla ? */
		if(count>7)
			osoba->fEmail->SetTo(entry[7]);
		for(i = 0; i < count; i++)
			xfree(entry[i]);
		fLista->AddItem(osoba);
		xfree(entry);
		xfree(buf);
	}
	SetValid(true);
	fclose(plik);
	return 0;
}

/* Zapisuje userliste do pliku w formacie eksportu z servera */
/* Kod w większości ze źródeł ekg/userlist.[c,h] */
int Userlist::Write(BString *name)
{
	BPath 		path;
	FILE	*	plik;
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	path.Append("BeGadu/Profiles");
	path.Append(name->String());
	path.Append("Userlist");
	char *	contacts, tmp[PATH_MAX];
	
	if(!(contacts = Dump()))
		return -1;
	snprintf(tmp, sizeof(tmp), "%s.%d.%ld", path.Path(), (int)getpid(), (long)time(NULL));
	if(!(plik = fopen(tmp, "w")))
	{
		xfree(contacts);
		return -2;
	}
	fputs(contacts, plik);
	if(fclose(plik) == EOF)
	{
		unlink(tmp);
		return -2;
	}
	xfree(contacts);
	if(rename(tmp,path.Path()) == -1)
		return -2;
	return 0;
}

/* Czyści status danej osobie */
/* Kod w większości ze źródeł ekg/userlist.[c,h] */
void Userlist::ClearStatus(Osoba *osoba)
{
	Osoba *o = NULL;

	for(int i = 0; i < fLista->CountItems(); i++)
	{
			o = (Osoba*) fLista->ItemAt(i);
			if(!osoba->fUIN || osoba->fUIN == o->fUIN)
			{
				o->fStatus = GG_STATUS_NOT_AVAIL;
				memset(&o->fIP, 0, sizeof(struct in_addr));
				memset(&o->fLastIP, 0, sizeof(struct in_addr));
				o->fPort = 0;
				o->fLastPort = 0;
				o->fOpis->SetTo("");
				o->fLastDescr->SetTo("");
				break;
			}
	}				
}

/* Dodaje osobę o numerze i ustawia jej nazwę wyświetlania */
/* Kod w większości ze źródeł ekg/userlist.[c,h] */
void Userlist::Add(uin_t nr, BString *Imie, BString *Nazwisko, BString *Nick, BString *Disp, BString *Telefon, BString *Email)
{
	Osoba *o = new Osoba();
	o->fUIN = nr;
	o->fImie = Imie;
	o->fNazwisko = Nazwisko;
	o->fNick = Nick;
	o->fDisplay = Disp;
	o->fTelefon = Telefon;
	o->fStatus = GG_STATUS_NOT_AVAIL;
	o->fEmail = Email;
	fLista->AddItem(o);
}

/* Usuwa daną osobę z listy */
/* Kod w większości ze źródeł ekg/userlist.[c,h] */
void Userlist::Remove(Osoba *osoba)
{
	Osoba *o = NULL;

	for(int i = 0; i < fLista->CountItems(); i++)
	{
		o = (Osoba*) fLista->ItemAt(i);
		if(o->fUIN == osoba->fUIN)
		{
			fLista->RemoveItem(o);
			break;
		}
	}
}

/* Podmienia osobę na liście */
/* Kod w większości ze źródeł ekg/userlist.[c,h] */
void Userlist::Replace(Osoba *osoba)
{
}

/* Wysyła listę na server */
/* Kod w większości ze źródeł ekg/userlist.[c,h] */
void Userlist::Send(struct gg_session *Sesja)
{
	Osoba 	*	o = NULL;
	uin_t 	*	numerki;
	int			count;
	count = fLista->CountItems();
	numerki = (uin_t*)malloc(count * sizeof(uin_t));
	for(int i = 0; i < fLista->CountItems(); i++)
	{
		o = (Osoba*) fLista->ItemAt(i);		
		numerki[i] = o->fUIN;
	}
	gg_notify(Sesja, numerki, count);
}

/* Szuka osoby po numerze i nazwie wyświetlanej */
/* Kod w większości ze źródeł ekg/userlist.[c,h] */
Osoba *	Userlist::Find(uin_t UIN)
{
	Osoba *o = NULL;
	for(int i = 0; i < fLista->CountItems(); i++)
	{
		o = (Osoba*) fLista->ItemAt(i);
		if(UIN && o->fUIN == UIN)
			return o;
	}
	return NULL;
}

/* Szuka osoby po numerze telefonu */
/* Kod w większości ze źródeł ekg/userlist.[c,h] */
Osoba * Userlist::FindMobile(const char *telefon)
{
	Osoba *o = NULL;
	for(int i = 0; i < fLista->CountItems(); i++)
	{
		o = (Osoba*) fLista->ItemAt(i);
		if(telefon && o->fTelefon && !strcasecmp(o->fTelefon->String(), telefon))
			return o;
	}
	return NULL;
}

/* Zrzuca listę w postaci tekstowej */
char * Userlist::Dump()
{
	string_t s = string_init(NULL);
	Osoba *o = NULL;
	char *line;
	for(int i = 0; i < fLista->CountItems(); i++)
	{
		o = (Osoba*) fLista->ItemAt(i);
		line = saprintf( "%s;%s;%s;%s;%s;%s;%s;%s%s\r\n",
						 o->fImie->String(),
						 o->fNazwisko->String(),
						 o->fNick->String(),
						 o->fDisplay->String(),
						 o->fTelefon->String(),
						 strdup(""),
						 itoa(o->fUIN),
						 o->fEmail->String(),
						 strdup(""));
		string_append(s, line);
		xfree(line);

	}
	return string_free(s, 0);
}

/* Czyści listę */
void Userlist::Clear()
{
	Osoba *o = NULL;
	for(int i = 0; i < fLista->CountItems(); i++)
	{
		o = (Osoba*) fLista->ItemAt(i);
		fLista->RemoveItem(o);
	}
}

/* Ustawia listę */
/* Kod w większości ze źródeł ekg/userlist.[c,h] */
int	* Userlist::Set(const char *contacts)
{
	fprintf(stderr, "Userlist::Set started\n");
	char *buf, *cont, *contsave;
	char **entry, *uin;
	Clear();
	
	cont = contsave = strdup(contacts);
	while((buf = gg_get_line(&cont)))
	{
		Osoba *o = new Osoba();
		int i, count;

		if (buf[0] == '#' || (buf[0] == '/' && buf[1] == '/'))
			continue;
		
		entry = array_make(buf, ";", 8, 0, 0);
		if((count = array_count(entry) < 7))
		{
			array_free(entry);
			continue;
		}
		o->fUIN = atoi(entry[6]);
		o->fImie->SetTo(entry[0]);
		o->fNazwisko->SetTo(entry[1]);
		o->fNick->SetTo(entry[2]);
		o->fDisplay->SetTo(entry[3]);
		o->fTelefon->SetTo(entry[4]);
		o->fStatus = GG_STATUS_NOT_AVAIL;
		if(count > 7)
			o->fEmail->SetTo(entry[7]);
		Add(o->fUIN, o->fImie, o->fNazwisko, o->fNick, o->fDisplay, o->fTelefon, o->fEmail);
		array_free(entry);
		delete o;
	}
	fIsValid = true;
//	Dump();
}

/* Formatuje */
/* Kod w większości ze źródeł ekg/userlist.[c,h] */
const char * Userlist::FormatUser(uin_t UIN)
{
}

/* Wczytuje/zwraca linijkę tekstu z pliku */
/* Kod w większości ze źródeł ekg */
char * Userlist::ReadLine(FILE *f)
{
	char buf[1024], *res = NULL;

	while (fgets(buf, sizeof(buf), f)) {
		int first = (res) ? 0 : 1;
		size_t new_size = ((res) ? strlen(res) : 0) + strlen(buf) + 1;

		res = (char*)xrealloc(res, new_size);
		if (first)
			*res = 0;
		strlcpy(res + strlen(res), buf, new_size - strlen(res));

		if (strchr(buf, '\n'))
			break;
	}

	if (res && strlen(res) > 0 && res[strlen(res) - 1] == '\n')
		res[strlen(res) - 1] = 0;
	if (res && strlen(res) > 0 && res[strlen(res) - 1] == '\r')
		res[strlen(res) - 1] = 0;

	return res;
}

bool Userlist::IsValid()
{
	return fIsValid;
}

void Userlist::SetValid(bool valid)
{
	fIsValid = valid;
}

void Userlist::Import(struct gg_session *fSesja, Lista *lista)
{
	char *blad;
	SetValid(false);
	if(gg_userlist_request(fSesja, GG_USERLIST_GET, NULL) == -1)
	{
		sprintf(blad, "Błąd pobierania listy kontaktów: %s", strerror(errno));
		BAlert *alert = new BAlert("Lista", blad, "OK");
		alert->Go();
	}
	else
	{
		Osoba *osoba;
		for(int i = 0; i < lista->CountItems(); i++)
		{
			osoba = (Osoba*) lista->ItemAt(i);
			lista->RemoveItem(osoba);
			delete osoba;
		}
	}
	fprintf(stderr,"import exit\n");
}

void Userlist::Export(struct gg_session *fSesja, Lista *lista)
{
}

Lista::Lista(int rozmiar) : BList(rozmiar)
{
}

bool Lista::IsValid()
{
	return fIsValid;
}

void Lista::SetValid(bool isValid)
{
	fIsValid = isValid;
}
