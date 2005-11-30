/*
 * ============================================================================
 *  Name     : GaduListItem from GaduListItem.h
 *  Project  : BeGadu
 *  Authors	 : 
 *		Artur Wyszynski <artur.wyszynski@blstream.pl>
 *  Description:
 *		My version of BListItem
 *  Version  : 1.2
 * ============================================================================
 */

#ifndef __BEGADU_GADULISTITEM_H__
#define __BEGADU_GADULISTITEM_H__

#include <ListItem.h>
#include <Resources.h>

/* external classes, included in src */
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
		const BString GetName() const;
		const BString GetDescription() const;
		int	GetState();

	private:
		BBitmap	* GetBitmap( const char *aName );
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
