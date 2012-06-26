/*****************************************************************************/
/*
 *	flatfs.c -- flat FLASH file-system.
 *
 *	Copyright (C) 1999, Greg Ungerer (gerg@snapgear.com).
 *	Copyright (C) 2001-2002, SnapGear (www.snapgear.com)
 */
/*****************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <sys/mount.h>

#ifdef USING_MTD_DEVICE
# include <linux/mtd/mtd.h>
#endif
#ifdef USING_BLKMEM_DEVICE
# include <linux/blkmem.h>
#endif

#include "flatfs.h"

/*****************************************************************************/

#define ERROR_CODE()	(-(__LINE__)) /* unique failure codes :-) */

/*****************************************************************************/

/*****************************************************************************/

/*
 *	Checksum the contents of FLASH file.
 *	Pretty bogus check-sum really, but better than nothing :-)
 */

unsigned int chksum(unsigned char *sp, unsigned int len)
{
	unsigned int	chksum;
	unsigned char	*ep;

	for (chksum = 0, ep = sp + len; (sp < ep);)
		chksum += *sp++;
	return(chksum);
}

/*****************************************************************************/
/*
 *	The buffer we write everything into
 */

static int            flat_fd = -1;
static char          *flat_buf = NULL;
static unsigned long  flat_len = 0;
static unsigned long  flat_size = 0;
static unsigned int   flat_sum = 0;

/*
 *	Open the flat device so we can start recording to it.  For
 *	real flash devices,  it's all done in memory until the close,
 *	for disk like devices we write it as we go.
 */

static int
flat_dev_open(char *flatfs)
{
#ifdef USING_MTD_DEVICE
	mtd_info_t      mtd_info;
#endif

	/* Open and get the size of the FLASH file-system. */
	if ((flat_fd = open(flatfs, O_RDWR)) < 0)
		return(- __LINE__);

#if defined(USING_DISKLIKE_DEVICE)
	{
		char	buf[32];
		memset(buf, 0, sizeof(buf));
		while (write(flat_fd, buf, sizeof(buf)) == sizeof(buf))
			;
	}
	flat_len = lseek(flat_fd, 0L, SEEK_END);
	if (flat_len == -1)
		return(ERROR_CODE());
	if (lseek(flat_fd, 0L, SEEK_SET) != 0)
		return(ERROR_CODE());
#elif defined(USING_MTD_DEVICE)
	if (ioctl(flat_fd, MEMGETINFO, &mtd_info) < 0) {
		close(flat_fd);
		return(ERROR_CODE());
	}
	flat_len = mtd_info.size;
#elif defined(USING_BLKMEM_DEVICE)
	if (ioctl(flat_fd, BMGETSIZEB, &flat_len) < 0) {
		close(flat_fd);
		return(ERROR_CODE());
	}
	if (ioctl(flat_fd, BMSGSIZE, &flat_size) < 0) {
		close(flat_fd);
		return(ERROR_CODE());
	}
#else
	#error "Unknown flash device type !"
#endif

#if !defined(USING_DISKLIKE_DEVICE)
	flat_buf = malloc(flat_len);
	if (!flat_buf) {
		close(flat_fd);
		return(ERROR_CODE());
	}
	memset(flat_buf, 0, flat_len);
#endif

	flat_sum = 0;
	return(0);
}

/*****************************************************************************/

/*
 *	Erase the "FLAT" file-system for flash devices, save and cleanup.
 *	If aborting, just cleanup.
 */

