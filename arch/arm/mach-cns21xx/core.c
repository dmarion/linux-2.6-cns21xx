/*
 *  Copyright (c) 2008 Cavium Networks
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, Version 2, as
 *  published by the Free Software Foundation.
 */

#include <linux/kernel.h>

#include <mach/hardware.h>
#include <mach/cns21xx-pm.h>

static unsigned long cns21xx_pll_freq;
static unsigned long cns21xx_cpu_freq;
static unsigned long cns21xx_ahb_freq;
static unsigned long cns21xx_apb_freq;

static void cns21xx_init_freq(void)
{
	static int freq_init_done;
	unsigned int t;

	if (freq_init_done)
		return;

	t = PWRMGT_SYSTEM_CLOCK_CONTROL_REG;
	switch (t & 0x3) {
	case 0x00:
		cns21xx_pll_freq = 175000000;
		break;

	case 0x01:
		cns21xx_pll_freq = 200000000;
		break;

	case 0x02:
		cns21xx_pll_freq = 225000000;
		break;

	case 0x03:
		cns21xx_pll_freq = 250000000;
		break;
	}

	cns21xx_cpu_freq = cns21xx_pll_freq / (((t >> 2) & 0x3) + 1);
	cns21xx_ahb_freq = cns21xx_cpu_freq / (((t >> 4) & 0x3) + 1);
	cns21xx_apb_freq = cns21xx_ahb_freq / (((t >> 8) & 0x3) + 1);

	freq_init_done = 1;
}

unsigned long cns21xx_get_pll_freq(void)
{
	cns21xx_init_freq();
	return cns21xx_pll_freq;
}

unsigned long cns21xx_get_cpu_freq(void)
{
	cns21xx_init_freq();
	return cns21xx_cpu_freq;
}

unsigned long cns21xx_get_ahb_freq(void)
{
	cns21xx_init_freq();
	return cns21xx_ahb_freq;
}

unsigned long cns21xx_get_apb_freq(void)
{
	cns21xx_init_freq();
	return cns21xx_apb_freq;
}
