/*
 * formpausesys.cpp
 *
 *  Created on: September 5, 2021
 *      Author: David Billsbrough
 */

#include <predef.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <startnet.h>
#include <ip.h>
#include <tcp.h>
#include <bsp.h>
#include <htmlfiles.h>
#include <http.h>
#include <string.h>
#include <multipartpost.h>
#include "formtools.h"
#include "configweb.h"
#include "s2e.h"

#define MAX_PATH_LEN 40

extern struct profile_labels Profiles;
extern struct config_data configData;
extern struct NV_SettingsStruct NV_Settings;

extern void SavingStoredData(void);
extern bool NeedsRebooted;
extern char current_profilename[20];
extern int currentRec;

/* function prototypes */
int PauseSystemforReload(int sock, char *url, char *pData, char *rxBuffer, char *post_url_destination);
int ChangeCurrentProfile( int sock, char *url, char *pData, char *rxBuffer , char *post_url_destination );
int DoMatch5(int sock, char *url, char *pData, char *rxBuffer, char *post_url_destination);
int DoMatch6(int sock, char *url, char *pData, char *rxBuffer, char *post_url_destination);

int DoMatch5(int sock, char *url, char *pData, char *rxBuffer, char *post_url_destination )
{
	iprintf("The return path is %s\n", post_url_destination);

	PauseSystemforReload( sock, url, pData, rxBuffer, post_url_destination );

	OSTimeDly(TICKS_PER_SECOND * 2);
	RedirectResponse(sock, post_url_destination);

	return 0;
}

int DoMatch6(int sock, char *url, char *pData, char *rxBuffer, char *post_url_destination)
{
	iprintf("The return path is %s\n", post_url_destination);

	ChangeCurrentProfile( sock, url, pData, rxBuffer, post_url_destination );

	OSTimeDly(TICKS_PER_SECOND * 2);
	RedirectResponse(sock, post_url_destination);

	return 0;
}

int PauseSystemforReload( int sock, char *url, char *pData, char *rxBuffer , char *post_url_destination )
{
	char buf[40];
	extern bool NeedsRebooted;

	if (ExtractPostData("buttonPaused", pData, buf, MAX_PATH_LEN) == -1) {   // was post data "buttonPaused"
		iprintf("Error reading post data: buttonPause\r\n");
	} else {
		iprintf("Did it really pause the system?\r\n");

		NeedsRebooted = true;

		if (strcmp(buf, "Paused+System") == 0) {
			NeedsRebooted = true;
			OSTimeDly(200);
			iprintf("pausing system\r\n");
		}
	}
	return 0;
}

int ChangeCurrentProfile( int sock, char *url, char *pData, char *rxBuffer , char *post_url_destination )
{
	char buf[15];
	char buf2[30];
	int answer = 11;
	int cur;

	struct config_data configData;
	//configData = &configData2;

	// struct profile_labels Profiles;

	// Process Field One
	if ((ExtractPostData( "changeProfile", pData, buf, 10 ) == -1)) {
		iprintf("Error reading post data: changeProfile\r\n");
	} else {
		answer = atoi(buf);
		currentRec = answer;
		Profiles.curProfile = answer;
		NV_Settings.currentRecord = answer;

		iprintf("We REALY did set an updated current record. it is %d\r\n", currentRec);
	}

	// Process Field Two
	//if (ExtractPostData( "systemReboot", pData, buf2, 10 ) == -1) {
	//	iprintf("Error reading post data: systemReboot\r\n");
	//	iprintf("pData is %s\r\n", pData);
	//	iprintf("value is %s\n", buf2);
	//} else {
	//	if (strcmp(buf2, "true") == 0) {
	//		NeedsRebooted = true;
	//	} else {
	//		NeedsRebooted = false;
	//	}
	//}

	NeedsRebooted = true;
	iprintf("NeedsRebooted is %d.\n", NeedsRebooted);

	if (NeedsRebooted == false) {
		iprintf("Doing FALSE block\r\n");
		cur = currentRec - 1;

		iprintf("cur is %d.\r\n", cur);

		//memmove( &configData, NV_Settings.arrData[cur], sizeof(config_data));
		configData.oamodesel = NV_Settings.arrData[cur].oamodesel;
		configData.modemip = NV_Settings.arrData[cur].modemip;
		configData.modemport = NV_Settings.arrData[cur].modemport;
		strcpy(configData.modemroot, NV_Settings.arrData[cur].modemroot);
		strcpy(configData.modemuser, NV_Settings.arrData[cur].modemuser);
		strcpy(configData.modempass, NV_Settings.arrData[cur].modempass);
		configData.modemsat = NV_Settings.arrData[cur].modemsat;
		configData.modempol = NV_Settings.arrData[cur].modempol;
		configData.antlock = NV_Settings.arrData[cur].antlock;

	} else {
		//iprintf("Doing TRUE block\r\n");
		cur = currentRec - 1;

		NV_Settings.currentRecord = answer;
		// memmove( &NV_Settings.arrData[cur], &configData, sizeof(configData));
		configData.oamodesel = NV_Settings.arrData[cur].oamodesel;
		configData.modemip = NV_Settings.arrData[cur].modemip;
		configData.modemport = NV_Settings.arrData[cur].modemport;
		strcpy(configData.modemroot, NV_Settings.arrData[cur].modemroot);
		strcpy(configData.modemuser, NV_Settings.arrData[cur].modemuser);
		strcpy(configData.modempass, NV_Settings.arrData[cur].modempass);
		configData.modemsat = NV_Settings.arrData[cur].modemsat;
		configData.modempol = NV_Settings.arrData[cur].modempol;
		configData.antlock = NV_Settings.arrData[cur].antlock;

		SavingStoredData();
		iprintf("\r\n");
	    iprintf("Rebooting ...");
	    OSTimeDly(TICKS_PER_SECOND * 3);
	    ForceReboot();
	}

	iprintf("Setup to change current profile!\n");

	return 0;
}

/* End of file */
