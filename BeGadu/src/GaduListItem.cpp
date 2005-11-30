#include <stdio.h>
#include <stdlib.h>
#include <Message.h>
#include <ListItem.h>
#include <Bitmap.h> 
#include <Rect.h>
#include <Font.h>
#include <String.h>
#include <View.h>
#include <Roster.h>
#include <Resources.h>
#include <File.h>
#include <DataIO.h>
#include "GaduListItem.h"
#include "GfxStuff.h"
extern "C" {
#include "libgadu.h"
}

GaduListItem::GaduListItem( BString *aName,
							int aStatus,
							BString *aDescription,
							BResources *aResources) : BListItem()
	{
	iResources = aResources;
	iStatus = aStatus;
	SetIcon( iStatus );
	iName = aName;
	iDescription = aDescription;
	iNameFont = new BFont( be_plain_font );
	iNameFont->SetSize( 15.0 );
	iDescriptionFont = new BFont( be_plain_font );
	iDescriptionFont->SetSize( 10.0 );
	float woi = iNameFont->StringWidth( iName->String() );
	SetWidth( woi );
	SetHeight( 35.0 );
	}

GaduListItem::~GaduListItem()
	{
	if( iIcon )
		delete iIcon;
		iIcon = NULL;
	}

void GaduListItem::SetIcon( int aStatus )
	{
	switch( aStatus )
		{
		case GG_STATUS_NOT_AVAIL:
			{
			iIcon = getBitmap( "NotAvail.png" );
			break;
			}
		case GG_STATUS_NOT_AVAIL_DESCR:
			{
			iIcon = getBitmap( "NotAvailDescr.png" );
			break;
			}
		case GG_STATUS_INVISIBLE:
			{
			iIcon = getBitmap( "Invisible.png" );
			break;
			}
		case GG_STATUS_INVISIBLE_DESCR:
			{
			iIcon = getBitmap( "InvisibleDescr.png" );
			break;
			}
		case GG_STATUS_BUSY:
			{
			iIcon = getBitmap( "Busy.png" );
			break;
			}
		case GG_STATUS_BUSY_DESCR:
			{
			iIcon = getBitmap( "BusyDescr.png" );
			break;
			}
		case GG_STATUS_AVAIL:
			{
			iIcon = getBitmap( "Avail.png" );
			break;
			}
		case GG_STATUS_AVAIL_DESCR:
			{
			iIcon = getBitmap( "AvailDescr.png" );
			break;
			}
		}
	}

void GaduListItem::DrawItem( BView *aOwner, BRect aFrame, bool aComplete )
	{
	rgb_color color;
	rgb_color color_name;
	rgb_color color_desc;

	if( IsSelected() || aComplete )
		{
		if( IsSelected() )
			{
			color.red = color.green = color.blue = 100;
			}
		else
			{
			color = aOwner->ViewColor();
			}
		}
	else
		{
		color = aOwner->ViewColor();
		}

	aOwner->SetHighColor( color );
	aOwner->FillRect( aFrame );

	if( iIcon )
		{
		aFrame.left += 2;
		aFrame.top += 8;
		aFrame.bottom = aFrame.top + 20;
		BRect r( aFrame.left,
				 aFrame.top,
				 aFrame.left + 18,
				 aFrame.top + 16 );
		aOwner->SetDrawingMode( B_OP_ALPHA );
		aOwner->SetLowColor( 255, 255, 255, 0 );
		aOwner->DrawBitmap( iIcon, r );
		}

	if( IsSelected() )
		{
		color_name.red = color_name.blue = color_name.green = color_name.alpha = 255;
		color_desc.red = color_desc.blue = color_desc.green = color_desc.alpha = 175;
		}
	else
		{
		color_name.red = color_name.blue = color_name.green = color_name.alpha = 0;
		color_desc.red = color_desc.blue = color_desc.green = color_desc.alpha = 50;
		}

	aOwner->SetDrawingMode( B_OP_OVER );
	// aOwner->MovePenTo( aFrame.left + 21, ( ( ( aFrame.bottom + aFrame.top ) / 2.0 ) ) +4.0 );
	aOwner->SetFont( iNameFont );
	aOwner->MovePenTo( aFrame.left + 21, aFrame.top + 9 );
	aOwner->SetHighColor( color_name );
	aOwner->DrawString( iName->String() );
	aOwner->SetFont( iDescriptionFont );
	aOwner->SetHighColor( color_desc );
	aOwner->MovePenTo( aFrame.left + 21, aFrame.top + 21 );
	aOwner->DrawString( iDescription->String() );	
	}

void GaduListItem::Update(BView *aOwner, const BFont *aFont)
	{
	BListItem::Update( aOwner, aFont);
	SetHeight( 35.0 );
	}

const BString GaduListItem::getName() const
	{
	return *iName;
	}

const BString GaduListItem::getDescription() const
	{
	return *iDescription;
	}

int GaduListItem::getState()
	{
	return iStatus;
	}

BBitmap* GaduListItem::getBitmap( const char *aName )
	{
	BBitmap 	*bitmap = NULL;
	size_t 		len = 0;
	status_t 	error;	

	const void *data = iResources->LoadResource( 'BBMP', aName, &len );

	BMemoryIO stream( data, len );
	
	BMessage archive;
	error = archive.Unflatten( &stream );
	if( error != B_OK )
		return NULL;
	bitmap = new BBitmap( &archive );
	if( !bitmap )
		return NULL;

	if( bitmap->InitCheck() != B_OK )
		{
		delete bitmap;
		return NULL;
		}
	return bitmap;
	}
