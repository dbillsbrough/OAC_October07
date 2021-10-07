/* amip.c - OpenAMIP functions */

/*
 * KSI - Martin Kleinpeter
 * 
 * 9/22/2020 - Initial creation.
 */

// test
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
//#include <stdbool.h>
#include <startnet.h>
#include "s2e.h"
//#include "s2e_common.h"

#define AMIP_TX_BUFF_SIZE	128
static char amipTxBuff[ AMIP_TX_BUFF_SIZE ];
#define AMIP_RX_BUFF_SIZE	2048
static char amipRxBuff[AMIP_RX_BUFF_SIZE]; // piece together and process rx bytes

#define AMIP_MAX_ARGS	10
typedef char* ArgPtr;
static char* rxCmd;
static int argCnt;
static ArgPtr argPtrs[AMIP_MAX_ARGS];

extern int fdnet;

void Amip_InitNewConnection( struct exch_data* exchData )
{
	// Init 'P' value
	exchData->polTypeFromAmip = 0; // So no ASCII char saved yet
	exchData->polTypeToAux = -1; // Legal is 0-2

	// Init 'S' value
	exchData->satLongitude = -1000.0; // Impossible value signifies not set yet.
	// Init 'H' value
	exchData->satFreq = -1000.0;	  // Impossible value signifies not set yet.

	// Init 's' parms
	exchData->sIntervalSecs = 15;
	exchData->antIsFunctional = 0;
	exchData->modemMayTx = 0;

	// Init 'w' parms
	exchData->wIntervalSecs = 15;
	exchData->locIsValid = 0;
	exchData->latitude = 0.0;
	exchData->longitude = 0.0;
	exchData->gpsSecs = 0;
	exchData->altitude = 0.0;

	exchData->snr = -999.9;
	exchData->isLocked = false;
	exchData->txIsOn = false;
	
	amipRxBuff[0] = 0; // Mark as empty.
	LatchTime(&(exchData->lastAmipWrite));
	LatchTime(&(exchData->lastStatusReqToAux));
	LatchTime(&(exchData->lastGpsReqToAux));
}

void Amip_Command_A(struct exch_data* exchData )
{
	// Keepalive time.  How often to send 's' message.
	if ( argCnt >= 1 ) {
		int intervalSecs = atoi( argPtrs[ 0 ] );
		if ( intervalSecs > 0 )
			exchData->sIntervalSecs = intervalSecs;
	}
}

void Amip_Command_F( struct exch_data* exchData )
{
	// Find the satellite

	// Send 's' command now.
	Amip_SendAntennaStatus(exchData);

	// Poll Aux Remote for Tx Allow
	Aux_QuerySts( exchData );
}

// Receive SNR from modem.
void Amip_Command_Ntc_S(struct exch_data* exchData )
{
	if ( argCnt >= 2 )
	{
		double snr = atof( argPtrs[1] );
		exchData->snr = snr;
		Aux_SendSnr( exchData );
	}
}

void Amip_Command_L(struct exch_data* exchData )
{
	// Store Lock and Tx Enable States
	if ( argCnt >= 2 )
	{
		if ( strcmp( argPtrs[0], "1" ) == 0 )
			exchData->isLocked = true;
		else
			exchData->isLocked = false;

		if ( strcmp( argPtrs[1], "1" ) == 0 )
			exchData->txIsOn = true;
		else
			exchData->txIsOn = false;
	}
}

// Send location at interval
void Amip_Command_W( struct exch_data* exchData )
{
	// Send location interval.  How often to send 'w' message.
	if ( argCnt >= 1 )
	{
		int intervalSecs = atoi( argPtrs[ 0 ] );
		if ( intervalSecs >= 0 )
			exchData->wIntervalSecs = intervalSecs;

		// Send Location immediately.
		Amip_SendLocation( exchData );

		// Poll Aux Remote to make sure we have correct location.
		Aux_QueryGps( exchData );
	}
}

void Amip_Command_S( struct exch_data* exchData )
{
	if ( argCnt >= 1 )
	{
		exchData->satLongitude = atof( argPtrs[0] );
		Aux_SendSat( exchData );
	}
}

void Amip_Command_H( struct exch_data* exchData )
{
	if ( argCnt >= 1 )
	{
		exchData->satFreq = atof( argPtrs[0] );
		Aux_SendSat( exchData );
	}
}

void Amip_Command_P( struct exch_data* exchData )
{
	char chPol;

	if ( argCnt >= 1 )
	{
		chPol = argPtrs[0][0];
		exchData->polTypeFromAmip = chPol;
		if ( chPol == 'H' )
			exchData->polTypeToAux = 0;
		else if ( chPol == 'V' )
			exchData->polTypeToAux = 1;
		else
			exchData->polTypeToAux = 2;
		
		Aux_SendPolType( exchData );
	}
}

