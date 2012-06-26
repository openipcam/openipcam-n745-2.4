#define  PPBUFSIZ   2048
void pushinit(unsigned char *buffer, unsigned int max);
void pushbit(int bit);
void pushblk(int blk, int bitsinblk);
void pushexit(void);
int pushedbits(void);
void pullinit(unsigned char *buffer);
int pullbit(void);
int pullblk(int bitsinblk);
void pullexit(void);
int pulledbits(void);
