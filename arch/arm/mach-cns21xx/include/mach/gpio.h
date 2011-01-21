/*
 *  Copyright (c) 2008 Cavium Networks
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (c) 2011 Damjan Marion <damjan.marion@gmail.com>
 *  
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, Version 2, as
 *  published by the Free Software Foundation.
 */

#ifndef _CNS21XX_GPIO_H
#define _CNS21XX_GPIO_H

#include <asm-generic/gpio.h>
#include <mach/irqs.h>
#include <mach/cns21xx-intc.h>
#include <mach/cns21xx-regs.h>

#define gpio_get_value	__gpio_get_value
#define gpio_set_value	__gpio_set_value
#define gpio_cansleep	__gpio_cansleep

static inline int gpio_to_irq(unsigned gpio)
{
	return (CNS21XX_GPIOA_BASE + gpio);
}

static inline int irq_to_gpio(int irq)
{
	return (irq - CNS21XX_GPIOA_BASE);
}

#endif /* _CNS21XX_GPIO_H */
