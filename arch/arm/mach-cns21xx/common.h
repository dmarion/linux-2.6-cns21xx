/*
 *  Copyright (c) 2008 Cavium Networks
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, Version 2, as
 *  published by the Free Software Foundation.
 */

#ifndef _MACH_CNS21XX_COMMON_H
#define _MACH_CNS21XX_COMMON_H

void __init cns21xx_map_io(void);
void __init cns21xx_init_irq(void);
void __init cns21xx_gpio_init(void);

extern struct sys_timer cns21xx_timer;

int __init cns21xx_register_uart0(void);
int __init cns21xx_register_uart1(void);
int __init cns21xx_register_usb(void);

#endif /* _MACH_CNS21XX_COMMON_H */
