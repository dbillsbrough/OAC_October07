/*
 * secondserial.cpp
 *
 *  Created on: June 30, 2021
 *      Author: David Billsbrough
 */

#include <predef.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <startnet.h>
#include <serial.h>
#include <iosys.h>
#include <utils.h>
#include "s2e.h"

#define SER2_TX_BUFF_SIZE	128
#define SER2_RX_BUFF_SIZE	2048

//extern struct exch_data exchData;
static char ser2RxBuff[SER2_RX_BUFF_SIZE]; // piece together and process rx bytes
extern int fdserial2;
extern int fd1;
extern int currentRec;
static char foundreply[30];
double response_snr;

extern char buffer3[TTY_BUF_SIZE];
extern struct NV_SettingsStruct NV_Settings;

#define AUX_MAX_ARGS	10
typedef char* ArgPtr;

/* function prototypes */

int match_pattern(char *line);
int match_pattern2(char *line);
bool ser_CmdIsValid( char* auxCmdStr );
void Serial_ProcessRxBytes( int fd, char* newBytes, int newByteCnt, struct exch_data* exchData, struct config_data* configData);
void Ser2_ParseSingleRxMessage( int fd1, char* serCmdStr, struct exch_data* exchData, struct config_data* configData );
void Ser2_ParseSingleRxMessage1( int fd1, char* serCmdStr, struct exch_data* exchData, struct config_data* configData );
void Ser2_ParseSingleRxMessage2( int fd1, char* serCmdStr, struct exch_data* exchData, struct config_data* configData );
void substring(char s[], char sub[], int p, int l);

//extern "C" {
//	void User6Main(void);
//}

extern char buffer3[TTY_BUF_SIZE];

bool ser_CmdIsValid( char* auxCmdStr )
{
	return true;
}

int match_pattern(char *line)
{
	//bool did_match = false;
	char *havefound;
	char searchstr[400];
	int fcode = -1;

	const char *match0 = "9series";
	const char *match1 = "ogin:";
	const char *match2 = "assword:";
	const char *match3 = "root@iDirect";
	const char *match4 = "9350";
	const char *match5 = " snr: ";

	havefound = (char*) malloc (400);

	strcpy(searchstr, line);

	if ((havefound = strstr(searchstr, match0))) {
		//printf("found: %s\n", havefound);
		fcode = 0;
	}

	if ((havefound = strstr(searchstr, match1))) {
		//printf("found: %s\n", havefound);
		fcode = 1;
	}

	if ((havefound = strstr(searchstr, match2))) {
		//printf("found: %s\n", havefound);
		fcode = 2;
	}

	if ((havefound = strstr(searchstr, match3))) {
		//printf("found: %s\n", havefound);
		fcode = 3;
	}

	if ((havefound = strstr(searchstr, match4))) {
		//printf("found: %s\n", havefound);
		fcode = 4;
	}

	if ((havefound = strstr(searchstr, match5))) {
		//printf("found: %s\n", havefound);
		strcpy(foundreply, havefound);
		fcode = 5;
	}

	free(havefound);

	//if ((!did_match) && (localcount <= 1)) {
	//	write( fd1, "\r\n", 2);
	//	localcount = 50000;
	//	// printf("\n");
	//}

	return fcode;
}

int match_pattern2(char *line)
{
	//bool did_match = false;
	char *havefound;
	char searchstr[400];
	int fcode = -1;

	const char *match20 = "armv";
	const char *match21 = "login:";
	const char *match22 = "assword:";
	const char *match23 = "# ";
	const char *match24 = "Username:";
	const char *match25 = "> ";
	const char *match26 = "Rx SNR:";

	havefound = (char*) malloc (400);

	strcpy(searchstr, line);

	if ((havefound = strstr(searchstr, match20))) {
		//printf("found: %s\n", havefound);
		//did_match = true;
		fcode = 0;
	}

	if ((havefound = strstr(searchstr, match21))) {
		//printf("found: %s\n", havefound);
		//did_match = true;
		fcode = 1;
	}

	if ((havefound = strstr(searchstr, match22))) {
		//printf("found: %s\n", havefound);
		//did_match = true;
		fcode = 2;
	}

	if ((havefound = strstr(searchstr, match23)) && strlen(searchstr) == 2) {
		//printf("found: %s\n", havefound);
		//did_match = true;
		fcode = 3;
	}

	if ((havefound = strstr(searchstr, match24))) {
		//printf("found: %s\n", havefound);
		fcode = 4;
	}

	if ((havefound = strstr(searchstr, match25)) && strlen(searchstr) == 2) {
		//printf("found: %s\n", havefound);
		fcode = 5;
	}

	if ((havefound = strstr(searchstr, match26))) {
		//printf("found: %s\n", havefound);
		//did_match = true;
		strcpy(foundreply, havefound);
		fcode = 6;
	}

	free(havefound);

	return fcode;
}


