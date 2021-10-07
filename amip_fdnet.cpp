/*
 * amip_fdnet.cpp
 *
 *  Created on: August 26, 2021
 *      Author: David B.
 **
 */

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <predef.h>
#include <ctype.h>
#include <startnet.h>
#include <serial.h>
#include <iosys.h>
#include <utils.h>
#include <ip.h>
#include <tcp.h>
#include <autoupdate.h>
#include <taskmon.h>
#include <signal.h>
#include <multipartpost.h>
#include <streamupdate.h>
#include <init.h>
#include <fdprintf.h>
#include <ucos.h>
#include "s2e.h"

#define DELAY5 0.5

extern "C" {
    void User6Main(void * pd);
}

static char buffer[NET_BUF_SIZE];

extern struct config_data configData;
extern struct exch_data exchData2;
extern struct NV_SettingsStruct NV_Settings;
int fdnet = -1;
extern int modem_mode;
extern int currentRec;

//#define STDIN 0  // file descriptor for standard input
#define TCP_WRITE_TIMEOUT2 100000

void UserTask6(void *)
{
	struct config_data configData;

    fd_set read_fds;
    fd_set error_fds;

	struct exch_data exchData;
	//exchData = &exchData2;

	int fdListen = listen( INADDR_ANY, modem_port, 5 );

    if ( fdListen > 0 ) {
    	IPADDR client_addr;
    	WORD port;
    	//writestring( fdserial, "WHEN NOT CONNECTED" );

    	fdnet = accept( fdListen, &client_addr, &port, 0 );
    	iprintf("Accepting TCP connection: %d\r\n", fdnet);
    	writestring( fdnet, "Connected to TCP/IP\r\n" );

		if (modem_mode == 4) {
			if ( Amip_SetSendSnrInterval( &exchData, 200 ) != 0 )
				printf( "Error: Could not send Lock Interval to AMIP client.\r\n" );
		}

		// Init new AMIP connection parms
		Amip_InitNewConnection( &exchData );

		// Every time we connect with modem, send initial openAMIP commands to it.
		// Tell Modem to send Status every 15 seconds.
		if ( Amip_SetSendLockInterval( &exchData, 5 ) != 0 )
			printf( "Error: Could not send Lock Interval to AMIP client.\r\n" );

		OSTimeDly (DELAY5); // Wait 0.5 seconds before sending another command.
		// Send initial 's' functional and mayTx info

		Amip_SendAntennaStatus( &exchData );

		while ( fdnet > 0 ) {

			FD_ZERO( &read_fds );
			FD_SET( fdnet, &read_fds );

         	if ( select( FD_SETSIZE, &read_fds, ( fd_set * ) 0, &error_fds, TCP_WRITE_TIMEOUT )) {
				if ( FD_ISSET( fdnet, &read_fds ) ) {

					int readLen;

					Amip_SendTimerMsgs( &exchData );

					if ((readLen = net_focusMission_read(fdnet, buffer)) < 0) {
						perror( NULL );
						printf( "TCP Read Error: %d\r\n", readLen );
						printf( "buffer: %s\r\n", buffer );
						// Close the network port on error.
						close(fdnet);
						iprintf("Ooops, we are going to bail!!\n\n");
						// Start again at top of infinite loop
						continue;
					} else if ( readLen > 0 ) {
						// Parse AMIP RX bytes
						Amip_ProcessRxBytes( buffer, readLen, &exchData );
						//iprintf("In main network reading loop!\r\n");
					}
					//printf("OpenAMIP Rx Bytes: %d\r\n", readLen );
				}
			}
    	}
    }
}

void User6Main(void)
{
    //Max2tv.tv_sec = 5;
    //Max2tv.tv_usec = 500000;

    iprintf("Application #6 started\n");
	if ( modem_mode == 1 || modem_mode == 4 ) {
		iprintf("Starting TCP/IP user task\r\n");
		iprintf("Starting to listen on TCP port: %d\n", NV_Settings.arrData[currentRec-1].modemport);
		OSSimpleTaskCreate( UserTask6, 51 );
	} else {
		iprintf("NOT Starting TCP/IP user task\r\n");
	}

    iprintf("TCP/IP port listening has started\n");
}

/* End of File*/
