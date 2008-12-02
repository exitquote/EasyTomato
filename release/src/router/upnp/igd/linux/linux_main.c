/*
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 *
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: linux_main.c,v 1.3 2005/05/25 02:13:10 honor Exp $
 */

#include <errno.h>			// for errno, of course.
#include <error.h>			// for perror
#include <signal.h>			// for signal, etc.
#include <assert.h>			// for assert, of course.
#include <stdlib.h>			// for malloc, free, etc.
#include <string.h>			// for memset, strncasecmp, etc.
#include <stdarg.h>			// for va_list, etc.
#include <stdio.h>			// for printf, perror, fopen, fclose, etc.
#include <net/if.h>			// for struct ifreq, etc.
#include <sys/ioctl.h>		// for SIOCGIFCONF, etc.
#include <fcntl.h>			// for fcntl, F_GETFL, etc.
#include <unistd.h>			// for read, write, etc.
#include <arpa/inet.h>		// for inet_aton, inet_addr, etc.
#include <time.h>			// for time
#include <netinet/in.h>		// for sockaddr_in
#include <wait.h>			// for sockaddr_in
#include <syslog.h>

#include "ctype.h"
#include "upnp_dbg.h"
#include "upnp.h"


extern void init_event_queue(int);


/* 20050524 by honor */
int ssdp_interval;
int max_age;

/*
static void reap(int sig)
{
	pid_t pid;

	while ((pid = waitpid(-1, NULL, WNOHANG)) > 0)
		UPNP_TRACE(("Reaped %d\n", pid));
}
*/

int main(int argc, char *argv[])
{
	extern char g_wandevs[];
	extern DeviceTemplate IGDeviceTemplate;
	char **argp = &argv[1];
	char *wanif = NULL;
	char *lanif = NULL;
	int daemonize = 0;
//	int sleep_time = 0;

	ssdp_interval = 0;
	max_age = 0;

	while (argp < &argv[argc]) {
		if (strcasecmp(*argp, "-L") == 0) {
			lanif = *++argp;
		}
		else if (strcasecmp(*argp, "-W") == 0) {
			wanif = *++argp;
			strcpy(g_wandevs, wanif);
		}
		else if (strcasecmp(*argp, "-D") == 0) {
			daemonize = 1;
		}
/*		else if (strcasecmp(*argp, "-S") == 0) {
			sleep_time = atoi(*++argp);
		}*/
		else if (strcasecmp(*argp, "-I") == 0) {
			ssdp_interval = atoi(*++argp);
		}
		else if (strcasecmp(*argp, "-A") == 0) {
			max_age = atoi(*++argp);
		}
		argp++;
	}

	if (!ssdp_interval) {
		ssdp_interval = UPNP_REFRESH;
	}
	if (!max_age) {
		max_age = SSDP_REFRESH;
	}

	if ((lanif == NULL) || (wanif == NULL)) {
		fprintf(stderr,
			"Usage: %s [options]\n"
			"* -L <lanif>  LAN interface\n"
			"* -W <wanif>  WAN interface\n"
			"  -D          Daemonize\n"
			"  -I <n>      SSDP interval\n"	// fixme
			"  -A <n>      Max age\n"		// fixme
			"* required\n\n"
			"  SIGUSR2     reload port forwarding\n",
				argv[0]);
		exit(1);
	}

	if (daemonize && daemon(1, 1) == -1) {
		perror("daemon");
		exit(errno);
	}

	UPNP_TRACE(("main nappy...\n"));	// zzz
	sleep(5);

	init_event_queue(40);

	/* We need to have a reaper for child processes we may create.
	That happens when we send signals to the dhcp process to
	release an renew a lease on the external interface. */
//	signal(SIGCHLD, reap);

	/* For some reason that I do not understand, this process gets
	a SIGTERM after sending SIGUSR1 to the dhcp process (to
	renew a lease).  Ignore SIGTERM to avoid being killed when
	this happens.  */
	//	signal(SIGTERM, SIG_IGN);

	signal(SIGUSR1, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);

	pmap_load();
	
/*
	if (sleep_time) {
		UPNP_TRACE(("SES2 first reboot, waiting for %d seconds to start UPNP\n", sleep_time));
		sleep(sleep_time);
		UPNP_TRACE(("Restart UPNP daemon.\n"));
	}
*/

	UPNP_TRACE(("calling upnp_main\n"));
	return upnp_main(&IGDeviceTemplate, lanif);
}