void Ser2_ParseSingleRxMessage( int fd1, char* ser2RxBuff, struct exch_data* exchData, struct config_data* configData )
{
	//struct config_data* configData;

	//configData = &configData2;

	if (modem_mode == 1 || modem_mode == 3) {
		//iprintf("parse type: 1\r\n");
		Ser2_ParseSingleRxMessage1( fd1, ser2RxBuff, exchData, configData );
	} else if (modem_mode == 2) {
		//iprintf("parse type: 2\r\n");
		Ser2_ParseSingleRxMessage2( fd1, ser2RxBuff, exchData, configData );
	}
}

void Ser2_ParseSingleRxMessage1( int fd1, char* serCmdStr, struct exch_data* exchData, struct config_data* configData )
{
	char answer[15];
	static long loopCounter = 0;
	double tempf;

	//configData = &configData2;

	if ( ser_CmdIsValid( serCmdStr ) ) {

		//printf( "\nGot data: '%s'\r\n", serCmdStr );

		int retcode = match_pattern(serCmdStr);

		if (retcode == 0) {
			OSTimeDly(20);
			//strcpy((buffer3), "\0");
			return;
		}

		if (retcode == 1) {
			writestring( fd1, "root\r");
			OSTimeDly(15);
			writestring( fd1, NV_Settings.arrData[currentRec-1].modemroot );
			writestring( fd1, "\r" );
			OSTimeDly(15);
			//writestring( fd1, "\r");
			OSTimeDly(5);
			writestring( fd1, "export TERM=dumb\r");
			OSTimeDly(5);
		}

		//if (retcode == 2) {
		//  printf ("Hi");
		//	writestring( fd1, "P@55w0rd!\r\n" );
		//}

		if (retcode == 3) {
			writestring( fd1, "mewsh\r" );
			OSTimeDly(15);
			writestring( fd1, "beam_lock\r" );
			OSTimeDly(15);
			//writestring( fd1, "\r" );
		}

		if (retcode == 4) {
			OSTimeDly(1);
			//++loopCounter;
			//if (loopCounter == 4) {
			writestring( fd1, "rx_snr\n" );
			//	loopCounter = 0;
			//}
		}

		if (retcode == 5) {
			//OSTimeDly(3);
			//printf("The SNR response is %s\n", foundreply);
	        substring(foundreply, answer, 7, 4);
			sscanf(answer, "%lf", &tempf);
			response_snr = roundf(tempf * 100.0f) / 100.0f;
			//printf("The SNR is %0.3lf\n", response_snr);
			exchData->snr = response_snr;
			Aux_SendSnr( exchData );
		}
	}
}