static int
flat_dev_close(int abort)
{
	int rc = 0;
#if defined(USING_MTD_DEVICE)
	erase_info_t    erase_info;
#endif
#if defined(USING_BLKMEM_DEVICE)
	int pos;
#endif

	if (abort)
		goto abort_cleanup;

#if defined(USING_DISKLIKE_DEVICE)
	/*
	 * nothing to do,  we write as we go
	 */
#elif defined(USING_MTD_DEVICE)
	erase_info.start = 0;
	erase_info.length = flat_len;
	if (ioctl(flat_fd, MEMERASE, &erase_info) < 0) {
		rc = ERROR_CODE();
		goto abort_cleanup;
	}
#elif defined(USING_BLKMEM_DEVICE)
	for (pos = flat_len - flat_size; (pos >= 0); pos -= flat_size) {
		if (ioctl(flat_fd, BMSERASE, pos) < 0) {
			rc = ERROR_CODE();
			break;
		}
	}
#else
	#error "Unknown flash device type !"
#endif

#if !defined(USING_DISKLIKE_DEVICE)
	/*
	 * Write everything out
	 */
	if (write(flat_fd, flat_buf, flat_len) != flat_len)
		rc = ERROR_CODE();
#endif

 abort_cleanup:
	if (flat_buf)
		free(flat_buf);
	flat_buf = NULL;
	flat_len = 0;
	flat_size = 0;
	close(flat_fd);
	flat_fd = -1;
	return(rc);
}

/*****************************************************************************/

static int
flat_dev_write(unsigned long offset, char *buf, int len)
{
#if defined(USING_DISKLIKE_DEVICE)
	if (lseek(flat_fd, offset, SEEK_SET) != offset)
		return(ERROR_CODE());
	if (write(flat_fd, buf, len) != len)
		return(ERROR_CODE());
#else
	if (offset + len > flat_len)
		return(ERROR_CODE());
	memcpy(&flat_buf[offset], buf, len);
#endif
	flat_sum += chksum(buf, len);
	return(len);
}

/*****************************************************************************/
#ifdef THIS_IS_NOT_USED_YET

static int
flat_dev_read(unsigned long offset, char *buf, int len)
{
#if defined(USING_DISKLIKE_DEVICE)
	if (lseek(flat_fd, offset, SEEK_SET) != offset)
		return(ERROR_CODE());
	if (read(flat_fd, buf, len) != len)
		return(ERROR_CODE());
	return(len);
#else
	if (offset + len > flat_len)
		return(ERROR_CODE());
	memcpy(buf, &flat_buf[offset], len);
	return(len);
#endif
}

#endif
/*****************************************************************************/

/*
 *	Read the contents of a flat file-system and dump them out as
 *	regular files. Mmap would be nice, but alas...
 */

