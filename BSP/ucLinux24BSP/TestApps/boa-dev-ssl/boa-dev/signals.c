/*
 *  Boa, an http server
 *  Copyright (C) 1995 Paul Phillips <psp@well.com>
 *  Some changes Copyright (C) 1996 Larry Doolittle <ldoolitt@jlab.org>
 *  Some changes Copyright (C) 1996,97 Jon Nelson <nels0988@tc.umn.edu>
 *  Some changes Copyright (C) 1997 Alain Magloire <alain.magloire@rcsm.ee.mcgill.ca>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 1, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/* boa: signals.c */

#include "boa.h"
#include <sys/wait.h>			/* wait */
#include <signal.h>				/* signal */

static struct sigaction g_SigAct_SIGTERM;
static struct sigaction g_SigAct_SIGINT;
static int iSigExit = 0;

void sigterm(int);

/*
 * Name: init_signals
 * Description: Sets up signal handlers for all our friends.
 */

void init_signals(void)
{
	struct sigaction sa;

	sa.sa_flags = 0;

	sigprocmask(SIG_SETMASK, NULL, &sa.sa_mask);

	sigaddset(&sa.sa_mask, SIGTERM);
	sigaddset(&sa.sa_mask, SIGINT);
	sigaddset(&sa.sa_mask, SIGPIPE);

	sa.sa_handler = sigterm;
	sigaction(SIGTERM, &sa, &g_SigAct_SIGTERM);

	sa.sa_handler = sigterm;
	sigaction(SIGINT, &sa, &g_SigAct_SIGINT);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, NULL);

}



void sigterm(int dummy)
{
	iSigExit = dummy;
	lame_duck_mode = 1;
}

void lame_duck_mode_run(int *pifd_Server, int iPortNum)
{
	int i;
	for (i = 0; i < iPortNum; i++)
	{
    	close(pifd_Server[i]);
    	FD_CLR(pifd_Server[i], &block_read_fdset);
	    close(pifd_Server[i]);
	}
   	my_syslog(LOG_INFO, "caught SIGTERM, starting shutdown\n");

    lame_duck_mode = 2;

   	if (iSigExit == SIGTERM)
	{
		if (g_SigAct_SIGTERM.sa_handler != 0 && g_SigAct_SIGTERM.sa_handler != SIG_IGN && g_SigAct_SIGTERM.sa_handler != SIG_DFL)
			(*g_SigAct_SIGTERM.sa_handler)(iSigExit);
	}
	else if (iSigExit == SIGINT)
	{
		if (g_SigAct_SIGINT.sa_handler != 0 && g_SigAct_SIGINT.sa_handler != SIG_IGN && g_SigAct_SIGINT.sa_handler != SIG_DFL)
			(*g_SigAct_SIGINT.sa_handler)(iSigExit);
	}
	die(SHUTDOWN);
}

