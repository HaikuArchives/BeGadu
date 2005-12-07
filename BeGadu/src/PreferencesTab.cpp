#include <Application.h>
#include <Bitmap.h>
#include <BitmapStream.h>
#include <Message.h>
#include <Path.h>
#include <Resources.h>
#include <Roster.h>
#include <TranslatorRoster.h>
#include <TranslationUtils.h>
#include <View.h>
#include <Window.h>
#include <stdio.h>
#include <stdlib.h>
#include "PreferencesTab.h"

#ifdef ZETA
#include <locale/Locale.h>
#else
#define _T(str) (str)
#endif


PreferencesTab::PreferencesTab( BRect aFrame, const char *aName, BResources *aResources )
	: BView( aFrame, aName, B_FOLLOW_ALL, B_WILL_DRAW | B_NAVIGABLE | B_ASYNCHRONOUS_CONTROLS )
	{
	iResources		= aResources;	
	iIconGeneral	= GetBitmap( "config_general" );
	iIconIM			= GetBitmap( "config_im" );
	iIconSounds		= GetBitmap( "config_sounds" );
	iIconCatalog	= GetBitmap( "config_catalog" );
	iIconNotify		= GetBitmap( "config_notify" );
	iIconTheme		= GetBitmap( "config_theme" );
	iIconConnection	= GetBitmap( "config_connection" );
	iIsOver			= false;
	iIsDown			= false;
	iItemOver		= 0;
	iItemOverOld	= 0;
	iItemOverNew	= 0;
	iItemSelected	= 1;
	iItemSelectedOld= 1;
	SetEventMask(B_POINTER_EVENTS, B_NO_POINTER_HISTORY | B_LOCK_WINDOW_FOCUS);

#ifdef ZETA
	app_info appinfo;
	be_app->GetAppInfo( &appinfo );
	BPath localization;
	BEntry entryloc( &appinfo.ref, true );
	entryloc.GetPath( &localization );
	localization.GetParent( &localization );
	localization.Append( "Language/Dictionaries/BeGadu" );
	BString localization_string;
	if( localization.InitCheck() != B_OK )
		localization_string.SetTo( "Language/Dictionaries/BeGadu" );
	else
		localization_string.SetTo( localization.Path() );
	be_locale.LoadLanguageFile( localization_string.String() );
#endif

	}

PreferencesTab::~PreferencesTab()
	{
	iResources		= NULL;
	delete iIconGeneral;
	iIconGeneral	= NULL;
	delete iIconIM;
	iIconIM			= NULL;
	delete iIconSounds;
	iIconSounds		= NULL;
	delete iIconCatalog;
	iIconCatalog	= NULL;
	delete iIconNotify;
	iIconNotify		= NULL;
	delete iIconTheme;
	iIconTheme		= NULL;
	delete iIconConnection;
	iIconConnection	= NULL;
	}

