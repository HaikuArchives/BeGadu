/*
 * ============================================================================
 *  Nazwa    : Chat z Chat.h
 *  Projekt  : BeGadu
 *  Authorzy : 
 *		Artur Wyszynski <artur.wyszynski@bellstream.pl>
 *  Opis:
 *		Klasa okienka z czatem
 *  Version  : 1.2
 * ============================================================================
 */

#ifndef __BEGADU_CHATWINDOW_H__
#define __BEGADU_CHATWINDOW_H__

#include <Window.h>
#include <libgadu.h>

/* zewnetrzne klasy, includowane w zrodle */
class Siec;
class MainWindow;
class Osoba;
class BTextControl;
class BScrollView;
class BTextView;

class ChatWindow : public BWindow
{
	public:
		ChatWindow(Siec *siec, MainWindow *window, uin_t kto);
		virtual void MessageReceived(BMessage *message);
		virtual bool QuitRequested();
		virtual void FrameResized(float, float);
		virtual void Show();
		virtual void WindowActivated(bool activated);
		
		Siec			*	fSiec;
		MainWindow		*	fWindow;
		uin_t				fKto;
		BTextControl	*	fPowiedzControl;
		BScrollView		*	fScrollView;
		BTextView		*	fRozmowa;
};

#endif // __BEGADU_CHATWINDOW_H__
