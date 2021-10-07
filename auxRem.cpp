/* auxRem.c - Aus Remote functions */

/*
 * KSI - Martin Kleinpeter
 * 
 * 9/22/2020 - Initial creation.
 */

#include <predef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <constants.h>
//#include <ctype.h>
#include <startnet.h>
#include "s2e.h"

#define DELAY5 0.500
#define AUX_TX_BUFF_SIZE	128
static char auxTxBuff[ AUX_TX_BUFF_SIZE ];
#define AUX_RX_BUFF_SIZE	2048
static char auxRxBuff[AUX_RX_BUFF_SIZE]; // piece together and process rx bytes
#define AUX_MAX_ARGS	11
typedef char* ArgPtr;

extern int ws_fd;
extern int fdnet;
extern int fdserial;
extern int fd1;
extern int modem_mode;
extern int currentRec;
extern struct config_data configData2;
//extern struct exch_data exchData;
extern OS_SEM waitingForWS;
static char displayLine[33];
extern void displayIDD(char *);
extern void UserTask(void * pd);
extern void SendConfigReport(int ws_fd, char *message, struct exch_data* exchData);

unsigned char Aux_CalcCheckSum( const char* msg )
{
	unsigned char chkSum = 0;

	for (int i = 0; i < (int) strlen(msg); i++)
	{
		chkSum ^= msg[i];
	}

	return( chkSum );
}

void Aux_TerminateAndSend()
{
	unsigned char checkSum = Aux_CalcCheckSum( auxTxBuff );
	char hexBuff[10];
	sprintf( hexBuff, "%02X", checkSum );

	if ((AUX_TX_BUFF_SIZE - strlen( auxTxBuff )) > 6 )
	{
		strcat( auxTxBuff, ";" ); // End char
		strcat( auxTxBuff, hexBuff ); // Checksum
		strcat( auxTxBuff, "\r\n" );	// CR

		if ( write( fdserial, auxTxBuff, strlen( auxTxBuff ) ) <= 0) {
			iprintf("sent: %s\r\n", auxTxBuff);
			close(fdserial);
		}
	}
}

bool Aux_CmdIsValid( char* auxCmdStr )
{
	bool isValid = true;
	int len = strlen( auxCmdStr );

	if ( strlen( auxCmdStr ) < 7 )
		isValid = false;
	// Check required start
	else if ( strncmp( auxCmdStr, "<00", 3 ) != 0 )
		isValid = false;
	else
	{
		if ( auxCmdStr[len-1] == ';' )
			auxCmdStr[len-1] = 0; // Omit terminator from processing
		else if ( auxCmdStr[len-3] == ';' )
		{
			auxCmdStr[len-3] = 0;
			unsigned char calcCheckSum = Aux_CalcCheckSum( auxCmdStr );
			unsigned char stringsChkSum = strtoul( &(auxCmdStr[len-2]), NULL, 16 );
			if ( calcCheckSum != stringsChkSum )
				isValid = false;
		}
		else
			isValid = false; // No terminator at end or before 2 char checksum
	}

	return( isValid );
}

void Aux_RxSTS( char* cmdIndex, int argCnt, ArgPtr* argPtrs, struct exch_data* exchData )
{
	int newTxPerm = 0;

	exchData->antIsFunctional = 1; // Antenna is talking

	if ( argCnt > 1 )
	{
		int state = atoi( argPtrs[1] );
		if ( state == 100 )
			newTxPerm = 1;
	}

	if ( newTxPerm != exchData->modemMayTx )
	{
		exchData->modemMayTx = newTxPerm;
		Amip_SendAntennaStatus(exchData);
	}
}

void Aux_RxIDD( char* cmdIndex, int argCnt, ArgPtr* argPtrs, struct exch_data* exchData )
{
	strcpy(displayLine, "");

	if ( argCnt >= 1 ) {
		strcpy(displayLine, argPtrs[0]);
	}

	//iprintf("Sending %s ...\r\n", displayLine);
	// iprintf("IDD arg count is %d\n", argCnt);
	//iprintf("IDD is %s\n", displayLine);
	// displayIDD(displayLine);

	SendConfigReport(ws_fd, displayLine, exchData);
}


