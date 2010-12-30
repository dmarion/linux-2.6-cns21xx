/*
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, Version 2, as
 *  published by the Free Software Foundation.
 */

#ifndef _CNS21XX_DEV_GEC_H
#define _CNS21XX_DEV_GEC_H

#include <mach/cns21xx_gec_platform.h>

extern struct cns21xx_gec_plat_data cns21xx_gec_data;

__init int cns21xx_register_gec(void);

#endif /* _CNS21XX_DEV_GEC_H */
