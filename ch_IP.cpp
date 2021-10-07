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

#include <predef.h>
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <dhcpclient.h>
#include <smarttrap.h>
#include <taskmon.h>
#include <string.h>
#include <networkdebug.h>
#include "configweb.h"

//extern void initWithWeb();

extern "C" { 
    void User3Main(void * pd);
}

DhcpObject *pTheDhcpObj;

/*-----------------------------------------------------------------------------
 * Handle HTTP Post
 *
 * In this case we only have one post. If your code has multiple forms figure
 * out from the URL what to call. If it's the html config call this.
 *-----------------------------------------------------------------------------*/
int DoMatch2(int sock, char *url, char *pData, char *rxBuffer)
{
    return ProcessAddressChangePost( sock, url, pData, rxBuffer , "changeIP.htm");
}


/*-----------------------------------------------------------------------------
 * UserMain
 *----------------------------------------------------------------------------*/
void User3Main(void)
{

    #ifdef _DEBUG
        InitializeNetworkGDB();
    #endif

    iprintf("Application 3 started\n");
}
