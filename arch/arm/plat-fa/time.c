/*
 *  Copyright (C) 2001-2006 Storlink, Corp.
 *  Copyright (C) 2008-2009 Paulius Zaleckas <paulius.zaleckas@teltonika.lt>
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/io.h>

#include <asm/mach/time.h>
#include <plat/time.h>

/*
 * Register definitions for the timers
 */
#define TIMER_COUNT(_base, _tmr)	((_base) + 0x00 + (_tmr) * 0x10)
#define TIMER_LOAD(_base, _tmr)		((_base) + 0x04 + (_tmr) * 0x10)
#define TIMER_MATCH1(_base, _tmr)	((_base) + 0x08 + (_tmr) * 0x10)
#define TIMER_MATCH2(_base, _tmr)	((_base) + 0x0c + (_tmr) * 0x10)

#define TIMER_CR(_base)			((_base) + 0x30)
#define TIMER_STATUS(_base)		((_base) + 0x34)
#define TIMER_MASK(_base)		((_base) + 0x38)

#define TIMER_SIZE		0x3c

#define TIMER_CR_ENABLE(x)	(1 << ((x) * 3))
#define TIMER_CR_CLOCK(x)	(1 << ((x) * 3 + 1))
#define TIMER_CR_INT(x)		(1 << ((x) * 3 + 2))
#define TIMER_CR_DOWN(x)	(1 << ((x) * 3 + 9))

#define TIMER_MASK_MATCH1(x)	(1 << ((x) * 3))
#define TIMER_MASK_MATCH2(x)	(1 << ((x) * 3 + 1))
#define TIMER_MASK_OF(x)	(1 << ((x) * 3 + 2))

#define TIMER_MASK_ALL		0x7ff

/*
 * IRQ handler for the timer
 */
static irqreturn_t fa_timer_interrupt(int irq, void *dev_id)
{
	timer_tick();
	return IRQ_HANDLED;
}

static struct irqaction fa_timer_irq = {
	.name		= "Timer Tick",
	.flags		= IRQF_DISABLED | IRQF_TIMER,
	.handler	= fa_timer_interrupt,
};

int __init fa_timer_init(unsigned int mapbase, unsigned int irq,
			 unsigned int timer, unsigned int freq)
{
	void __iomem *base;

	base = ioremap(mapbase, TIMER_SIZE);
	if (!base)
		return -ENOMEM;

	/* disable timers, clear status and mask all interrupts */
	__raw_writel(0, TIMER_CR(base));
	__raw_writel(0, TIMER_STATUS(base));
	__raw_writel(TIMER_MASK_ALL, TIMER_MASK(base));

	/*
	 * Make irqs happen for the system timer
	 */
	setup_irq(irq, &fa_timer_irq);

	/* Setup the timer */
	__raw_writel(freq / HZ, TIMER_COUNT(base, timer));
	__raw_writel(freq / HZ, TIMER_LOAD(base, timer));
	__raw_writel(0, TIMER_MATCH1(base, timer));
	__raw_writel(0, TIMER_MATCH2(base, timer));

	/* Enable interrupt and start the timer */
	__raw_writel(TIMER_MASK_ALL & ~TIMER_MASK_OF(timer),
		     TIMER_MASK(base));

	__raw_writel(TIMER_CR_ENABLE(timer) |
		     TIMER_CR_INT(timer) |
		     TIMER_CR_DOWN(timer),
		     TIMER_CR(base));

	iounmap(base);

	return 0;
}
