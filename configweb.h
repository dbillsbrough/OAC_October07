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

#ifndef CONFIG_WEB_H
#define CONFIG_WEB_H

extern int ProcessAddressChangePost( int sock, char *url, char *pData, char *rxBuffer , const char *post_url_destination);
extern int ChangeAdminConfiguration( int sock, char *url, char *pData, char *rxBuffer , char *post_url_destination);
extern int ProcessButtonForms(int sock, char *url, char *pData, char *rxBuffer);

#endif
