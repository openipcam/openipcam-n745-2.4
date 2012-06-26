/*
 *  linux/fs/binfmt_flat.c
 *
 *	Copyright (C) 2002 SnapGear, by Paul Dale <pauli@snapgear.com>
 *	Copyright (C) 2000, 2001 Lineo, by David McCullough <davidm@lineo.com>
 *  based heavily on:
 *
 *  linux/fs/binfmt_aout.c:
 *      Copyright (C) 1991, 1992, 1996  Linus Torvalds
 *  linux/fs/binfmt_flat.c for 2.0 kernel
 *	    Copyright (C) 1998  Kenneth Albanowski <kjahds@kjahds.com>
 *	JAN/99 -- coded full program relocation (gerg@snapgear.com)
 */

#include <linux/module.h>

#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/a.out.h>
#include <linux/errno.h>
#include <linux/signal.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/stat.h>
#include <linux/fcntl.h>
#include <linux/ptrace.h>
#include <linux/user.h>
#include <linux/slab.h>
#include <linux/binfmts.h>
#include <linux/personality.h>
#include <linux/init.h>
#include <linux/flat.h>
#include <linux/config.h>

#include <asm/byteorder.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/pgalloc.h>
#include <asm/unaligned.h>

#define DEBUG

#ifdef DEBUG
#define	DBG_FLT(a...)	printk(##a)
#else
#define	DBG_FLT(a...)
#endif

#define RELOC_FAILED 0xff00ff01		/* Relocation incorrect somewhere */
#define UNLOADED_LIB 0x7ff000ff		/* Placeholder for unused library */

#ifndef MAX
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

struct lib_info {
	struct {
		unsigned long start_code;		/* Start of text segment */
		unsigned long start_data;		/* Start of data segment */
		unsigned long start_brk;		/* End of data segment */
		unsigned long text_len;			/* Length of text segment */
		unsigned long entry;			/* Start address for this module */
		unsigned long build_date;		/* When this one was compiled */
		short loaded;				/* Has this library been loaded? */
	} lib_list[MAX_SHARED_LIBS];
};

#ifdef CONFIG_BINFMT_SHARED_FLAT
static int load_flat_shared_library(int id, struct lib_info *p);
#endif

static int load_flat_binary(struct linux_binprm *, struct pt_regs * regs);
static int load_flat_library(struct file*);
static int flat_core_dump(long signr, struct pt_regs * regs, struct file *file);

extern void dump_thread(struct pt_regs *, struct user *);

static struct linux_binfmt flat_format = {
	NULL, THIS_MODULE, load_flat_binary, load_flat_library,
	flat_core_dump, PAGE_SIZE
};


/*
 * Routine writes a core dump image in the current directory.
 * Currently only a stub-function.
 */

static int flat_core_dump(long signr, struct pt_regs * regs, struct file *file)
{
	printk("Process %s:%d received signr %d and should have core dumped\n",
			current->comm, current->pid, (int) signr);
	return(1);
}


/*
 * create_flat_tables() parses the env- and arg-strings in new user
 * memory and creates the pointer tables from them, and puts their
 * addresses on the "stack", returning the new stack pointer value.
 */
static unsigned long create_flat_tables(
	unsigned long pp,
	struct linux_binprm * bprm)
{
	unsigned long *argv,*envp;
	unsigned long * sp;
	char * p = (char*)pp;
	int argc = bprm->argc;
	int envc = bprm->envc;
	char dummy;

	sp = (unsigned long *) ((-(unsigned long)sizeof(char *))&(unsigned long) p);

#ifdef __alpha__
/* whee.. test-programs are so much fun. */
	put_user((unsigned long) 0, --sp);
	put_user((unsigned long) 0, --sp);
	if (bprm->loader) {
		put_user((unsigned long) 0, --sp);
		put_user((unsigned long) 0x3eb, --sp);
		put_user((unsigned long) bprm->loader, --sp);
		put_user((unsigned long) 0x3ea, --sp);
	}
	put_user((unsigned long) bprm->exec, --sp);
	put_user((unsigned long) 0x3e9, --sp);
#endif

