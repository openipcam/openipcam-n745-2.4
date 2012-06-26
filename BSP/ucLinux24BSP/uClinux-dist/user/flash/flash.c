#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/version.h>
#include <linux/config.h>
#include <config/autoconf.h>

#define FNAME	"flash"
#define FNAME_S	(sizeof(FNAME) - 1)

typedef struct {
	const char	*name;		/* User visible name		*/
	const char	*dev;		/* /dev/flash/xxx name		*/
	const char	*opts;		/* Extra options for command	*/
	unsigned int	 offset;	/* Position inside segment	*/
	unsigned	 lockp:1;	/* lock/unlock segment?		*/
	unsigned	 preserve:1;	/* preserve rest of segment?	*/
	unsigned	 binary:1;	/* Binary format default?	*/
	unsigned	 netflash:1;	/* Netflash instead of flashw	*/
} Segment;


Segment seg_list[] = {
#ifdef CONFIG_COLDFIRE
	/*	name		segment		opts	offset	lu p bin netflash       */
	{	"image",	NULL,		NULL,	0,	0, 0, 1, 1 },
	{	"boot",		"boot",		"-ni",	0,	0, 0, 1, 1 },
	{	"mac0",		"ethmac",	NULL,	0,	0, 1, 1, 0 },
	{	"mac1",		"ethmac",	NULL,	6,	0, 1, 1, 0 },
	{	"arg",		"bootarg",	NULL,	0,	0, 0, 0, 0 },
	{	"all",		"all",		"-n",	0,	0, 0, 1, 1 }
#endif	/* CONFIG_COLDFIRE */
	
	
#ifdef CONFIG_SH_SECUREEDGE5410
#ifdef CONFIG_MTD_DOC2001PLUS
#define B	0x24000
#define L	0
#else	/* CONFIG_MTD_DOC2001PLUS */
#define B 	0xc000
#ifdef CONFIG_MTD_DOCPROBE
#define L	0
#else	/* CONFIG_MTD_DOCPROBE */
#define L	1
#endif	/* CONFIG_MTD_DOCPROBE */
#endif	/* CONFIG_MTD_DOC2001PLUS */
				/* boot == ethmac == bootarg here */
	/*	name		segment		opts	offset	lu p bin netflash       */
	{	"image",	NULL,		NULL,	0,	0, 0, 1, 1 },
#ifndef CONFIG_MTD_DOCPROBE
	/* With DOC the command is radically different... */
	{	"boot",		"boot",		"-ni",	0,	L, 1, 1, 1 },
#endif
	{	"mac0",		"boot",		NULL,	B+0x000,L, 1, 1, 0 },
	{	"mac1",		"boot",		NULL,	B+0x006,L, 1, 1, 0 },
	{	"mac2",		"boot",		NULL,	B+0x00c,L, 1, 1, 0 },
	{	"mac3",		"boot",		NULL,	B+0x012,L, 1, 1, 0 },
	{	"mac4",		"boot",		NULL,	B+0x018,L, 1, 1, 0 },
	{	"magic",	"boot",		NULL,	B+0x01e,L, 1, 1, 0 },
	{	"arg",		"boot",		NULL,	B+0x020,L, 1, 0, 0 },
	{	"blip",		"boot",		NULL,	B+0x200,L, 1, 1, 0 },
//	{	"blnm",		"boot",		NULL,	B+0x204,L, 1, 1, 0 },
//	{	"blgw",		"boot",		NULL,	B+0x208,L, 1, 1, 0 },
	{	"blsv",		"boot",		NULL,	B+0x20c,L, 1, 1, 0 },
	{	"blfn",		"boot",		NULL,	B+0x210,L, 1, 0, 0 },
	{	"ckey",		"boot",		NULL,	B+0x240,L, 1, 1, 0 },
	{	"all",		"all",		"-n",	0,	0, 0, 1, 1 }
#undef B
#endif	/* CONFIG_SH_SECUREEDGE5410 */


#ifdef CONFIG_X86
#ifdef CONFIG_AMD_BOOT
#define L	0
#else
#define L	1
#endif
				/* ethmac == bootarg here */
	/*	name		segment		opts	offset	lu p bin netflash       */
	{	"image",	NULL,		NULL,	0,	0, 0, 1, 1 },
	{	"boot",		"boot",		"-ni",	0,	L, L, 1, 1 },
	{	"mac0",		"ethmac",	"-F",	0,	L, 1, 1, 0 },
	{	"mac1",		"ethmac",	NULL,	6,	L, 1, 1, 0 },
	{	"arg",		"ethmac",	NULL,	0x2000,	L, 1, 0, 0 },
	{	"blip",		"ethmac",	NULL,	0x2200,	L, 1, 1, 0 },
//	{	"blnm",		"ethmac",	NULL,	0x2204,	L, 1, 1, 0 },
//	{	"blgw",		"ethmac",	NULL,	0x2208,	L, 1, 1, 0 },
	{	"blsv",		"ethmac",	NULL,	0x220c,	L, 1, 1, 0 },
	{	"blfn",		"ethmac",	NULL,	0x2210,	L, 1, 0, 0 },
	{	"ckey",		"ethmac",	NULL,	0x2240,	L, 1, 1, 0 },
	{	"all",		"all",		"-n",	0,	0, 0, 1, 1 }
#endif	/* CONFIG_X86 */
};
#define NSEG_LIST	(sizeof(seg_list)/sizeof(Segment))

