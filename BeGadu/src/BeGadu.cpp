/*
	BeGadu.cpp - Client G*adu-G*adu for BeOS
	Code: aljen <aljen@gumisie.org>
	Homepage: http://gadu.beos.pl
*/

#include <Deskbar.h>
#include <Path.h>
#include <Roster.h>
#include <StorageKit.h>
#include <File.h>
#include <Screen.h>
#include <String.h>
#include <Alert.h>

#include "Msg.h"
#include "BeGadu.h"
#include "Siec.h"
#include "Main.h"
#include "Osoba.h"
#include "Debug.h"
#include "Opcje.h"
#include "ProfilWizard.h"

BeGadu::BeGadu() : BApplication("application/x-vnd.BeGadu")
{
	/* sprawdzamy konfiguracje */
	fFirstRun = false;
	fHideAtStart = false;
	fLastProfile = new BString();
	fLastProfile->SetTo("");
	BPath path;
	BEntry entry;
	status_t error;
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	BDirectory * bg_conf = new BDirectory(path.Path());
	if(bg_conf->FindEntry("BeGadu", &entry) != B_OK)
	{
		path.Append("BeGadu");
		bg_conf->CreateDirectory(path.Path(), bg_conf);
	}
	if(bg_conf->FindEntry("Profile", &entry) != B_OK)
	{
		
		find_directory(B_USER_SETTINGS_DIRECTORY, &path);
		path.Append("BeGadu/Profile");
		error = bg_conf->CreateDirectory(path.Path(), bg_conf);
		fFirstRun = true;
	}
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	path.Append("BeGadu/Profile");
	bg_conf->SetTo(path.Path());
	if(bg_conf->CountEntries() == 0)
		fFirstRun = true;
	else
		fFirstRun = false;	

	/* laduje config */
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	path.Append("BeGadu/Config");
	BFile file(path.Path(), B_READ_ONLY);
	fprintf(stderr, "Laduje config z %s\n", path.Path());
	BMessage *cfgmesg = new BMessage();
	if(file.InitCheck() == B_OK)
	{
		cfgmesg->Unflatten(&file);
		file.Unset();
	}
	if( cfgmesg->FindBool("fFirstRun", &fFirstRun) != B_OK )
		fFirstRun =  true;
	if( cfgmesg->FindString("fLastProfile", fLastProfile) != B_OK )
		fLastProfile->SetTo("");
	delete cfgmesg;

	if(fFirstRun)
	{
		fprintf(stderr, "Pierwsze uruchomienie, startuje Kreator Profili\n");
		BMessenger(this).SendMessage(new BMessage(OPEN_PROFILE_WIZARD));
	}
	else
	{
		fprintf(stderr, "Konfig ok :)\n");
		BMessenger(this).SendMessage(new BMessage(CONFIG_OK));
	}
}

void BeGadu::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		/* sending mesgs from libgadu to network */
		case MAM_WIADOMOSC:
		case DODAJ_HANDLER:
		case USUN_HANDLER:
		{
			BMessenger(mWindow->fSiec).SendMessage(message);
			break;
		}
		case SET_AVAIL:
		case SET_BRB:
		case SET_INVIS:
		case SET_NOT_AVAIL:
		case SET_DESCRIPTION:
		case BEGG_ABOUT:
		{
			if(mWindow)
				BMessenger(mWindow).SendMessage(message);
			break;
		}
		case OPEN_PROFILE_WIZARD:
		{
			ProfilWizard *pw = new ProfilWizard();
			pw->Show();
			break;
		}

		case CONFIG_OK:
		{
			mWindow = new MainWindow(fLastProfile);
			if( !fHideAtStart )
			{
				mWindow->Show();
			}
			else
			{
				mWindow->Show();
				mWindow->Hide();
			}
			break;
		}
		case PROFILE_CREATED:
		{
			message->FindString("ProfileName", fLastProfile);
			fprintf(stderr, "Ustawiam ostatni profil na %s\n", fLastProfile->String());
			fFirstRun = false;
			mWindow = new MainWindow(fLastProfile);
			if(mWindow->IsHidden())
			{
				mWindow->Show();
			}
			else
				mWindow->Activate();
			break;
		}
		case REMOVE_FROM_DESKBAR:
		{
			BMessenger(this).SendMessage( B_QUIT_REQUESTED );
			break;
		}
		default:
			BApplication::MessageReceived(message);
			break;
	}
}

void BeGadu::ReadyToRun()
{
	fprintf( stderr, "BeGadu::ReadyToRun()\n" );
	AddDeskbarIcon();
}

bool BeGadu::QuitRequested()
{
	DelDeskbarIcon();
	if(mWindow->Lock())
		mWindow->Quit();
	/* zapisujemy konfig */
	BMessage * cfgmesg = new BMessage();
	cfgmesg->AddBool("fFirstRun", fFirstRun);
	cfgmesg->AddBool("fHideAtStart", fHideAtStart);
	cfgmesg->AddString("fLastProfile", *fLastProfile);
	BPath path;
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	path.Append("BeGadu/Config");
	fprintf(stderr, "Ostatni profil %s\n", fLastProfile->String());
	fprintf(stderr, "Zapisuje konfig do %s\n", path.Path());
	BFile file(path.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
	if(file.InitCheck() == B_OK)
	{
		cfgmesg->Flatten(&file);
		file.Unset();
	}
	delete cfgmesg;
	BApplication::QuitRequested();
	return true;
}

void BeGadu::AddDeskbarIcon()
{
	fprintf( stderr, "BeGadu::AddDeskbarIcon()\n" );
	BDeskbar deskbar;
	if( !deskbar.HasItem( "BGDeskbar" ) )
	{
		BRoster roster;
		entry_ref ref;
		status_t status = roster.FindApp( "application/x-vnd.BeGadu", &ref );
		if( status != B_OK )
		{
			fprintf( stderr, "Can't find BeGadu running %s\n", strerror( status ) );
			return;
		}
		status = deskbar.AddItem( &ref );
		if( status != B_OK )
		{
			fprintf( stderr, "Can't put BeGadu into Deskbar %s\n", strerror( status ) );
			return;
		}
	}
}

void BeGadu::DelDeskbarIcon()
{
	fprintf( stderr, "BeGadu::DelDeskbarIcon()\n" );
	BDeskbar deskbar;
	if( deskbar.HasItem( "BGDeskbar" ) )
		deskbar.RemoveItem( "BGDeskbar" );
}

int main()
{
	BeGadu	*bgg = new BeGadu();
	bgg->Run();
	delete bgg;
	return 0;
}
