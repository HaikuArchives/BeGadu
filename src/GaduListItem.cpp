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

GaduListItem::GaduListItem(char *osoba, int status, char *opis) : BListItem()
{
	fOpis = opis;
	fStatus = status;
	SetIcon(fStatus);
	SetHeight(40.0);
	fNazwa = osoba;
	fNormalFont = new BFont(be_plain_font);
	fSelectedFont = new BFont(be_bold_font);
	fStatusFont = new BFont(be_plain_font);
	fNormalFont->SetSize(15.0);
	fSelectedFont->SetSize(15.0);
	fStatusFont->SetSize(14.0);
	float woi = fSelectedFont->StringWidth(fNazwa);
	SetWidth(woi);
}

GaduListItem::~GaduListItem()
{
	delete fIkona;
	delete fNormalFont;
	delete fSelectedFont;
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
		frame.left += 5;
		frame.top += 3;
		frame.bottom = frame.top + 40;
		BRect r( frame.left,
				 frame.top + 10,
				 frame.left + 20,
				 frame.top + 30);
		owner->SetDrawingMode(B_OP_ALPHA);
		owner->SetLowColor(255, 255, 255, 0);
		owner->DrawBitmap(fIkona, r);
	}
	frame.left = frame.left + 22;
	if(IsSelected() || complete)
	{
		if(IsSelected())
		{
			owner->SetFont(fSelectedFont);
			kolor.red = kolor.green = kolor.blue = 100;
		}
		else
		{
			owner->SetFont(fNormalFont);
			kolor = owner->ViewColor();
		}
		owner->SetHighColor(kolor);
		owner->FillRect(frame);
	}
	if(IsSelected())
		kolor.red = kolor.blue = kolor.green = kolor.alpha = 255;
	else
		kolor.red = kolor.blue = kolor.green = kolor.alpha = 0;
	owner->SetDrawingMode(B_OP_OVER);
	owner->MovePenTo(frame.left + 15, frame.top + 15.0);
	owner->SetHighColor(kolor);
	owner->DrawString(fNazwa);
	owner->MovePenTo(frame.left + 15, frame.top + 30.0);
	owner->SetFont(fStatusFont);
	owner->DrawString(fOpis);
	owner->SetFont(fNormalFont);
}

void GaduListItem::Update(BView *owner, const BFont *font)
{
}
