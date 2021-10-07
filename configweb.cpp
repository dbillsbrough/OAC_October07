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
#include <serial.h>
#include <iosys.h>
#include <utils.h>
#include <ip.h>
#include <tcp.h>
#include <htmlfiles.h>
#include <http.h>
#include <autoupdate.h>
#include <system.h>
#include <dhcpclient.h>
#include <utils.h>
#include <string.h>
#include <dns.h>
#include "formtools.h"

extern BOOL bShowDebug;

#define dbprintf if (bShowDebug) iprintf

extern DhcpObject *pTheDhcpObj;
#define IP_ADDR_MODE_DHCP (1)
#define IP_ADDR_MODE_STATIC (2)


//----- Function Prototypes -----
extern "C" {
   void IPMode( int sock, PCSTR url );
   void DeviceAddress( int sock, PCSTR url );
   void DisplayDeviceAddress( int sock, PCSTR url );
   void DeviceMask( int sock, PCSTR url );
   void DisplayDeviceMask( int sock, PCSTR url );
   void DeviceGate( int sock, PCSTR url );
   void DisplayDeviceGate( int sock, PCSTR url );
   void DeviceDns( int sock, PCSTR url );
   void DisplayDeviceDns( int sock, PCSTR url );
}


/* Declare the selection lists for the web page configuration */
const char *IPModeList[] = { "DHCP", "Static settings", "\0" };


bool InDhcpMode()
{
    if ( gConfigRec.ip_Addr.IsNull() )
        return true;
    return false;
}

void IPMode(int sock, PCSTR url)
{
    BYTE Old_Addr_mode;

    if ( gConfigRec.ip_Addr.IsNull() )
    {
        Old_Addr_mode = IP_ADDR_MODE_DHCP;
    }
    else
    {
        Old_Addr_mode = IP_ADDR_MODE_STATIC;
    }
    FormOutputSelect(sock, "IPMode", Old_Addr_mode, IPModeList);
}

void DeviceAddress(int sock, PCSTR url)
{
    FormOutputIPInput(sock, "DeviceAddress", gConfigRec.ip_Addr);
}

void DisplayDeviceAddress(int sock, PCSTR url)
{
    if ( InDhcpMode() )
        ShowIP2Sock(sock, EthernetIP);
}

void DeviceMask(int sock, PCSTR url)
{
    FormOutputIPInput(sock, "DeviceMask", gConfigRec.ip_Mask);
}

void DisplayDeviceMask(int sock, PCSTR url)
{
    if ( InDhcpMode() )
        ShowIP2Sock(sock, EthernetIpMask);
}

void DeviceGate(int sock, PCSTR url)
{
    FormOutputIPInput(sock, "DeviceGate", gConfigRec.ip_GateWay);
}

void DisplayDeviceGate(int sock, PCSTR url)
{
    if ( InDhcpMode() )
        ShowIP2Sock(sock, EthernetIpGate);
}

void DeviceDns(int sock, PCSTR url)
{
    FormOutputIPInput(sock, "DeviceDns", gConfigRec.ip_DNS_server);
}

void DisplayDeviceDns(int sock, PCSTR url)
{
    if ( InDhcpMode() )
        ShowIP2Sock(sock, EthernetDNS);
}

