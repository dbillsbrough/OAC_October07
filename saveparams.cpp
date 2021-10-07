/*
 * save_params.cpp
 *
 *  Created on: July 9, 2021
 *      Author: David Billsbrough
 */

#include "predef.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <startnet.h>
#include <autoupdate.h>
#include <dhcpclient.h>
#include <bsp.h>
#include <utils.h>
#include <system.h>
#include <netinterface.h>
#include <constants.h>
#include <ucos.h>
#include <init.h>
#include <iosys.h>
#include "formtools.h"
#include "HtmlVar.h"
#include "s2e.h"

#define DEFAULT_PORT (5217)
#define VERIFY_KEY (0x48866064)  // NV Settings key code

bool bCheckedNV;
struct config_data pData;
struct NV_SettingsStruct NV_Settings;
extern struct config_data configData;
extern struct profile_labels Profiles;
extern struct exch_data exchData;

extern char current_profilename[20];
extern int currentRec;

/* function prototypes */

void InitializeSettings(struct config_data* configData);
void SaveParams(void);
//void DisplayParams(struct config_data* configData);
void CheckNVSettings();
//void SetupRuntimeConfig(int num, struct config_data* configData);
//void DisplayRuntimeConfig(struct config_data* configData);
void DisplayNVSettings(int num);
void SavingStoredData(void);
//void Initialize_NewRecords(int num, struct config_data* configData);
void Initialize_Profiles( struct profile_labels Profiles );

extern "C" {
	void User5Main(void *pd);
}

/*------------------------------------------------------------------------
 * Assign default values if VerifyKey has changed or is not initialized
 *------------------------------------------------------------------------*/
void CheckNVSettings()
{
	struct config_data configData;

	iprintf( "Checking NV_Settings User Parameter Flash ...\r\n");
	iprintf( "Size of NV Structure: %ld Bytes\r\n", sizeof(NV_Settings));

	NV_SettingsStruct *pData = ( NV_SettingsStruct * ) GetUserParameters();
	iprintf("Verify key = 0x%lX\r\n", pData->VerifyKey);
	// We will check the struct size as well to try and protect those who forgot to change
	// the verify key in order to avoid a crash or incorrect results

	IPADDR4 NewEthernet;

	if ((pData->VerifyKey != VERIFY_KEY ) || (pData->StructSize != sizeof(NV_Settings))) {
		NewEthernet.SetFromAscii("192.168.10.204");
		iprintf("Verify key = 0x%lX\r\n", (long unsigned int) VERIFY_KEY);
		if (pData->VerifyKey != VERIFY_KEY ) iprintf("Flash verification key has changed - initializing Flash\r\n");
		if (pData->StructSize != sizeof(NV_Settings)) iprintf("Struct size has changed - initializing Flash\r\n");

		NV_Settings.VerifyKey = VERIFY_KEY;
		NV_Settings.currentRecord = 1;

		Initialize_Profiles( Profiles );
		memmove( &NV_Settings.profiles, &Profiles, sizeof(profile_labels));

		InitializeSettings(&configData);

		for (int i = 0; i < 10; i++) {
			memmove( &NV_Settings.arrData[i], &configData, sizeof(config_data));
		}

		NV_Settings.nvChar = 'a';					    			// non volatile char
		NV_Settings.StructSize = sizeof(NV_Settings);

		//SaveUserParameters( &NV_Settings, sizeof( NV_Settings ) );
		SavingStoredData();
		OSTimeDly(TICKS_PER_SECOND * 3);
   } else {
	   iprintf("Flash verification is VALID - reading values from Flash\r\n");

	   NV_Settings.VerifyKey = pData->VerifyKey;

	   for (int i = 0; i < 10; i++) {
		   NV_Settings.arrData[i].oamodesel = pData->arrData[i].oamodesel;				// non volatile byte
		   NV_Settings.arrData[i].modemip = pData->arrData[i].modemip;
		   NV_Settings.arrData[i].modemport = pData->arrData[i].modemport;
		   strcpy(NV_Settings.arrData[i].modemroot, pData->arrData[i].modemroot);
		   strcpy(NV_Settings.arrData[i].modemuser, pData->arrData[i].modemuser);
		   strcpy(NV_Settings.arrData[i].modempass, pData->arrData[i].modempass);
		   NV_Settings.arrData[i].modemsat = pData->arrData[i].modemsat;
		   NV_Settings.arrData[i].modempol = pData->arrData[i].modempol;
		   NV_Settings.arrData[i].antlock = pData->arrData[i].antlock;
		   NV_Settings.arrData[i].serialspeed = pData->arrData[i].serialspeed;
		   NV_Settings.arrData[i].vscan = pData->arrData[i].vscan;
		   NV_Settings.arrData[i].stepadjust = pData->arrData[i].stepadjust;
		   NV_Settings.arrData[i].stepignore = pData->arrData[i].stepignore;
		   NV_Settings.arrData[i].trackgain = pData->arrData[i].trackgain;
		   NV_Settings.arrData[i].lockthresh = pData->arrData[i].lockthresh;
		   NV_Settings.arrData[i].snrthreshold = pData->arrData[i].snrthreshold;
		   NV_Settings.arrData[i].snrhigh = pData->arrData[i].snrhigh;
		   NV_Settings.arrData[i].snrmin = pData->arrData[i].snrmin;
	   }

	   NV_Settings.currentRecord = pData->currentRecord;
	   strcpy(NV_Settings.profiles.profile_label_01, pData->profiles.profile_label_01);
	   strcpy(NV_Settings.profiles.profile_label_02, pData->profiles.profile_label_02);
	   strcpy(NV_Settings.profiles.profile_label_03, pData->profiles.profile_label_03);
	   strcpy(NV_Settings.profiles.profile_label_04, pData->profiles.profile_label_04);
	   strcpy(NV_Settings.profiles.profile_label_05, pData->profiles.profile_label_05);
	   strcpy(NV_Settings.profiles.profile_label_06, pData->profiles.profile_label_06);
	   strcpy(NV_Settings.profiles.profile_label_07, pData->profiles.profile_label_07);
	   strcpy(NV_Settings.profiles.profile_label_08, pData->profiles.profile_label_08);
	   strcpy(NV_Settings.profiles.profile_label_09, pData->profiles.profile_label_09);
	   strcpy(NV_Settings.profiles.profile_label_10, pData->profiles.profile_label_10);

	   NV_Settings.nvChar = pData->nvChar;
	   NV_Settings.StructSize = pData->StructSize;

   }

   bCheckedNV = true;
}

