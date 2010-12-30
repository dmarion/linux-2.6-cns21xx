/*
 *  Copyright (c) 2008 Cavium Networks
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, Version 2, as
 *  published by the Free Software Foundation.
 */

#include <mach/cns21xx.h>
#include <mach/irqs.h>

#define INTC_IRQ_STATUS	0x1c

	.macro	disable_fiq
	.endm

	.macro  get_irqnr_preamble, base, tmp
	.endm

	.macro  arch_ret_to_user, tmp1, tmp2
	.endm

	.macro get_irqnr_and_base, irqnr, irqstat, base, tmp
	ldr		\base, =(CNS21XX_INTC_BASE_VIRT + INTC_IRQ_STATUS)
	ldr		\irqstat, [\base]
	mov		\irqnr, #0
9001:
	tst		\irqstat, #1
	bne		9002f
	add		\irqnr, \irqnr, #1
	mov		\irqstat, \irqstat, lsr #1
	cmp		\irqnr, #CNS21XX_NR_INTC_IRQS
	bcc		9001b
9002:
	.endm

	.macro irq_prio_table
	.endm
