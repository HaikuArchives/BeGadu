/*
	BeGadu.cpp - Client G*adu-G*adu for BeOS
	Code: aljen <aljen@gumisie.org>
	Homepage: http://gadu.beos.pl
*/

#include <Path.h>
#include <StorageKit.h>
#include <File.h>
#include <Screen.h>
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
	/* we're checking configuration [; */
	fFirstRun = false;
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
	if(bg_conf->FindEntry("Profiles", &entry) != B_OK)
	{
		
		find_directory(B_USER_SETTINGS_DIRECTORY, &path);
		path.Append("BeGadu/Profiles");
		error = bg_conf->CreateDirectory(path.Path(), bg_conf);
		fFirstRun = true;
	}
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	path.Append("BeGadu/Profiles");
	bg_conf->SetTo(path.Path());
	if(bg_conf->CountEntries() == 0)
		fFirstRun = true;
	else
		fFirstRun = false;	

	/* loading config [; */
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	path.Append("BeGadu/Config");
	BFile file(path.Path(), B_READ_ONLY);
	fprintf(stderr, "Loading config from: %s\n", path.Path());
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
		PostMessage(new BMessage(OPEN_PROFILE_WIZARD));
	else
		PostMessage(new BMessage(CONFIG_OK));
}

void BeGadu::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		/* sending mesgs from libgadu to network */
		case MAM_WIADOMOSC:
		case DODAJ_HANDLER:
		case USUN_HANDLER:
			mWindow->fSiec->PostMessage(message);
			break;

		case BEGG_ABOUT:
		{
			if(mWindow)
				mWindow->PostMessage(message);
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
			fprintf(stderr,"Config ok\n");
			mWindow = new MainWindow(fLastProfile);
			mWindow->Show();
			break;
		}
		case PROFILE_CREATED:
		{
			message->FindString("ProfileName", fLastProfile);
			fprintf(stderr, "Setting last profile to: %s\n", fLastProfile->String());
			fFirstRun = false;
			/* creating & running network handler */
			mWindow = new MainWindow(fLastProfile);
			mWindow->Show();
			break;
		}
		default:
			BApplication::MessageReceived(message);
			break;
	}
}

bool BeGadu::QuitRequested()
{
	if(mWindow->Lock())
		mWindow->Quit();
	/* saving config */
	BMessage * cfgmesg = new BMessage();
	cfgmesg->AddBool("fFirstRun", fFirstRun);
	cfgmesg->AddString("fLastProfile", *fLastProfile);
	BPath path;
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	path.Append("BeGadu/Config");
	fprintf(stderr, "Last profile: %s\n", fLastProfile->String());
	fprintf(stderr, "Saving config to: %s\n", path.Path());
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

int main()
{
	BeGadu	*bgg = new BeGadu();
	bgg->Run();
	delete bgg;
	return 0;
}