int ProcessAddressChangePost(int sock, char *url, char *pData, char *rxBuffer, const char * post_url_destination)
{
    BYTE Old_Addr_mode;

    if ( gConfigRec.ip_Addr.IsNull() )
    {
        Old_Addr_mode = IP_ADDR_MODE_DHCP;
    }
    else
    {
        Old_Addr_mode = IP_ADDR_MODE_STATIC;
    }

    BYTE new_IP_Addr_mode = FormExtractSel("IPMode", pData, IPModeList, 1);
    IPADDR4 new_addr = FormExtractIP("DeviceAddress", pData, gConfigRec.ip_Addr);
    IPADDR4 new_mask = FormExtractIP("DeviceMask", pData, gConfigRec.ip_Mask);
    IPADDR4 new_gate = FormExtractIP("DeviceGate", pData, gConfigRec.ip_GateWay);
    IPADDR4 new_dns = FormExtractIP("DeviceDns", pData, gConfigRec.ip_DNS_server);

    if ( (new_IP_Addr_mode == IP_ADDR_MODE_DHCP) && (Old_Addr_mode != IP_ADDR_MODE_DHCP) )
    {
        /* We have switched from static to DHCP mode */
        /* First save the old IP address information */
        IPADDR4 OldEthernetIP = EthernetIP;
        IPADDR4 OldEthernetIpMask = EthernetIpMask;
        IPADDR4 OldEthernetIpGate = EthernetIpGate;
        IPADDR4 OldEthernetDNS = EthernetDNS;

        /* Now negotiate a DHCP address */
        pTheDhcpObj->StartDHCP(); //Start DHCP
        if ( OSSemPend(&pTheDhcpObj->NotifySem, 10 * TICKS_PER_SECOND) == OS_TIMEOUT ) //Wait 10 seconds
        {
            //DHCP did not initialize handle the error here
            iprintf("DHCP failed to initialize, system has IP address of Zero\r\n");
        } else {
            // We have a new DHCP address
            // Save this new DHCP address
            IPADDR4 DHCP_EthernetIP = EthernetIP;
            IPADDR4 DHCP_EthernetIpMask = EthernetIpMask;
            IPADDR4 DHCP_EthernetIpGate = EthernetIpGate;
            IPADDR4 DHCP_EthernetDNS = EthernetDNS;

            // Set the address back to the old address
            EthernetIP = OldEthernetIP;
            EthernetIpMask = OldEthernetIpMask;
            EthernetIpGate = OldEthernetIpGate;
            EthernetDNS = OldEthernetDNS;

            OSTimeDly(TICKS_PER_SECOND * 2); /* Let Stuff clear in Network land */

            /* Write out a web page redirect so the web browser will go to the new address */
            char buf[80];
            ConfigRecord new_rec;
            memcpy(&new_rec, &gConfigRec, sizeof(new_rec));

            /* Redirect to new address */
            PBYTE ipb = (PBYTE) &DHCP_EthernetIP;
            sniprintf(buf, 80, "http://%d.%d.%d.%d", (int) ipb[0], (int) ipb[1], (int) ipb[2], (int) ipb[3]);
            RedirectResponse(sock, buf);
            close(sock);

            OSTimeDly(TICKS_PER_SECOND * 2); /* Let Stuff clear in Network land */

            /* Now that the web page stuff is done reset the addresses to the new addresses */
            EthernetIP = DHCP_EthernetIP;
            EthernetIpMask = DHCP_EthernetIpMask;
            EthernetIpGate = DHCP_EthernetIpGate;
            EthernetDNS = DHCP_EthernetDNS;

            new_rec.ip_Addr = 0;
            new_rec.ip_Mask = 0;
            new_rec.ip_GateWay = 0;
            new_rec.ip_DNS_server = 0;
            UpdateConfigRecord(&new_rec); /* Save the IP config record */
            return 0;
        }
    }/* End of switch from static to DHCP */
    else if ( new_IP_Addr_mode == IP_ADDR_MODE_STATIC )
    {
        /* Just switched from DHCP to static address or changed static address */
        if ( (new_addr != gConfigRec.ip_Addr) || (new_mask != gConfigRec.ip_Mask) || (new_gate != gConfigRec.ip_GateWay)
                || (new_dns != gConfigRec.ip_DNS_server) || (Old_Addr_mode != IP_ADDR_MODE_STATIC) )
        {
            /* Redirect to new address */
            char buf[80];
            ConfigRecord new_rec;
            memcpy(&new_rec, &gConfigRec, sizeof(new_rec));
            PBYTE ipb = (PBYTE) &new_addr;
            sniprintf(buf, 80, "http://%d.%d.%d.%d", (int) ipb[0], (int) ipb[1], (int) ipb[2], (int) ipb[3]);
            RedirectResponse(sock, buf);
            close(sock);

            OSTimeDly(TICKS_PER_SECOND * 2); /* Let Stuff clear in Network land */

            if ( Old_Addr_mode != IP_ADDR_MODE_STATIC )
            {
                pTheDhcpObj->StopDHCP();
            }

            /* Set up new address */
            EthernetIP = new_addr;
            EthernetIpMask = new_mask;
            EthernetIpGate = new_gate;
            EthernetDNS = new_dns;

            new_rec.ip_Addr = new_addr;
            new_rec.ip_Mask = new_mask;
            new_rec.ip_GateWay = new_gate;
            new_rec.ip_DNS_server = new_dns;

            UpdateConfigRecord(&new_rec);
            return 0;
        }
    }
    RedirectResponse(sock, post_url_destination);

    return 0;
}