void InitializeNewRecords( int num, struct config_data configData )
{
	int cur = num;
	IPADDR4 NewEthernet;

	NewEthernet.SetFromAscii("192.168.10.10");

	NV_Settings.arrData[cur].oamodesel = configData.oamodesel;
	NV_Settings.arrData[cur].modemip = configData.modemip;
	NV_Settings.arrData[cur].modemport = configData.modemport;
	strcpy(NV_Settings.arrData[cur].modemroot, configData.modemroot);
	strcpy(NV_Settings.arrData[cur].modemuser, configData.modemuser);
	strcpy(NV_Settings.arrData[cur].modempass, configData.modempass);
	NV_Settings.arrData[cur].modemsat = configData.modemsat;
	NV_Settings.arrData[cur].modempol = configData.modempol;
	NV_Settings.arrData[cur].antlock = configData.antlock;
	NV_Settings.arrData[cur].serialspeed = configData.serialspeed;
	NV_Settings.arrData[cur].vscan = configData.vscan;
	NV_Settings.arrData[cur].stepadjust = configData.stepadjust;
	NV_Settings.arrData[cur].stepignore = configData.stepignore;
	NV_Settings.arrData[cur].trackgain = configData.trackgain;
	NV_Settings.arrData[cur].lockthresh = configData.lockthresh;
	NV_Settings.arrData[cur].snrthreshold = configData.snrthreshold;
	NV_Settings.arrData[cur].snrhigh = configData.snrhigh;
	NV_Settings.arrData[cur].snrmin = configData.snrmin;
}

void Initialize_Profiles( struct profile_labels Profiles2 )
{
	Profiles.curProfile = currentRec;

	strcpy(Profiles.profile_label_01, "FACTORY DEFAULTS");
	strcpy(Profiles.profile_label_02, "(spare)");
	strcpy(Profiles.profile_label_03, "(spare)");
	strcpy(Profiles.profile_label_04, "(spare)");
	strcpy(Profiles.profile_label_05, "(spare)");
	strcpy(Profiles.profile_label_06, "(spare)");
	strcpy(Profiles.profile_label_07, "(spare)");
	strcpy(Profiles.profile_label_08, "(spare)");
	strcpy(Profiles.profile_label_09, "(spare)");
	strcpy(Profiles.profile_label_10, "(spare)");

	iprintf("Setting up the blank profiles ...\n\n");
}

