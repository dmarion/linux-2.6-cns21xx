/*
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, Version 2, as
 *  published by the Free Software Foundation.
 */

#include <plat/gpio.h>

#include <mach/cns21xx.h>
#include <mach/irqs.h>

static struct fa_gpio_chip cns21xx_gpio_chips[] = {
	{
		.gpio_chip = {
			.label	= "GPIOA",
			.base	= 0,
			.ngpio	= 32,
		},

		.map_base	= CNS21XX_GPIOA_BASE,
		.irq_base	= CNS21XX_GPIO_IRQ_BASE,
	}, {
		.gpio_chip = {
			.label	= "GPIOB",
			.base	= 32,
			.ngpio	= 32,
		},

		.map_base	= CNS21XX_GPIOB_BASE,
		.irq_base	= CNS21XX_GPIO_IRQ_BASE + 32,
	}
};

static struct fa_gpio_data cns21xx_gpio_data = {
	.chips	= cns21xx_gpio_chips,
	.nchips = ARRAY_SIZE(cns21xx_gpio_chips),
	.irq	= CNS21XX_IRQ_GPIO,
};

void __init cns21xx_gpio_init(void)
{
	fa_gpio_init(&cns21xx_gpio_data);
}
