#ifndef _GADULISTITEM_H
#define _GADULISTITEM_H

#include <Bitmap.h>
#include <Message.h>
#include <ListItem.h>
#include <Bitmap.h> 
#include <Rect.h>
#include <Font.h>
#include <String.h>

/*
	Moja wersja IconListItem
	Code: aljen
*/

class GaduListItem : public BListItem
{
	public:
		GaduListItem(char *osoba, int status, char *opis);
		~GaduListItem();
		void SetIcon(int status);
		virtual void DrawItem(BView *owner, BRect frame, bool complete);
		virtual void Update(BView *owner, const BFont *font);
		char		*	fNazwa;
		char		*	fOpis;
		int 			fStatus;
		BBitmap		*	fIkona;
		BFont		*	fNormalFont;
		BFont		*	fSelectedFont;
		BFont		*	fStatusFont;
};
#endif // _GADULISTITEM_H

