/*
 * ============================================================================
 *  Nazwa    : TabsBox z TabsBox.h
 *  Projekt  : BeGadu
 *  Authorzy : 
 *		Artur Wyszynski <artur.wyszynski@bellstream.pl>
 *  Opis:
 *		Slider box :P
 *  Version  : 1.2
 * ============================================================================
 */

#ifndef __TABSBOX_H__
#define __TABSBOX_H__

#include <Box.h>

class BList;
class BMessage;
class BPoint;


class TabsBox : public BBox
{
	public:
		TabsBox(BRect bounds, );
};

#endif /* __TABSBOX_H__ */
