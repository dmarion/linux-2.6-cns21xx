/*
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, Version 2, as
 *  published by the Free Software Foundation.
 */

#ifndef _FA_GPIO_H
#define _FA_GPIO_H

#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/io.h>

struct fa_gpio_chip {
	struct gpio_chip	gpio_chip;
	unsigned int		map_base;
	unsigned int		irq_base;

	void __iomem		*mem_base;
};

struct fa_gpio_data {
	struct fa_gpio_chip	*chips;
	unsigned int		nchips;
	unsigned int		irq;
};

void __init fa_gpio_init(struct fa_gpio_data *data);

#endif /* _FA_GPIO_H */
