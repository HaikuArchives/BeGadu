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
}

Profil::Profil(char *nazwa)
{
	fCfgMsg = new BMessage();
	fCfgMsg->AddRect("fRect", BRect(50,50,300,350));
	fCfgMsg->AddInt32("fNumer", 0);
	fCfgMsg->AddString("fHaslo", "");
	fCfgMsg->AddInt32("fAutoStatus", GG_STATUS_AVAIL);
	fCfgMsg->AddString("fImie", "");
	fCfgMsg->AddString("fNazwisko", "");
	fCfgMsg->AddString("fNick", "");
	fCfgMsg->AddString("fNazwaProfilu", "Nowy");
	fCfgMsg->AddString("fTelefon", "");
	fCfgMsg->AddString("fEmail", "");
	fNazwaProfilu = nazwa;
	fUserlista = new Userlist();
	Load();
}

Profil::Profil()
{
	fCfgMsg = new BMessage();
	fCfgMsg->AddRect("fRect", BRect(50,50,300,350));
	fCfgMsg->AddInt32("fNumer", 0);
	fCfgMsg->AddString("fHaslo", "");
	fCfgMsg->AddInt32("fAutoStatus", GG_STATUS_AVAIL);
	fCfgMsg->AddString("fImie", "");
	fCfgMsg->AddString("fNazwisko", "");
	fCfgMsg->AddString("fNick", "");
	fCfgMsg->AddString("fNazwaProfilu", "Nowy");
	fCfgMsg->AddString("fTelefon", "");
	fCfgMsg->AddString("fEmail", "");
	fNazwaProfilu = "Nowy";
	fUserlista = new Userlist();
	Load();
}

void Profil::Load()
{
	BPath path;
	BString plik = "BeGadu.";
	plik << fNazwaProfilu;
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	path.Append(plik.String());
	BFile file(path.Path(), B_READ_ONLY);
	if(file.InitCheck() == B_OK)
	{
		fCfgMsg->Unflatten(&file);
		file.Unset();
	}
	if( fCfgMsg->FindRect("fRect", &fRect) != B_OK )
		fRect =  BRect(50,50,300,350);
	if( fCfgMsg->FindInt32("fNumer", (int32*)&fNumer) != B_OK )
		fNumer = 0;
	if( fCfgMsg->FindString("fHaslo", &fHaslo) != B_OK )
		fHaslo = "";
	if( fCfgMsg->FindInt32("fAutoStatus", (int32*)&fAutoStatus) != B_OK )
		fAutoStatus = GG_STATUS_AVAIL;
	if( fCfgMsg->FindString("fImie", &fImie) != B_OK )
		fImie = "";
	if( fCfgMsg->FindString("fNazwisko", &fNazwisko) != B_OK )
		fNazwisko = "";
	if( fCfgMsg->FindString("fNick", &fNick) != B_OK )
		fNick = "";
	if( fCfgMsg->FindString("fNazwaProfilu", &fNazwaProfilu) != B_OK )
		fNazwaProfilu = "";
	if( fCfgMsg->FindString("fTelefon", &fTelefon) != B_OK )
		fTelefon = "";
	if( fCfgMsg->FindString("fEmail", &fEmail) != B_OK )
		fEmail = "";
	fUserlista->Read();
}

void Profil::Save()
{
	fCfgMsg->ReplaceRect("fRect", fRect );
	fCfgMsg->ReplaceInt32("fNumer", fNumer);
	fCfgMsg->ReplaceString("fHaslo", fHaslo);
	fCfgMsg->ReplaceInt32("fAutoStatus", fAutoStatus);
	fCfgMsg->ReplaceString("fImie", fImie);
	fCfgMsg->ReplaceString("fNazwisko", fNazwisko);
	fCfgMsg->ReplaceString("fNick", fNick);
	fCfgMsg->ReplaceString("fNazwaProfilu", fNazwaProfilu);
	fCfgMsg->ReplaceString("fTelefon", fTelefon);
	fCfgMsg->ReplaceString("fEmail", fEmail);

	BPath path;
	BString plik = "BeGadu.";
	plik << fNazwaProfilu;
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	path.Append(plik.String());
	BFile file(path.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
	if(file.InitCheck() == B_OK)
	{
		fCfgMsg->Flatten(&file);
		file.Unset();
	}
	fUserlista->Write();
}

/* Konstruktor userlisty */
Userlist::Userlist()
{	
	/* sądze, że nikt nie ma >512 osób na liście :P */
	fIsValid = false;
	fLista = new Lista(512);
}

/* Wczytuje userliste z pliku w formacie eksportu z servera */
/* Kod w większości ze źródeł ekg/userlist.[c,h] */
int Userlist::Read()
{
	BPath 		path;
	FILE	*	plik;
	char	*	buf;
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	path.Append("BeGadu.userlist");
	
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
		osoba->fImie = strdup(entry[0]);
		osoba->fNazwisko = strdup(entry[1]);
		osoba->fNick = strdup(entry[2]);
		osoba->fDisplay = strdup(entry[3]);
		osoba->fTelefon = strdup(entry[4]);
		osoba->fStatus = GG_STATUS_NOT_AVAIL;
		/* mamy mejla ? */
		if(count>7)
			osoba->fEmail = xstrdup(entry[7]);
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
int Userlist::Write()
{
	BPath 		path;
	FILE	*	plik;
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	path.Append("BeGadu.userlist");
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
				free(o->fOpis);
				free(o->fLastDescr);
				o->fOpis = NULL;
				o->fLastDescr = NULL;
				break;
			}
	}				
}

/* Dodaje osobę o numerze i ustawia jej nazwę wyświetlania */
/* Kod w większości ze źródeł ekg/userlist.[c,h] */
void Userlist::Add(uin_t nr, const char *Imie, const char *Nazwisko, const char *Nick, const char *Disp, const char *Telefon, const char *Email)
{
	Osoba *o = new Osoba();
	o->fUIN = nr;
	o->fImie = strdup(Imie);
	o->fNazwisko = strdup(Nazwisko);
	o->fNick = strdup(Nick);
	o->fDisplay = strdup(Disp);
	o->fTelefon = strdup(Telefon);
	o->fStatus = GG_STATUS_NOT_AVAIL;
	o->fEmail = strdup(Email);
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
		if(telefon && o->fTelefon && !strcasecmp(o->fTelefon, telefon))
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
						 o->fImie,
						 o->fNazwisko,
						 o->fNick,
						 o->fDisplay,
						 o->fTelefon,
						 strdup(""),
						 itoa(o->fUIN),
						 o->fEmail,
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
		o->fImie = entry[0];
		o->fNazwisko = strdup(entry[1]);
		o->fNick = strdup(entry[2]);
		o->fDisplay = strdup(entry[3]);
		o->fTelefon = strdup(entry[4]);
		o->fStatus = GG_STATUS_NOT_AVAIL;
		if(count > 7)
			o->fEmail = strdup(entry[7]);
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
