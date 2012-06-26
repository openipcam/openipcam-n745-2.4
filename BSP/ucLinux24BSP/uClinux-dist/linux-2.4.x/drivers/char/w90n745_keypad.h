/****************************************************************************
 *
 * Copyright (c) 2004 - 2006 Winbond Electronics Corp. All rights reserved.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * FILENAME
 *     w90n745_keypad.h
 *
 * VERSION
 *     1.0
 *
 * DESCRIPTION
 *	The winbond 64-keyboard driver header file
 *
 * DATA STRUCTURES
 *		None
 *
 * FUNCTIONS
 *		None
 *
 * HISTORY
 *	2005/09/09		 Ver 1.0 Created by PC34 YHan
 *
 * REMARK
 *     None
 *************************************************************************/

#ifndef W90N745__KEYPAD_H
#define W90N745__KEYPAD_H

//#define KPI_DEBUG

#define KPICONF     0xFFF88000
#define KPI3KCONF   0xFFF88004
#define KPILPCONF   0xFFF88008
#define KPISTATUS   0xFFF8800C

#ifdef __WB_EVB__
#define GPIO_CFG        0xFFF83020
#define GPIO_DIR	0xFFF83024
#define GPIO_CFG_MASK   0xFFF00000
#define GPIO_CFG_VALUE  0x000AAAAA
#define KPICONF_VALUE   0x000405FA
#endif



#define KEYPAD_MAGIC 'k'
#define KEYPAD_MAXNR  2

#define KPD_BLOCK			_IOW('k', 1, unsigned int)
#define KPD_NONBLOCK		_IOW('k', 2, unsigned int)

#endif
