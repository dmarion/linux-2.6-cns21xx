/*
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, Version 2, as
 *  published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/etherdevice.h>

#include <asm/sizes.h>
#include <mach/cns21xx-regs.h>
#include <mach/irqs.h>

#include "nic.h"

static u8 cns21xx_ethaddr[ETH_ALEN];
struct cns21xx_gec_plat_data cns21xx_gec_data;

static struct resource cns21xx_gec_resources[] = {
	{
		.start	= CNS21XX_NIC_BASE,
		.end	= CNS21XX_NIC_BASE + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	}, {
		.name	= CNS21XX_GEC_STATUS_IRQ_NAME,
		.start	= CNS21XX_IRQ_NIC_CTRL,
		.end	= CNS21XX_IRQ_NIC_CTRL,
		.flags	= IORESOURCE_IRQ,
	}, {
		.name	= CNS21XX_GEC_RXRC_IRQ_NAME,
		.start	= CNS21XX_IRQ_NIC_DMA_FNRC,
		.end	= CNS21XX_IRQ_NIC_DMA_FNRC,
		.flags	= IORESOURCE_IRQ,
	}, {
		.name	= CNS21XX_GEC_RXQF_IRQ_NAME,
		.start	= CNS21XX_IRQ_NIC_DMA_FNQF,
		.end	= CNS21XX_IRQ_NIC_DMA_FNQF,
		.flags	= IORESOURCE_IRQ,
	}, {
		.name	= CNS21XX_GEC_TXTC_IRQ_NAME,
		.start	= CNS21XX_IRQ_NIC_DMA_TNTC,
		.end	= CNS21XX_IRQ_NIC_DMA_TNTC,
		.flags	= IORESOURCE_IRQ,
	}, {
		.name	= CNS21XX_GEC_TXQE_IRQ_NAME,
		.start	= CNS21XX_IRQ_NIC_DMA_TNQE,
		.end	= CNS21XX_IRQ_NIC_DMA_TNQE,
		.flags	= IORESOURCE_IRQ,
	}
};

static u64 cns21xx_gec_dmamask = DMA_BIT_MASK(32);
static struct platform_device cns21xx_gec_device = {
	.name			= "cns21xx-gec",
	.id			= -1,
	.resource		= cns21xx_gec_resources,
	.num_resources		= ARRAY_SIZE(cns21xx_gec_resources),
	.dev = {
		.dma_mask		= &cns21xx_gec_dmamask,
		.coherent_dma_mask 	= DMA_BIT_MASK(32),
		.platform_data		= &cns21xx_gec_data,
	},
};

static int __init cns21xx_ethaddr_setup(char *str)
{
	int t;

	t = sscanf(str, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
		   &cns21xx_ethaddr[0], &cns21xx_ethaddr[1],
		   &cns21xx_ethaddr[2], &cns21xx_ethaddr[3],
		   &cns21xx_ethaddr[4], &cns21xx_ethaddr[5]);

	if (t != ETH_ALEN)
		pr_err("cns21xx: failed to parse mac address \"%s\"\n", str);

	return 1;
}

__setup("ethaddr=", cns21xx_ethaddr_setup);

__init int cns21xx_register_gec(void)
{
	if (cns21xx_gec_data.mac_addr == NULL)
		cns21xx_gec_data.mac_addr = cns21xx_ethaddr;

	if (!is_valid_ether_addr(cns21xx_gec_data.mac_addr)) {
		random_ether_addr(cns21xx_gec_data.mac_addr);
		pr_debug("cns21xx: using random MAC address \"%s\"\n",
			cns21xx_gec_data.mac_addr);
	}

	return platform_device_register(&cns21xx_gec_device);
}
