/*
 *  Copyright (c) 2008 Cavium Networks
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, Version 2, as
 *  published by the Free Software Foundation.
 */

#include <mach/cns21xx.h>

/* TODO: make it configurable */
#define CNS21XX_DEBUG_UART_BASE		CNS21XX_UART0_BASE
#define CNS21XX_DEBUG_UART_BASE_VIRT	CNS21XX_UART0_BASE_VIRT

	.macro	addruart, rp, rv
	mov	\rp, #(CNS21XX_DEBUG_UART_BASE)				@ physical
	mov	\rv, #(CNS21XX_DEBUG_UART_BASE_VIRT & 0xff000000)	@virtual
	orr	\rv, \rv, #(CNS21XX_DEBUG_UART_BASE_VIRT & 0x00ff0000)
	orr	\rv, \rv, #(CNS21XX_DEBUG_UART_BASE_VIRT & 0x0000ff00)
	orr	\rv, \rv, #(CNS21XX_DEBUG_UART_BASE_VIRT & 0x000000ff)
	.endm

#define UART_SHIFT 2
#include <asm/hardware/debug-8250.S>