void Initailize_legacy_settings(int num, struct exch_data exchData)
{
	int cur = num - 1;

	iprintf("Setup of legacy parameters\r\n");

	modem_mode = NV_Settings.arrData[cur].oamodesel;
	exchData.satLongitude = NV_Settings.arrData[cur].modemsat;
	exchData.polTypeFromAmip = NV_Settings.arrData[cur].modempol;
	exchData.antlock = NV_Settings.arrData[cur].antlock;
	iprintf("satLONG = %e, PolType = %c, AntLock is %ld\r\n", exchData.satLongitude, exchData.polTypeFromAmip, exchData.antlock );

	exchData.vscan = (float) NV_Settings.arrData[cur].vscan * 100;  // 2.00 scaled
	exchData.trackgain = NV_Settings.arrData[cur].trackgain;
	exchData.stepadjust = NV_Settings.arrData[cur].stepadjust;
	exchData.stepignore = NV_Settings.arrData[cur].stepignore;
	exchData.lockthresh = (float) NV_Settings.arrData[cur].lockthresh * 100;
	exchData.snrthreshold = NV_Settings.arrData[cur].snrthreshold;		// snrthreshold
	exchData.snrhigh = (float) NV_Settings.arrData[cur].snrhigh * 100;	// long snrhigh
	exchData.snrmid = (float) NV_Settings.arrData[cur].snrmin * 100;	// long snrmid
}

void InitializeSettings(struct config_data* configData)
{
	IPADDR4 NewEthernet;

	NewEthernet.SetFromAscii("192.168.10.204");

	configData->oamodesel = 1;					// non volatile byte
	configData->modemip = NewEthernet; 			// Modem IP number
	configData->modemport = DEFAULT_PORT;		// Modem Port
	strcpy(configData->modemroot, "iDirect"); 	// ROOT password
	strcpy(configData->modemuser, "admin");	 	// User Name
	strcpy(configData->modempass, "P@55w0ld!");	// User Password
	configData->modemsat = 12345;				// Modem Satellite
	configData->modempol = 'H'; 				// Modem Pol
	configData->antlock = 10000;				// Lock Method
	configData->serialspeed = '1';				// pData->arrData[i].serialspeed;
	configData->vscan = 300;					// pData->arrData[i].vscan;
	configData->stepadjust = 25;				// pData->arrData[i].stepadjust;
	configData->stepignore = 0;					// pData->arrData[i].stepignore;
	configData->trackgain = 1510;				// pData->arrData[i].trackgain;
	configData->lockthresh = 100;				// pData->arrData[i].lockthresh;
	configData->snrthreshold = 0;				// pData->arrData[i].snrtheshold;
	configData->snrhigh = 0;					// pData->arrData[i].snrhigh;
	configData->snrmin = 0;						// pData->arrData[i].snrmin;
}

/*-------------------------------------------------------------------
 Display the values of the NV_Settings structure.
 ------------------------------------------------------------------*/
