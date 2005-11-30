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

GaduListItem::GaduListItem(BString *osoba, int status, BString *opis, BResources *res) : BListItem()
{
	fResources = res;
	fStatus = status;
	SetIcon(fStatus);
	fNazwa = osoba;
	fOpis = opis;
	fNazwaFont = new BFont(be_plain_font);
	fNazwaFont->SetSize(15.0);
	fOpisFont = new BFont(be_plain_font);
	fOpisFont->SetSize(10.0);
	float woi = fNazwaFont->StringWidth(fNazwa->String());
	SetWidth(woi);
	SetHeight(35.0);
}

GaduListItem::~GaduListItem()
{
	if( fIkona )
		delete fIkona;
}

void GaduListItem::SetIcon(int status)
{
	switch(status)
	{
		case GG_STATUS_NOT_AVAIL:
		{
			fIkona = GetBitmap("NotAvail.png");
			break;
		}
		case GG_STATUS_NOT_AVAIL_DESCR:
		{
			fIkona = GetBitmap("NotAvailDescr.png");
			break;
		}
		case GG_STATUS_INVISIBLE:
		{
			fIkona = GetBitmap("Invisible.png");
			break;
		}
		case GG_STATUS_INVISIBLE_DESCR:
		{
			fIkona = GetBitmap("InvisibleDescr.png");
			break;
		}
		case GG_STATUS_BUSY:
		{
			fIkona = GetBitmap("Busy.png");
			break;
		}
		case GG_STATUS_BUSY_DESCR:
		{
			fIkona = GetBitmap("BusyDescr.png");
			break;
		}
		case GG_STATUS_AVAIL:
		{
			fIkona = GetBitmap("Avail.png");
			break;
		}
		case GG_STATUS_AVAIL_DESCR:
		{
			fIkona = GetBitmap("AvailDescr.png");
			break;
		}
	}
}

void GaduListItem::DrawItem(BView *owner, BRect frame, bool complete)
{
	rgb_color kolor;
	rgb_color kolor_nazwa;
	rgb_color kolor_opis;
	if(IsSelected() || complete)
	{
		if(IsSelected())
		{
			kolor.red = kolor.green = kolor.blue = 100;
		}
		else
		{
			kolor = owner->ViewColor();
		}
	}
	else
	{
		kolor = owner->ViewColor();
	}
	owner->SetHighColor(kolor);
	owner->FillRect(frame);
	if(fIkona)
	{
		frame.left += 2;
		frame.top += 8;
		frame.bottom = frame.top + 20;
		BRect r( frame.left,
				 frame.top,
				 frame.left + 18,
				 frame.top + 16);
		owner->SetDrawingMode(B_OP_ALPHA);
		owner->SetLowColor(255, 255, 255, 0);
		owner->DrawBitmap(fIkona, r);
	}
	if(IsSelected())
	{
		kolor_nazwa.red = kolor_nazwa.blue = kolor_nazwa.green = kolor_nazwa.alpha = 255;
		kolor_opis.red = kolor_opis.blue = kolor_opis.green = kolor_opis.alpha = 175;
	}
	else
	{
		kolor_nazwa.red = kolor_nazwa.blue = kolor_nazwa.green = kolor_nazwa.alpha = 0;
		kolor_opis.red = kolor_opis.blue = kolor_opis.green = kolor_opis.alpha = 50;
	}
	owner->SetDrawingMode(B_OP_OVER);
//	owner->MovePenTo(frame.left + 21, (((frame.bottom+frame.top)/2.0))+4.0);
	owner->SetFont(fNazwaFont);
	owner->MovePenTo(frame.left + 21, frame.top + 9 );
	owner->SetHighColor(kolor_nazwa);
	owner->DrawString(fNazwa->String());
	owner->SetFont(fOpisFont);
	owner->SetHighColor(kolor_opis);
	owner->MovePenTo(frame.left + 21, frame.top + 21);
	owner->DrawString(fOpis->String());	
}

void GaduListItem::Update(BView *owner, const BFont *font)
{
	BListItem::Update(owner, font);
	SetHeight(35.0);
//	Invalidate();
}

BBitmap *GaduListItem::GetBitmap(const char *name)
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