void Aux_RxGGA( char* cmdIndex, int argCnt, ArgPtr* argPtrs, struct exch_data* exchData )
{
	int newValid = 0;
	char buf[40];
	float newLat = 0.0;
	float newLong = 0.0;
	unsigned long newTime = 0;
	float newAltitude = 0.0;
	char NS;
	char WE;

	//printf( "GGA\r\n" );
	//for( int i=0; i<argCnt; i++ )
	//{
	//	printf( "   arg: %d - '%s'\r\n", i, argPtrs[i] );
	//}

	if ( argCnt > 9 )
	{
		newTime = atol( argPtrs[1] );
		newLat = atof( argPtrs[2] );
		// Now convert DDMM.MMMM to DD.DDDD
		int intDeg = (int) newLat / 100;
		float fracDeg = (newLat - (float) (intDeg*100)) / 60.0;
		newLat = (float) intDeg + fracDeg;
		if ( ( argPtrs[3][0] == 'S' ) || ( argPtrs[3][0] == 's' ) )
			newLat = -newLat;
		newLong = atof( argPtrs[4] );
		// Now convert DDMM.MMMM to DD.DDDD
		intDeg = (int) newLong / 100;
		fracDeg = (newLong - (float) (intDeg*100)) / 60.0;
		newLong = (float) intDeg + fracDeg;
		if ( ( argPtrs[5][0] == 'W' ) || ( argPtrs[5][0] == 'w' ) )
			newLong = -newLong;
		if ( argPtrs[6][0] == '0' )
			newValid = 0;
		else
			newValid = 1;
		newAltitude = atof( argPtrs[9] );

		exchData->gpsSecs = newTime;
		exchData->altitude = newAltitude;
	}

	if ( ( newValid != exchData->locIsValid ) ||
	     ( newLat != exchData->latitude ) ||
	     ( newLong != exchData->longitude ) )
	{
		exchData->locIsValid = newValid;
		exchData->latitude = newLat;
		exchData->longitude = newLong;
		if (newLat >= 0.0) NS = 'N'; else NS = 'S';
		if (newLong >= 0.0) WE = 'E'; else WE = 'W';
		newLat = abs(newLat);
		newLong = abs(newLong);
		if (modem_mode == 2) {
			// do something different
			sprintf(buf, "latlong %1.3f %c %1.3f %c\r\n", newLat, NS, newLong, WE);
			write(fd1, buf, strlen(buf));
		} else {
			Amip_SendLocation( exchData );
		}
	}
}

void Aux_ParseSingleRxMessage( char* auxCmdStr, struct exch_data* exchData )
{
	char  *auxCmd;
	char  auxCmdIndex[5];
	int argCnt = 0;
	ArgPtr argPtrs[AUX_MAX_ARGS];
	// char* nextArg;
	int searchIndex;
	int origLen;

	if ( Aux_CmdIsValid( auxCmdStr ) )
	{
		//printf( "Got Aux Rx: '%s'\r\n", auxCmdStr );
		// Because of GGA possible multiple commas, do custom parse rather than strtok parse.
		auxCmd = &(auxCmdStr[3]);
		//auxCmd = strtok( &(auxCmdStr[3]), "," );
		if ( auxCmd != NULL )
		{
			origLen = strlen( auxCmdStr );
			for ( argCnt=0, searchIndex = 3; 
				( searchIndex < origLen ); 
				searchIndex++ )
			{
				if ( auxCmdStr[ searchIndex ] == ',' )
				{
					if ( argCnt < AUX_MAX_ARGS )
					{
						argPtrs[ argCnt ] = &(auxCmdStr[ searchIndex+1 ]);
						argCnt++;
					}
					auxCmdStr[ searchIndex ] = 0; // Term previous arg
				}
			}

			// Check if Cmd has Index
			if ( strlen( auxCmd ) > 3 )
			{
				strncpy( auxCmdIndex, &(auxCmd[3]), 3 );
				auxCmdIndex[2] = 0;	// Index is max of 2 chars
				auxCmd[3] = 0; // Only 1st 3 chars are command
			}
			else
				auxCmdIndex[0] = 0; // Empty string (NO Index)

			// Because of GGA possible multiple commas, do custom parse rather than strtok parse.
			//while ( ( (nextArg = strtok( NULL, "," )) != NULL ) && ( argCnt < AUX_MAX_ARGS ) )
			//{
			//	argPtrs[ argCnt ] = nextArg;
			//	argCnt++;
			//}

			//printf( "Cmd: %s  Arg Count: %d\r\n", auxCmd, argCnt );

			if ( strcmp( auxCmd, "STS" ) == 0 )
				Aux_RxSTS( auxCmdIndex, argCnt, argPtrs, exchData );
			else if ( strcmp( auxCmd, "IDD" ) == 0 )
				Aux_RxIDD( auxCmdIndex, argCnt, argPtrs, exchData );
			else if ( strcmp( auxCmd, "GGA" ) == 0 )
				Aux_RxGGA( auxCmdIndex, argCnt, argPtrs, exchData );
		}
	}
}

