/**
 * ============================================================================
 *  Name     : Main from Main.h
 *  Project  : BeGadu
 *  Authors  : 
 *		Artur Wyszynski <artur.wyszynski@blstream.com>
 *  Description :
 *		App main window
 *  Version  : 1.2
 * ============================================================================
 */

#ifndef __BEGADU_MAINWINDOW_H__
#define __BEGADU_MAINWINDOW_H__

#include <Window.h>
#include <Resources.h>

/* external classes, included in src */
class Network;
class Preferences;
class List;
class BStringView;
class BMenuItem;
class BMenu;
class BBitmap;
class BListItem;
class BListView;
class Profile;
class BPopUpMenu;
class BMenuField;
class GaduListItem;
class GaduMenuItem;
class BScrollView;

class MainWindow : public BWindow
	{
	public:
		MainWindow( BString *aProfile );
		virtual bool QuitRequested();
		virtual void MessageReceived( BMessage *aMessage );
		void SetStatus( char *aStatus );
		void SetProfile( BString *aProfile );
		static int SortUsers( const void *, const void * );
		void LoadIcons();
		BBitmap	*GetBitmap( const char *aName );

		/* to gg */
		Profile				*	iProfile;
		Network				*	iNetwork;

		/* to interfejsu */
		BResources				iResources;
		BView				*	iGaduView;
		BScrollView			*	iScrollView;
		BListView			*	iListView;
		GaduListItem		*	iListItem;
		List				*	iListItems;
		BMenu				*	iSubMenu;
		BMenuItem			*	iProfileItem;

		BMenuItem			*	iListMenu;
		BMenuItem			*	iListImport;
		BMenuItem			*	iListExport;

		BMenuItem			*	iAddPerson;
		BMenuItem			*	iDelPerson;
		BMenuItem			*	iCatalog;
		BMenuItem			*	iAbout;
		BMenuItem			*	iPreferences;
		BView				*	iIconsView;

		BMenuField			*	iStatus;
		BPopUpMenu			*	iStatusMenu;
		GaduMenuItem		*	iAvail;
		GaduMenuItem		*	iBrb;
		GaduMenuItem		*	iInvis;
		GaduMenuItem		*	iNotAvail;
		GaduMenuItem		*	iDescr;
		BBitmap				*	iIconAvail;
		BBitmap				*	iIconBrb;
		BBitmap				*	iIconInvis;
		BBitmap				*	iIconNotAvail;
		BBitmap				*	iIconAvailDescr;
		BBitmap				*	iIconBrbDescr;
		BBitmap				*	iIconInvisDescr;
		BBitmap				*	iIconNotAvailDescr;
	};

#endif /* __BEGADU_MAINWINDOW_H__ */
