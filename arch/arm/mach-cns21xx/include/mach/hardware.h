/*
 *  Copyright (c) 2008 Cavium Networks
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, Version 2, as
 *  published by the Free Software Foundation.
 */

#ifndef _CNS21XX_HARDWARE_H
#define _CNS21XX_HARDWARE_H

#define PCIBIOS_MIN_IO	0x00000000
#define PCIBIOS_MIN_MEM	0x00000000

#ifndef __ASSEMBLY__
extern unsigned long cns21xx_get_pll_freq(void);
extern unsigned long cns21xx_get_cpu_freq(void);
extern unsigned long cns21xx_get_ahb_freq(void);
extern unsigned long cns21xx_get_apb_freq(void);
#endif

#endif /* _CNS21XX_HARDWARE_H */
