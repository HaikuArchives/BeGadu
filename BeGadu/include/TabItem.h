/*
 * ============================================================================
 *  Nazwa    : TabItem z TabItem.h
 *  Projekt  : BeGadu
 *  Authorzy : 
 *		Artur Wyszynski <artur.wyszynski@bellstream.pl>
 *  Opis:
 *		Tab dla TabView
 *  Version  : 1.2
 * ============================================================================
 */

#ifndef __TABITEM_H__
#define __TABITEM_H__

class BBitmap;
class BMessage;
class BRect;
class BResources;
class BView;

class TabItem
{
	public:
		TabItem();
		TabItem(char *name, uint32 flags, const char *smallIcon, const char *bigIcon, BResources *res , BMessage *message);
		virtual ~TabItem();
		
		void Draw(BView *owner, BRect bounds, bool main);
		
		const char *Name() const
		{
			return fName;
		}
		const BMessage *Message() const
		{
			return fMessage;
		}
		const uint32 Flags() const
		{
			return fFlags;
		}
		const BBitmap *SmallIcon() const
		{
			return fSmallIcon;
		}
		const BBitmap *BigIcon() const
		{
			return fBigIcon;
		}

		BBitmap	*GetBitmap(const char *name);

	private:
		uint32			fFlags;
		char		*	fName;
		BBitmap		*	fSmallIcon;
		BBitmap		*	fBigIcon;
		BMessage	*	fMessage;
		BResources	*	fResources;
};

#endif /* __TABITEM_H__ */
