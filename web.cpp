/* Revision: 2.9.5 */

/******************************************************************************
* Copyright 1998-2021 NetBurner, Inc.  ALL RIGHTS RESERVED
*
*    Permission is hereby granted to purchasers of NetBurner Hardware to use or
*    modify this computer program for any use as long as the resultant program
*    is only executed on NetBurner provided hardware.
*
*    No other rights to use this program or its derivatives in part or in
*    whole are granted.
*
*    It may be possible to license this or other NetBurner software for use on
*    non-NetBurner Hardware. Contact sales@Netburner.com for more information.
*
*    NetBurner makes no representation or warranties with respect to the
*    performance of this computer program, and specifically disclaims any
*    responsibility for any damages, special or consequential, connected with
*    the use of this program.
*
* NetBurner
* 16855 W Bernardo Dr
* San Diego, CA 92127
* www.netburner.com
******************************************************************************/

/**
 @file       PostApplicationUpdate/main.cpp
 @brief      Web Form Application Update Example

 This example demonstrates how to update your NetBurner device application
 image from a web page using MultiPart Forms. The POST handler will process
 the incoming application file and reprogram the NetBurner device's
 application flash memory. If the reboot checkbox was checked, it will reboot
 the NetBurner device.
 **/
#include <predef.h>
#include <stdio.h>
#include <constants.h>
#include <basictypes.h>
#include <string.h>
#include <startnet.h>
#include <iosys.h>
#include <http.h>
#include <multipartpost.h>
#include <streamupdate.h>
#include <bsp.h>
#include <tcp.h>
#include <init.h>
#include <fdprintf.h>

#define MAX_PATH_LEN    255
#define MAX_FILE_SIZE   10000
#define MAX_BUF_LEN 80

char textForm1[MAX_BUF_LEN];
char textForm2[MAX_BUF_LEN];
char textForm3[MAX_BUF_LEN];
char textForm4[MAX_BUF_LEN];
char textForm5[MAX_BUF_LEN];
char textForm6[MAX_BUF_LEN];

extern const char *PlatformName;

extern "C"
{
	void WebTextForm1(int sock, PCSTR url);
	void WebTextForm2(int sock, PCSTR url);
	void WebTextForm3(int sock, PCSTR url);
	void WebTextForm4(int sock, PCSTR url);
	void WebTextForm5(int sock, PCSTR url);
	void WebTextForm6(int sock, PCSTR url);
};

int MyDoPost(int sock, char *url, char *pData, char *rxBuffer);
int DoMatch1(int sock, char *url, char *pData, char *rxBuffer);
extern int DoMatch2(int, char *, char *, char *);
extern int DoMatch3(int, char *, char *, char *);
extern int DoMatch4(int, char *, char *, char *);
extern int DoMatch5(int, char *, char *, char *, char *);
extern int DoMatch6(int, char *, char *, char *, char *);

static TwoPartUpdateStruct up_struct;
static DWORD WroteHeaderTime;

void WebTextForm1(int sock, PCSTR url)
{
	writestring(sock, textForm1);
}

void WebTextForm2(int sock, PCSTR url)
{
	writestring(sock, textForm2);
}

void WebTextForm3(int sock, PCSTR url)
{
	writestring(sock, textForm3);
}

void WebTextForm4(int sock, PCSTR url)
{
	writestring(sock, textForm4);
}

void WebTextForm5(int sock, PCSTR url)
{
	writestring(sock, textForm5);
}

void WebTextForm6(int sock, PCSTR url)
{
	writestring(sock, textForm6);
}


/*-----------------------------------------------------------------------------
 * Write beginning of web page delivered to web client
 *----------------------------------------------------------------------------*/
void SpecialFileReader(int fd, const char * url)
{
    SendHTMLHeader(fd);
    writestring(fd, "<HTML><BODY><B>Starting to uploaded file<BR>");
    ReadTwoPartAppUdate(fd, up_struct);
    WroteHeaderTime = TimeTick;
    return;
}

/*-------------------------------------------------------------------
 * This POST handler will process the incoming application file
 * and reprogram the NetBurner device's application flash memory.
 * If the reboot checkbox was checked, it will reboot the NetBurner
 * device.
 * ----------------------------------------------------------------*/
