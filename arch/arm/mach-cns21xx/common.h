/*
 * Common CNS21xx architecture functions
 *
 * Copyright (C) 2010 Damjan Marion
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __CNS21XX_COMMON_H__
#define __CNS21XX_COMMON_H__


void cns21xx_map_io(void);
void cns21xx_init_irq(void);
void __init cns21xx_gpio_init(void);
int __init cns21xx_register_uart0(void);
int __init cns21xx_register_uart1(void);
int __init cns21xx_register_uart0_early(void);
void __init cns21xx_register_usb(void);

//struct spi_board_info;
void __init cns21xx_register_spi_master(int id, struct spi_board_info *info,
                                       unsigned int n);

extern struct sys_timer cns21xx_timer;

//extern void cns21xx_early_init(void);

/*
struct mtd_partition;

extern void gemini_map_io(void);
extern void gemini_init_irq(void);
extern void gemini_timer_init(void);
extern void gemini_gpio_init(void);
*/
/* Common platform devices registration functions */

/*
extern int platform_register_uart(void);
extern int platform_register_pflash(unsigned int size,
				    struct mtd_partition *parts,
				    unsigned int nr_parts);
				
*/

#endif /* __CNS21XX_COMMON_H__ */