int flatread(char *flatfs)
{
	struct flathdr	hdr;
	int		version;
	struct flatent	ent;
	unsigned int	len, n, size, sum;
	int		fdflat, fdfile;
	char		filename[128];
	unsigned char	buf[1024];
	mode_t		mode;
#ifdef USING_MTD_DEVICE
	mtd_info_t	mtd_info;
#endif
	char *confbuf, *confline, *confdata;
	time_t t;

	if (chdir(DSTDIR) < 0)
		return(ERROR_CODE());

	if ((fdflat = open(flatfs, O_RDONLY)) < 0)
		return(ERROR_CODE());

#if defined(USING_DISKLIKE_DEVICE)
	len = lseek(fdflat, 0L, SEEK_END);
	if (len == -1)
		return(ERROR_CODE());
	if (lseek(fdflat, 0L, SEEK_SET) != 0)
		return(ERROR_CODE());
#elif defined(USING_MTD_DEVICE)
	if (ioctl(fdflat, MEMGETINFO, &mtd_info) < 0)
		return(ERROR_CODE());
	len = mtd_info.size;
#elif defined(USING_BLKMEM_DEVICE)
	if (ioctl(fdflat, BMGETSIZEB, &len) < 0)
		return(ERROR_CODE());
#else
	#error "Unknown flash device type !"
#endif

	/* Check that header is valid */
	if (read(fdflat, (void *) &hdr, sizeof(hdr)) != sizeof(hdr))
		return(ERROR_CODE());

	if (hdr.magic == FLATFS_MAGIC) {
		version = 1;
	} else if (hdr.magic == FLATFS_MAGIC_V2) {
		version = 2;
	} else {
		fprintf(stderr, "flatfsd: invalid header magic\n");
		return(ERROR_CODE());
	}

	/* Check contents are valid */
	for (sum = 0, size = sizeof(hdr); (size < len); size += sizeof(buf)) {
		n = (size > sizeof(buf)) ? sizeof(buf) : size;
		if (read(fdflat, (void *) &buf[0], n) != n)
			return(ERROR_CODE());
		sum += chksum(&buf[0], n);
	}

	if (sum != hdr.chksum) {
		fprintf(stderr, "flatfsd: bad header checksum\n");
		return(ERROR_CODE());
	}

	if (lseek(fdflat, sizeof(hdr), SEEK_SET) < sizeof(hdr))
		return(ERROR_CODE());

	for (numfiles = 0, numbytes = 0; ; numfiles++) {
		/* Get the name of next file. */
		if (read(fdflat, (void *) &ent, sizeof(ent)) != sizeof(ent))
			return(ERROR_CODE());

		if (ent.filelen == FLATFS_EOF)
			break;

		n = ((ent.namelen + 3) & ~0x3);
		if (n > sizeof(filename))
			return(ERROR_CODE());

		if (read(fdflat, (void *) &filename[0], n) != n)
			return(ERROR_CODE());

		if (version >= 2) {
			if (read(fdflat, (void *) &mode, sizeof(mode)) != sizeof(mode))
				return(ERROR_CODE());
		} else {
			mode = 0644;
		}

		if (strcmp(filename, FLATFSD_CONFIG) == 0) {
			/* Read our special flatfsd config file into memory */
			confbuf = malloc(ent.filelen);
			if (!confbuf)
				return(ERROR_CODE());

			if (read(fdflat, confbuf, ent.filelen) != ent.filelen)
				return(ERROR_CODE());

			confline = strtok(confbuf, "\n");
			while (confline) {
				confdata = strchr(confline, ' ');
				if (confdata) {
					*confdata = '\0';
					confdata++;
					if (!strcmp(confline, "time")) {
						t = atol(confdata);
						if (t > time(NULL))
							stime(&t);
					}
				}
				confline = strtok(NULL, "\n");
			}
		} else {
			/* Write contents of file out for real. */
			fdfile = open(filename, (O_WRONLY | O_TRUNC | O_CREAT), mode);
			if (fdfile < 0)
				return(ERROR_CODE());
			
			for (size = ent.filelen; (size > 0); size -= n) {
				n = (size > sizeof(buf)) ? sizeof(buf) : size;
				if (read(fdflat, &buf[0], n) != n)
					return(ERROR_CODE());
				if (write(fdfile, (void *) &buf[0], n) != n)
					return(ERROR_CODE());
			}

			close(fdfile);
		}

		/* Read alignment padding */
		n = ((ent.filelen + 3) & ~0x3) - ent.filelen;
		if (read(fdflat, &buf[0], n) != n)
			return(ERROR_CODE());

		numbytes += ent.filelen;
	}

	close(fdflat);
	return(0);
}

/*****************************************************************************/