void Amip_ParseSingleRxMessage( char* amipCmdStr, struct exch_data* exchData )
{
	char* comment;
	char* nextArg;
	int argIndex = 0;

	// First, peel off any comment from the command string
	comment = strchr( amipCmdStr, '#' );
	if ( comment != NULL )
		*comment = 0;	// Terminate the string at the comment marker

	rxCmd = strtok( amipCmdStr, " \t\r" );
	while ( ( (nextArg = strtok( NULL, " \t\r" )) != NULL ) && ( argCnt < AMIP_MAX_ARGS ) )
	{
		argPtrs[ argIndex ] = nextArg;
		argIndex++;
	}
	argCnt = argIndex;

	if ( strcmp( rxCmd, "Ntc_S" ) == 0 )
		Amip_Command_Ntc_S( exchData );
	else if ( strcmp( rxCmd, "L" ) == 0 )
		Amip_Command_L( exchData );
	else if ( strcmp( rxCmd, "F" ) == 0 )
		Amip_Command_F( exchData );
	else if ( strcmp( rxCmd, "S" ) == 0 )
		Amip_Command_S( exchData );
	else if ( strcmp( rxCmd, "H" ) == 0 )
		Amip_Command_H( exchData );
	else if ( strcmp( rxCmd, "P" ) == 0 )
		Amip_Command_P( exchData );
	else if ( strcmp( rxCmd, "A" ) == 0 )
		Amip_Command_A( exchData );
	else if ( strcmp( rxCmd, "W" ) == 0 )
		Amip_Command_W( exchData );


	//printf( "Cmd: %s\r\n", rxCmd );
	//for (int i=0; i<argCnt; i++ )
	//	printf( "%s ", argPtrs[ i ] );
	//if ( argCnt > 0 )
	//	printf( "\r\n" );


}

void Amip_ProcessRxBytes( char* newBytes, int newByteCnt,  exch_data* exchData )
{
	char* newPtr = newBytes;
	char* nextLf;
	char* nextCr;
	char* nextLfCr;
	int availSpace, reqSpace;

	// Loop through bytes using CR/LF as delimiter between AMIP commands.
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
			if ( (strlen( amipRxBuff) + strlen( newPtr )) < AMIP_RX_BUFF_SIZE )
			{
				// Save bytes for until remainder arrives
				strcat( amipRxBuff, newPtr );
			}
			else
			{
				// We can't save over 2048 bytes.  Dump everything
				amipRxBuff[0] = 0; // Empty
			}
			// Get out of message search loop
			newPtr = newBytes + newByteCnt; // Advance past newBytes buffer
		}
		else
		{
			// We have a CR/LF terminated message, so process it!
			availSpace = AMIP_RX_BUFF_SIZE - strlen( amipRxBuff) - 1;	// -1 leaves space for term 0
			reqSpace = nextLfCr - newPtr;
			if ( reqSpace <= availSpace )
			{
				strncat( amipRxBuff, newPtr, reqSpace );
				Amip_ParseSingleRxMessage( amipRxBuff, exchData );
			}

			amipRxBuff[0] = 0; // Mark empty for next pass
			newPtr = nextLfCr; // Advance past processed bytes
			while ( (*newPtr == '\r') || (*newPtr == '\n') )
				newPtr++; // Advance past CR and LF
		}
	}

}

void Amip_SendTimerMsgs( struct exch_data* exchData )
{
	if ( TimerElapsed( &(exchData->lastStatusToAmip), (double) (exchData->sIntervalSecs) ) )
		Amip_SendAntennaStatus(exchData);
	if ( TimerElapsed( &(exchData->lastLocationToAmip), (double) (exchData->wIntervalSecs) ) )
		Amip_SendLocation( exchData );
}

int Amip_SendAntennaStatus( struct exch_data* exchData )
{
	int sendFailed = 0;
	int bufLen;

	sprintf( amipTxBuff, "s %d %d\r\n", exchData->antIsFunctional, exchData->modemMayTx );
	bufLen = strlen( amipTxBuff );

	if ( write(fdnet, amipTxBuff, bufLen) < bufLen )
		sendFailed = -1;

	LatchTime( &(exchData->lastAmipWrite) );
	LatchTime( &(exchData->lastStatusToAmip) );

	return( sendFailed );
}

int Amip_SendLocation( struct exch_data* exchData )
{
	int sendFailed = 0;
	int bufLen;

	sprintf( amipTxBuff, "w %d %0.3f %0.3f %ld %0.3f 0.0 0.0 0.0 0.0 0.0\r\n", 
		exchData->locIsValid, exchData->latitude, exchData->longitude,
		exchData->gpsSecs, exchData->altitude );
	bufLen = strlen( amipTxBuff );

	if ( write(fdnet, amipTxBuff, bufLen) < bufLen )
		sendFailed = -1;

	LatchTime( &(exchData->lastAmipWrite) );
	LatchTime( &(exchData->lastLocationToAmip) );

	return( sendFailed );
}

int Amip_SetSendLockInterval( struct exch_data* exchData, int intervalSecs )
{
	int sendFailed = 0;
	int bufLen;

	sprintf( amipTxBuff, "a %d\r\n", intervalSecs );
	bufLen = strlen( amipTxBuff );

	if ( write (fdnet, amipTxBuff, bufLen ) < bufLen )
		sendFailed = -1;

	LatchTime( &(exchData->lastAmipWrite) );

	return( sendFailed );
}

int Amip_SetSendSnrInterval( struct exch_data* exchData, int interval_mSec )
{
	int sendFailed = 0;
	int bufLen;

	sprintf( amipTxBuff, "Ntc_s %d\r\n", interval_mSec );
	bufLen = strlen( amipTxBuff );

	if ( write( fdnet, amipTxBuff, bufLen ) < bufLen )
		sendFailed = -1;

	LatchTime( &(exchData->lastAmipWrite) );

	return( sendFailed );
}

/*
 * End of amip.c
 */

