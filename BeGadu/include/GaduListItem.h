/*
 * ============================================================================
 *  Nazwa    : GaduListItem z GaduListItem.h
 *  Projekt  : BeGadu
 *  Authorzy : 
 *		Artur Wyszynski <artur.wyszynski@bellstream.pl>
 *  Opis:
 *		Moja implementacja BListItem
 *  Version  : 1.2
 * ============================================================================
 */


#ifndef __BEGADU_GADULISTITEM_H__
#define __BEGADU_GADULISTITEM_H__

#include <ListItem.h>
#include <Resources.h>

/* zewnetrzne klasy, includowane w zrodle */
class BBitmap;
class BMessage;
class BRect;
class BFont;
class BString;

class GaduListItem : public BListItem
{
	public:
		GaduListItem(BString *osoba, int status, BString *opis, BResources *res );
		~GaduListItem();
		void SetIcon(int status);
		virtual void DrawItem(BView *owner, BRect frame, bool complete);
		virtual void Update(BView *owner, const BFont *font);
		BBitmap	*GetBitmap(const char *name);
		BString		*	fNazwa;
		BString		*	fOpis;
		BFont		*	fNazwaFont;
		BFont		*	fOpisFont;
		int 			fStatus;
		BBitmap		*	fIkona;
		BResources	*	fResources;
};

#endif /* __BEGADU_GADULISTITEM_H__ */
