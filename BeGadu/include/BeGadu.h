/*
 * ============================================================================
 *  Nazwa    : BeGadu z BeGadu.h
 *  Projekt  : BeGadu
 *  Authorzy : 
 *		Artur Wyszynski <artur.wyszynski@bellstream.pl>
 *  Opis:
 *		Glowny modul
 *  Version  : 1.2
 * ============================================================================
 */

#ifndef __BEGADU_H__
#define __BEGADU_H__

#include <Application.h>

/* zewnetrzne klasy, includowane w zrodle */
class DebugWindow;
class MainWindow;

class BeGadu : public BApplication
{
	public:
		BeGadu();
		virtual bool QuitRequested();
		virtual void MessageReceived(BMessage *message);
		virtual void ReadyToRun();
		void AddDeskbarIcon();
		void DelDeskbarIcon();

	private:
		MainWindow  	*	mWindow;
		DebugWindow 	*	dWindow;
		bool				fFirstRun;
		bool				fHideAtStart;
		BString			*	fLastProfile;
};

#endif /* __BEGADU_H__ */
