/*
 * formadmin.cpp
 *
 *  Created on: July 22, 2021
 *      Author: David Billsbrough
 */

#include <predef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <startnet.h>
#include <ip.h>
#include <tcp.h>
#include <htmlfiles.h>
#include <http.h>
#include <bsp.h>
#include <netinterface.h>
#include <utils.h>
#include <constants.h>
#include <ucos.h>
#include <system.h>
#include <init.h>
#include <iosys.h>
#include "formtools.h"
#include "configweb.h"
#include "s2e.h"

extern struct exch_data exchData;
extern struct config_data configData;
struct profile_labels Profiles;
extern struct NV_SettingsStruct NV_Settings;
extern int currentRec;

/* function prototypes */
int DoMatch3(int sock, char *url, char *pData, char *rxBuffer);
extern void SavingStoredData(void);
extern void InitializeNewRecords( int num, struct config_data configData );
//extern void resave_profiles_to_stored_data(int num, struct profile_labels *Profiles);
extern void resave_profiles_to_stored_data( int, struct profile_labels * );

extern "C" {
	void ModemSel(int sock, PCSTR url);
	void oamode1(int sock, PCSTR url);
	void oamode2(int sock, PCSTR url);
	void oamode3(int sock, PCSTR url);
	void oamode4(int sock, PCSTR url);
	void oamode5(int sock, PCSTR url);
	void oamode6(int sock, PCSTR url);
	void ModemAddress(int sock, PCSTR url);
	void DisplayModemAddress(int sock, PCSTR url);
	void ModemPort(int sock, PCSTR url);
	void DisplayModemPort(int sock, PCSTR url);
	void ModemRoot(int sock, PCSTR url);
	void DisplayModemRoot(int sock, PCSTR url);
	void ModemUser(int sock, PCSTR url);
	void DisplayModemUser(int sock, PCSTR url);
	void ModemPass(int sock, PCSTR url);
	void DisplayModemPass(int sock, PCSTR url);
	void ModemSat(int sock, PCSTR url);
	void DisplayModemSat(int sock, PCSTR url);
	void ModemPol1(int sock, PCSTR url);
	void ModemPol2(int sock, PCSTR url);
	void ModemPol3(int sock, PCSTR url);
	void DisplayModemPol(int sock, PCSTR url);
	void AntLock(int sock, PCSTR url);
	void LockMode1(int sock, PCSTR url);
	void LockMode2(int sock, PCSTR url);
	void SerialSpeed1(int sock, PCSTR url);
	void SerialSpeed2(int sock, PCSTR url);
	void Vscan(int sock, PCSTR url);
	void TrackGain(int sock, PCSTR url);
	void StepAdjust(int sock, PCSTR url);
	void StepIgnore(int sock, PCSTR url);
	void LockThresh(int sock, PCSTR url);
	void SNRthreshold(int sock, PCSTR url);
	void SNRhigh(int sock, PCSTR url);
	void SNRmin(int sock, PCSTR url);
	void DisplayProfiles(int sock, PCSTR url);
}

extern struct exch_data exchData;
extern void CheckNVSettings();
extern char *resave_profiles_to_stored_data( int, struct profile_labels );
extern bool NeedsRebooted;

extern char current_profilename[20];
extern int currentRec;

int sock;
char *url;
char *rxBuffer;
char *post_url_destination = "pages/config.html";

/* global variables */
char gPostBuf[1000];
IPADDR4 New_Ethernet;


int GetMyModemSelect()
{
	int MyMode = 0;
	//struct config_data* configData;

	MyMode = NV_Settings.arrData[currentRec-1].oamodesel;

	return MyMode;
}

int GetMyAntennaLock()
{
	int MyLock = 0;
	//struct config_data* configData;

	MyLock = NV_Settings.arrData[currentRec-1].antlock;
	//iprintf("the current antenna lock select is %d\n", MyLock);

	return MyLock;
}

int GetMyModemSpeed()
{
	char MySpeed = '0';

	MySpeed = NV_Settings.arrData[currentRec-1].serialspeed;
	//iprintf("The current modem baud rate is '%c'\n", MySpeed);

	return MySpeed;

}

