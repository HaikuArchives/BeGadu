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

#include <Bitmap.h>
#include <Message.h>
#include <Rect.h>
#include <Resources.h>
#include <View.h>
#include <stdlib.h>
#include "TabItem.h"

TabItem::TabItem()
	:	fFlags(0),
		fName(NULL),
		fSmallIcon(NULL),
		fBigIcon(NULL),
		fMessage(NULL),
		fResources(NULL)
{
	/* empty */
}

TabItem::TabItem( char *name,
				  uint32 flags,
				  const char *smallIcon,
				  const char *bigIcon,
				  BResources *res,
				  BMessage *message )
	:	fFlags(flags),
		fName(name),
		fSmallIcon(NULL),
		fBigIcon(NULL),
		fMessage(message),
		fResources(res)
{
	fSmallIcon = GetBitmap(smallIcon);
	fBigIcon = GetBitmap(bigIcon);
}

TabItem::~TabItem()
{
	free(fName);
	delete fSmallIcon;
	delete fBigIcon;
}

void TabItem::Draw(BView *owner, BRect bounds, bool main)
{
	BRect rect;
	if(main)
	{
		rect = fBigIcon->Bounds();
		rect.OffsetTo(bounds.LeftTop());
		rect.OffsetBy(2,2);
		owner->DrawBitmap(fBigIcon, rect);
	}
	else
	{
		rect = fSmallIcon->Bounds();
		rect.OffsetTo(bounds.LeftTop());
		rect.OffsetBy(10,10);
		owner->DrawBitmap(fSmallIcon, rect);
	}
}

BBitmap *TabItem::GetBitmap(const char *name)
{
	BBitmap 	*bitmap = NULL;
	size_t 		len = 0;
	status_t 	error;	

	const void *data = fResources->LoadResource('BBMP', name, &len);

	BMemoryIO stream(data, len);
	
	BMessage archive;
	error = archive.Unflatten(&stream);
	if (error != B_OK)
		return NULL;
	bitmap = new BBitmap(&archive);
	if(!bitmap)
		return NULL;

	if(bitmap->InitCheck() != B_OK)
	{
		delete bitmap;
		return NULL;
	}
	
	return bitmap;
}
