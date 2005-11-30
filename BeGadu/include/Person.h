/*
 * ============================================================================
 *  Name     : Person from Person.h
 *  Project  : BeGadu
 *  Authors  : 
 *		Artur Wyszynski <artur.wyszynski@blstream.pl>
 *  Description :
 *		Klasy zwiazane z obsluga userlist i osob
 *  Version  : 1.2
 * ============================================================================
 */

#ifndef __BEGADU_PERSON_H__
#define __BEGADU_PERSON_H__

#include <List.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>

#include <libgadu.h>

/* klasa pojedynczej osoby */
class Person
	{
	public:
		Person();
		
		// setters & getters
		const BString GetName() const;
		const BString GetSurname() const;
		const BString GetNick() const;
		const BString GetDisplay() const;
		const BString GetPhone() const;
		const BString GetEmail() const;
		uin_t GetUIN();
		int GetStatus();
		const BString GetDescription() const;
		struct in_addr GetIP();
		unsigned short GetPort();
		int GetProtocol();
		time_t GetLastSeen();
		const BString GetLastDescription() const;
		struct in_addr GetLastIp();
		unsigned short GetLastPort();
		int GetImageSize();
	
	private:
		BString	* iName;
		BString	* iSurname;
		BString	* iNick;
		BString	* iDisplay;
		BString	* iPhone;
		BString	* iEmail;
		uin_t iUIN;
		int	iStatus;
		BString	* iDescription;
		struct in_addr iIP;
		unsigned short iPort;
		int	iProtocol;
		time_t iLastSeen;
		BString * iLastDescr;
		struct in_addr iLastIP;
		unsigned short iLastPort;
		int iImgSize;
	};

class List : public BList
	{
	public:
		List( int aSize );
		bool IsValid();
		void SetValid( bool aValid );
	
	private:
		bool iIsValid;
	};

/* Klasa do obsługi userlisty */
class Userlist
	{
	public:
		Userlist();
		void Add( uin_t aUIN,
				  BString *aName,
				  BString *aSurname,
				  BString *aNick,
				  BString *aDisplay,
				  BString *aPhone,
				  BString *aEmail ); 
		void Remove( Person *aPerson );
		void Replace( Person *aPerson );
		void Import( struct gg_session *aSession, List *aList );
		void Export( struct gg_session *aSession, List *aList );
		bool IsValid();
		void SetValid( bool aValid );
		Person * Find( uin_t aUIN );
		Person * FindMobile( const char *aPhone );
		int	* Set( const char *aContacts );
		const List GetList() const;
		
	private:
		int	Read( BString *aName );
		void ClearStatus( Person *aPerson );
		void Send( struct gg_session *aSession );
		char * Dump();
		void Clear();
		const char * FormatUser( uin_t aUIN );
		char * ReadLine( FILE *aFile );
		List * iList;
		bool iIsValid;
	};

class Profile
	{
	public:
		Profile();
		void Load( BString *aProfile );
		void Save();
		void SetUIN( uin_t aUIN );
		uin_t GetUIN();
		void SetPassword( BString *aPassword );
		const BString GetPassword() const;
		void SetProfileName( BString *aName );
		const BString GetProfileName() const;
		void SetRect( BRect aRect );
		const BRect GetRect() const;
		void SetDescription( BString *aDescription );
		const BString GetDescription() const;
		const Userlist GetUserlist() const;
		int GetAutoStatus();
		bool NeedImport();

	private:
		Userlist * iUserlist;
		BRect iRect;
		uin_t iUIN;
		BString	* iPassword;
		int	iAutoStatus;
		BString	* iName;
		BString * iSurname;
		BString * iNick;
		BString	* iProfileName;
		BString	* iPhone;
		BString	* iEmail;
		BString	* iDescription;
		bool iNeedImport;
	};

#endif /* __BEGADU_PERSON_H__ */