int DoMatch3(int sock, char *url, char *pData, char *rxBuffer)
{
	iprintf("Debugging the save settings form.\r\n");

	return ChangeAdminConfiguration( sock, url, pData, rxBuffer, post_url_destination );
}

void ModemSel(int sock, PCSTR url)
{
	const char *pList[6] = { "oamode1", "oamode2", "oamode3", "oamode4", "oamode5", "oamode6"};

	FormOutputSelect(sock, "oamodesel", 1, pList );
}

void oamode1(int sock, PCSTR url)
{
	char buf[40];
	char option[20];
	int mode = 1;

	if ( mode == GetMyModemSelect()) {
		strcpy(option, "selected");
	} else {
		strcpy(option, "");
	}
	sprintf( buf, "VALUE=\"%d\" %s", mode, option);
	writestring( sock, buf );
}

void oamode2(int sock, PCSTR url)
{
	char buf[40];
	char option[20];
	int mode = 2;

	if ( mode == GetMyModemSelect()) {
		strcpy(option, "selected");
	} else {
		strcpy(option, "");
	}
	sprintf( buf, "VALUE=\"%d\" %s", mode, option);
	writestring( sock, buf );
}

void oamode3(int sock, PCSTR url)
{
	char buf[40];
	char option[20];
	int mode = 3;

	if ( mode == GetMyModemSelect()) {
		strcpy(option, "selected");
	} else {
		strcpy(option, "");
	}
	sprintf( buf, "VALUE=\"%d\" %s", mode, option);
	writestring( sock, buf );
}

void oamode4(int sock, PCSTR url)
{
	char buf[40];
	char option[20];
	int mode = 4;

	if ( mode == GetMyModemSelect()) {
		strcpy(option, "selected");
	} else {
		strcpy(option, "");
	}
	sprintf( buf, "VALUE=\"%d\" %s", mode, option);
	writestring( sock, buf );
}

void oamode5(int sock, PCSTR url)
{
	char buf[40];
	char option[20];
	int mode = 5;

	if ( mode == GetMyModemSelect()) {
		strcpy(option, "selected");
	} else {
		strcpy(option, "");
	}
	sprintf( buf, "VALUE=\"%d\" %s", mode, option);
	writestring( sock, buf );
}

void oamode6(int sock, PCSTR url)
{
	char buf[40];
	char option[20];
	int mode = 6;

	if ( mode == GetMyModemSelect() ) {
		strcpy(option, "selected");
	} else {
		strcpy(option, "");
	}
	sprintf( buf, "VALUE=\"%d\" %s", mode, option);
	writestring( sock, buf );
}

void ModemAddress(int sock, PCSTR url)
{
	//FormOutputIPInput(sock, "modemip", NV_Settings.modemip);
	char buf[40];
	char buf2[20];

	IPADDR4 ip;

	ip = NV_Settings.arrData[currentRec-1].modemip;
	ip.sprintf(buf2, 19);
	sprintf( buf, "VALUE=\"%s\" size=\"20\" ", buf2);
	writestring( sock, buf );
}

void DisplayModemAddress(int sock, PCSTR url)
{
		ShowIP2Sock(sock, configData.modemip);
		IPADDR4 ip;
}

void ModemPort(int sock, PCSTR url)
{
	char buf[40];

	sprintf( buf, "VALUE=\"%d\" size=\"15\" ", NV_Settings.arrData[currentRec-1].modemport);
	writestring( sock, buf );
}

void DisplayModemPort(int sock, PCSTR url)
{
	//FormExtractNum( "modemport", pData, 1234 );
}

void ModemRoot(int sock, PCSTR url)
{
	char buf[40];

	sprintf(buf, "VALUE=\"%s\" size=\"15\" ", NV_Settings.arrData[currentRec-1].modemroot);
	writestring( sock, buf );
}

void DisplayModemRoot(int sock, PCSTR url)
{
	FormOutputInput(sock, "modemroot", 20, NV_Settings.arrData[currentRec-1].modemroot);
}