	sp -= envc+1;
	envp = sp;
	sp -= argc+1;
	argv = sp;
#if defined(__i386__) || defined(__mc68000__) || defined(__arm__) || defined(__H8300H__)
	--sp; put_user((unsigned long) envp, sp);
	--sp; put_user((unsigned long) argv, sp);
#endif
	put_user(argc,--sp);
	current->mm->arg_start = (unsigned long) p;
	while (argc-->0) {
		put_user((unsigned long) p, argv++);
		do {
			get_user(dummy, p); p++;
		} while (dummy);
	}
	put_user((unsigned long) NULL, argv);
	current->mm->arg_end = current->mm->env_start = (unsigned long) p;
	while (envc-->0) {
		put_user((unsigned long)p, envp); envp++;
		do {
			get_user(dummy, p); p++;
		} while (dummy);
	}
	put_user((unsigned long) NULL, envp);
	current->mm->env_end = (unsigned long) p;
	return (unsigned long)sp;
}



#ifdef CONFIG_BINFMT_ZFLAT
/*
 * this is fairly harmless unless you use it.  It hasn't had a lot
 * of testing but I have run systems with every binary compressed (davidm)
 *
 * here are the zlib hacks - to replace globals with locals
 */

typedef unsigned char  uch;
typedef unsigned short ush;
typedef unsigned long  ulg;
#define INBUFSIZ 4096
#define WSIZE 0x8000    /* window size--must be a power of two, and */
                        /*  at least 32K for zip's deflate method */
struct s_zloc
{
	struct linux_binprm *bprm;
	unsigned long start_pos;
	unsigned long bytes2read;
	char *data_pointer, *out_pointer;
	uch *inbuf;
	uch *window;
	unsigned insize;  /* valid bytes in inbuf */
	unsigned inptr;   /* index of next byte to be processed in inbuf */
	unsigned outcnt;  /* bytes in output buffer */
	int exit_code;
	long bytes_out;
	int crd_infp, crd_outfp;
	ulg bb;                         /* bit buffer */
	unsigned bk;                    /* bits in bit buffer */
	ulg crc_32_tab[256];
	ulg crc; /* shift register contents */
	unsigned hufts;
};

static int fill_inbuf(struct s_zloc *zloc)
{
	int i;
	loff_t fpos;

	if(zloc->exit_code)
		return -1;
	i = (zloc->bytes2read > INBUFSIZ) ? INBUFSIZ : zloc->bytes2read;
	fpos = zloc->start_pos;
	i = zloc->bprm->file->f_op->read(zloc->bprm->file,
			(char *) zloc->data_pointer, i, &fpos);
	if (i >= (unsigned long) -4096)
		return -1;
	zloc->bytes2read -= i;
	zloc->start_pos += i;
	zloc->insize = i;
	zloc->inptr = 1;
	return zloc->inbuf[0];
}

static void flush_window(struct s_zloc *zloc)
{
	ulg c = zloc->crc;
	unsigned n;
	uch *in, ch;
	memcpy(zloc->out_pointer, zloc->window, zloc->outcnt);
	in = zloc->window;
	for(n = 0; n < zloc->outcnt; n++)
	{
		ch = *in++;
		c = zloc->crc_32_tab[((int)c ^ch) & 0xff] ^(c >> 8);
	}
	zloc->crc = c;
	zloc->out_pointer += zloc->outcnt;
	zloc->bytes_out += (ulg)zloc->outcnt;
	zloc->outcnt = 0;
}

#define inbuf (zloc->inbuf)
#define window (zloc->window)
#define insize (zloc->insize)
#define inptr (zloc->inptr)
#define outcnt (zloc->outcnt)
#define exit_code (zloc->exit_code)
#define bytes_out (zloc->bytes_out)
#define crd_infp (zloc->crd_infp)
#define crd_infp (zloc->crd_infp)
#define bb (zloc->bb)
#define bk (zloc->bk)
#define crc (zloc->crc)
#define crc_32_tab (zloc->crc_32_tab)
#define hufts (zloc->hufts)

#define get_byte()  (inptr < insize ? inbuf[inptr++] : fill_inbuf(zloc))
#define memzero(s, n)     memset ((s), 0, (n))

#define OF(args)  args
#define Assert(cond,msg)
#define Trace(x)
#define Tracev(x)
#define Tracevv(x)
#define Tracec(c,x)
#define Tracecv(c,x)

#define STATIC static

#define malloc(arg) kmalloc(arg, GFP_KERNEL)
#define free(arg) kfree(arg)

#define error(arg) printk("zflat:" arg "\n")

#include "../lib/inflate2.c"

#undef error

#undef malloc
#undef free

