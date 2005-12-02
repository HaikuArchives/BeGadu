/*
 * ============================================================================
 *  Nazwa    : Msg z Msg.h
 *  Projekt  : BeGadu
 *  Authorzy : 
 *		Artur Wyszynski <artur.wyszynski@bellstream.pl>
 *  Opis:
 *		Rozne stale i definicje
 *  Version  : 1.2
 * ============================================================================
 */

#ifndef __BEGADU_MSG_H__
#define __BEGADU_MSG_H__

/* messages from libgadu */
#define GOT_MESSAGE				'0000'
#define GOT_PEOPLE				'0001'
#define ADD_HANDLER				'0002'
#define DEL_HANDLER				'0003'

/* messages to interface */
#define UPDATE_STATUS			'0004'
#define UPDATE_LIST				'0005'
#define SHOW_MESSAGE			'0006'

/* messages from interface */
#define DO_LOGIN				'0007'
#define DO_LOGOUT				'0008'
#define ADD_PERSON				'0009'
#define DEL_PERSON				'0010'
#define OPEN_MESSAGE			'0011'
#define SEND_MESSAGE			'0012'
#define CLOSE_MESSAGE			'0013'

/* local messages from interface */
#define BEGG_PREFERENCES		'0014'
#define BEGG_ABOUT				'0015'
#define BEGG_PERSON_SELECTED	'0016'
#define	BEGG_PERSON_ACTION		'0017'
#define BEGG_SEND				'0018'
#define SET_AVAIL				'0019'
#define SET_BRB					'0020'
#define	SET_INVIS				'0021'
#define SET_NOT_AVAIL			'0022'
#define SET_DESCRIPTION			'0023'
#define BEGG_IMPORT_LIST		'0024'
#define BEGG_EXPORT_LIST		'0025'
#define BEGG_QUIT				'0026'

/* misc */
#define BEGG_CONNECTING			'0027'
#define PREFERENCES_OK			'0028'
#define PREFERENCES_CANCEL		'0029'
#define PROFILE_CREATED			'0030'
#define OPEN_PROFILE_WIZARD 	'0031'
#define CONFIG_OK				'0032'
#define SHOW_MAIN_WINDOW		'0033'
#define BGDESKBAR_CHSTATE		'0034'
#define ADD_MESSENGER			'0035'
#define DEL_MESSENGER			'0036'
#define CHANGE_DESCRIPTION		'0037'
#define DESCRIPTION_OK			'0038'
#define DESCRIPTION_CANCEL		'0039'

#define ONLINE_SOUND			"BeGadu state changed"
#define MESSAGE_SOUND			"BeGadu new message"

#endif /* __BEGADU_MSG_H__ */