void DisplayNVSettings(int num)
{
	int cur = num - 1;

	if (!bCheckedNV)
		CheckNVSettings();

	iprintf( "NV_Settings.oamodesel = %d\r\n", NV_Settings.arrData[cur].oamodesel);
    iprintf( "NV_Settings.modemip = "); ShowIP(NV_Settings.arrData[cur].modemip); iprintf("\r\n");
    iprintf( "NV_Settings.modemport = %d\r\n", NV_Settings.arrData[cur].modemport);
    iprintf( "NV_Settings.modemroot = %s\r\n", NV_Settings.arrData[cur].modemroot);
    iprintf( "NV_Settings.modemuser = %s\r\n", NV_Settings.arrData[cur].modemuser);
    iprintf( "NV_Settings.modempass = %s\r\n", NV_Settings.arrData[cur].modempass);
    iprintf( "NV_Settings.modemsat = %ld\r\n", NV_Settings.arrData[cur].modemsat);
    iprintf( "NV_Settings.modempol = '%c'\r\n", NV_Settings.arrData[cur].modempol);
    iprintf( "NV_Settings.antlock = %ld\r\n", NV_Settings.arrData[cur].antlock);
	iprintf( "NV_Settings.serialspeed = %c\r\n", NV_Settings.arrData[cur].serialspeed);
	iprintf( "NV_Settings.vscan = %ld\r\n", NV_Settings.arrData[cur].vscan);
	iprintf( "NV_Settings.stepadjust = %d\r\n", NV_Settings.arrData[cur].stepadjust);
	iprintf( "NV_Settings.stepignore = %d\r\n", NV_Settings.arrData[cur].stepignore);
	iprintf( "NV_Settings.trackgain = %d\r\n", NV_Settings.arrData[cur].trackgain);
	iprintf( "NV_Settings.lockthresh = %ld\r\n", NV_Settings.arrData[cur].lockthresh);
	iprintf( "NV_Settings.snrthreshold = %c\r\n", NV_Settings.arrData[cur].snrthreshold);
	iprintf( "NV_Settings.snrhigh = %ld\r\n", NV_Settings.arrData[cur].snrhigh);
	iprintf( "NV_Settings.snrmin = %ld\r\n", NV_Settings.arrData[cur].snrmin);
    //iprintf( "NV_Settings.nvChar = %d\r\n", NV_Settings.nvChar );
    iprintf("\r\n");
}


void DisplayRuntimeConfig(struct config_data* configData)
{
	iprintf("\r\n\r\n--- RUNTIME CONFIG SETTINGS ---\r\n");
	// Display current runtime values using Interface read-only functions
	iprintf("Mode Sel: %d\r\n", configData->oamodesel);
	iprintf("Modem IP:   "); ShowIP(configData->modemip); iprintf("\r\n");
	iprintf("Modem Port: %d\r\n", configData->modemport);
	iprintf("Modem root: %s\r\n", configData->modemroot);
	iprintf("Modem user: %s\r\n", configData->modemuser);
	iprintf("Modem password: %s\r\n", configData->modempass);
	iprintf("Satellite: %ld\r\n", configData->modemsat);
	iprintf("Modem pol: %c\r\n", configData->modempol);
	iprintf("Ant Lock: %ld\r\n", configData->antlock);
	iprintf("Serial speed: %c\r\n", configData->serialspeed);
	iprintf("Scan Velocity: %ld\r\n", configData->vscan);
	iprintf("Step Adjust: %d\r\n", configData->stepadjust);
	iprintf("Step Ignore: %d\r\n", configData->stepignore);
	iprintf("Track Gain: %d\r\n", configData->trackgain);
	iprintf("Lock Thresh: %ld\r\n", configData->lockthresh);
	iprintf("SNR Threshold: %c\r\n", configData->snrthreshold);
	iprintf("SNR High: %ld\r\n", configData->snrhigh);
	iprintf("SNR Min: %ld\r\n", configData->snrmin);

	OSTimeDly(TICKS_PER_SECOND * 2);
}


void SavingStoredData(void)
{
	bCheckedNV = false;

	uint32_t maxUserParamSize = 0;

	maxUserParamSize = 8 * 1024;
	iprintf("Maximum User Parameter Size = %ldK Bytes\r\n", maxUserParamSize/1024);

	iprintf("Incrementing all NV_Settings values by 1\r\n");
	NV_Settings.nvChar++;

	//CheckNVSettings();

	if ( sizeof(NV_Settings) < maxUserParamSize ) {
		iprintf("Saving...");
		int rc = SaveUserParameters( &NV_Settings, sizeof( NV_Settings ) );
		iprintf("complete and wrote %d bytes.\r\n", rc);
	} else {
		iprintf("*** ERROR: Your structure exceeds User Parameter Flash space, save aborted\r\n");
	}
	OSTimeDly(TICKS_PER_SECOND * 1);

	ForceReboot();

	iprintf("\r\n\r\n");
}

void User5Main(void)
{
	iprintf("Application: reading NVRAM\r\n");

	CheckNVSettings();

	iprintf("Initialize system parameters\r\n");
	//Initailize_legacy_settings(currentRec, exchData);
	//DisplayNVSettings(1);
	//DisplayNVSettings(2);
	//DisplayNVSettings(3);
	iprintf("System parameters are configured.\r\n");
}

/* End of File */
