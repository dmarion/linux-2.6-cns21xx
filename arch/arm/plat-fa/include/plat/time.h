/*
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef _FA_TIME_H
#define _FA_TIME_H

#define FA_TIMER1	0
#define FA_TIMER2	1
#define FA_TIMER3	2

int __init fa_timer_init(unsigned int mapbase, unsigned int irq,
			 unsigned int timer, unsigned int freq);

#endif /* _FA_TIME_H */
