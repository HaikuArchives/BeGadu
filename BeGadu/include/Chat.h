/*
	Chat.h - chat window for BeGadu
	Code: aljen <aljen@gumisie.org>
	Homepage: http://gadu.beos.pl
*/

#ifndef _CHAT_WINDOW_H
#define _CHAT_WINDOW_H

#include <Window.h>

extern "C" {
#include "libgadu.h"
}

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
		
		Siec			*	fSiec;
		MainWindow		*	fWindow;
		uin_t				fKto;
		BTextControl	*	fPowiedzControl;
		BScrollView		*	fScrollView;
		BTextView		*	fRozmowa;
};

#endif // _CHAT_WINDOW_H