int writefile(char *name, unsigned int *ptotal)
{
	struct flatent ent;
	struct stat    st;
	unsigned int   size;
	int            fdfile, zero = 0;
	mode_t		   mode;
	char           *fbuf;

	/*
	 *	Write file entry into flat fs. Names and file
	 *	contents are aligned on long word boundaries.
	 *	They are padded to that length with zeros.
	 */
	if (stat(name, &st) < 0)
		return(ERROR_CODE());

	size = strlen(name) + 1;
	if (size > 128) {
		numdropped++;
		return(ERROR_CODE());
	}

	ent.namelen = size;
	ent.filelen = st.st_size;
	if (flat_dev_write(*ptotal, (char *) &ent, sizeof(ent)) < 0)
		return(ERROR_CODE());
	*ptotal += sizeof(ent);

	/* Write file name out, with padding to align */
	if (flat_dev_write(*ptotal, name, size) < 0)
		return(ERROR_CODE());
	*ptotal += size;
	size = ((size + 3) & ~0x3) - size;
	if (flat_dev_write(*ptotal, (char *)&zero, size) < 0)
		return(ERROR_CODE());
	*ptotal += size;

	/* Write out the permissions */
	mode = (mode_t) st.st_mode;
	size = sizeof(mode);
	if (flat_dev_write(*ptotal, (char *) &mode, size) < 0)
		return(ERROR_CODE());
	*ptotal += size;

	/* Write the contents of the file. */
	size = st.st_size;

	if (size > 0) {
		fbuf = malloc(size);
		if (!fbuf)
			return(ERROR_CODE());

		if ((fdfile = open(name, O_RDONLY)) < 0) {
			free(fbuf);
			return(ERROR_CODE());
		}
		if (read(fdfile, fbuf, size) != size) {
			free(fbuf);
			close(fdfile);
			return(ERROR_CODE());
		}
		close(fdfile);
		if (flat_dev_write(*ptotal, fbuf, size) < 0) {
			free(fbuf);
			return(ERROR_CODE());
		}
		free(fbuf);
		*ptotal += size;

		/* Pad to align */
		size = ((st.st_size + 3) & ~0x3)- st.st_size;
		if (flat_dev_write(*ptotal, (char *)&zero, size) < 0)
			return(ERROR_CODE());
		*ptotal += size;
	}

	numfiles++;
	numbytes += ent.filelen;

	return 0;
}

/*****************************************************************************/

/*
 *	Write out the contents of the local directory to flat file-system.
 *	The writing process is not quite as easy as read. Use the usual
 *	write system call so that FLASH programming is done properly.
 */

int flatwrite(char *flatfs)
{
	FILE            *hfile;
	DIR             *dirp;
	struct dirent	*dp;
	struct flathdr	hdr;
	unsigned int	total;
	struct flatent	ent;
	int             rc;

	if (chdir(SRCDIR) < 0)
		return(ERROR_CODE());

	rc = flat_dev_open(flatfs);
	if (rc < 0)
		return(rc);

	/* Write out contents of all files, skip over header */
	numfiles = 0;
	numbytes = 0;
	numdropped = 0;
	total = sizeof(hdr);

	/* Create a special config file */
	hfile = fopen(FLATFSD_CONFIG, "w");
	if (!hfile) {
		rc = ERROR_CODE();
		goto cleanup;
	}
	fprintf(hfile, "time %ld\n", time(NULL));
	fclose(hfile);
	rc = writefile(FLATFSD_CONFIG, &total);
	unlink(FLATFSD_CONFIG);
	if (rc < 0)
		goto cleanup;

	/* Scan directory */
	if ((dirp = opendir(".")) == NULL) {
		rc = ERROR_CODE();
		goto cleanup;
	}

	while ((dp = readdir(dirp)) != NULL) {

		if ((strcmp(dp->d_name, ".") == 0) ||
		    (strcmp(dp->d_name, "..") == 0))
			continue;

		rc = writefile(dp->d_name, &total);
		if (rc < 0) {
			closedir(dirp);
			goto cleanup;
		}
	}

	closedir(dirp);

	/* Write the terminating entry */
	ent.namelen = FLATFS_EOF;
	ent.filelen = FLATFS_EOF;
	if (flat_dev_write(total, (char *) &ent, sizeof(ent)) < 0) {
		rc = ERROR_CODE();
		goto cleanup;
	}

	/* Construct header */
	hdr.magic = FLATFS_MAGIC_V2;
	hdr.chksum = flat_sum;
	if (flat_dev_write(0L, (char *) &hdr, sizeof(hdr)) < 0) {
		rc = ERROR_CODE();
		goto cleanup;
	}

	return(flat_dev_close(0));

cleanup:
	flat_dev_close(1);
	return(rc);
}

/*****************************************************************************/
