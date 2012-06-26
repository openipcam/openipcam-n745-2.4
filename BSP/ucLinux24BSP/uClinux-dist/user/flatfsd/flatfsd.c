/*****************************************************************************/
/*
 *	flatfsd.c -- Flat file-system daemon.
 *
 *	(C) Copyright 1999-2001, Greg Ungerer <gerg@snapgear.com>
 *	(C) Copyright 2000-2001, Lineo Inc. (www.lineo.com)
 *	(C) Copyright 2001-2002, SnapGear (www.snapgear.com)
 *	(C) Copyright 2002, David McCullough <davidm@snapgear.com>
 */
/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#include <linux/config.h>
#include <linux/ledman.h>

#ifdef USING_MTD_DEVICE
# include <linux/mtd/mtd.h>
#endif
#ifdef USING_BLKMEM_DEVICE
# include <linux/blkmem.h>
#endif

#include "flatfs.h"
#include "reboot.h"

/*****************************************************************************/

#define	FILEFS	"/dev/flash/config"

/*
 *	Globals for file and byte count.
 *
 *	This is a kind of ugly way to do it, but we are using LCP (Least Change Principle)
 */

int	numfiles;
int	numbytes;
int	numdropped;

/*****************************************************************************/
/*
 * The code to do Reset/Erase button menus
 */

static int current_cmd = 0;
static void no_action(void) { }
static void reset_config(void);
static void static_ip_reset(void);

#define MAX_LED_PATTERN 4
#define	ACTION_TIMEOUT 5		/* timeout before action in seconds */

static struct {
	void			(*action)(void);
	unsigned long	led;
} cmd_list[] = {
	{ no_action,		0 },
	{ reset_config,		LEDMAN_RESET  },
	{ static_ip_reset,	LEDMAN_STATIC },
	{ NULL,				0 }
};

/*****************************************************************************/

static int recv_hup = 0;	/* SIGHUP = reboot device */
static int recv_usr1 = 0;	/* SIGUSR1 = write config to flash */
static int recv_usr2 = 0;	/* SIGUSR2 = erase flash and reboot */

static void block_sig(int blp)
{
	sigset_t sigs;

	sigemptyset(&sigs);
	sigaddset(&sigs, SIGUSR1);
	sigaddset(&sigs, SIGUSR2);
	sigaddset(&sigs, SIGHUP);
	sigprocmask(blp?SIG_BLOCK:SIG_UNBLOCK, &sigs, NULL);
}

static void sigusr1(int signr)
{
	recv_usr1 = 1;
}

static void sigusr2(int signr)
{
	recv_usr2 = 1;
}

static void sighup(int signr)
{
	recv_hup = 1;
}

/*****************************************************************************/
/*
 * Save the filesystem to flash in flat format for retrieval
 * later
 */

static void save_config_to_flash(void)
{
#if !defined(USING_FLASH_FILESYSTEM)
	int	rc;
#endif
	block_sig(1);

#if !defined(USING_FLASH_FILESYSTEM)
	if ((rc = flatwrite(FILEFS)) < 0) {
		printf("FLATFSD: failed to write flatfs, err=%d errno=%d\n",
			rc, errno);
	}
#endif
	block_sig(0);
}

/*****************************************************************************/
/*
 *	Default the config filesystem
 */

static void reset_config_fs(int static_ip)
{
	int rc;

	block_sig(1);

	flatclean();
	if ((rc = flatnew(DEFAULTDIR)) < 0) {
		printf("FLATFSD: failed to create new flatfs from %s, "
			"err=%d errno=%d\n", DEFAULTDIR, rc, errno);
		fflush(stdout);
		exit(1);
	}
	if (static_ip) {
		if ((rc = flatnew(SDEFAULTDIR)) < 0) {
			printf("FLATFSD: failed to add static files from %s, "
				"err=%d errno=%d\n", SDEFAULTDIR, rc, errno);
			fflush(stdout);
			/* no exit here, at least let them have the non-static setup */
		}
	}
	save_config_to_flash();

	reboot_now();
	block_sig(0);
}

static void reset_config(void)
{
	reset_config_fs(0);
}

static void static_ip_reset(void)
{
	reset_config_fs(1);
}

/*****************************************************************************/

int creatpidfile()
{
	FILE	*f;
	pid_t	pid;
	char	*pidfile = "/var/run/flatfsd.pid";

	pid = getpid();
	if ((f = fopen(pidfile, "w")) == NULL) {
		printf("FLATFSD: failed to open(%s), errno=%d\n",
			pidfile, errno);
		return(-1);
	}
	fprintf(f, "%d\n", pid);
	fclose(f);
	return(0);
}

/*****************************************************************************/

/*
 *	Lodge ourselves with the kernel LED manager. If it gets an
 *	interrupt from the reset switch it will send us a SIGUSR2.
 */

