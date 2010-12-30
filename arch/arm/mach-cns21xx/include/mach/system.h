/*
 *  Copyright (c) 2008 Cavium Networks
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, Version 2, as
 *  published by the Free Software Foundation.
 */

#ifndef _CNS21XX_SYSTEM_H
#define _CNS21XX_SYSTEM_H

#include <mach/cns21xx.h>
#include <mach/cns21xx_powermgmt.h>

static inline void arch_idle(void)
{
	/*
	 * Because of broken hardware we have to enable interrupts or the CPU
	 * will never wakeup... Acctualy it is not very good to enable
	 * interrupts here since scheduler can miss a tick, but there is
	 * no other way around this. Platforms that needs it for power saving
	 * should call enable_hlt() in init code, since by default it is
	 * disabled.
	 */
	local_irq_enable();
	cpu_do_idle();
}

static inline void arch_reset(char mode, const char *cmd)
{
	PWRMGT_SOFTWARE_RESET_CONTROL_REG |=
		(1UL << PWRMGT_GLOBAL_SOFTWARE_RESET_MASK_BIT_INDEX);
	PWRMGT_SOFTWARE_RESET_CONTROL_REG &=
		~(1UL << PWRMGT_GLOBAL_SOFTWARE_RESET_MASK_BIT_INDEX);
}

#endif /*  _CNS21XX_SYSTEM_H */
