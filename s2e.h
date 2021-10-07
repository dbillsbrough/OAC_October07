/* s2e.h - Serial To Ethernet header */

/* Copyright (c) 2010, Lantronix, Inc. All rights reserved. */

/*
modification history
--------------------
01b,15jan10,nss  added SSH/SSL.
01a,05aug08,nss  written.
*/

#ifndef _S2E_H
#define _S2E_H

#include <stdio.h>
#include <stdlib.h>
//#include <stdbool.h>
//#include <stddef.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
//#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <sys/ioctl.h>
#include <sys/errno.h>
//#include <unistd.h>
#include <fcntl.h>

#ifdef	INET6
#include <netdb.h>
#endif	/* INET6 */
#ifdef LIBSSH
#include <libssh/libssh.h>
#include <libssh/server.h>
#endif	/* LIBSSH */
#ifdef OPENSSL
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif	/* OPENSSL */

#define	NET_BUF_SIZE	2048	/* size of network buffer */
#define	TTY_BUF_SIZE	2048	/* size of tty buffer */

#ifndef B230400
#define  B230400  0010003
#endif

#ifndef B460800
#define  B460800  0010004
#endif

#ifndef B921600
#define  B921600  0010007
#endif

#define	PERROR(x)	if (debug) perror(x)

#ifdef USE_SSH
#define NET_MAX_USER_LEN		32
#define NET_MAX_PASS_LEN		32
#endif	/* USE_SSH */

/*
 *  config_data holds data retrieved from web page settings
 */

struct config_data
{
	int oamodesel;
	IPADDR4 modemip;
	int  modemport;
	char modemroot[15];
	char modemuser[15];
	char modempass[20];
	long modemsat;
	char modempol;
	long antlock;
	char serialspeed;
	long vscan;
	int trackgain;
	int stepadjust;
	int stepignore;
	long lockthresh;
	char snrthreshold;
	long snrhigh;
	long snrmin;
	DWORD VerifyKey;
} __attribute__( ( packed ) ) ;

extern void recall_to_config_block(int num, struct config_data* configData);
extern void display_profiles( int num, struct profile_labels* Profiles );

struct profile_labels
{
	int curProfile;
	char profile_label_01[20];
	char profile_label_02[20];
	char profile_label_03[20];
	char profile_label_04[20];
	char profile_label_05[20];
	char profile_label_06[20];
	char profile_label_07[20];
	char profile_label_08[20];
	char profile_label_09[20];
	char profile_label_10[20];
} __attribute__( ( packed ) ) ;

extern int modem_mode;
extern int modem_port;
extern struct config_data configData2;


// This is the structure that will be stored in Flash. It is also used to update variables at runtime
// *** WARNING: CHANGE THE VERIFY KEY ANY TIME YOU MAKE A CHANGE TO THIS STRUCTURE ***

struct NV_SettingsStruct
{
      DWORD VerifyKey;  	// Changes when the structure is modified so we can detect the change
      DWORD StructSize; 	// Store structure size
      int currentRecord;
      profile_labels profiles;
      config_data arrData[10];
      char nvChar;			// non volatile char
} __attribute__( ( packed ) ) ;

/*
 * exch_data holds data retrieved from both Aux Remote and from OpenAMIP that will be required by opposite device.
 */
struct exch_data
{
	double snr; // Comes from AMIP 'Ntc_S'; goes out Aux Remote 'SNR' command

	// From 'S' Command
	float satLongitude;	// bird location (- = West; Correct sign usage)
	// From 'H' Command
	double satFreq;
	// Only after we receive both a 'S' and 'H' command will we send the data to Aux via the SAT command.

	// From 'P' Command
	char polTypeFromAmip;
	int  polTypeToAux;

	// From 'L' Command
	bool isLocked;
	bool txIsOn;

	// for 'w' AMIP command
	int wIntervalSecs;
	struct timeval lastLocationToAmip;
	int locIsValid;
	float latitude;
	float longitude;
	unsigned long gpsSecs;
	float altitude;

	// for 's' AMIP command
	int sIntervalSecs;
	struct timeval lastStatusToAmip;
	int antIsFunctional;
	int modemIsFunctional;
	int modemMayTx;

	char displayALL[33];
	char displayLCD1[17];
	char displayLCD2[17];

	struct timeval lastAmipWrite;

	// for STS Aux command
	struct timeval lastStatusReqToAux;
	// for IDD Aux command
	struct timeval lastIddReqToAux;
	// for GPS Aux command
	struct timeval lastGpsReqToAux;

	long antlock = 10000;
	float vscan = 2.00;  // 200 scaled on conversion
	int trackgain = 1015;
	int stepadjust = 25;
	int stepignore = 0;
	float lockthresh = 1.00;
	char snrthreshold = '0';
	float snrhigh = 0.00;
	float snrmid = 0.00;

} __attribute__( ( packed ) ) ;

// extern int read_config (const char *conf_file, struct s2e_t *conf_data);
extern int LatchTime( struct timeval* tmr );
extern bool TimerElapsed( struct timeval* tmr, double checkTime );
extern int net_write(int fdnet, char *buffer, int size);
extern int net_focusMission_read(int, char *);

// amip.c functions
extern void Amip_InitNewConnection( struct exch_data* exchData );
extern void Amip_ProcessRxBytes( char* newBytes, int newByteCnt, struct exch_data* exchData );
extern void Amip_SendTimerMsgs( struct exch_data* exchData );
extern int Amip_SendAntennaStatus( struct exch_data* exchData );
extern int Amip_SendLocation(  struct exch_data* exchData );
extern int Amip_SetSendLockInterval( struct exch_data* exchData, int intervalSecs );
extern int Amip_SetSendSnrInterval( struct exch_data* exchData, int interval_mSec );
extern int fdnet;

// auxRem.c functions
extern void Aux_ProcessRxBytes( char* newBytes, int newByteCnt, struct exch_data* exchData );
extern void Aux_SendTimerMsgs( struct exch_data* exchData );
extern void Aux_QuerySts( struct exch_data* exchData );
extern void Aux_QueryIdd( struct exch_data* exchData );
extern void Aux_QueryGps( struct exch_data* exchData );
extern void Aux_SendSnr( struct exch_data* exchData );
extern void Aux_SendLOK( struct exch_data* exchData );
extern void Aux_SendSat( struct exch_data* exchData );
extern void Aux_SendSCN( struct exch_data* exchData );
extern void Aux_SendPolType( struct exch_data* exchData );
extern void Aux_SendTRK( struct exch_data* exchData );
extern void Aux_SendLTE( struct exch_data* exchData );
extern void Aux_SendSIP( struct exch_data* exchData );

extern void Aux_TerminateAndSend();

// second serial
void Serial_ProcessRxBytes( int fd, char* newBytes, int newByteCnt, struct exch_data* exchData, struct config_data* configData);

extern int debug;
extern int net_socket;

#endif	/* _S2E_H */
