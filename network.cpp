// written: 19 May 2021

#include <predef.h>
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <serial.h>
#include <iosys.h>
#include <utils.h>
#include <ip.h>
#include <tcp.h>

#include "s2e.h"

int net_focusMission_read(int fdnet, char *buffer)
{
	int bufsize = 2048;
	int bytesRead = 0;
	int statusRead = -1;	// <0 = ERROR (caller should close and restart)
							// 0 = All OK, but no bytes available
			    			// >0 = Number of bytes read

	statusRead = dataavail(fdnet);

	if ( statusRead == 1 ) {
		bytesRead = read(fdnet, buffer, bufsize);
		return bytesRead;
	}

	return bytesRead;
}

int LatchTime( struct timeval* tmr )
{
	gettimeofday( tmr, NULL );

	//iprintf("timer: %lld\n", tmr->tv_sec );
	//iprintf("timer2: %ld\n", tmr->tv_usec );

	return 0;
}

//
bool TimerElapsed( struct timeval* tmr, double checkTime )
{
	bool timeExpired = false;
	struct timeval nowTime;
	double elapsedTime;

	gettimeofday( &nowTime, NULL );
	elapsedTime = (double) (nowTime.tv_sec - tmr->tv_sec) +
		      (double) (nowTime.tv_usec - tmr->tv_usec) / 1000000.0;
	if ( elapsedTime >= checkTime )
		timeExpired = true;

	return( timeExpired );
}

/* End of file */