#undef inbuf
#undef window
#undef insize
#undef inptr
#undef outcnt
#undef exit_code
#undef bytes_out
#undef crd_infp
#undef crd_outp
#undef bb
#undef bk
#undef crc
#undef crc_32_tab

#undef get_byte

#ifndef CONFIG_ARCH_WINBOND
#undef memzero
#endif


static int decompress_exec(
	struct linux_binprm *bprm,
	unsigned long offset,
	char * buffer,
	long len,
	int fd)
{
	struct s_zloc *zloc;
	int res;
	zloc = kmalloc(sizeof(*zloc), GFP_KERNEL);
	if(!zloc)
	{
		return -ENOMEM;
	}
	memset(zloc, 0, sizeof(*zloc));
	zloc->bprm = bprm;
	zloc->out_pointer = buffer;
	zloc->inbuf = kmalloc(INBUFSIZ, GFP_KERNEL);
	if(!zloc->inbuf)
	{
		kfree(zloc);
		return -ENOMEM;
	}
	zloc->window = kmalloc(WSIZE, GFP_KERNEL);
	if(!zloc->window)
	{
		kfree(zloc->inbuf);
		kfree(zloc);
		return -ENOMEM;
	}
	zloc->data_pointer = zloc->inbuf;
	zloc->bytes2read = len;
	zloc->start_pos = offset;
	zloc->crc = (ulg)0xffffffffL;
	makecrc(zloc);
	res = gunzip(zloc);
	kfree(zloc->window);
	kfree(zloc->inbuf);
	kfree(zloc);
	return res ? -ENOMEM : 0;
}
#endif /* CONFIG_BINFMT_ZFLAT */

#ifdef CONFIG_ARCH_WINBOND
static unsigned long
calc_reloc(unsigned long r, struct lib_info *p, int curid, int internalp, int* flag)
#else
static unsigned long
calc_reloc(unsigned long r, struct lib_info *p, int curid, int internalp)
#endif
{
	unsigned long addr;
	int id;
	unsigned long start_brk;
	unsigned long start_data;
	unsigned long text_len;
	unsigned long start_code;


#ifdef CONFIG_BINFMT_SHARED_FLAT
	if (r == 0)
		id = curid;	/* Relocs of 0 are always self referring */
	else {
		id = (r >> 24) & 0xff;	/* Find ID for this reloc */
		r &= 0x00ffffff;	/* Trim ID off here */
	}
	if (id >= MAX_SHARED_LIBS) {
		printk("BINFMT_FLAT: reference 0x%x to shared library %d",
				(unsigned) r, id);
		goto failed;
	}
	if (curid != id) {
		if (internalp) {
			printk("BINFMT_FLAT: reloc address 0x%x not in same module "
					"(%d != %d)", (unsigned) r, curid, id);
			goto failed;
		} else if ( ! p->lib_list[id].loaded &&
				load_flat_shared_library(id, p) > (unsigned long) -4096) {
			printk("BINFMT_FLAT: failed to load library %d", id);
			goto failed;
		}
		/* Check versioning information (i.e. time stamps) */
		if (p->lib_list[id].build_date && p->lib_list[curid].build_date &&
				p->lib_list[curid].build_date < p->lib_list[id].build_date) {
			printk("BINFMT_FLAT: library %d is younger than %d", id, curid);
			goto failed;
		}
	}
#else
	id = 0;
#endif

	start_brk = p->lib_list[id].start_brk;
	start_data = p->lib_list[id].start_data;
	start_code = p->lib_list[id].start_code;
	text_len = p->lib_list[id].text_len;

	if (r > start_brk - start_data + text_len) {
		printk("BINFMT_FLAT: reloc outside program 0x%x (0 - 0x%x/0x%x)",
					(int) r,(int)(start_brk-start_code),(int)text_len);
#ifdef CONFIG_ARCH_WINBOND	
			if(*flag!=0)
			{
				if(*flag==1)
					addr = r + start_code;
				else if(*flag==2)
					addr = r - text_len + start_data;
				return addr;
			}
#endif
			goto failed;
		}

		if (r < text_len)			/* In text segment */
		{
			addr = r + start_code;
#ifdef CONFIG_ARCH_WINBOND
			*flag=1;
#endif
		}
		else					/* In data segment */
		{
			addr = r - text_len + start_data;
#ifdef CONFIG_ARCH_WINBOND
			*flag=2;
#endif
		}

	/* Range checked already above so doing the range tests is redundant...*/
	return(addr);

failed:
	printk(", killing %s!\n", current->comm);
	send_sig(SIGSEGV, current, 0);

	return RELOC_FAILED;
}


