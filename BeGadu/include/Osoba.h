/*
 * ============================================================================
 *  Nazwa    : Osoba z Osoba.h
 *  Projekt  : BeGadu
 *  Authorzy : 
 *		Artur Wyszynski <artur.wyszynski@bellstream.pl>
 *  Opis:
 *		Klasy zwiazane z obsluga userlist i osob
 *  Version  : 1.2
 * ============================================================================
 */

#ifndef __BEGADU_OSOBA_H__
#define __BEGADU_OSOBA_H__

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

#include <libgadu.h>

/* klasa pojedyńczej osoby */
class Osoba
{
	public:
							Osoba();
		BString			*	fImie;					/* Imię */
		BString			*	fNazwisko;				/* Nazwisko */
		BString			*	fNick;					/* Pseudonim */
		BString			*	fDisplay;				/* Wyświetl jako */
		BString			*	fTelefon;				/* Telefon */
		BString			*	fEmail;					/* E-Mail */
		uin_t				fUIN;					/* Numer GG */
		int					fStatus;				/* Aktualny status */
		BString			*	fOpis;					/* Aktualny opis */
		struct in_addr		fIP;					/* Adres IP osoby */
		unsigned short		fPort;					/* Port osoby */
		int					fProtokol;				/* Wersja protokołu */
		time_t				fLastSeen;				/* Jeśli jest niedost./ukryty, to od kiedy */
		BString			*	fLastDescr;				/* J.w. ostatni opis */
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
		int				Read(BString *name);			 /* Ładuje userliste z pliku */
		int				Write(BString *name);			 /* Zapisuje ją do pliku */
		void			ClearStatus(Osoba *osoba);		 /* Czyści status danej osobie */
		void			Add(uin_t nr, BString *Imie, BString *Nazwisko, BString *Nick, BString *Disp, BString *Telefon, BString *Email); 
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
								Profil();
		void					Load(BString *profile);
		void					Save();
		void					SetUIN(uin_t number);
		void					SetPass(BString *password);
		void					SetName(BString *name);
		void 					SetRect(BRect rect);
		void					SetDesc(BString *description);

		Userlist			*	fUserlista;
		BRect					fRect;
		uin_t					fNumer;
		BString				*	fHaslo;
		int						fAutoStatus;
		BString				*	fImie;
		BString				*	fNazwisko;
		BString				*	fNick;
		BString				*	fNazwaProfilu;
		BString				*	fTelefon;
		BString				*	fEmail;
		BString				*	fOpis;
		bool					fNeedImport;
};


#endif /* __BEGADU_OSOBA_H__ */
