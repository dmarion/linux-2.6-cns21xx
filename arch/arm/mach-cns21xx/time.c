/*
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, Version 2, as
 *  published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/irq.h>

#include <asm/mach/time.h>

#include <plat/time.h>
#include <mach/hardware.h>
#include <mach/cns21xx-regs.h>

//#include "common.h"

static void __init cns21xx_timer_init(void)
{
	fa_timer_init(CNS21XX_TIMER_BASE, CNS21XX_IRQ_TIMER1, FA_TIMER1,
		      cns21xx_get_apb_freq());
}

struct sys_timer cns21xx_timer = {
	.init		= cns21xx_timer_init,
};
