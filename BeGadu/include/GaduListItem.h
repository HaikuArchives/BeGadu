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
		GaduListItem( BString *aName,
					  int aStatus,
					  BString *aDescription,
					  BResources *aResources );
		~GaduListItem();
		void SetIcon( int aStatus );
		virtual void DrawItem( BView *aOwner, BRect aFrame, bool aComplete );
		virtual void Update( BView *aOwner, const BFont *aFont );
		const BString getName() const;
		const BString getDescription() const;
		int	getState();
	private:
		BBitmap	* getBitmap( const char *aName );
		BString	* iName;
		BString	* iDescription;
		BFont *	iNameFont;
		BFont *	iDescriptionFont;
		int iStatus;
		BBitmap	* iIcon;
		BResources *	iResources;
		color_map iColorMap;
};

#endif /* __BEGADU_GADULISTITEM_H__ */
