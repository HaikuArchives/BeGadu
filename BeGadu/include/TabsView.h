/*
 * ============================================================================
 *  Nazwa    : TabsView z TabsView.h
 *  Projekt  : BeGadu
 *  Authorzy : 
 *		Artur Wyszynski <artur.wyszynski@bellstream.pl>
 *  Opis:
 *		Slider box :P
 *  Version  : 1.2
 * ============================================================================
 */

#ifndef __TABSVIEW_H__
#define __TABSVIEW_H__

#include <View.h>

class BList;
class BMessage;
class BPoint;
class BRect;
class TabsMgr;

class TabsView : public BBox
{
	public:
		TabsView(BRect bounds);
		virtual void Draw(BRect update);
		virtual void Pulse();
		virtual void GetPrefferedSize(float *w, float *h);
		void ScrollTo(float x, float y)
		{
			ScrollTo(BPoint(x,y));
		}
		virtual void ScrollTo(BPoint where);
		void ShowIndex(int32 index);
		BRect FrameOf(int32 index) const;
		TabsBox *Box() const;
	
	private:
		int32 		fCurrentTab;
		float		fTabHeight;
		TabsMgr	*	fMgr;
		TabsBox	*	fBox;
};

#endif /* __TABSVIEW_H__ */
