/*
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, Version 2, as
 *  published by the Free Software Foundation.
 */

#ifndef _CNS21XX_GEC_PLATFORM_H
#define _CNS21XX_GEC_PLATFORM_H

#define CNS21XX_GEC_STATUS_IRQ_NAME	"status"
#define CNS21XX_GEC_RXRC_IRQ_NAME	"rxrc"
#define CNS21XX_GEC_RXQF_IRQ_NAME	"rxqf"
#define CNS21XX_GEC_TXTC_IRQ_NAME	"txtc"
#define CNS21XX_GEC_TXQE_IRQ_NAME	"txqe"

enum cns21xx_gec_phy_type {
	CNS21XX_GEC_PHY_TYPE_INTERNAL = 0,
	CNS21XX_GEC_PHY_TYPE_VSC8601,
	CNS21XX_GEC_PHY_TYPE_IP101A,
	CNS21XX_GEC_PHY_TYPE_IP1001,
};

struct cns21xx_gec_plat_data {
	u8				*mac_addr;
	enum cns21xx_gec_phy_type	phy_type;
	u8				phy_addr;
};

#endif /* _CNS21XX_GEC_PLATFORM_H */
