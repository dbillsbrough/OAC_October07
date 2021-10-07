/*
 * main.cpp
 *
 *  Created on: August 19, 2021
 *      Author: David Billsbrough
 */


/* Revision: 0.0.1 */


#include <predef.h>
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <serial.h>
#include <iosys.h>
#include <utils.h>
#include <ip.h>
#include <tcp.h>
#include <autoupdate.h>
#include <taskmon.h>
#include <dhcpclient.h>
#include <signal.h>
#include <multipartpost.h>
#include <streamupdate.h>
#include <init.h>
#include <fdprintf.h>
#include "s2e.h"
#include <iosys.h>

#define DELAY5 0.500
#define MAXCOUNT 50

int debug = 0;
int multi = 0;

struct exch_data exchData2;
struct config_data configData2;
//struct NV_SettingsStruct NV_Settings;

int fd1;
int fdserial;
int fdnull;
int modem_mode;
int modem_port;

static int mainloopCounter = MAXCOUNT;
bool NeedsRebooted = false;
bool OnlyDoneOnce = false;

extern void displayIDD(char *);

static char buffer2[TTY_BUF_SIZE];
static char buffer3[TTY_BUF_SIZE];

char *extra_name;

/* function prototypes */
extern void User2Main(void);
extern void User3Main(void);
extern void User4Main(void);
extern void User5Main(void);
extern void User6Main(void);
extern void User7Main(void);
extern void CleanUpWS(void);
extern int DoWSUpgrade(HTTP_Request *, int, PSTR, PSTR);
extern void SaveParams(void);
extern void usage(int argc, char *argv[]);

extern int ws_fd;
extern http_wshandler *TheWSHandler;

char current_profilename[20];
int currentRec;

#define READ_SERIAL_TIMEOUT (1)

/*
 * Since this may be used for machine to machine connections we allow you
 * to easily change the messages that are sent.
 *
 * MTS messages are Message To Serial
 */

#define MTS_WHEN_NOTCONNECTED  "Waiting for connection \r\n"
#define MTS_WHEN_CONNECTION_OPENED  "New Connection Opened\r\n"
#define MTS_WHEN_CONNECTION_CLOSED "Connection Closed by Network \r\n"
#define MTS_WHEN_CONNECTION_TIMEDOUT "Connection Timed out and Closed\r\n"
#define MTS_WHEN_CONNECTION_OVERIDDEN "This Connection is being Overidden.\r\n"

#define SERIALPORT_TO_USE (1) //0 for the main port, 1 for the 10pin aux serial port
#define BAUDRATE_TO_USE (9600)
#define STOP_BITS (1)
#define DATA_BITS (8)

#define SERIALPORT_TO_USE1 (0) //0 for the main port, 1 for the 10pin aux serial port
#define BAUDRATE_TO_USE1 (9600)
#define STOP_BITS1 (1)
#define DATA_BITS1 (8)

extern "C"
{
   void UserMain( void *pd );
}

const char *AppName = "Legacy Mode2 Pathfinder";

