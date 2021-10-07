/*
 * formbutton.cpp
 *
 *  Created on: July 26, 2021
 *      Author: David Billsbrough
 */

#include <predef.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <startnet.h>
#include <ip.h>
#include <tcp.h>
#include <htmlfiles.h>
#include <http.h>
#include <string.h>
#include <multipartpost.h>
#include "formtools.h"
#include "configweb.h"
#include "s2e.h"

#define MAX_PATH_LEN 40
#define AUX_TX_BUFF_SIZE 40

extern char auxTxBuff[AUX_TX_BUFF_SIZE];

extern void SendRunFromRunButton();
extern void SendOffFromStopButton();
extern void SendMinusOffFromStowButton();

/* function prototypes */
void DoMatch4(int, char *url, char *pData, char *rxBuffer);
int ProcessButtonForms(int sock, char *url, char *pData, char *rxBuffer);

void DoMatch4(int sock, char *url, char *pData, char *rxBuffer)
{
	// iprintf("pData is %s\r\n", pData);

	ProcessButtonForms(sock, url, pData, rxBuffer);
}

int ProcessButtonForms(int sock, char *url, char *pData, char *rxBuffer)
{
	bool button1 = false;
	bool button2 = false;
	bool button3 = false;
	char buf[30];

	if (ExtractPostData("buttonPressed", pData, buf, MAX_PATH_LEN)) {   // was post data "buttonPressed"

		//iprintf("buf is %s.\r\n", (buf));

		if (strcmp(buf, "Run") == 0) {
			button1 = true;
			iprintf("Run Command button selected.\r\n");
		} else if (strcmp(buf, "Stop") == 0) {
			button2 = true;
			iprintf("Stow Command button selected.\r\n");
		} else if (strcmp(buf, "Stow") == 0) {
			button3 = true;
			iprintf("Deploy Command button selected.\r\n");
		} else {
			iprintf("NO VALID INPUT DATA\r\n ");
		}
	}

	iprintf("B1 = %d, B2 = %d, B3 = %d\r\n", button1, button2, button3);

	if (button1 == true) {

		// process the RUN button
		SendRunFromRunButton();
		OSTimeDly(TICKS_PER_SECOND * 0.25);

	} else if (button2 == true) {

		// process the STOP button
		SendOffFromStopButton();
		OSTimeDly(TICKS_PER_SECOND * 0.25);

	} else if (button3 == true) {

		// process the STOW button
		SendMinusOffFromStowButton();
		OSTimeDly(TICKS_PER_SECOND * 0.25);
	}

	OSTimeDly(TICKS_PER_SECOND * 3);

	RedirectResponse(sock, "index.html");

	return 0;
}

/* End of file */