int MyDoPost(int sock, char *url, char *pData, char *rxBuffer)
{
	char responce_page[30];

	strcpy(responce_page, "pages/admin.html");

    iprintf("----- Processing Post -----\r\n");
    iprintf("Post URL: %s\r\n", url);
    iprintf("Post Data: %s\r\n", pData);

    if (httpstricmp(url + 1, "FILEPOST.HTM")) {

    	ExtractPostData("textForm1", pData, textForm1, MAX_BUF_LEN);
    	iprintf("DEBUG: trying match #1\r\n");
    	iprintf(" data is %s\r\n", pData);
    	DoMatch1(sock, url, pData, rxBuffer);

    } else if (httpstricmp(url + 1, "FORMPOST.HTM")) {

    	ExtractPostData("textForm2", pData, textForm2, MAX_BUF_LEN);
    	iprintf("DEBUG: trying match #2\r\n");
    	iprintf(" data is %s\r\n", pData);
    	DoMatch2(sock, url, pData, rxBuffer);

    } else if (httpstricmp(url + 1, "FORMADMIN.HTM")) {

        ExtractPostData("textForm3", pData, textForm3, MAX_BUF_LEN);
        iprintf("DEBUG: trying match #3\r\n");
        iprintf(" data is %s\r\n", pData);
        DoMatch3(sock, url, pData, rxBuffer);

    } else if (httpstricmp(url + 1, "BUTTONPOST.HTM")) {

        ExtractPostData("textForm4", pData, textForm4, MAX_BUF_LEN);
        iprintf("DEBUG: trying match #4\r\n");
        iprintf(" data is %s\r\n", pData);
        DoMatch4(sock, url, pData, rxBuffer);

    } else if (httpstricmp(url + 1, "PAUSEPOST.HTM")) {

        ExtractPostData("textForm5", pData, textForm5, MAX_BUF_LEN);
        iprintf("DEBUG: trying match #5\r\n");
        iprintf(" data is %s\r\n", pData);
        DoMatch5(sock, url, pData, rxBuffer, responce_page);

    } else if (httpstricmp(url + 1, "LOADPROFILE.HTM")) {

        ExtractPostData("textForm6", pData, textForm6, MAX_BUF_LEN);
        iprintf("DEBUG: trying match #6\r\n");
        iprintf(" data is %s\r\n", pData);
        DoMatch6(sock, url, pData, rxBuffer, responce_page);

	} else {
		NotFoundResponse(sock, url);
		iprintf("We did not match any page\r\n");
	}
	return 1;
}

int DoMatch1(int sock, char *url, char *pData, char *rxBuffer)
{
    char buf[MAX_PATH_LEN];
    BOOL bReboot = false;
    BOOL bSuccess = false;

	// Look for reboot checkbox
    ExtractPostData("rebootCheckbox", pData, buf, MAX_PATH_LEN);

    //iprintf("pData: %s\n", pData);
    iprintf("buf is %s from the reboot checkbox\n", buf);

    if (strcmp (buf, "true")) {
        bReboot = true;
        iprintf("Reboot checkbox selected\r\n ");
    } else {
    	bReboot = false;
        iprintf("Reboot checkbox not selected\r\n ");
    }

    if (WroteHeaderTime < (TimeTick - 5)) {
        SendHTMLHeader(sock);
        writestring(sock, "<HTML><BODY><B>File did not upload correctly<BR>");
    } else {
        writestring(sock, "<B>Upload Complete<BR>");
    }

    if (up_struct.Result == STREAM_UP_OK) {
        clrsockoption(sock, SO_NOPUSH | SO_NONAGLE); // Make socket writes happen immediately so web page sees progress...
        if (up_struct.S0Record[0]) {
            if (strcmp((char *) up_struct.S0Record, PlatformName) == 0) {
                writestring(sock, "File platforms match<BR>");
            }
        } else {
            writestring(sock, "Could not find file platform version<BR>");
        }

        writestring(sock, "Beginning to program application file<BR>");

        if (DoTwoPartAppUpdate(up_struct) == STREAM_UP_OK) {
            if (bReboot) {
                PBYTE ipb = (PBYTE) &EthernetIP;
                fdprintf(sock, "Board will complete reboot in %d seconds<BR>\r\n", gConfigRec.wait_seconds + 3);
                fdprintf(sock, "To load the new web page click "
                        "<A HREF=\"http://%d.%d.%d.%d\">here</a><BR>\r\n", (int) ipb[0], (int) ipb[1], (int) ipb[2], (int) ipb[3]);
                bSuccess = true;
            } else {
                writestring(sock, "The board was not rebooted, you must reboot for the change to take effect.<BR>");
            }
        } else {
            writestring(sock, "Flash program failure<BR>");
        }
    } else {
        writestring(sock, "Stream update failure<BR>");
    }

    writestring(sock, "</B></BODY></HTML>");
    close(sock);

    AbortTwoPartAppUpdate(up_struct); //Clean up

    bReboot = true;

    if ((bReboot) && (bSuccess)) {
        iprintf("Rebooting ...");
        OSTimeDly(TICKS_PER_SECOND * 2);
        ForceReboot();
    }

    return 0;
}


void User2Main(void)
{
    //initWithWeb();

    iprintf("MultiPart Form Application Update\r\n");

    if (EnableMultiPartForms(MAX_FILE_SIZE)) {
    	RegisterSpecialLongFile("userfile1", SpecialFileReader);
        iprintf("Allocated %ld bytes of memory for MultiPart Forms\r\n", (DWORD) MAX_FILE_SIZE);
        iprintf("Maximum file path length: %d\r\n", (WORD) MAX_PATH_LEN);
        SetNewPostHandler(MyDoPost);
    } else {
        iprintf("Could not allocate MultiPart Form memory\r\n");
    }

    iprintf("MultiPart form initialization complete.\r\n");
}