void ModemUser(int sock, PCSTR url)
{
	char buf[40];

	sprintf(buf, "VALUE=\"%s\" size=\"15\" ", NV_Settings.arrData[currentRec-1].modemuser);
	writestring( sock, buf );
}

void DisplayModemUser(int sock, PCSTR url)
{
	FormOutputInput(sock, "modemuser", 20, configData.modemuser);
}

void ModemPass(int sock, PCSTR url)
{
	char buf[40];

	sprintf(buf, "VALUE=\"%s\" size=\"15\" ", NV_Settings.arrData[currentRec-1].modempass);
	writestring( sock, buf );
}

void DisplayModemPass(int sock, PCSTR url)
{
	FormOutputInput(sock, "modempass", 15, configData.modempass);
}

void ModemSat(int sock, PCSTR url)
{
	char buf[80];
	float temp;

	temp = (float) NV_Settings.arrData[currentRec-1].modemsat / 100L;
	sprintf(buf, "VALUE=\"%1.2f\" size=\"15\" ", (float)(temp));
	writestring( sock, buf );
}

void DisplayModemSat(int sock, PCSTR url)
{
	FormOutputRealInput(sock, "modemsat", 10, configData.modemsat);
}

void ModemPol1(int sock, PCSTR url)
{
	char buf[80];
	char str[15];

	if (NV_Settings.arrData[currentRec-1].modempol == 'H') {
		strcpy(str, " checked");
	} else
		strcpy(str, "");

	sprintf(buf, "%s", str);
	writestring( sock, buf );
}

void ModemPol2(int sock, PCSTR url)
{
	char buf[80];
	char str[15];

	if (NV_Settings.arrData[currentRec-1].modempol == 'V') {
		strcpy(str, " checked");
	} else
		strcpy(str, "");

	sprintf(buf, "%s", str);
	writestring( sock, buf );
}

void ModemPol3(int sock, PCSTR url)
{
	char buf[80];
	char str[15];

	if (NV_Settings.arrData[currentRec-1].modempol == 'N') {
		strcpy(str, " checked");
	} else
		strcpy(str, "");

	sprintf(buf, "%s", str);
	writestring( sock, buf );
}

void DisplayModemPol(int sock, PCSTR url)
{
	PCSTR str = "";
	FormOutputInput(sock, "modempol", 10, str);
	configData.modempol = str[0];
}

void AntLock(int sock, PCSTR url)
{
	char buf[40];

	sprintf( buf, "VALUE=\"%ld\" size=\"15\" ", NV_Settings.arrData[currentRec-1].antlock);
	writestring( sock, buf );
}

void LockMode1(int sock, PCSTR url)
{
	char buf[40];
	char option[20];
	int lock = 1;

	if ( 10000 == GetMyAntennaLock() ) {
		strcpy(option, "selected");
	} else {
		strcpy(option, "");
	}
	sprintf( buf, "VALUE=\"%d\" %s", lock, option);
	writestring( sock, buf );
}

void LockMode2(int sock, PCSTR url)
{
	char buf[40];
	char option[20];
	int lock = 2;

	if ( 1022 == GetMyAntennaLock() ) {
		strcpy(option, "selected");
	} else {
		strcpy(option, "");
	}
	sprintf( buf, "VALUE=\"%d\" %s", lock, option);
	writestring( sock, buf );
}

void DisplayAntLock(int sock, PCSTR url)
{
	exchData.antlock = FormExtractNum( url, "antlock", 20000 );
}

/*** ADD NEW FUNCTIONS HERE ***/

void SerialSpeed1(int sock, PCSTR url)
{
	char buf[40];
	char option[20];
	char speed = '1';

	if ( speed == GetMyModemSpeed() ) {
		strcpy(option, "selected");
	} else {
		strcpy(option, "");
	}
	sprintf( buf, "VALUE=\"%c\" %s", speed, option);
	writestring( sock, buf );
}

void SerialSpeed2(int sock, PCSTR url)
{
	char buf[40];
	char option[20];
	char speed = '2';

	if ( speed == GetMyModemSpeed() ) {
		strcpy(option, "selected");
	} else {
		strcpy(option, "");
	}
	sprintf( buf, "VALUE=\"%c\" %s", speed, option);
	writestring( sock, buf );
}

