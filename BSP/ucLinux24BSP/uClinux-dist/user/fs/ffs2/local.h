// This header binds the reader code to the Linux Kernel structures

#include <linux/string.h>
#include <linux/fs.h>

#include "ffs2_fs.h"
#include "ffs2_fs_sb.h"

/* As a independent module we dont have the right kernel labels, but
   our size fits into the alloted space */
#define getFFS2_sb(x) (*(struct ffs2_sb_info *)&(x)->u)