void old_reloc(unsigned long rl)
{
#ifdef DEBUG
	char *segment[] = { "TEXT", "DATA", "BSS", "*UNKNOWN*" };
#endif
	flat_v2_reloc_t	r;
	unsigned long *ptr;

#if !defined(__H8300H__)	
	r.value = rl;
#if defined(CONFIG_COLDFIRE)
	ptr = (unsigned long *) (current->mm->start_code + r.reloc.offset);
#else
	ptr = (unsigned long *) (current->mm->start_data + r.reloc.offset);
#endif

#ifdef DEBUG
	printk("Relocation of variable at DATASEG+%x "
		"(address %p, currently %x) into segment %s\n",
		r.reloc.offset, ptr, (int)*ptr, segment[r.reloc.type]);
#endif
	
	switch (r.reloc.type) {
	case OLD_FLAT_RELOC_TYPE_TEXT:
		*ptr += current->mm->start_code;
		break;
	case OLD_FLAT_RELOC_TYPE_DATA:
		*ptr += current->mm->start_data;
		break;
	case OLD_FLAT_RELOC_TYPE_BSS:
		*ptr += current->mm->end_data;
		break;
	default:
		printk("BINFMT_FLAT: Unknown relocation type=%x\n", r.reloc.type);
		break;
	}

#ifdef DEBUG
	printk("Relocation became %x\n", (int)*ptr);
#endif
#else
	ptr = (unsigned long *)(current->mm->start_code+rl);
	if (ptr >= current->mm->end_code)
		ptr += sizeof(MAX_SHARED_LIBS) / sizeof (*ptr);
	*ptr += current->mm->start_code;
	if ((*ptr & 0x00ffffff) >= current->mm->end_code)
		*ptr += MAX_SHARED_LIBS * sizeof(unsigned long);
#endif
}		

static int load_flat_file(struct linux_binprm * bprm,
		struct lib_info *libinfo, int id, unsigned long *extra_stack)
{
	struct flat_hdr * hdr;
	unsigned long textpos = 0, datapos = 0, result;
	unsigned long realdatastart = 0;
	unsigned long text_len, data_len, bss_len, stack_len, flags;
	unsigned long memp = 0; /* for finding the brk area */
	unsigned long extra, rlim;
	unsigned long *reloc = 0, *rp;
	struct inode *inode;
	int i, rev, relocs = 0;
	loff_t fpos;
	unsigned long start_code, end_code;

	DBG_FLT("BINFMT_FLAT: Loading file: %s\n", bprm->filename);

	hdr = ((struct flat_hdr *) bprm->buf);		/* exec-header */
	inode = bprm->file->f_dentry->d_inode;

	text_len  = ntohl(hdr->data_start);
	data_len  = ntohl(hdr->data_end) - ntohl(hdr->data_start);
	bss_len   = ntohl(hdr->bss_end) - ntohl(hdr->data_end);
	stack_len = ntohl(hdr->stack_size);
	if (extra_stack) {
		stack_len += *extra_stack;
		*extra_stack = stack_len;
	}
	relocs    = ntohl(hdr->reloc_count);
	flags     = ntohl(hdr->flags);
	rev       = ntohl(hdr->rev);

	if (strncmp(hdr->magic, "bFLT", 4) ||
			(rev != FLAT_VERSION && rev != OLD_FLAT_VERSION)) {
		/*
		 * because a lot of people do not manage to produce good
		 * flat binaries,  we leave this printk to help them realise
		 * the problem.  We only print the error if its * not a script file
		 */
		if (strncmp(hdr->magic, "#!", 2))
			printk("BINFMT_FLAT: bad magic/rev (0x%x, need 0x%x)\n",
					rev, (int) FLAT_VERSION);
		return -ENOEXEC;
	}
	
	/* Don't allow old format executables to use shared libraries */
	if (rev == OLD_FLAT_VERSION && id != 0) {
		printk("BINFMT_FLAT: shared libraries are not available before rev 0x%x\n",
				(int) FLAT_VERSION);
		return -ENOEXEC;
	}

