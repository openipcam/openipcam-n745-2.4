/*****************************************************************************/

/*
 *	cpu.c -- simple CPU usage reporting tool.
 *
 *	(C) Copyright 2000, Greg Ungerer (gerg@snapgear.com)
 */

/*****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <getopt.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/time.h>

/*****************************************************************************/

char *version = "1.0.0";

/*****************************************************************************/

struct stats {
	unsigned int	user;
	unsigned int	nice;
	unsigned int	system;
	unsigned int	idle;
	unsigned int	total;
};

/*****************************************************************************/

int getdata(FILE *fp, struct stats *st)
{
	unsigned char	buf[80];

	if (fseek(fp, 0, SEEK_SET) < 0)
		return(-1);
	fscanf(fp, "%s %d %d %d %d", &buf[0], &st->user, &st->nice,
		&st->system, &st->idle);

	st->total = st->user + st->nice + st->system + st->idle;
	return(0);
}

/*****************************************************************************/

void usage(FILE *fp, int rc)
{
	fprintf(fp, "Usage: cpu [-h?ra] [-s seconds] [-c count] "
		"[-d <device>]\n\n");
	fprintf(fp, "        -h?            this help\n");
	fprintf(fp, "        -v             print version info\n");
	fprintf(fp, "        -r             repeated output\n");
	fprintf(fp, "        -a             print system average\n");
	fprintf(fp, "        -c count       repeat count times\n");
	fprintf(fp, "        -s seconds     seconds between output\n");
	fprintf(fp, "        -d <device>    proc device to use (default /proc/stat)\n");
	exit(rc);
}

/*****************************************************************************/

int main(int argc, char *argv[])
{
	FILE		*fp;
	struct stats	st, stold;
	unsigned int	curtotal;
	int		c, cnt, repeat, delay, average;
	char		*procdevice;

	repeat = 0;
	delay = 1;
	procdevice = "/proc/stat";
	cnt = 1;
	average = 0;

	while ((c = getopt(argc, argv, "ravh?s:d:c:")) > 0) {
		switch (c) {
		case 'v':
			printf("%s: version %s\n", argv[0], version);
			exit(0);
		case 'a':
			average++;
			break;
		case 'r':
			repeat++;
			break;
		case 's':
			delay = atoi(optarg);
			break;
		case 'd':
			procdevice = optarg;
			break;
		case 'c':
			cnt = atoi(optarg);
			break;
		case 'h':
		case '?':
			usage(stdout, 0);
			break;
		default:
			fprintf(stderr, "ERROR: unkown option '%c'\n", c);
			usage(stderr, 1);
			break;
		}
	}

	/*
	 *	Check device is real, and open it.
	 */
	if ((fp = fopen(procdevice, "r")) == NULL) {
		fprintf(stderr, "ERROR: failed to open %s, errno=%d\n",
			procdevice, errno);
		exit(0);
	}

	getdata(fp, &st);

	if (average) {
		printf("CPU:  average %d%%  (system=%d%% user=%d%% "
			"nice=%d%% idle=%d%%)\n",
			(st.system + st.user + st.nice) * 100 / st.total,
			st.system * 100 / st.total, st.user * 100 / st.total,
			st.nice * 100 / st.total, st.idle * 100 / st.total);
		cnt = repeat = 0;
	}

	for (c = 0; ((c < cnt) || repeat); c++) {
		sleep(delay);
		stold = st;
		getdata(fp, &st);

		curtotal = st.total - stold.total;

		printf("CPU:  busy %d%%  (system=%d%% user=%d%% "
			"nice=%d%% idle=%d%%)\n",
			((st.system + st.user + st.nice) -
			 (stold.system + stold.user + stold.nice)) *
			 100 / curtotal,
			(st.system - stold.system) * 100 / curtotal,
			(st.user - stold.user) * 100 / curtotal,
			(st.nice - stold.nice) * 100 / curtotal,
			(st.idle - stold.idle) * 100 / curtotal);
	}

	fclose(fp);
	exit(0);
}

/*****************************************************************************/
