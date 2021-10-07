/*
 * profilesconfig.cpp
 *
 *  Created on: August 26, 2021
 *      Author: David Billsbrough
 */

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
#include <init.h>
#include <fdprintf.h>
#include "s2e.h"

extern struct NV_SettingsStruct NV_Settings;
extern struct profile_labels Profiles;
//extern struct config_data configData;

extern char current_profilename[20];
extern int currentRec;

extern "C"
{
	void resave_profiles_to_stored_data( int, struct profile_labels * );
}

/* function prototypes */
void display_profiles( int num, struct profile_labels* Profiles );


void resave_profiles_to_stored_data( int num, struct profile_labels* Profiles )
{
	Profiles->curProfile = num;
	NV_Settings.currentRecord = num;

	memmove( &NV_Settings.profiles, &Profiles, sizeof(Profiles) );
	iprintf("Resaving profile data to NVRAM.\n");
}

void display_profiles( int num, struct profile_labels* Profiles )
{
	Profiles->curProfile = num;

	iprintf("\r\n");
	//iprintf("*** PROFILE NAMES ***\r\n");
	//iprintf("\r\n");
	//iprintf(" *  1. %s\n", NV_Settings.profiles.profile_label_01);
	//iprintf(" *  2. %s\n", NV_Settings.profiles.profile_label_02);
	//iprintf(" *  3. %s\n", NV_Settings.profiles.profile_label_03);
	//iprintf(" *  4. %s\n", NV_Settings.profiles.profile_label_04);
	//iprintf(" *  5. %s\n", NV_Settings.profiles.profile_label_05);
	//iprintf(" *  6. %s\n", NV_Settings.profiles.profile_label_06);
	//iprintf(" *  7. %s\n", NV_Settings.profiles.profile_label_07);
	//iprintf(" *  8. %s\n", NV_Settings.profiles.profile_label_08);
	//iprintf(" *  9. %s\n", NV_Settings.profiles.profile_label_09);
	//iprintf(" * 10. %s\n", NV_Settings.profiles.profile_label_10);
	//iprintf("\r\n");
	//iprintf("Current profile: %d\r\n", NV_Settings.profiles.curProfile);
	//iprintf("\r\n");
}

void User7Main()
{
	//struct config_data* configData;

    iprintf("Application: setup the profiles.\r\n");
    strcpy(current_profilename, " ");
    currentRec = NV_Settings.currentRecord;
	iprintf("Current profile: %d\r\n", currentRec);
	//resave_profiles_to_stored_data( currentRec, &Profiles );
    //memcpy( &Profiles, &NV_Settings.profiles, sizeof(profile_labels));
    //memcpy( &configData, &NV_Settings.arrData[currentRec], sizeof(config_data));
	display_profiles( currentRec, &Profiles );
}

/* End of File */
