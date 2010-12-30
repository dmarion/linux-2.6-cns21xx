/*
 *  Copyright (c) 2008 Cavium Networks
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, Version 2, as
 *  published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/irq.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>

#include <mach/cns21xx.h>

static u64 cns21xx_usb_dmamask = DMA_BIT_MASK(32);

static struct resource cns21xx_ohci_resources[] = {
	[0] = {
		.start	= CNS21XX_OHCI_CTRL_BASE,
		.end	= CNS21XX_OHCI_CTRL_BASE + SZ_1M - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= CNS21XX_IRQ_OHCI,
		.end	= CNS21XX_IRQ_OHCI,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device cns21xx_ohci_device = {
	.name		= "cns21xx-ohci",
	.id		= -1,
	.dev = {
		.dma_mask		= &cns21xx_usb_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
	},
	.resource	= cns21xx_ohci_resources,
	.num_resources	= ARRAY_SIZE(cns21xx_ohci_resources),
};

static struct resource cns21xx_ehci_resources[] = {
	[0] = {
		.start	= CNS21XX_EHCI_CTRL_BASE,
		.end	= CNS21XX_EHCI_CTRL_BASE + SZ_1M - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= CNS21XX_IRQ_EHCI,
		.end	= CNS21XX_IRQ_EHCI,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device cns21xx_ehci_device = {
	.name		= "cns21xx-ehci",
	.id		= -1,
	.dev		= {
		.dma_mask		= &cns21xx_usb_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
	},
	.resource	= cns21xx_ehci_resources,
	.num_resources	= ARRAY_SIZE(cns21xx_ehci_resources),
};

void __init cns21xx_register_usb(void)
{
	platform_device_register(&cns21xx_ehci_device);
	platform_device_register(&cns21xx_ohci_device);
}
