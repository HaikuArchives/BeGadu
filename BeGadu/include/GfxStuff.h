// GfxStuff.h
#ifndef GFX_STUFF_H
#define GFX_STUFF_H
#include <Bitmap.h>
#include <View.h>
#include <Message.h>


BBitmap *LoadBMP(char*);
BBitmap *LoadGFX(char*);

/* 
	Icon button
	Code: aljen 
*/
class IconControl : public BView
{
	public:
		IconControl(BRect frame, const char *name, BBitmap *MOut, BBitmap *MOver, BMessage *message);
		~IconControl();
		virtual void Draw(BRect updateRect);
		virtual void AttachedToWindow();
		virtual void MouseMoved(BPoint point, uint32 transit, const BMessage *message);
		virtual void MouseDown(BPoint point);
		virtual void MouseUp(BPoint point);
		virtual void MessageReceived(BMessage *message);
	private:
		BMessage *mesg;
		BBitmap *IconOut;
		BBitmap *IconOver;
		bool IsDown;
		bool IsMouseOver;
};



class BitmapView : public BView
{
	public:
		BitmapView(BRect frame, BBitmap *bmap);
		~BitmapView();
		void Draw(BRect updateRect);
		void AttachedToWindow();
		
	private:
		BBitmap	*themap;
		BRect bmbounds;
};

#endif // GFX_STUFF_H