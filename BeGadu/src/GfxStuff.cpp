// GfxStuff.cpp - funkcje do ladowania .bmp'kow itp itd :)
#include <File.h>
#include <stdlib.h>
#include <Bitmap.h>
#include <BitmapStream.h>
#include <TranslatorRoster.h>
#include <TranslationUtils.h>
#include <View.h>
#include <Window.h>
#include <Resources.h>
#include <stdio.h>
#include <stdlib.h>
#include "GfxStuff.h"


IconControl::IconControl(BRect frame, const char *name, BBitmap *MOut, BBitmap *MOver, BMessage *message) : BView(frame, name, B_FOLLOW_ALL, B_WILL_DRAW | B_NAVIGABLE | B_ASYNCHRONOUS_CONTROLS)
{
	IconOut = MOut;
	IconOver = MOver;
	IsMouseOver = false;
	IsDown = false;
	mesg = message;
	SetEventMask(B_POINTER_EVENTS, B_NO_POINTER_HISTORY | B_LOCK_WINDOW_FOCUS);

}

IconControl::~IconControl()
{
	delete IconOut;
	delete IconOver;
}

void IconControl::AttachedToWindow()
{
	if( Parent() )
		SetViewColor( Parent()->ViewColor() );
	else
		SetViewColor(100, 100, 100);
}

void IconControl::Draw(BRect updateRect)
{
	SetDrawingMode(B_OP_ALPHA);
	SetLowColor(255,255,255,0);
	SetViewColor(B_TRANSPARENT_32_BIT);
	if(Window()->Lock())
	{
		if(IsMouseOver)
		{
			DrawBitmap(IconOver);
		}
		else
			DrawBitmap(IconOut);
		Window()->Unlock();
	}
}

void IconControl::MouseMoved(BPoint point, uint32 transit, const BMessage *message)
{
	BRect r = Bounds();
	if(point.x >= r.left && point.x <= r.right && point.y >= r.top && point.y <= r.bottom)
	{
		IsMouseOver = true;
		Draw(r);
	}
	else
	{
		IsMouseOver = false;
		Draw(r);
	}
}

void IconControl::MouseDown(BPoint point)
{
	uint32 a;
	GetMouse(&point, &a, true);
	BRect r = Bounds();
	if(point.x >= r.left && point.x <= r.right && point.y >= r.top && point.y <= r.bottom)
	{
		IsDown = true;
	}
	else
		IsDown = false;
}

void IconControl::MouseUp(BPoint point)
{
	uint32 a;
	GetMouse(&point, &a, true);
	BRect r = Bounds();
	if(IsDown)
	{
		if(point.x >= r.left && point.x <= r.right && point.y >= r.top && point.y <= r.bottom)
		{
			if(Window())
				Window()->PostMessage(mesg);
			IsDown = false;
		}
	}
}

void IconControl::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		default:
			BView::MessageReceived(message);
			break;
	}
}

BBitmap* LoadBMP(char* filename)
{
	BFile file(filename, B_READ_ONLY);
	BTranslatorRoster *roster = BTranslatorRoster::Default();
	BBitmapStream stream;
	BBitmap *result = NULL;
	if (roster->Translate(&file, NULL, NULL, &stream, B_TRANSLATOR_BITMAP) < B_OK) return NULL;
	stream.DetachBitmap(&result);
	return result;
}

BBitmap *LoadGFX(char *filename)
{
	return BTranslationUtils::GetBitmap(filename);
}
	
BitmapView::BitmapView( BRect frame, BBitmap *bmap )
			   : BView( frame, "bitmap_view", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP, B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE )
{
	SetViewColor( 255, 255, 255 );
	themap = bmap;
	bmbounds = themap->Bounds();
}


BitmapView::~BitmapView() {
	delete themap;
}


void BitmapView::AttachedToWindow() {
	if( Parent() )
		SetViewColor( Parent()->ViewColor() );
	else
		SetViewColor( 255, 255, 255 );
}


void BitmapView::Draw( BRect updateRect ) {

	// calculate the offsets to center the bitmap
	float LOffset = (Bounds().Width() - bmbounds.Width()) / 2;
	float TOffset = (Bounds().Height() - bmbounds.Height()) / 2;	

	// Make a new rect w/ those coordinates
	BRect drawRect( LOffset, TOffset, Bounds().Width() - LOffset, Bounds().Height() - TOffset );

	if ( Window()->Lock() )
	{
		DrawBitmap( themap, drawRect );
		Window()->Unlock();
	}
}
