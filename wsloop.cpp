/* Revision: 2.9.5 */

#include <predef.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <startnet.h>
#include <autoupdate.h>
#include <dhcpclient.h>
#include <http.h>
#include <websockets.h>
#include <webclient/json_lexer.h>
#include <iosys.h>
#include "s2e.h"
#include <ucos.h>

#define SERVER_IP "192.168.10.210"
#define REPORT_BUF_SIZE 512

extern "C" {
	void User4Main(void *pd);
}

char ReportBuffer[REPORT_BUF_SIZE];
extern http_wshandler *TheWSHandler;
int ws_fd = -1;

OS_SEM waitingForWS;

fd_set read_fds;
fd_set write_fds;
fd_set error_fds;

void UserTask(void);

void displayIDD(char *displayLine)
{
	char datafield[35];

    strcpy(datafield, displayLine);

	FD_SET(ws_fd, &write_fds);
	FD_SET(ws_fd, &error_fds);

	select(1, &write_fds, NULL, &error_fds, 0);

	if (FD_ISSET(ws_fd, &error_fds)) {
		iprintf("Closing Socket\r\n");
		OSTimeDly(1);
		close(ws_fd);
		ws_fd = -1;
		iprintf("Socket Closed\r\n");

    }
    if (FD_ISSET(ws_fd, &write_fds)) {
    	// OSTimeDly(TICKS_PER_SECOND * 1);

    	int n = write( ws_fd, datafield, 33 );
    	iprintf("Sent %d bytes\r\n", n);
    	NB::WebSocket::ws_flush( ws_fd );
    }

    FD_ZERO( &write_fds );
    FD_ZERO( &error_fds );
}

void SendConfigReport(int ws_fd, char *displayLine, struct exch_data* exchData)
{
	SMPoolPtr pq;
	ParsedJsonDataSet JsonOutObject;
	char datafield[35];
	char datafield2[35];
	char datafield3[3];
	char datafield4[3];
	//double temp = exchData->snr;

	strcpy(datafield, displayLine);
	//sprintf(datafield2, "%3.2lf", 9.22222);
	sprintf(datafield2, "%3.2lf", exchData->snr);
	sprintf(datafield3, "%d", exchData->antIsFunctional);
	sprintf(datafield4, "%d", exchData->modemIsFunctional);

	// Assemble JSON object
	JsonOutObject.StartBuilding();
	JsonOutObject.AddObjectStart("status");
	JsonOutObject.Add("IDD", datafield);
	JsonOutObject.Add("SNR", datafield2);
	JsonOutObject.Add("ANT", datafield3);
	JsonOutObject.Add("MOD", datafield4);
	JsonOutObject.EndObject();
	JsonOutObject.DoneBuilding();

	// If you would like to print the JSON object to serial to see the format, uncomment the next line
	//JsonOutObject.PrintObject(true);

    int dataLen = JsonOutObject.PrintObjectToBuffer(ReportBuffer, REPORT_BUF_SIZE);
    writeall(ws_fd, ReportBuffer, dataLen);

	// Print JSON object to a buffer and write the buffer to the WebSocket file descriptor
	//int dataLen = JsonOutObject.PrintObjectToBuffer(ReportBuffer, REPORT_BUF_SIZE);
	//writeall(ws_fd, ReportBuffer, dataLen);
}

int DoWSUpgrade( HTTP_Request *req, int sock, PSTR url, PSTR rxb ) {

	if (httpstricmp(url, "WS")) {
		int rv = WSUpgrade( req, sock );
		if (rv >= 0) {
			ws_fd = rv;
			NB::WebSocket::ws_setoption(ws_fd, WS_SO_TEXT);
			OSSemPost( &waitingForWS );
			//iprintf("Reading from WS: amount: %d\r\n", rv);
			return 2;
		} else {
			return 0;
        }
    }
	NotFoundResponse( sock, url );
	return 0;
}


void UserTask(void *)
{
    SMPoolPtr pp;
    fd_set read_fds;
    fd_set error_fds;

    //extern char *displayLine;
    char aline[35];
    strcpy(aline, "@@@@@");

    FD_ZERO( &read_fds );
    FD_ZERO( &error_fds );

    while (ws_fd > 0) {
        FD_SET(ws_fd, &read_fds);
        FD_SET(ws_fd, &error_fds);
        select(1, &read_fds, NULL, &error_fds, 0);
        if (FD_ISSET(ws_fd, &error_fds)) {
            iprintf("Closing Socket\r\n");
            OSTimeDly(1);
            close(ws_fd);
            ws_fd = -1;
            iprintf("Socket Closed\r\n");
            break;
        }
        if (FD_ISSET(ws_fd, &read_fds)) {
            //int n = read( ws_fd, (char *)pp->pData, ETHER_BUFFER_SIZE );
            //n = writeall( 1, (char *)pp->pData, n );
            int n = read( ws_fd, (char *)aline, 33 );
        	n = writeall( 1, (char *)aline, sizeof(aline));
        	//iprintf("Sending %d bytes to websocket\r\n", n);
            NB::WebSocket::ws_flush( ws_fd );
        }
        FD_ZERO( &read_fds );
        FD_ZERO( &error_fds );
    }
}

void User4Main(void)
{
	//InitializeStack();/* Setup the TCP/IP stack buffers etc.. */
    OSChangePrio(MAIN_PRIO + 1);/* Change our priority from highest to something in the middle */
    //EnableAutoUpdate();/* Enable the ability to update code over the network */
    OSSemInit( &waitingForWS, 0 );
    //StartHTTP();

    iprintf("Application: Start Websockets\n");
    ws_fd = NB::WebSocket::Connect(SERVER_IP, "/WS");
    //iprintf("ws_fd: %d\r\n", ws_fd);
    OSSimpleTaskCreate( UserTask, MAIN_PRIO +3 );  //3
    SMPoolPtr pp;
    TheWSHandler = DoWSUpgrade;
    OSSemPendNoWait( &waitingForWS ); //5
	iprintf("Application #4 complete\n");
}
