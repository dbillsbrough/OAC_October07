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
#include <stdlib.h>
#include <ctype.h>
#include <startnet.h>
#include <ip.h>
#include <tcp.h>
#include <htmlfiles.h>
#include <http.h>
#include <string.h>
#include "formtools.h"

void FormOutputSelect( int sock, const char *name, int selnum, const char **list )
{
   writestring( sock, "<SELECT NAME=\"" );
   writestring( sock, name );
   writestring( sock, "\" SIZE=1>" );
   const char **lp = list;
   int n = 0;
   while ( lp[n][0] )
   {
      if ( ( n + 1 ) == selnum )
      {
         writestring( sock, "<OPTION SELECTED>" );
      }
      else
      {
         writestring( sock, "<OPTION>" );
      }
      writesafestring( sock, lp[n] );
      n++;
   }
   writestring( sock, "</SELECT>\n" );
}

void FormOutputCheckbox( int sock, const char *name, BOOL checked )
{
   writestring( sock, "<INPUT TYPE=\"checkbox\" NAME=\"" );
   writestring( sock, name );
   if ( checked )
   {
      writestring( sock, "\" VALUE=\"checked\" CHECKED>\n" );
   }
   else
   {
      writestring( sock, "\" VALUE=\"checked\">\n" );
   }
}

void FormOutputInput( int sock, const char *name, int siz, const char *val )
{
   char buf[10];

   writestring( sock, "<INPUT NAME=\"" );
   writestring( sock, name );
   writestring( sock, "\" TYPE=\"text\" SIZE=\"" );
   sniprintf( buf, 10, "%d", siz );
   writestring( sock, buf );
   writestring( sock, "\" VALUE=\"" );
   writesafestring( sock, val );
   writestring( sock, "\" " );
}

void FormOutputNumInput( int sock, const char *name, int siz, int val )
{
   char buf[20];
   sniprintf( buf, 20, "%d", val );
   FormOutputInput( sock, name, siz, buf );
}

void FormOutputRealInput( int sock, const char *name, int siz, float val )
{
   char buf[20];
   sniprintf( buf, 20, "%1.2f", val );
   FormOutputInput( sock, name, siz, buf );
}

void FormOutputIPInput(int sock, const char *name, IPADDR4 ip)
{
   char buf[20];
   ip.sprintf(buf, 19);
   FormOutputInput( sock, name, 20, buf );
}

IPADDR4 FormExtractIP(const char *name, char *pData, IPADDR4 defValue)
{
    char rxBuf[40];
    IPADDR4 ip;

    if (ExtractPostData(name, pData, rxBuf, 40))
    {
        ip.SetFromAscii(rxBuf);
    }
    else
    {
        ip = defValue;
    }
    return ip;
}

long FormExtractNum( const char *name, char *pData, long def_val )
{
   char rx_buf[40];
   if ( ExtractPostData( name, pData, rx_buf, 40 ) )
   {
      return atol( rx_buf );
   }
   return def_val;
}

double FormExtractReal( const char *name, char *pData, double def_val )
{
	char rx_buf[40];

	if ( ExtractPostData( name, pData, rx_buf, 40 ) ) {
	      return atof( rx_buf );
	}
	return def_val;
}

BOOL FormExtractCheck( const char *name, char *pData, BOOL def_val )
{
   char rx_buf[40];
   if ( ExtractPostData( name, pData, rx_buf, 40 ) )
   {
      if ( ( rx_buf[0] == 'C' ) || ( rx_buf[0] == 'c' ) )
      {
         return TRUE;
      }
      else
      {
         return FALSE;
      }
   }
   return def_val;
}

int FormExtractSel( const char *name, char *pData, const char **pList, int defsel )
{
   char rx_buf[100];
   if ( ExtractPostData( name, pData, rx_buf, 40 ) )
   {
      int n = 0;
      while ( pList[n][0] )
      {
         if ( strcmp( pList[n], rx_buf ) == 0 )
         {
            return n + 1;
         }
         n++;
      }
   }
   return defsel;
}

char tbuf[40];
void ShowIP2Sock( int sock, IPADDR ip )
{
    ip.fdprint(sock);
}