	/*
	 * fix up the flags for the older format,  there were all kinds
	 * of endian hacks,  this only works for the simple cases
	 */
	if (rev == OLD_FLAT_VERSION && flags)
		flags = FLAT_FLAG_RAM;

#ifndef CONFIG_BINFMT_ZFLAT
	if (flags & (FLAT_FLAG_GZIP|FLAT_FLAG_GZDATA)) {
		printk("Support for ZFLAT executables is not enabled.\n");
		return -ENOEXEC;
	}
#endif

	/*
	 * Check initial limits. This avoids letting people circumvent
	 * size limits imposed on them by creating programs with large
	 * arrays in the data or bss.
	 */
	rlim = current->rlim[RLIMIT_DATA].rlim_cur;
	if (rlim >= RLIM_INFINITY)
		rlim = ~0;
	if (data_len + bss_len > rlim)
		return -ENOMEM;

	/* Flush all traces of the currently running executable */
	if (id == 0) {
		result = flush_old_exec(bprm);
		if (result)
			return result;

		/* do this up here so we can fail cleanly */
		result = setup_arg_pages(bprm);
		if (result < 0) {
			/* Someone check-me: is this error path enough? */ 
			send_sig(SIGKILL, current, 0); 
			return result;
		}

		/* OK, This is the point of no return */
#if !defined(__sparc__)
		set_personality(PER_LINUX);
#else
		set_personality(PER_SUNOS);
#if !defined(__sparc_v9__)
		memcpy(&current->thread.core_exec, &ex, sizeof(struct exec));
#endif
#endif
	}

	/*
	 * there are a couple of cases here,  the seperate code/data
	 * case,  and then the fully copied to RAM case which lumps
	 * it all together.
	 */
	if ((flags & (FLAT_FLAG_RAM|FLAT_FLAG_GZIP)) == 0) {
		/*
		 * this should give us a ROM ptr,  but if it doesn't we don't
		 * really care
		 */
		DBG_FLT("BINFMT_FLAT: ROM mapping of file (we hope)\n");

		down_write(&current->mm->mmap_sem);
		textpos = do_mmap(bprm->file, 0, text_len, PROT_READ|PROT_EXEC, 0, 0);
		up_write(&current->mm->mmap_sem);
		if (!textpos  || textpos >= (unsigned long) -4096) {
			if (!textpos)
				textpos = (unsigned long) -ENOMEM;
			printk("Unable to mmap process text, errno %d\n", (int)-textpos);
		}

		extra = MAX(bss_len + stack_len, relocs * sizeof(unsigned long));

		down_write(&current->mm->mmap_sem);
		realdatastart = do_mmap(0, 0, data_len + extra +
				MAX_SHARED_LIBS * sizeof(unsigned long),
				PROT_READ|PROT_WRITE|PROT_EXEC, 0, 0);
		up_write(&current->mm->mmap_sem);

		if (realdatastart == 0 || realdatastart >= (unsigned long)-4096) {
			if (!realdatastart)
				realdatastart = (unsigned long) -ENOMEM;
			printk("Unable to allocate RAM for process data, errno %d\n",
					(int)-datapos);
			do_munmap(current->mm, textpos, text_len);
			return realdatastart;
		}
		datapos = realdatastart + MAX_SHARED_LIBS * sizeof(unsigned long);

		DBG_FLT("BINFMT_FLAT: Allocated data+bss+stack (%d bytes): %x\n",
				(int)(data_len + bss_len + stack_len), (int)datapos);

		fpos = ntohl(hdr->data_start);
#ifdef CONFIG_BINFMT_ZFLAT
		if (flags & FLAT_FLAG_GZDATA) {
			result = decompress_exec(bprm, fpos, (char *) datapos, 
						 data_len + (relocs * sizeof(unsigned long)), 0);
		} else
#endif
		{
			result = bprm->file->f_op->read(bprm->file, (char *) datapos,
					data_len + (relocs * sizeof(unsigned long)), &fpos);
		}
		if (result >= (unsigned long)-4096) {
			printk("Unable to read data+bss, errno %d\n", (int)-result);
			do_munmap(current->mm, textpos, text_len);
			do_munmap(current->mm, realdatastart, data_len + extra);
			return result;
		}

		reloc = (unsigned long *) (datapos+(ntohl(hdr->reloc_start)-text_len));
		memp = realdatastart;

	} else {

		/*
		 * calculate the extra space we need to map in
		 */

		extra = MAX(bss_len + stack_len, relocs * sizeof(unsigned long));

		down_write(&current->mm->mmap_sem);
		textpos = do_mmap(0, 0, text_len + data_len + extra +
					MAX_SHARED_LIBS * sizeof(unsigned long),
				PROT_READ | PROT_EXEC | PROT_WRITE, 0, 0);
		up_write(&current->mm->mmap_sem);
		if (!textpos  || textpos >= (unsigned long) -4096) {
			if (!textpos)
				textpos = (unsigned long) -ENOMEM;
			printk("Unable to allocate RAM for process text/data, errno %d\n",
					(int)-textpos);
		}

		realdatastart = textpos + ntohl(hdr->data_start);
		datapos = realdatastart + MAX_SHARED_LIBS * sizeof(unsigned long);
		reloc = (unsigned long *) (textpos + ntohl(hdr->reloc_start) +
				MAX_SHARED_LIBS * sizeof(unsigned long));
		memp = textpos;

#ifdef CONFIG_BINFMT_ZFLAT
		/*
		 * load it all in and treat it like a RAM load from now on
		 */
		if (flags & FLAT_FLAG_GZIP) {
			result = decompress_exec(bprm, sizeof (struct flat_hdr),
					 (((char *) textpos) + sizeof (struct flat_hdr)),
					 (text_len + data_len + (relocs * sizeof(unsigned long))
						  - sizeof (struct flat_hdr)),
					 0);
			memmove(datapos, realdatastart,
					data_len + (relocs * sizeof(unsigned long)));
		} else if (flags & FLAT_FLAG_GZDATA) {
			fpos = 0;
			result = bprm->file->f_op->read(bprm->file,
					(char *) textpos, text_len, &fpos);
			if (result < (unsigned long) -4096)
				result = decompress_exec(bprm, text_len, (char *) datapos,
						 data_len + (relocs * sizeof(unsigned long)), 0);
		}
		else
#endif
		{
			fpos = 0;
			result = bprm->file->f_op->read(bprm->file,
					(char *) textpos, text_len, &fpos);
			if (result < (unsigned long) -4096) {
				fpos = ntohl(hdr->data_start);
				result = bprm->file->f_op->read(bprm->file, (char *) datapos,
					data_len + (relocs * sizeof(unsigned long)), &fpos);
			}
		}
		if (result >= (unsigned long)-4096) {
			printk("Unable to read code+data+bss, errno %d\n",(int)-result);
			do_munmap(current->mm, textpos, text_len + data_len + extra +
				MAX_SHARED_LIBS * sizeof(unsigned long));
			return result;
		}
	}