void Vscan(int sock, PCSTR url)
{
	char buf[80];
	float temp;

	temp = (float) NV_Settings.arrData[currentRec-1].vscan / 100L;
	sprintf(buf, "VALUE=\"%1.2f\" size=\"15\" ", (float)(temp));
	writestring( sock, buf );
}

void TrackGain(int sock, PCSTR url)
{
	char buf[40];

	sprintf( buf, "VALUE=\"%d\" size=\"15\" ", NV_Settings.arrData[currentRec-1].trackgain);
	writestring( sock, buf );
}

void StepAdjust(int sock, PCSTR url)
{
	char buf[40];

	sprintf( buf, "VALUE=\"%d\" size=\"15\" ", NV_Settings.arrData[currentRec-1].stepadjust);
	writestring( sock, buf );
}

void StepIgnore(int sock, PCSTR url)
{
	char buf[40];

	sprintf( buf, "VALUE=\"%d\" size=\"15\" ", NV_Settings.arrData[currentRec-1].stepignore);
	writestring( sock, buf );
}

void LockThresh(int sock, PCSTR url)
{
	char buf[80];
	float temp;

	temp = (float) NV_Settings.arrData[currentRec-1].lockthresh / 100L;
	sprintf(buf, "VALUE=\"%1.2f\" size=\"15\" ", (float)(temp));
	writestring( sock, buf );
}

void SNRthreshold(int sock, PCSTR url)
{
	char buf[80];
	char str[15];

	if (NV_Settings.arrData[currentRec-1].snrthreshold == '1') {
		strcpy(str, " checked");
	} else
		strcpy(str, "");

	sprintf(buf, "%s", str);
	writestring( sock, buf );
}

void SNRhigh(int sock, PCSTR url)
{
	char buf[80];
	float temp;

	temp = (float) NV_Settings.arrData[currentRec-1].snrhigh / 100L;
	sprintf(buf, "VALUE=\"%1.2f\" size=\"15\" ", (float)(temp));
	writestring( sock, buf );
}

void SNRmin(int sock, PCSTR url)
{
	char buf[80];
	float temp;

	temp = (float) NV_Settings.arrData[currentRec-1].snrmin / 100L;
	sprintf(buf, "VALUE=\"%1.2f\" size=\"15\" ", (float)(temp));
	writestring( sock, buf );
}

/*** END OF NEW FUNCTIONS/VARIABLES ***/

void DisplayProfiles(int sock, PCSTR url)
{
	char buf[15];
	char buf1[30];

	NV_Settings.profiles.curProfile = currentRec;

	sprintf( buf, " \"%d\" ", NV_Settings.profiles.curProfile);
	writestring( sock, buf );

	sprintf( buf1, " \"%s\" ", NV_Settings.profiles.profile_label_01);
	writestring( sock, buf1 );
	sprintf( buf1, " \"%s\" ", NV_Settings.profiles.profile_label_02 );
	writestring( sock, buf1 );
	sprintf( buf1, " \"%s\" ", NV_Settings.profiles.profile_label_03 );
	writestring( sock, buf1 );
	sprintf( buf1, " \"%s\" ", NV_Settings.profiles.profile_label_04 );
	writestring( sock, buf1 );
	sprintf( buf1, " \"%s\" ", NV_Settings.profiles.profile_label_05 );
	writestring( sock, buf1 );
	sprintf( buf1, " \"%s\" ", NV_Settings.profiles.profile_label_06 );
	writestring( sock, buf1 );
	sprintf( buf1, " \"%s\" ", NV_Settings.profiles.profile_label_07 );
	writestring( sock, buf1 );
	sprintf( buf1, " \"%s\" ", NV_Settings.profiles.profile_label_08 );
	writestring( sock, buf1 );
	sprintf( buf1, " \"%s\" ", NV_Settings.profiles.profile_label_09 );
	writestring( sock, buf1 );
	sprintf( buf1, " \"%s\" ", NV_Settings.profiles.profile_label_10 );
	writestring( sock, buf1 );
}

