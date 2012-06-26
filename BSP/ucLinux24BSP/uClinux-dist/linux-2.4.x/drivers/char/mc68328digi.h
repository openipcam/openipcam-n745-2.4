#ifndef _MC68328DIGI_H
#define _MC68328DIGI_H

#include <linux/ioctl.h>

enum mc68328digi_mode {
	MC68328DIGI_RARE,		/* debounced up/down and absolute x,y */
	MC68328DIGI_DEBUG,		/* up/down, debounced, transitions, button */
	MC68328DIGI_PS2,		/* ps2 relative (default) */ 
};


struct mc68328digi_params {
	enum mc68328digi_mode mode;
	int	bounce_interval;
	int	sticky_drag;
	int	tap_interval;
	int	irq;
	int	io;
	int	calibrated;
	long	x_a, x_b;
	long	y_a, y_b;
};

#define MS *HZ/1000

/* Appears as device major=10 (MISC), minor=PC110_PAD */
#define MC68328DIGI_MINOR		9

#define MC68328DIGI_IOCTL_TYPE		0x9a

#define MC68328DIGIIOCGETP _IOR(MC68328DIGI_IOCTL_TYPE, 0, struct mc68328digi_params)
#define MC68328DIGIIOCSETP _IOR(MC68328DIGI_IOCTL_TYPE, 1, struct mc68328digi_params)
 
#endif /* _MC68328DIGI_H */