	DBG_FLT("Mapping is %x, Entry point is %x, data_start is %x\n",
			(int)textpos, 0x00ffffff & ntohl(hdr->entry), ntohl(hdr->data_start));

	/* The main program needs a little extra setup in the task structure */
	start_code = textpos + sizeof (struct flat_hdr);
	end_code = textpos + text_len;
	if (id == 0) {
		current->mm->start_code = start_code;
		current->mm->end_code = end_code;
		current->mm->start_data = datapos;
		current->mm->end_data = datapos + data_len;
#ifdef NO_MM
		/*
		 *	set up the brk stuff (uses any slack left in data/bss/stack allocation
		 *	We put the brk after the bss (between the bss and stack) like other
		 *	platforms.
		 */
		current->mm->start_brk = datapos + data_len + bss_len;
		current->mm->brk = (current->mm->start_brk + 3) & ~3;
		current->mm->end_brk = memp + ksize((void *) memp) - stack_len;
#else
		current->mm->mmap = NULL;
#endif
		current->mm->rss = 0;
	}
	DBG_FLT("%s %s: TEXT=%x-%x DATA=%x-%x BSS=%x-%x\n",
		id?"Lib":"Load", bprm->filename,
		(int) start_code, (int) end_code,
		(int) datapos, (int) datapos + data_len,
		(int) datapos + data_len, (int) ((datapos + data_len + bss_len) + 3) & ~3);

	text_len -= sizeof(struct flat_hdr); /* the real code len */

	/* Store the current module values into the global library structure */
	libinfo->lib_list[id].start_code = start_code;
	libinfo->lib_list[id].start_data = datapos;
#ifdef NO_MM
	libinfo->lib_list[id].start_brk = datapos + data_len + bss_len;
#endif
	libinfo->lib_list[id].text_len = text_len;
	libinfo->lib_list[id].loaded = 1;
	libinfo->lib_list[id].entry = (0x00ffffff & ntohl(hdr->entry)) + textpos;
	libinfo->lib_list[id].build_date = ntohl(hdr->build_date);
	