void Ser2_ParseSingleRxMessage2( int fd1, char* serCmdStr, struct exch_data* exchData, struct config_data* configData )
{
	char answer[15];
	static long loopCounter = 0;

	if ( ser_CmdIsValid( serCmdStr ) ) {

		//OSTimeDly(1);
		//printf( "\nGot data: '%s'\r\n", serCmdStr );

		int retcode = match_pattern2(serCmdStr);

		if (retcode == 0) {
			OSTimeDly(2);
			writestring( fd1, "root\r");
			OSTimeDly(5);
			//writestring( fd1, configData->modemroot );
			//writestring( fd1, "\r" );
		}

		if (retcode == 1) {
			OSTimeDly(2);
			writestring( fd1, "root\r");
			OSTimeDly(5);
			//writestring( fd1, configData->modemroot );
			//writestring( fd1, "\r" );
		}

		if (retcode == 2) {
			//writestring( fd1, configData.modemroot );
			//writestring( fd1, "iDirect\n" );
			OSTimeDly(2);
			return;
		}

		if (retcode == 3) {
			//writestring( fd1, "telnet localhost\r" );
			OSTimeDly(2);
			writestring( fd1, NV_Settings.arrData[currentRec-1].modemuser );
			writestring( fd1, "\r" );
			OSTimeDly(2);
			writestring( fd1, NV_Settings.arrData[currentRec-1].modempass );
			writestring( fd1, "\r" );
			OSTimeDly(2);
			writestring( fd1, "csp enable\r" );
		}

		if (retcode == 4) {
			OSTimeDly(6);
			writestring( fd1, NV_Settings.arrData[currentRec-1].modemuser);
			writestring( fd1, "\r" );
			OSTimeDly(2);
			writestring( fd1, NV_Settings.arrData[currentRec-1].modempass );
			writestring( fd1, "\r" );
			OSTimeDly(2);
			writestring( fd1, "csp enable\r" );
		}

		if (retcode == 5) {
			OSTimeDly(1);
			++loopCounter;
			if (loopCounter == 4) {
				writestring( fd1, "rx snr\r\n");
				loopCounter = 0;
			}
			//Aux_SendSnr( exchData );
		}

		if (retcode == 6 ) {
			//printf("The SNR response is %s\n", foundreply);
	        substring(foundreply, answer, 8, 6);
			sscanf(answer, "%lf", &response_snr);
			//iprintf("## The SNR is %0.3lf\n", response_snr);
			exchData->snr = response_snr;
			//iprintf("snr is %lf\n", exchData->snr);
			Aux_SendSnr( exchData );
			writestring( fd1, "rx snr\r\n");
		}
	}
}

// C substring function definition

void substring(char s[], char sub[], int p, int l)
{
	int c = 0;

	while (c < l) {
		sub[c] = s[p+c-1];
		c++;
	}
	sub[c] = '\0';
}

void Serial_ProcessRxBytes( int fd1, char* newBytes, int newByteCnt, struct exch_data* exchData, struct config_data* configData)
{
	char* newPtr = newBytes;
	char* nextLf;
	char* nextCr;
	char* nextLfCr;
	int availSpace, reqSpace;

	// Loop through bytes using CR/LF as delimiter between AUX commands.
	while ( newPtr < (newBytes + newByteCnt) ) {

		// Look for next CR or LF
		nextLf = strchr( newPtr, '\n' );
		nextCr = strchr( newPtr, '\r' );
		if ( ( nextLf == NULL ) && ( nextCr == NULL ) )
			nextLfCr = NULL;
		else if ( nextLf == NULL )
			nextLfCr = nextCr;
		else if ( nextCr == NULL )
			nextLfCr = nextLf;
		else if ( nextCr < nextLf )
			nextLfCr = nextCr;
		else
			nextLfCr = nextLf;

		if ( nextLfCr == NULL ) {
			// We don't have a complete msg (No CR or LF) to parse.
			if ( (strlen( ser2RxBuff) + strlen( newPtr )) < SER2_RX_BUFF_SIZE ) {
				// Save bytes for until remainder arrives
				strcat( ser2RxBuff, newPtr );
			} else {
				// We can't save over 2048 bytes.  Dump everything
				ser2RxBuff[0] = 0; // Empty
			}
			// Get out of message search loop
			newPtr = newBytes + newByteCnt; // Advance past newBytes buffer
		} else {
			// We have a CR/LF terminated message, so process it!
			availSpace = SER2_RX_BUFF_SIZE - strlen( ser2RxBuff ) - 1; // -1 leaves space for term 0
			reqSpace = nextLfCr - newPtr;

			if ( reqSpace <= availSpace ) {
				strncat( ser2RxBuff, newPtr, reqSpace );
				Ser2_ParseSingleRxMessage( fd1, ser2RxBuff, exchData, configData );
				//iprintf("single msg: %s\n", ser2RxBuff);
			}

			ser2RxBuff[0] = 0; // Mark empty for next pass
			newPtr = nextLfCr; // Advance past processed bytes

			while ( (*newPtr == '\r') || (*newPtr == '\n') )
				newPtr++; // Advance past CR and LF
		}
	}
}

/* End of File */