void Aux_ProcessRxBytes( char* newBytes, int newByteCnt, struct exch_data* exchData )
{
	char* newPtr = newBytes;
	char* nextLf;
	char* nextCr;
	char* nextLfCr;
	int availSpace, reqSpace;

	// Loop through bytes using CR/LF as delimiter between AUX commands.
	while ( newPtr < (newBytes + newByteCnt) )
	{
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

		if ( nextLfCr == NULL )
		{
			// We don't have a complete msg (No CR or LF) to parse.
			if ( (strlen( auxRxBuff) + strlen( newPtr )) < AUX_RX_BUFF_SIZE )
			{
				// Save bytes for until remainder arrives
				strcat( auxRxBuff, newPtr );
			}
			else
			{
				// We can't save over 2048 bytes.  Dump everything
				auxRxBuff[0] = 0; // Empty
			}
			// Get out of message search loop
			newPtr = newBytes + newByteCnt; // Advance past newBytes buffer
		}
		else
		{
			// We have a CR/LF terminated message, so process it!
			availSpace = AUX_RX_BUFF_SIZE - strlen( auxRxBuff ) - 1; // -1 leaves space for term 0
			reqSpace = nextLfCr - newPtr;
			if ( reqSpace <= availSpace ) {
				strncat( auxRxBuff, newPtr, reqSpace );
				Aux_ParseSingleRxMessage( auxRxBuff, exchData );
				//iprintf("single msg: %s\n", auxRxBuff);
			}

			auxRxBuff[0] = 0; // Mark empty for next pass
			newPtr = nextLfCr; // Advance past processed bytes
			while ( (*newPtr == '\r') || (*newPtr == '\n') )
				newPtr++; // Advance past CR and LF
		}
	}
}

void Aux_SendTimerMsgs( struct exch_data* exchData )
{
	if ( TimerElapsed( &(exchData->lastStatusReqToAux), 10.0 )) { /* *** change back to 1 */
		//OSTimeDly(5);
		Aux_QuerySts( exchData );
	}
	if ( TimerElapsed( &(exchData->lastIddReqToAux), 5.0 )) {
		//OSTimeDly(5);
		Aux_QueryIdd( exchData );
	}
	if ( TimerElapsed( &(exchData->lastGpsReqToAux), 60.0 )) {
		//OSTimeDly (5);
		Aux_QueryGps( exchData );
		Aux_SendSIP( exchData );
	}
}

void Aux_QuerySts( struct exch_data* exchData )
{
	strcpy( auxTxBuff, "?02STS" );
	Aux_TerminateAndSend();
	LatchTime( &(exchData->lastStatusReqToAux) );
	//iprintf("sending STS request\n");
}

void Aux_QueryIdd( struct exch_data* exchData )
{
	strcpy( auxTxBuff, "?02IDD" );
	Aux_TerminateAndSend();
	LatchTime( &(exchData->lastIddReqToAux) );
	//iprintf("sending IDD request\n");
}