void replace_plus_signs()
{
	int ctr = 0;
	char str[30];
	char new_char;

	strcpy(str, current_profilename);

	while (str[ctr]) {
		new_char = str[ctr];
		if ((new_char) == '+')
			new_char = ' ';
		putchar(new_char);
		ctr++;
	}

	strcpy(current_profilename, str);
}

void select_profile_name(int num, char *labelname)
{
	iprintf("Setting the labelname to %s\n", labelname);

	switch ( num ) {
		case 1:
			strcpy(Profiles.profile_label_01, labelname);
			strcpy(NV_Settings.profiles.profile_label_01, labelname);
			break;
		case 2:
			strcpy(Profiles.profile_label_02, labelname);
			strcpy(NV_Settings.profiles.profile_label_02, labelname);
			break;
		case 3:
			strcpy(Profiles.profile_label_03, labelname);
			strcpy(NV_Settings.profiles.profile_label_03, labelname);
			break;
		case 4:
			strcpy(Profiles.profile_label_04, labelname);
			strcpy(NV_Settings.profiles.profile_label_04, labelname);
			break;
		case 5:
			strcpy(Profiles.profile_label_05, labelname);
			strcpy(NV_Settings.profiles.profile_label_05, labelname);
			break;
		case 6:
			strcpy(Profiles.profile_label_06, labelname);
			strcpy(NV_Settings.profiles.profile_label_06, labelname);
			break;
		case 7:
			strcpy(Profiles.profile_label_07, labelname);
			strcpy(NV_Settings.profiles.profile_label_07, labelname);
			break;
		case 8:
			strcpy(Profiles.profile_label_08, labelname);
			strcpy(NV_Settings.profiles.profile_label_08, labelname);
			break;
		case 9:
			strcpy(Profiles.profile_label_09, labelname);
			strcpy(NV_Settings.profiles.profile_label_09, labelname);
			break;
		case 10:
			strcpy(Profiles.profile_label_10, labelname);
			strcpy(NV_Settings.profiles.profile_label_10, labelname);
			break;
		default:
			iprintf("Not a valid profile label position\r\n");
	}
}

void resave_profiles_to_stored_data2( int num, struct profile_labels *Profiles )
{
	Profiles->curProfile = num;
	NV_Settings.currentRecord = num;

	strcpy(NV_Settings.profiles.profile_label_01, Profiles->profile_label_01);
	strcpy(NV_Settings.profiles.profile_label_02, Profiles->profile_label_02);
	strcpy(NV_Settings.profiles.profile_label_03, Profiles->profile_label_03);
	strcpy(NV_Settings.profiles.profile_label_04, Profiles->profile_label_04);
	strcpy(NV_Settings.profiles.profile_label_05, Profiles->profile_label_05);
	strcpy(NV_Settings.profiles.profile_label_06, Profiles->profile_label_06);
	strcpy(NV_Settings.profiles.profile_label_07, Profiles->profile_label_07);
	strcpy(NV_Settings.profiles.profile_label_08, Profiles->profile_label_08);
	strcpy(NV_Settings.profiles.profile_label_09, Profiles->profile_label_09);
	strcpy(NV_Settings.profiles.profile_label_10, Profiles->profile_label_10);

	iprintf("\r\n");
	//memmove( &NV_Settings.profiles, &Profiles, sizeof(NV_Settings.profiles) );
	iprintf("Resaving profile data to NVRAM.\n");
}

struct exch_data exchData;

