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

#include <List.h>
#include <Message.h>
#include <Rect.h>
#include <Point.h>
#include <View.h>
#include <stdlib.h>
#include "TabsBox.h"
#include "TabsMgr.h"
#include "TabsView.h"

TabsView::TabsView(BRect bounds, char *name)
	:	BView( bounds, name, B_FOLLOW_NONE, B_WILL_DRAW | B_PULSE_NEEDED ),
		fCurrentTab(0),
		fMgr(NULL),
		fBox(NULL)
{
	SetViewColor(216, 216, 216);
	SetFont(be_plain_font);
	fMgr = new TabsMgr();
	fBox = new TabsBox();
}

void TabsView::ScrollTo(BPoint where)
{
	BView::ScrollTo(where);
	fBox->DrawScrollers(true);
}

BRect TabsView::FrameOf(int32 index)
{
	return BRect(index * fTabWidth, 0, ((index+1)*fTabWidth)-1, 100);
}

const int32 kTabsViewScrollSteps = 3;

void TabsView::GetPrefferedSize(float *w, float *h)
{
	font_width fw;
	be_plain_font->GetWidth(&fw);
	fTabWidth = (int32) fw.ascent + fw.descent;
	fTabWidth = fTabWidth + 3 + 3;
	fTabWidth = (((int) fTabWidth) + kTabsViewScrollSteps) / kTabsViewScrollSteps *
		kTabsViewScrollSteps;
	*w = fTabWidth;
	*h = Bounds().Height(); 
}

void TabsView::ShowIndex(int32 index)
{
	BPoint point(index * fTabWidth, 0);
	BRect bounds = Bounds();
	
	int32 tabIndex = fMgr->CurrentIndex();
	TabItem *tabItem = (TabItem*)fMgr->TabsList()->ItemAt(tabIndex);
	if(!tabItem)
		return;
	if(bounds.left == point.x)
		return;
	int32 oldIndex = (int32) (bounds.left / fTabWidth);
	int32 stepSize = (int32) (fTabWidth / kTabsViewScrollSteps);
	int32 scrollValue = (index>oldIndex)?stepSize : -stepSize;
	int32 total = 0;
	int32 nslots = abs(index - oldIndex);
	while(total<(nslots*(int32)fTabWidth))
	{
		ScrollBy(scrollValue, 0);
		snooze(10000);
		total += stepSize;
		Window()->UpdateIfNeeded();
	}
}

void TabsView::Draw(BRect update)
{
	int32 tabIndex = fMgr->CurrentIndex();
	TabItem *tabItem = (TabItem*)fMgr->TabsList()->ItemAt(tabIndex);
	if(!tabItem)
		return;
	BRect bounds = Bounds();
	int32 tabIndex = (int32) (bounds.left / fTabWidth);
	BRect tabRect = bounds;
	
	tabRect.left = tabIndex * fTabWidth;
	tabRect.right = ((tabIndex+1)*fTabWidth)-1;
	
	for(int32 i = 0; i < 3; i++)
	{
		if(!update.Intersects(tabRect))
		{
			tabIndex++;
			tabRect.OffsetBy(fTabWidth,0);
			continue;
		}
		
	}
}