static int segcmp(const void *v1, const void *v2) {
	return strcmp(((const Segment *)v1)->name, ((const Segment *)v2)->name);
}

static const char *pname;
static void usage(const char *) __attribute__ ((noreturn));
static void usage(const char *m) {
	int i, c = 0;
	printf("%s: %s <segment> [-pniH] <args>\n"
		"\t-p\tprint command without executing it\n"
		"\t-n\tskip commit to flash\n"
		"\t-i\tignore firmware version (netflash only)\n"
		"\t-H\tignore hardware version (netflash only)\n\n"
		"<args> is either the value to be flashed or a <host> <file> pair.\n\n"
		"<segment> is one of:\n", m, pname);
	qsort(seg_list, NSEG_LIST, sizeof(Segment), segcmp);
	for (i=0; i<NSEG_LIST; i++) {
		if (c == 0)
			c += printf("\t%s", seg_list[i].name);
		else if (strlen(seg_list[i].name) + 2 + c > 70)
			c = printf(",\n\t%s", seg_list[i].name) - 3;
		else
			c += printf(", %s", seg_list[i].name);
	}
	puts(".");
	exit(1);
}

int main(int argc, char *argv[]) {
	int type, i;
	const char *p;
	int opt, nop = 0, lockp = 0, unlockp = 0, ignoreverp = 0;
	int ignorehwp = 0, pp = 0;
	char *av[50], *prog;
	int ac = 0;
	char obuf[16], sbuf[200];
	int argcount = 1;

	if ((pname = strrchr(argv[0], '/')) != NULL)
		pname++;
	else
		pname = argv[0];
	if (strcmp(pname, FNAME))		p = pname;
	else if (!--argc)			usage("no command");
	else					p = *++argv;
	for (type=0; type<NSEG_LIST; type++)
		if (!strcmp(p, seg_list[type].name))
			goto gotname;
	usage("unknown command");

gotname:
	while ((opt = getopt(argc, argv, "pniH")) > 0) {
		switch (opt) {
		case 'p':	pp = 1;		break;
		case 'n':	nop = 1;	break;
		case 'i':	ignoreverp = 1;	break;
		case 'H':	ignorehwp = 1;	break;
		default:
			usage("bad argument");
		}
	}
	if (strcmp(argv[optind-1], "--") == 0)
		argcount = 0;
	if (seg_list[type].dev)
		sprintf(sbuf, "/dev/flash/%s", seg_list[type].dev);
	av[ac++] = seg_list[type].netflash?"netflash":"flashw";
	if (seg_list[type].lockp) {
		av[ac++] = "-l";
		av[ac++] = "-u";
	}
	if (seg_list[type].preserve)	av[ac++] = "-p";
	if (seg_list[type].opts)
		av[ac++] = (char *)seg_list[type].opts;
	if (seg_list[type].netflash) {
		if (argcount) {
			if (strncmp(argv[optind], "http://", 7)) {
				if (optind != argc-2)
					usage("incorrect arg count");
			} else if (optind != argc-1)
				usage("incorrect arg count");
		}
		p = "/bin/netflash";
		if (nop)	av[ac++] = "-t";
		if (ignoreverp)	av[ac++] = "-i";
		if (ignorehwp)	av[ac++] = "-H";
		if (seg_list[type].dev) {
			av[ac++] = "-r";
			av[ac++] = sbuf;
		}
		for (i=optind; i<argc; i++)
			av[ac++] = argv[i];
	} else {
		if (argcount && optind != argc-1)
			usage("incorrect arg count");
		p = "/bin/flashw";
		if (seg_list[type].offset)
			sprintf(av[ac++] = obuf, "-o%d", seg_list[type].offset);
		if (seg_list[type].binary)
			av[ac++] = "-b";
		for (i=optind; i<argc; i++)
			av[ac++] = argv[i];
		av[ac++] = sbuf;
		if (nop) pp = 1;
	}
	av[ac++] = NULL;
	if (pp)
		for (i=0; av[i] != NULL; i++)
			printf("%s%s", av[i], av[i+1]!=NULL?" ":"\n");
	else
		return -execv(p, av);
	return 0;
}