	/*
	 * We just load the allocations into some temporary memory to
	 * help simplify all this mumbo jumbo
	 *
	 * We've got two different sections of relocation entries.
	 * The first is the GOT which resides at the begining of the data segment
	 * and is terminated with a -1.  This one can be relocated in place.
	 * The second is the extra relocation entries tacked after the image's
	 * data segment. These require a little more processing as the entry is
	 * really an offset into the image which contains an offset into the
	 * image.
	 */
	if (flags & FLAT_FLAG_GOTPIC) {
		for (rp = (unsigned long *)datapos; *rp != 0xffffffff; rp++) {
			unsigned long addr;
			if (*rp) {
#ifdef CONFIG_ARCH_WINBOND
				int test_data=0;
				addr = calc_reloc(*rp, libinfo, id, 0,&test_data);
#else
				addr = calc_reloc(*rp, libinfo, id, 0);
#endif
				if (addr == RELOC_FAILED)
					return -ENOEXEC;
				*rp = addr;
			}
		}
	}

	/*
	 * Now run through the relocation entries.
	 * We've got to be careful here as C++ produces relocatable zero
	 * entries in the constructor and destructor tables which are then
	 * tested for being not zero (which will always occur unless we're
	 * based from address zero).  This causes an endless loop as __start
	 * is at zero.  The solution used is to not relocate zero addresses.
	 * This has the negative side effect of not allowing a global data
	 * reference to be statically initialised to _stext (I've moved
	 * __start to address 4 so that is okay).
	 */
	if (rev > OLD_FLAT_VERSION) {
#ifdef CONFIG_ARCH_WINBOND
		int test_data=0;
		for (i=0; i < relocs; i++) {
			unsigned long addr;

			/* Get the address of the pointer to be
			   relocated (of course, the address has to be
			   relocated first).  */
			test_data=0;
			rp = (unsigned long *) calc_reloc(ntohl(reloc[i]), libinfo, id, 1,&test_data);
			if (rp == (unsigned long *)RELOC_FAILED)
				return -ENOEXEC;

			/* Get the pointer's value.  */
			addr = get_unaligned (rp);

			if (addr != 0) {
				/*
				 * Do the relocation.  PIC relocs in the data section are
				 * already in target order
				 */
				addr = calc_reloc(
						(flags & FLAT_FLAG_GOTPIC) ? addr : ntohl(addr),
						libinfo, id, 0,&test_data);
				if (addr == RELOC_FAILED)
					return -ENOEXEC;
				/* Write back the relocated pointer.  */
				put_unaligned (addr, rp);
			}
		}
#else
	for (i=0; i < relocs; i++) {
			unsigned long addr;

			/* Get the address of the pointer to be
			   relocated (of course, the address has to be
			   relocated first).  */
			
			rp = (unsigned long *) calc_reloc(ntohl(reloc[i]), libinfo, id, 1);
			if (rp == (unsigned long *)RELOC_FAILED)
				return -ENOEXEC;

			/* Get the pointer's value.  */
			addr = get_unaligned (rp);
			if (addr != 0) {
				/*
				 * Do the relocation.  PIC relocs in the data section are
				 * already in target order
				 */
				addr = calc_reloc(
						(flags & FLAT_FLAG_GOTPIC) ? addr : ntohl(addr),
						libinfo, id, 0);
				if (addr == RELOC_FAILED)
					return -ENOEXEC;
				/* Write back the relocated pointer.  */
				put_unaligned (addr, rp);
			}
		}
#endif
	} else {
		for (i=0; i < relocs; i++)
			old_reloc(ntohl(reloc[i]));
	}
	
	flush_icache_range(start_code, end_code);

	/* zero the BSS,  BRK and stack areas */
	memset((void*)(datapos + data_len), 0, bss_len + 
			(memp + ksize((void *) memp) - stack_len -	/* end brk */
			libinfo->lib_list[id].start_brk) +		/* start brk */
			stack_len);
	return 0;
}


#ifdef CONFIG_BINFMT_SHARED_FLAT
/* Load a shared library into memory.  The library gets its own data
 * segment (including bss) but not argv/argc/environ.
 */
