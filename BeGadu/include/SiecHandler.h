/*
	SiecHandler.h
	Ten plik jest częscią kodu źródłowego BeGadu.
	Homepage: http://gadu.beos.pl
*/

#ifndef _BEGADU_HANDLER_H
#define _BEGADU_HANDLER_H


#include "Main.h"

class Siec;

class SiecHandler
{
	public:
		SiecHandler(Siec *siec, int id, int fd, int cond, void *data);
		void Run();
		void Stop();
		
		int				fId;
		int				fFd;
		int				fCond;
		void 		*	fData;
		volatile bool 	fDie;
		Siec		*	fSiec;
	
	private:
		volatile int	fThreadID;
};

#endif // _BEGADU_HANDLER_H