void UserMain( void *pd )
{
	struct exch_data* exchData;
	exchData = &exchData2;

	struct config_data* configData;
	configData = &(configData2);

	extern struct NV_SettingsStruct NV_Settings;

	InitializeStack();
	initWithWeb(); // Makes things better!
	OSChangePrio( 49 );
	EnableAutoUpdate();
	EnableTaskMonitor();

	User5Main();

	currentRec = NV_Settings.currentRecord;
	int cur = currentRec - 1;
	//iprintf("status is %d\n", currentRec);

	long baudrate2 = BAUDRATE_TO_USE1;

	if (NV_Settings.arrData[cur].serialspeed == '1') {
		iprintf("Setting baud rate to 9600.\n");
		baudrate2 = 9600;
	} else if (NV_Settings.arrData[cur].serialspeed == '2') {
		iprintf("Setting baud rate to 115200.\n");
		baudrate2 = 115200;
	} else {
		iprintf("Setting baud rate to 4800.\n");
		baudrate2 = 4800;
	}

	//Close the serial port incase it is already open.
	SerialClose( SERIALPORT_TO_USE );
	SerialClose( SERIALPORT_TO_USE1 );

	//Open the serial port
	fdserial = OpenSerial( SERIALPORT_TO_USE, BAUDRATE_TO_USE, STOP_BITS, DATA_BITS, eParityNone );
	fd1 = OpenSerial( SERIALPORT_TO_USE1, baudrate2, STOP_BITS1, DATA_BITS1, eParityNone );

	//ReplaceStdio( 0, fdnull ); //stdin
	//ReplaceStdio( 1, fdnull ); //stdout aka printf etc...
	//ReplaceStdio( 2, fdnull ); //stderror

	exchData->modemIsFunctional = 0;
	exchData->snr = -999.9;

	modem_mode = NV_Settings.arrData[cur].oamodesel;  /* please re-visit */
	modem_port = NV_Settings.arrData[cur].modemport;
	exchData->antlock = NV_Settings.arrData[cur].antlock;
	exchData->satLongitude = (float) NV_Settings.arrData[cur].modemsat;
	exchData->polTypeFromAmip = NV_Settings.arrData[cur].modempol;
	exchData->vscan = (float) NV_Settings.arrData[cur].vscan / 100L;  // 2.00 scaled
	exchData->trackgain = NV_Settings.arrData[cur].trackgain;
	exchData->stepadjust = NV_Settings.arrData[cur].stepadjust;
	exchData->stepignore = NV_Settings.arrData[cur].stepignore;
	exchData->lockthresh = (float) NV_Settings.arrData[cur].lockthresh / 100L;
	exchData->snrthreshold = NV_Settings.arrData[cur].snrthreshold;			// snrthreshold
	exchData->snrhigh = (float) NV_Settings.arrData[cur].snrhigh / 100L;	// long snrhigh
	exchData->snrmid = (float) NV_Settings.arrData[cur].snrmin / 100L;		// long snrmid

	strcpy(current_profilename, "*****  BOGUS  *****");

	iprintf("the current record is %d\n", currentRec);
	iprintf("the current modem selected is %d\n", modem_mode);
	iprintf("the antenna lock is %ld\n", exchData->antlock);

	User2Main();
	User3Main();
	User7Main();
	User6Main();
	User4Main();

	fd_set read_fds;
	fd_set error_fds;

	FD_ZERO( &read_fds );
	FD_ZERO( &error_fds );

	Aux_SendLOK( exchData );
	Aux_SendSCN( exchData );
	Aux_SendTRK( exchData );
	Aux_SendLTE( exchData );

	Aux_SendSIP( exchData );

	// setup the manual configuration of Satellite longitude

	if ( modem_mode == 2 || modem_mode == 3 ) {
		Amip_InitNewConnection( exchData );
		exchData->satLongitude = (float) NV_Settings.arrData[currentRec-1].modemsat / 100L;
		Aux_SendSat( exchData );

		// 0 = Horz 1 = Vert 2 = No Pol (if used

		if (NV_Settings.arrData[currentRec-1].modempol == 'H') {
			exchData->polTypeToAux = 0;
		} else if (NV_Settings.arrData[currentRec-1].modempol == 'V') {
			exchData->polTypeToAux = 1;
		} else {
			exchData->polTypeToAux = 2;
		}
		Aux_SendPolType( exchData );
	}

	// NeedsRebooted = true;

	while (! NeedsRebooted) {

		FD_ZERO(&read_fds);
		FD_SET (fdserial, &read_fds);
		FD_SET (fd1, &read_fds);

		if ( FD_ISSET( fdserial, &read_fds )) {
			int readLen2 = -1;

			// 13.3 ReadWithTimeout -- page 159  (*** HDB)
			if ((readLen2 = ReadWithTimeout(fdserial, buffer2, sizeof(buffer2 - 1), READ_SERIAL_TIMEOUT)) < 0) {
				perror(NULL);
				printf("TTY Read Error: %d\r\n", readLen2);
				close(fdserial);
				continue;
			} else if (readLen2 == 0) {
				//write( fdserial, "?02STS\r\n", 8 );
				//iprintf("sending to first serial port status request\n");
			} else if (readLen2 > 0) {
				if (readLen2 < (int) sizeof(buffer2))
					buffer2[readLen2] = 0;
				else
					buffer2[sizeof(buffer2 - 1)] = 0;
			}
			//iprintf("processing aux bytes%s\n", buffer2);
			Aux_ProcessRxBytes( buffer2, readLen2, exchData);
		}

		// Check for any required messages to send to OpenAMIP client.
		//Amip_SendTimerMsgs( exchData );

		// Check for any required messages to send to Aux Remote.
		Aux_SendTimerMsgs( exchData );

		// Send Tcp test write if nothing has been written for 2 seconds
		if ( TimerElapsed( &(exchData->lastAmipWrite), 2.0 ) ) {
			LatchTime( &(exchData->lastAmipWrite) );
		}

		// Sleep for loop rest unless rx arrives from either AMIP or Aux Remote



		if (FD_ISSET( fd1, &read_fds )) {

			int readLen3 = -1;

			if ((readLen3 = ReadWithTimeout(fd1, buffer3, sizeof(buffer3 - 1), READ_SERIAL_TIMEOUT)) < 0) {
				perror(NULL);
				printf("TTY 2 Read Error: %d\r\n", readLen3);
				close(fd1);
			} else if (readLen3 == 0) {
				mainloopCounter--;
				if (mainloopCounter <= 0) {   // Make this a counter
					write( fd1, "\r\n", 2 );
					mainloopCounter = MAXCOUNT;
				}
				//iprintf("sending extra CRLFs\n");
			} else if (readLen3 > 0) {
				if (readLen3 < (int) sizeof(buffer3))
					buffer3[readLen3] = 0;
				else
					buffer3[sizeof(buffer3 - 1)] = 0;

				Serial_ProcessRxBytes( fd1, buffer3, readLen3, exchData, configData );
			}
		}

		// bypass_serial:			/* label to jump to to bypass read from Modem serial port */

		if (exchData->snr > (float) -101.0) {
			exchData->modemIsFunctional = 1;
		}
	}
}

/* End of File */