static int load_flat_shared_library(int id, struct lib_info *libs)
{
	struct linux_binprm bprm;
	int res;
	char buf[16];

	/* Create the file name */
	sprintf(buf, "/lib/lib%d.so", id);

	/* Open the file up */
	bprm.filename = buf;
	bprm.file = open_exec(bprm.filename);
	res = PTR_ERR(bprm.file);
	if (IS_ERR(bprm.file))
		return res;

	res = prepare_binprm(&bprm);

	if (res <= (unsigned long)-4096)
		res = load_flat_file(&bprm, libs, id, NULL);
	if (bprm.file) {
		allow_write_access(bprm.file);
		fput(bprm.file);
		bprm.file = NULL;
	}
	return(res);
}
#endif


/*
 * These are the functions used to load flat style executables and shared
 * libraries.  There is no binary dependent code anywhere else.
 */
static int load_flat_binary(struct linux_binprm * bprm, struct pt_regs * regs)
{
	struct lib_info libinfo;
	unsigned long p = bprm->p;
	unsigned long stack_len;
	unsigned long start_addr;
	unsigned long *sp;
	int res;
	int i, j;

	memset(&libinfo, 0, sizeof(libinfo));
	/*
	 * We have to add the size of our arguments to our stack size
	 * otherwise it's too easy for users to create stack overflows
	 * by passing in a huge argument list.  And yes,  we have to be
	 * pedantic and include space for the argv/envp array as it may have
	 * a lot of entries.
	 */
#define TOP_OF_ARGS (PAGE_SIZE * MAX_ARG_PAGES - sizeof(void *))
	stack_len = TOP_OF_ARGS - bprm->p;             /* the strings */
	stack_len += (bprm->argc + 1) * sizeof(char *); /* the argv array */
	stack_len += (bprm->envc + 1) * sizeof(char *); /* the envp array */

	
	res = load_flat_file(bprm, &libinfo, 0, &stack_len);
	if (res > (unsigned long)-4096)
		return res;
	
	/* Update data segment pointers for all libraries */
	for (i=0; i<MAX_SHARED_LIBS; i++)
		if (libinfo.lib_list[i].loaded)
			for (j=0; j<MAX_SHARED_LIBS; j++)
				(-(j+1))[(unsigned long *)(libinfo.lib_list[i].start_data)] =
					(libinfo.lib_list[j].loaded)?
						libinfo.lib_list[j].start_data:UNLOADED_LIB;

	compute_creds(bprm);
 	current->flags &= ~PF_FORKNOEXEC;

	set_binfmt(&flat_format);

	p = ((current->mm->end_brk + stack_len + 3) & ~3) - 4;
	DBG_FLT("p=%x\n", (int)p);

	/* copy the arg pages onto the stack, this could be more efficient :-) */
	for (i = TOP_OF_ARGS - 1; i >= bprm->p; i--)
		* (char *) --p =
			((char *) page_address(bprm->page[i/PAGE_SIZE]))[i % PAGE_SIZE];

	sp = (unsigned long *) create_flat_tables(p, bprm);
	
	/* Fake some return addresses to ensure the call chain will
	 * initialise library in order for us.  We are required to call
	 * lib 1 first, then 2, ... and finally the main program (id 0).
	 */
	start_addr = libinfo.lib_list[0].entry;

#ifdef CONFIG_BINFMT_SHARED_FLAT
	for (i = MAX_SHARED_LIBS-1; i>0; i--) {
		if (libinfo.lib_list[i].loaded) {
			/* Push previos first to call address */
			--sp;	put_user(start_addr, sp);
			start_addr = libinfo.lib_list[i].entry;
		}
	}
#endif
	
	/* Stash our initial stack pointer into the mm structure */
	current->mm->start_stack = (unsigned long )sp;

	
	DBG_FLT("start_thread(regs=0x%x, entry=0x%x, start_stack=0x%x)\n",
		(int)regs, (int)start_addr, (int)current->mm->start_stack);
	
	start_thread(regs,
		     start_addr,
		     current->mm->start_stack);

	if (current->ptrace & PT_PTRACED)
		send_sig(SIGTRAP, current, 0);

	return 0;
}


static int load_flat_library(struct file *file)
{
	return(-ENOEXEC);
}

static int __init init_flat_binfmt(void)
{
	return register_binfmt(&flat_format);
}

static void __exit exit_flat_binfmt(void)
{
	unregister_binfmt(&flat_format);
}

EXPORT_NO_SYMBOLS;

module_init(init_flat_binfmt);
module_exit(exit_flat_binfmt);
