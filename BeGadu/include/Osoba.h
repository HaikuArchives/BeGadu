/*
	Osoba.h
	Ten plik jest częscią kodu źródłowego BeGadu.
	Homepage: http://gadu.beos.pl
*/

#ifndef _OSOBA_H
#define _OSOBA_H

#include <List.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
extern "C" {
#include "libgadu.h"
}

/* klasa pojedyńczej osoby */
class Osoba
{
	public:
							Osoba();
		char			*	fImie;					/* Imię */
		char			*	fNazwisko;				/* Nazwisko */
		char			*	fNick;					/* Pseudonim */
		char			*	fDisplay;				/* Wyświetl jako */
		char			*	fTelefon;				/* Telefon */
		char			*	fEmail;					/* E-Mail */
		uin_t				fUIN;					/* Numer GG */
		int					fStatus;				/* Aktualny status */
		char			*	fOpis;					/* Aktualny opis */
		struct in_addr		fIP;					/* Adres IP osoby */
		unsigned short		fPort;					/* Port osoby */
		int					fProtokol;				/* Wersja protokołu */
		time_t				fLastSeen;				/* Jeśli jest niedost./ukryty, to od kiedy */
		char			*	fLastDescr;				/* J.w. ostatni opis */
		struct in_addr		fLastIP;				/* J.w. ostatni IP */
		unsigned short		fLastPort;				/* J.w. ostatni port */
		int 				fImgSize;				/* Maksymalny rozmiar obrazków */
};

class Lista : public BList
{
	public:
		Lista(int rozmiar);
		bool IsValid();
		void SetValid(bool isValid);
	
	private:
		bool fIsValid;
};

/* Klasa do obsługi userlisty */

class Userlist
{
	public:
						Userlist();						 /* Konstruktor */
		int				Read();							 /* Ładuje userliste z pliku */
		int				Write();						 /* Zapisuje ją do pliku */
		void			ClearStatus(Osoba *osoba);		 /* Czyści status danej osobie */
		void			Add(uin_t nr, const char *Imie, const char *Nazwisko, const char *Nick, const char *Disp, const char *Telefon, const char *Email); 
		void			Remove(Osoba *osoba);			 /* Usuwa osobę */
		void			Replace(Osoba *osoba);			 /* Podmienia osobę */
		void			Import(struct gg_session *fSesja, Lista *lista);
														 /* Importuje z servera */
		void			Export(struct gg_session *fSesja, Lista *lista);
														 /* Eksportuje na server */
		void			Send(struct gg_session *Sesja);	 /* Wysyła liste na server */
		bool			IsValid();
		void			SetValid(bool valid);
		Osoba		*	Find(uin_t nr);					 /* Szuka osoby */
		Osoba		*	FindMobile(const char *tel);	 /* Szuka osoby po telefonie */
		char		*	Dump();							 /* Zrzuca */
		void			Clear();						 /* Czyści */
		int			*	Set(const char *contacts); 		 /* Ustawia */
		const char 	*	FormatUser(uin_t UIN);			 /* Formatuje */
		char 		*	ReadLine(FILE *f);				 /* Wczytuje/zwraca linijke tekstu z pliku */
		Lista		*	fLista;
		bool			fIsValid;
};

class Profil
{
	public:
								Profil(char *nazwa);
								Profil();
		void					Load();
		void					Save();
		
		BMessage			*	fCfgMsg;
		BRect					fRect;
		uin_t					fNumer;
		const char			*	fHaslo;
		int						fAutoStatus;
		const char			*	fImie;
		const char			*	fNazwisko;
		const char			*	fNick;
		const char			*	fNazwaProfilu;
		const char			*	fTelefon;
		const char			*	fEmail;
		Userlist			*	fUserlista;
};


#endif // _OSOBA_H
