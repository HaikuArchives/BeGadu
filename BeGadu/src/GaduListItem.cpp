#include <stdio.h>
#include <stdlib.h>
#include <Message.h>
#include <ListItem.h>
#include <Bitmap.h> 
#include <Rect.h>
#include <Font.h>
#include <String.h>
#include <View.h>
#ifndef _GADULISTITEM_H
#include "GaduListItem.h"
#endif
#include "GfxStuff.h"
extern "C" {
#include "libgadu.h"
}

GaduListItem::GaduListItem(char *osoba, int status) : BListItem()
{
	fStatus = status;
	SetIcon(fStatus);
	SetHeight(20.0);
	fNazwa = osoba;
	BFont font(be_plain_font);
	font.SetSize(14.0);
	float woi = font.StringWidth(fNazwa);
	SetWidth(woi);
}

GaduListItem::~GaduListItem()
{
	delete fIkona;
}

void GaduListItem::SetIcon(int status)
{
	switch(status)
	{
		case GG_STATUS_NOT_AVAIL:
		{
			fIkona = LoadGFX("offline.png");
			break;
		}
		case GG_STATUS_NOT_AVAIL_DESCR:
		{
			fIkona = LoadGFX("offline_d.png");
			break;
		}
		case GG_STATUS_INVISIBLE:
		{
			fIkona = LoadGFX("invisible.png");
			break;
		}
		case GG_STATUS_INVISIBLE_DESCR:
		{
			fIkona = LoadGFX("invisible_d.png");
			break;
		}
		case GG_STATUS_BUSY:
		{
			fIkona = LoadGFX("busy.png");
			break;
		}
		case GG_STATUS_BUSY_DESCR:
		{
			fIkona = LoadGFX("busy_d.png");
			break;
		}
		case GG_STATUS_AVAIL:
		{
			fIkona = LoadGFX("online.png");
			break;
		}
		case GG_STATUS_AVAIL_DESCR:
		{
			fIkona = LoadGFX("online_d.png");
			break;
		}
	}
}

void GaduListItem::DrawItem(BView *owner, BRect frame, bool complete)
{
	rgb_color kolor;
	if(fIkona)
	{
		frame.left = frame.left - 10;
		frame.top += 3;
		frame.bottom = frame.top + 20;
		BRect r( frame.left,
				 frame.top,
				 frame.left + 20,
				 frame.top + 20);
		owner->SetDrawingMode(B_OP_ALPHA);
		owner->SetLowColor(255, 255, 255, 0);
		owner->DrawBitmap(fIkona, r);
	}
	frame.left = frame.left + 20;
	if(IsSelected() || complete)
	{
		if(IsSelected())
			kolor.red = kolor.green = kolor.blue = 100;
		else
			kolor = owner->ViewColor();
		owner->SetHighColor(kolor);
		owner->FillRect(frame);
	}
	if(IsSelected())
		kolor.red = kolor.blue = kolor.green = kolor.alpha = 255;
	else
		kolor.red = kolor.blue = kolor.green = kolor.alpha = 0;
	owner->SetDrawingMode(B_OP_OVER);
	owner->MovePenTo(frame.left + 21, (((frame.bottom+frame.top)/2.0))+4.0);
	owner->SetHighColor(kolor);
	owner->DrawString(fNazwa);
}

void GaduListItem::Update(BView *owner, const BFont *font)
{
}