void SendRunFromRunButton()
{
	strcpy( auxTxBuff, ">02CMD,1" );
	Aux_TerminateAndSend();
}

void SendOffFromStopButton()
{
	strcpy( auxTxBuff, ">02OFF" );
	Aux_TerminateAndSend();
	OSTimeDly(20);
	strcpy( auxTxBuff, ">02IDD" );
	Aux_TerminateAndSend();
}

void SendMinusOffFromStowButton()
{
	strcpy( auxTxBuff, ">02CMD,2" );
	Aux_TerminateAndSend();
}

void Aux_SendSCN( struct exch_data* exchData )
{
	sprintf( auxTxBuff, ">02SCN,%1.2lf", exchData->vscan );
	Aux_TerminateAndSend();
}

void Aux_SendTRK( struct exch_data* exchData )
{
	int tempval;

	if (exchData->stepignore != 0) {
		tempval = (exchData->stepignore * 100) + exchData->stepadjust;
		sprintf( auxTxBuff, ">02TRK,%d,%d,%1.2lf", exchData->trackgain, tempval, exchData->lockthresh );
	} else
		sprintf( auxTxBuff, ">02TRK,%d,%d,%1.2lf", exchData->trackgain, exchData->stepadjust, exchData->lockthresh );

	Aux_TerminateAndSend();
}

void Aux_SendLTE( struct exch_data* exchData )
{
	sprintf( auxTxBuff, ">02LTE,%c,%1.2f,%1.2f", exchData->snrthreshold, exchData->snrhigh, exchData->snrmid );

	Aux_TerminateAndSend();
}

void Aux_QueryGps( struct exch_data* exchData )
{
	strcpy( auxTxBuff, "?02GGA" );
	Aux_TerminateAndSend();
	LatchTime( &(exchData->lastGpsReqToAux) );
	//iprintf("sending GGA request\n");
}

void Aux_SendSnr( struct exch_data* exchData )
{
	sprintf( auxTxBuff, ">02SNR,%0.3f", exchData->snr );
	Aux_TerminateAndSend();
}

void Aux_SendLOK( struct exch_data* exchData )
{
	sprintf( auxTxBuff, ">02LOK,%ld", exchData->antlock );
	iprintf("sending lock - %ld\n", exchData->antlock);
	Aux_TerminateAndSend();
}

void Aux_SendSIP( struct exch_data* exchData )
{
	IPADDR4 ip;
	char progname[20];
	char version[20];
	DWORD ipnum;

	sprintf(progname, "%s", "OAC PathFinder");
	sprintf(version, "%s", "ver.1.00.B");
	ipnum = (DWORD) gConfigRec.ip_Addr;

	//sprintf( auxTxBuff, ">02SIP,%ld,%s,%s", ipnum, progname, version );

	Aux_TerminateAndSend();
}

void Aux_SendSat( struct exch_data* exchData )
{
	float satLong = exchData->satLongitude;
	float satFreq = 10799;
	//float satFreq = exchData->satFreq;

	// First validate that both the 'S' and 'H' have been received from the modem
	if ( ( satLong >= -360.0 ) && ( satLong <= 360.0 ) &&
	     ( satFreq > 0.0 ) )
	{
		sprintf( auxTxBuff, ">02OFF" );	// Stop prior to changing SAT settings.
		Aux_TerminateAndSend();
		//OSTimeDly (DELAY5);

		sprintf( auxTxBuff, ">02SAT0,%0.2f", -satLong );
		Aux_TerminateAndSend();
		//OSTimeDly (DELAY5);

		sprintf( auxTxBuff, ">02SAT1,%0.2f", -satLong );
		//sprintf( auxTxBuff, ">02SAT1,%0.2f,%0.6f", -satLong, satFreq );
		Aux_TerminateAndSend();
		//OSTimeDly (DELAY5);
	}
}

void Aux_SendPolType( struct exch_data* exchData )
{
	sprintf( auxTxBuff, ">02PSW,%d", exchData->polTypeToAux );
	Aux_TerminateAndSend();
}


/*
 * End of auxRem.c
 */

