/* Rubin encoder/decoder header       */
/* work started at   : aug   3, 1994  */
/* last modification : aug  15, 1994  */

#define RUBIN_REG_SIZE   16
#define UPPER_BIT_RUBIN    (((long) 1)<<RUBIN_REG_SIZE-1)
#define LOWER_BITS_RUBIN   ((((long) 1)<<RUBIN_REG_SIZE-1)-1)

void init_rubin (void);
void encode (long, long, int);
void end_rubin (void);
void init_decod (void);
int decode (long, long);