void PreferencesTab::Draw( BRect aRect )
	{
	fprintf( stderr, "l: %f, t: %f, r: %f, b: %f\n", aRect.left, aRect.top, aRect.right, aRect.bottom );
	int itemWidth = 90;
	int offset = 29;
	rgb_color selected;
	selected.red = selected.green = 190;
	selected.blue = 255;
	selected.alpha = 255;

	rgb_color over;
	over.red = over.green =  216;
	over.blue = 255;
	over.alpha = 255;


	BRect r;
	BRect r1( 0, 0, itemWidth, 60 );
	BRect r2( itemWidth, 0, itemWidth * 2, 60 );
	BRect r3( itemWidth * 2, 0, itemWidth * 3, 60 );
	BRect r4( itemWidth * 3, 0, itemWidth * 4, 60 );
	BRect r5( itemWidth * 4, 0, itemWidth * 5, 60 );
	BRect r6( itemWidth * 5, 0, itemWidth * 6, 60 );
	BRect r7( itemWidth * 6, 0, itemWidth * 7, 60 );

	if( aRect != Bounds() )
		{
		/* checking that mouse is over tab and on which button */
		if( iIsOver )
			{
			switch( iItemOverOld )
				{
				case 1:
					{
					if( iItemOverOld != iItemSelected )
						{
						r = r1;
						SetHighColor( 255, 255, 255, 255 );
						FillRect( r );
						SetDrawingMode( B_OP_ALPHA );
						SetLowColor( 0, 0, 0, 255 );
						DrawBitmap( iIconGeneral,	 BPoint( offset, 5 ) );
						SetDrawingMode( B_OP_OVER );
						}
					break;
					}
				case 2:
					{
					r = r2;
					SetHighColor( 255, 255, 255, 255 );
					FillRect( r );
					SetDrawingMode( B_OP_ALPHA );
					SetLowColor( 0, 0, 0, 255 );
					DrawBitmap( iIconIM,		 BPoint( itemWidth + offset, 5 ) );
					SetDrawingMode( B_OP_OVER );
					break;
					}
				case 3:
					{
					r = r3;
					SetHighColor( 255, 255, 255, 255 );
					FillRect( r );
					SetDrawingMode( B_OP_ALPHA );
					SetLowColor( 0, 0, 0, 255 );
					DrawBitmap( iIconSounds,	 BPoint( itemWidth * 2 + offset, 5 ) );
					SetDrawingMode( B_OP_OVER );
					break;
					}
				case 4:
					{
					r = r4;
					SetHighColor( 255, 255, 255, 255 );
					FillRect( r );
					SetDrawingMode( B_OP_ALPHA );
					SetLowColor( 0, 0, 0, 255 );
					DrawBitmap( iIconCatalog,	 BPoint( itemWidth * 3 + offset, 5 ) );
					SetDrawingMode( B_OP_OVER );
					break;
					}
				case 5:
					{
					r = r5;
					SetHighColor( 255, 255, 255, 255 );
					FillRect( r );
					SetDrawingMode( B_OP_ALPHA );
					SetLowColor( 0, 0, 0, 255 );
					DrawBitmap( iIconNotify,	 BPoint( itemWidth * 4 + offset, 5 ) );
					SetDrawingMode( B_OP_OVER );
					break;
					}
				case 6:
					{
					r = r6;
					SetHighColor( 255, 255, 255, 255 );
					FillRect( r );
					SetDrawingMode( B_OP_ALPHA );
					SetLowColor( 0, 0, 0, 255 );
					DrawBitmap( iIconTheme,		 BPoint( itemWidth * 5 + offset, 5 ) );
					SetDrawingMode( B_OP_OVER );
					break;
					}
				case 7:
					{
					r = r7;
					SetHighColor( 255, 255, 255, 255 );
					FillRect( r );
					SetDrawingMode( B_OP_ALPHA );
					SetLowColor( 0, 0, 0, 255 );
					DrawBitmap( iIconConnection, BPoint( itemWidth * 6 + offset, 5 ) );
					SetDrawingMode( B_OP_OVER );
					break;
					}
				}

			switch( iItemOver )
				{
				case 1:
					{
					if( iItemOver != iItemSelected )
						{
						r = r1;
						SetHighColor( over );
						FillRect( r );
						SetDrawingMode( B_OP_ALPHA );
						SetLowColor( 0, 0, 0, 255 );
						DrawBitmap( iIconGeneral,	 BPoint( offset, 5 ) );
						SetDrawingMode( B_OP_OVER );
						}
					break;
					}
				case 2:
					{
					if( iItemOver != iItemSelected )
						{
						r = r2;
						SetHighColor( over );
						FillRect( r );
						SetDrawingMode( B_OP_ALPHA );
						SetLowColor( 0, 0, 0, 255 );
						DrawBitmap( iIconIM,		 BPoint( itemWidth + offset, 5 ) );
						SetDrawingMode( B_OP_OVER );
						}
					break;
					}
				case 3:
					{
					if( iItemOver != iItemSelected )
						{
						r = r3;
						SetHighColor( over );
						FillRect( r );
						SetDrawingMode( B_OP_ALPHA );
						SetLowColor( 0, 0, 0, 255 );
						DrawBitmap( iIconSounds,	 BPoint( itemWidth * 2 + offset, 5 ) );
						SetDrawingMode( B_OP_OVER );
						}
					break;
					}
				case 4:
					{
					if( iItemOver != iItemSelected )
						{
						r = r4;
						SetHighColor( over );
						FillRect( r );
						SetDrawingMode( B_OP_ALPHA );
						SetLowColor( 0, 0, 0, 255 );
						DrawBitmap( iIconCatalog,	 BPoint( itemWidth * 3 + offset, 5 ) );
						SetDrawingMode( B_OP_OVER );
						}
					break;
					}
				case 5:
					{
					if( iItemOver != iItemSelected )
						{
						r = r5;
						SetHighColor( over );
						FillRect( r );
						SetDrawingMode( B_OP_ALPHA );
						SetLowColor( 0, 0, 0, 255 );
						DrawBitmap( iIconNotify,	 BPoint( itemWidth * 4 + offset, 5 ) );
						SetDrawingMode( B_OP_OVER );
						}
					break;
					}
				case 6:
					{
					if( iItemOver != iItemSelected )
						{
						r = r6;
						SetHighColor( over );
						FillRect( r );
						SetDrawingMode( B_OP_ALPHA );
						SetLowColor( 0, 0, 0, 255 );
						DrawBitmap( iIconTheme,		 BPoint( itemWidth * 5 + offset, 5 ) );
						SetDrawingMode( B_OP_OVER );
						}
					break;
					}
				case 7:
					{
					if( iItemOver != iItemSelected )
						{
						r = r7;
						SetHighColor( over );
						FillRect( r );
						SetDrawingMode( B_OP_ALPHA );
						SetLowColor( 0, 0, 0, 255 );
						DrawBitmap( iIconConnection, BPoint( itemWidth * 6 + offset, 5 ) );
						SetDrawingMode( B_OP_OVER );
						}
					break;
					}
				}
			}
		}
	else
		{
		SetDrawingMode( B_OP_COPY );
		SetHighColor( 255, 255, 255, 255 );
		FillRect( Bounds() );
		
		SetHighColor( selected );
		switch( iItemSelected )
			{
			case 1:
				{
				r = r1;
				SetHighColor( selected );
				FillRect( r );
				SetDrawingMode( B_OP_ALPHA );
				SetLowColor( 0, 0, 0, 255 );
				DrawBitmap( iIconGeneral,	 BPoint( offset, 5 ) );
				SetDrawingMode( B_OP_OVER );
				break;
				}
			case 2:
				{
				r = r2;
				SetHighColor( selected );
				FillRect( r );
				SetDrawingMode( B_OP_ALPHA );
				SetLowColor( 0, 0, 0, 255 );
				DrawBitmap( iIconIM,		 BPoint( itemWidth + offset, 5 ) );
				SetDrawingMode( B_OP_OVER );
				break;
				}
			case 3:
				{
				r = r3;
				SetHighColor( selected );
				FillRect( r );
				SetDrawingMode( B_OP_ALPHA );
				SetLowColor( 0, 0, 0, 255 );
				DrawBitmap( iIconSounds,	 BPoint( itemWidth * 2 + offset, 5 ) );
				SetDrawingMode( B_OP_OVER );
				break;
				}
			case 4:
				{
				r = r4;
				SetHighColor( selected );
				FillRect( r );
				SetDrawingMode( B_OP_ALPHA );
				SetLowColor( 0, 0, 0, 255 );
				DrawBitmap( iIconCatalog,	 BPoint( itemWidth * 3 + offset, 5 ) );
				SetDrawingMode( B_OP_OVER );
				break;
				}
			case 5:
				{
				r = r5;
				SetHighColor( selected );
				FillRect( r );
				SetDrawingMode( B_OP_ALPHA );
				SetLowColor( 0, 0, 0, 255 );
				DrawBitmap( iIconNotify,	 BPoint( itemWidth * 4 + offset, 5 ) );
				SetDrawingMode( B_OP_OVER );
				break;
				}
			case 6:
				{
				r = r6;
				SetHighColor( selected );
				FillRect( r );
				SetDrawingMode( B_OP_ALPHA );
				SetLowColor( 0, 0, 0, 255 );
				DrawBitmap( iIconTheme,		 BPoint( itemWidth * 5 + offset, 5 ) );
				SetDrawingMode( B_OP_OVER );
				break;
				}
			case 7:
				{
				r = r7;
				SetHighColor( selected );
				FillRect( r );
				SetDrawingMode( B_OP_ALPHA );
				SetLowColor( 0, 0, 0, 255 );
				DrawBitmap( iIconConnection, BPoint( itemWidth * 6 + offset, 5 ) );
				SetDrawingMode( B_OP_OVER );
				break;
				}
			}
		
		SetDrawingMode( B_OP_ALPHA );
		SetLowColor( 0, 0, 0, 255 );
		DrawBitmap( iIconGeneral,	 BPoint( offset, 5 ) );
		DrawBitmap( iIconIM,		 BPoint( itemWidth + offset, 5 ) );
		DrawBitmap( iIconSounds,	 BPoint( itemWidth * 2 + offset, 5 ) );
		DrawBitmap( iIconCatalog,	 BPoint( itemWidth * 3 + offset, 5 ) );
		DrawBitmap( iIconNotify,	 BPoint( itemWidth * 4 + offset, 5 ) );
		DrawBitmap( iIconTheme,		 BPoint( itemWidth * 5 + offset, 5 ) );
		DrawBitmap( iIconConnection, BPoint( itemWidth * 6 + offset, 5 ) );
		}

	SetDrawingMode( B_OP_COPY );
	SetLowColor( 255, 255, 255, 255 );
	SetHighColor( 0, 0, 0, 255 );
	BFont *font = new BFont( be_bold_font );
	font->SetEncoding( B_ISO_8859_2 );
	font->SetSize( 12.0 );
	SetFont( font );
	MovePenTo( BPoint( 0, 50 ) );
	DrawString( _T( "General" ) );
	MovePenTo( BPoint( itemWidth, 50 ) );
	DrawString( _T( "IM" ) );
	MovePenTo( BPoint( itemWidth * 2, 50 ) );
	DrawString( _T( "Sounds" ) );
	MovePenTo( BPoint( itemWidth * 3, 50 ) );
	DrawString( _T( "Catalog" ) );
	MovePenTo( BPoint( itemWidth * 4, 50 ) );
	DrawString( _T( "Notify" ) );
	MovePenTo( BPoint( itemWidth * 5, 50 ) );
	DrawString( _T( "Theme" ) );
	MovePenTo( BPoint( itemWidth * 6, 50 ) );
	DrawString( _T( "Connection" ) );
	}

