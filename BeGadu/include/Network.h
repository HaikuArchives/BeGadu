/*
 * ============================================================================
 *  Nazwa    : Siec z Siec.h
 *  Projekt  : BeGadu
 *  Authorzy : 
 *		Artur Wyszynski <artur.wyszynski@bellstream.pl>
 *  Opis:
 *		Glowny modul do obslugi sieci
 *  Version  : 1.2
 * ============================================================================
 */

#ifndef __BEGADU_SIEC_H__
#define __BEGADU_SIEC_H__

#include <Looper.h>
#include <libgadu.h>

/* zewnetrzne klasy, includowane w zrodle */
class MainWindow;
class ChatWindow;
class Lista;
class Osoba;
class Profil;

class Siec : public BLooper
{
	public:
		Siec(Profil *profil, Lista *lista);
		virtual void Quit();
		virtual void MessageReceived(BMessage *message);
		void GotWindow(MainWindow *window);
		int GetIdent()
			{ return fIdent; }
		int GetStatus()
			{ return fStatus; }
		/* funkcje wywoływane z interfejsu */
		void Login();
		void Login(int status);
		void Logout();
		void AddPerson(uin_t kogo);
		void RemovePerson(uin_t kogo);
		void GotMsg(uin_t odkogo, const char *msg);
		void SendMsg(uin_t komu, const char *msg);


		struct gg_session 	*	fSesja;
		struct gg_login_params	fLoginParam;
		struct gg_event		*	fZdarzenie;
		MainWindow			*	fWindow;
		int						fStatus;

	
	private:
		/* klasy globalne */
		Profil		*fProfil;
		Lista		*fLista;
		
		/* klasy lokalne */
		BList 			*fHandlerList;
		BList			*fWinList;
		
		/* informacje o połączeniu */
		int						fId;
		int						fIdent;
		
		/* dodatki */
		ChatWindow	*GetMesgWinForUser(uin_t kto);
		Osoba		*GetOsobaForUser(uin_t kto);
		
		/* te funkcje robią to co libgadu chce by było zrobione */
		void AddHandler(int fd, int cond, void *data);
		void RemoveHandler(int fd);		

};

#endif /* __BEGADU_SIEC_H__ */
