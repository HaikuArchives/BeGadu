/*
	Debug.h
	Code: aljen <aljen@gumisie.org>
	Homepage: http://gadu.beos.pl
*/

#ifndef _DEBUG_WINDOW_H
#define _DEBUG_WINDOW_H

#include <Window.h>
#include <TextView.h>

class DebugWindow : public BWindow
{
	private:
		BTextView	*dview;
	public:
		DebugWindow();
		void AddLog(char *entry);
		virtual void MessageReceived(BMessage *message);
};

#endif // _DEBUG_WINDOW_H