void PreferencesTab::AttachedToWindow()
	{
	SetViewColor( 255, 255, 255 );
	}

void PreferencesTab::MouseMoved( BPoint aPoint, uint32 aTransit, const BMessage *aMessage )
	{
	BRect r = Bounds();
	int itemWidth = 90;
	
	BRect r1( 0, 0, itemWidth, 60 );
	BRect r2( itemWidth, 0, itemWidth * 2, 60 );
	BRect r3( itemWidth * 2, 0, itemWidth * 3, 60 );
	BRect r4( itemWidth * 3, 0, itemWidth * 4, 60 );
	BRect r5( itemWidth * 4, 0, itemWidth * 5, 60 );
	BRect r6( itemWidth * 5, 0, itemWidth * 6, 60 );
	BRect r7( itemWidth * 6, 0, itemWidth * 7, 60 );

	/* item 1 */
	if( aPoint.x >= r1.left &&
		aPoint.x <= r1.right &&
		aPoint.y >= r1.top &&
		aPoint.y <= r1.bottom )
		{
		iIsOver = true;
		iItemOverNew = 1;
		r = r1;
		}
	else if( aPoint.x >= r2.left &&
			 aPoint.x <= r2.right &&
			 aPoint.y >= r2.top &&
			 aPoint.y <= r2.bottom )
		{
		iIsOver = true;
		iItemOverNew = 2;
		r = r2;
		}
	else if( aPoint.x >= r3.left &&
			 aPoint.x <= r3.right &&
			 aPoint.y >= r3.top &&
			 aPoint.y <= r3.bottom )
		{
		iIsOver = true;
		iItemOverNew = 3;
		r = r3;
		}
	else if( aPoint.x >= r4.left &&
			 aPoint.x <= r4.right &&
			 aPoint.y >= r4.top &&
			 aPoint.y <= r4.bottom )
		{
		iIsOver = true;
		iItemOverNew = 4;
		r = r4;
		}
	else if( aPoint.x >= r5.left &&
			 aPoint.x <= r5.right &&
			 aPoint.y >= r5.top &&
			 aPoint.y <= r5.bottom )
		{
		iIsOver = true;
		iItemOverNew = 5;
		r = r5;
		}
	else if( aPoint.x >= r6.left &&
			 aPoint.x <= r6.right &&
			 aPoint.y >= r6.top &&
			 aPoint.y <= r6.bottom )
		{
		iIsOver = true;
		iItemOverNew = 6;
		r = r6;
		}
	else if( aPoint.x >= r7.left &&
			 aPoint.x <= r7.right &&
			 aPoint.y >= r7.top &&
			 aPoint.y <= r7.bottom )
		{
		iIsOver = true;
		iItemOverNew = 7;
		r = r7;
		}
	else if( iItemOverNew != iItemSelected )
		{
		iIsOver = false;
		}

	if( iIsOver )
		{
		if( iItemOver != iItemOverNew )
			{
			iItemOverOld = iItemOver;
			iItemOver = iItemOverNew;
			Draw( r );
			}
		}
	}

void PreferencesTab::MouseDown( BPoint aPoint )
	{
	}

void PreferencesTab::MouseUp( BPoint aPoint )
	{
	}

void PreferencesTab::MessageReceived( BMessage *aMessage )
	{
	}

BBitmap* PreferencesTab::GetBitmap( const char* aName )
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
