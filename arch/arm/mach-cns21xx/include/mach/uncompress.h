/*
 *  Copyright (c) 2008 Cavium Networks
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, Version 2, as
 *  published by the Free Software Foundation.
 */

#ifndef _CNS21XX_UNCOMPRESS_H
#define _CNS21XX_UNCOMPRESS_H

#define UART_BASE	0x78000000

#define UART_REG(offs)	(*((volatile unsigned int *)(UART_BASE + offs)))

#define UART_THR	UART_REG(0x00)
#define UART_LSR	UART_REG(0x14)
#define THR_EMPTY	(1 << 5)

#define UART_THR_EMPTY()     (((UART_LSR) & THR_EMPTY) == (THR_EMPTY))

static void putc(int c)
{
	if (c != 0) {
		while (!UART_THR_EMPTY())
			barrier();

		UART_THR = (int)(c & 0xFF);
	}
}

static inline void flush(void)
{
}

#define arch_decomp_setup()
#define arch_decomp_wdog()

#endif /* _CNS21XX_UNCOMPRESS_H */