int register_resetpid(void)
{
#if defined(CONFIG_LEDMAN) && defined(LEDMAN_CMD_SIGNAL)
	int	fd;

	if ((fd = open("/dev/ledman", O_RDONLY)) < 0) {
		printf("FLATFSD: failed to open(/dev/ledman), errno=%d\n",
			errno);
		return(-1);
	}
	if (ioctl(fd, LEDMAN_CMD_SIGNAL, 0) < 0) {
		printf("FLATFSD: failed to register pid, errno=%d\n", errno);
		return(-2);
	}
	close(fd);
#endif
	return(0);
}

/*****************************************************************************/

#define CHECK_FOR_SIG(x) \
	do { usleep(x); if (recv_usr1 || recv_usr2 || recv_hup) goto skip_out; } while(0);

static void
led_pause(void)
{
#if defined(CONFIG_LEDMAN) && defined(LEDMAN_CMD_SIGNAL)

	unsigned long start = time(0);

	ledman_cmd(LEDMAN_CMD_ALT_ON, LEDMAN_ALL); /* all leds on */
	ledman_cmd(LEDMAN_CMD_ON | LEDMAN_CMD_ALTBIT, LEDMAN_ALL); /* all leds on */
	CHECK_FOR_SIG(100000);
	ledman_cmd(LEDMAN_CMD_OFF | LEDMAN_CMD_ALTBIT, LEDMAN_ALL); /* all leds off */
	CHECK_FOR_SIG(100000);
	ledman_cmd(LEDMAN_CMD_ON | LEDMAN_CMD_ALTBIT, cmd_list[current_cmd].led);
	CHECK_FOR_SIG(250000);

	while (time(0) - start < ACTION_TIMEOUT) {
		CHECK_FOR_SIG(250000);
	}

	block_sig(1);
	ledman_cmd(LEDMAN_CMD_ON | LEDMAN_CMD_ALTBIT, LEDMAN_ALL); /* all leds on */
	(*cmd_list[current_cmd].action)();
	block_sig(0);

skip_out:
	ledman_cmd(LEDMAN_CMD_RESET | LEDMAN_CMD_ALTBIT, LEDMAN_ALL);
	ledman_cmd(LEDMAN_CMD_ALT_OFF, LEDMAN_ALL); /* all leds on */

#else
	pause();
#endif
}

/*****************************************************************************/

void usage(int rc)
{
	printf("usage: flatfsd [-rwh?]\n");
	exit(rc);
}

/*****************************************************************************/

int main(int argc, char *argv[])
{
	struct sigaction	act;
	int			rc, readonly, clobbercfg;

	clobbercfg = readonly = 0;

	if ((rc = getopt(argc, argv, "rwh?")) != EOF) {
		switch(rc) {
		case 'w':
			clobbercfg++;
		case 'r':
			readonly++;
			break;
		case 'h':
		case '?':
			usage(0);
			break;
		default:
			usage(1);
			break;
		}
	}

	if (readonly) {
		if (clobbercfg ||
#if defined(USING_FLASH_FILESYSTEM)
			((rc = flatfilecount()) <= 0)
#else
			((rc = flatread(FILEFS)) < 0)
#endif
		) {
			printf("FLATFSD: non-existent or bad flatfs (%d), "
					"creating new one...\n", rc);
			flatclean();
			if ((rc = flatnew(DEFAULTDIR)) < 0) {
				printf("FLATFSD: failed to create new "
					"flatfs, err=%d errno=%d\n",
					rc, errno);
				fflush(stdout);
				exit(1);
			}
			save_config_to_flash();
		}
		printf("FLATFSD: created %d configuration files (%d bytes)\n",
			numfiles, numbytes);
		fflush(stdout);
		exit(0);
	}

	/*
	 *	Spin forever, waiting for a signal to write...
	 */
	creatpidfile();

	act.sa_handler = sighup;
	memset(&act.sa_mask, 0, sizeof(act.sa_mask));
	act.sa_flags = SA_RESTART;
	act.sa_restorer = 0;
	sigaction(SIGHUP, &act, NULL);

	act.sa_handler = sigusr1;
	memset(&act.sa_mask, 0, sizeof(act.sa_mask));
	act.sa_flags = SA_RESTART;
	act.sa_restorer = 0;
	sigaction(SIGUSR1, &act, NULL);

	act.sa_handler = sigusr2;
	memset(&act.sa_mask, 0, sizeof(act.sa_mask));
	act.sa_flags = SA_RESTART;
	act.sa_restorer = 0;
	sigaction(SIGUSR2, &act, NULL);

	register_resetpid();

	for (;;) {
		if (recv_usr1) {
			recv_usr1 = 0;
			save_config_to_flash();
		}

		if (recv_hup) {
                	/* Make sure we do the check above first so that we commit
                         * to flash before rebooting
                         */
			recv_hup = 0;
                        reboot_now();
                        /*notreached*/
                        exit(1);
		}

		if (recv_usr2) {
			recv_usr2 = 0;
			current_cmd++;
			if (cmd_list[current_cmd].action == NULL) /* wrap */
				current_cmd = 0;
		}
			
		if (current_cmd) {
			led_pause();
		} else if (!recv_usr1 && !recv_usr2)
			pause();
	}
	exit(0);
}

/*****************************************************************************/