int ChangeAdminConfiguration( int sock, char *url, char *pData, char *rxBuffer , char *post_url_destination )
{
	char *buf = gPostBuf;
	IPADDR4 NewEthernet;
	//struct NV_SettingsStruct NV_Settings;
	struct config_data tempData;
	struct profile_labels Profiles;

	//char *current_profilename;
	int cur;

	NewEthernet.SetFromAscii("192.168.1.1");

	//const char *pList[6] = { "oamode1", "oamode2", "oamode3", "oamode4", "oamode5"};

	iprintf("Collecting data to save!\r\n");

	// Process Field One
	char rxBuf1[40];

	if (ExtractPostData( "oamodesel", pData, rxBuf1, 40)) {
		tempData.oamodesel = atoi(rxBuf1);
	}

	// Process Field Two
	char rxBuf[40];
    IPADDR4 ip, defValue;

    defValue.SetFromAscii("192.1.1.192");

    if (ExtractPostData("modemip", pData, rxBuf, 40)) {
        ip.SetFromAscii(rxBuf);
    } else {
        ip = defValue;
    }

	tempData.modemip = ip;

	// Process Field Three (new)

	if (ExtractPostData( "modemport", pData, buf, 15 ) == -1)
		iprintf("Error reading post data: modemport\r\n");
	else
		tempData.modemport = atoi(buf);

	// Process Field Four
	if (ExtractPostData( "modemroot", pData, buf, 15 ) == -1)
		iprintf("Error reading post data: modemroot\r\n");
	else
		strcpy(tempData.modemroot, (buf));

	// Process Field Five
	if (ExtractPostData( "modemuser", pData, buf, 15 ) == -1)
		iprintf("Error reading post data: modemuser\r\n");
	else
		strcpy(tempData.modemuser, (buf));

	// Process Field Six
	if (ExtractPostData( "modempass", pData, buf, 15 ) == -1)
		iprintf("Error reading post data: modempass\r\n");
	else
		strcpy(tempData.modempass, (buf));

	// Process Field Seven

	if (ExtractPostData( "modemsat", pData, buf, 15 ) == -1)
		iprintf("Error reading post data: modemsat\r\n");
	else
		tempData.modemsat = (long)(100 * atof(buf));

	// Process Field Eight
	if (ExtractPostData( "modempol", pData, buf, 10 ) == -1)
		iprintf("Error reading post data: modempol\r\n");
	else {
		if (strcmp(buf, "H") == 0) {
			tempData.modempol = 'H';
		} else if (strcmp(buf, "V") == 0) {
			tempData.modempol = 'V';
		} else if (strcmp(buf, "N") == 0) {
			tempData.modempol = 'N';
		} else
			tempData.modempol = '*';
	}

	// Process Field Nine
	if (ExtractPostData( "antlock", pData, buf, 10 ) == -1)
		iprintf("Error reading post data: antlock\r\n");
	else {
		//tempData->antlock = atoi(buf);
		if (strcmp(buf, "1") == 0) {
			tempData.antlock = 10000;
		} else if (strcmp(buf, "2") == 0) {
			tempData.antlock = 1022;
		}
		exchData.antlock = tempData.antlock;
	}

	// Process Field Ten
	if (ExtractPostData( "serialspeed", pData, buf, 10 ) == -1)
		iprintf("Error reading post data: serialspeed\r\n");
	else {
		iprintf("The value of buf is %s\n", buf);
		tempData.serialspeed = buf[0]; // modem baud rate
	}

	// Process Field Eleven
	if (ExtractPostData( "vscan", pData, buf, 10 ) == -1)
		iprintf("Error reading post data: vscan\r\n");
	else {
		tempData.vscan = (long) (100 * atof(buf));
	}

	// Process Field Twelve
	if (ExtractPostData( "trackgain", pData, buf, 10 ) == -1)
		iprintf("Error reading post data: trackgain\r\n");
	else {
		tempData.trackgain = atoi(buf);
	}

	// Process Field Thirteen
	if (ExtractPostData( "stepadjust", pData, buf, 10 ) == -1)
		iprintf("Error reading post data: stepadjust\r\n");
	else {
		tempData.stepadjust = atoi(buf);
	}

	// Process Field Fourteen
	if (ExtractPostData( "stepignore", pData, buf, 10 ) == -1)
		iprintf("Error reading post data: stepignore\r\n");
	else {
		tempData.stepignore = atoi(buf);
	}

	// Process Field Fifteen
	if (ExtractPostData( "lockthresh", pData, buf, 10 ) == -1)
		iprintf("Error reading post data: lockthresh\r\n");
	else {
		tempData.lockthresh = (long)(100 * atof(buf));
	}

	// Process Field Sixteen
	if (ExtractPostData( "snrthreshold", pData, buf, 10 ) == -1)
		iprintf("Error reading post data: snrthreshold\r\n");
	else {

		if (strcmp(buf, "off") == 0) {
			tempData.snrthreshold = '0';  // No
		} else if (strcmp(buf, "on") == 0) {
			tempData.snrthreshold = '1';  // Yes
		}
	}

	// Process Field Seventeen
	if (ExtractPostData( "snrhigh", pData, buf, 10 ) == -1)
		iprintf("Error reading post data: snrhigh\r\n");
	else {
		tempData.snrhigh = (long)(100 * atof(buf));
	}

	// Process Field Eighteen
	if (ExtractPostData( "snrmin", pData, buf, 10 ) == -1)
		iprintf("Error reading post data: snrmin\r\n");
	else {
		tempData.snrmin = (long)(100 * atof(buf));
	}

	// Process Field Nineteen

	// (RESERVED)

	// Process Field Profiles
	if (ExtractPostData( "profilename", pData, buf, 20 ) == -1)
		iprintf("Error reading post data: profilename\r\n");
	else {
		if (strlen(buf) != 0) {
			strcpy(current_profilename, buf);
		} else {
			strcpy(current_profilename, "new profile entry");
			// WARNING: the pluses need to be convert to spaces!
		}
		select_profile_name(currentRec, current_profilename);
	}

	//replace_plus_signs(); /* retest this later */

	iprintf("currentRec is %d and label to %s\r\n", currentRec, current_profilename);

	//resave_profiles_to_stored_data2( currentRec, &Profiles );

	cur = currentRec - 1;

	// Now to Store it in flash.

	NV_Settings.currentRecord = currentRec;
	memmove( &NV_Settings.arrData[currentRec-1], &tempData, sizeof(NV_Settings.arrData[cur]));

	// WARNING: If new settings are added, remember to add them to
	//          NV Settings default initialization!

	SavingStoredData();

	/*iprintf("Debugging:\r\n\r\n");

	iprintf("** %d\r\n", NV_Settings.arrData[cur].oamodesel);
	iprintf("** "); ShowIP(NV_Settings.arrData[cur].modemip); printf("\r\n");
	iprintf("** %d\r\n", NV_Settings.arrData[cur].modemport);
	iprintf("** %s\r\n", NV_Settings.arrData[cur].modemroot);
	iprintf("** %s\r\n", NV_Settings.arrData[cur].modemuser);
	iprintf("** %s\r\n", NV_Settings.arrData[cur].modempass);
	iprintf("** %ld\r\n", NV_Settings.arrData[cur].modemsat);
	iprintf("** %c\r\n", NV_Settings.arrData[cur].modempol);
	iprintf("** %ld\r\n", NV_Settings.arrData[cur].antlock);
	iprintf("** %c\r\n", NV_Settings.arrData[cur].serialspeed);
	iprintf("** %ld\r\n", NV_Settings.arrData[cur].vscan);
	iprintf("** %ld\r\n", NV_Settings.arrData[cur].trackgain);
	iprintf("** %ld\r\n", NV_Settings.arrData[cur].stepadjust);
	iprintf("** %ld\r\n", NV_Settings.arrData[cur].stepignore);
	iprintf("** %ld\r\n", NV_Settings.arrData[cur].lockthresh);
	iprintf("** %d\r\n", NV_Settings.arrData[cur].snrthreshold);
	iprintf("** %d\r\n", NV_Settings.arrData[cur].snrhigh);
	iprintf("** %d\r\n", NV_Settings.arrData[cur].snrmin);
	iprintf("** \n");
	iprintf("** currentRec is %d\r\n", currentRec);
	iprintf("** Label is %s\r\n", current_profilename);*/

	OSTimeDly(TICKS_PER_SECOND * 4);

    if ( NeedsRebooted ) {
        iprintf("Rebooting ...");
        OSTimeDly(TICKS_PER_SECOND * 2);
        ForceReboot();
    }

	RedirectResponse(sock, post_url_destination);

	return 0;
}

/* End of file */
