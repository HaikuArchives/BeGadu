/*
 * ============================================================================
 *  Nazwa    : TabsMgr z TabsMgr.h
 *  Projekt  : BeGadu
 *  Authorzy : 
 *		Artur Wyszynski <artur.wyszynski@bellstream.pl>
 *  Opis:
 *		Messenger dla Tabow
 *  Version  : 1.2
 * ============================================================================
 */

#ifndef __TABSMGR_H__
#define __TABSBGR_H__

class BList;
class BMessage;
class BPoint;


class TabsMgr : public BHandler
{
	public:
		TabsMgr();
};

#endif /* __TABSMGR_H__ */
