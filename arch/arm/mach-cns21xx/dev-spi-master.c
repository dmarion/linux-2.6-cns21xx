/*
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, Version 2, as
 *  published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/spi/spi.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>

#include <mach/hardware.h>
#include <mach/cns21xx.h>
#include <mach/cns21xx_misc.h>
#include <mach/cns21xx_powermgmt.h>
#include <mach/irqs.h>

#include "common.h"

static u64 spi_dmamask = DMA_BIT_MASK(32);
static struct resource cns21xx_spi_resources[] = {
	[0] = {
		.start	= CNS21XX_SPI_BASE,
		.end	= CNS21XX_SPI_BASE + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= CNS21XX_IRQ_SPI,
		.end	= CNS21XX_IRQ_SPI,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device cns21xx_spi_master_device = {
	.name		= "cns21xx-spi",
	.id		= -1,
	.dev		= {
		.dma_mask		= &spi_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
	},
	.resource	= cns21xx_spi_resources,
	.num_resources	= ARRAY_SIZE(cns21xx_spi_resources),
};

void __init cns21xx_register_spi_master(int id, struct spi_board_info *info,
					unsigned int n)
{
	unsigned int i;

	/* Enable SPI pins */
	HAL_MISC_ENABLE_SPIDR_PINS();
	HAL_MISC_ENABLE_SPICLK_PINS();
	for (i = 0; i < n; i++) {
		switch (info[i].chip_select) {
		case 0:
			HAL_MISC_ENABLE_SPICSN0_PINS();
			break;
		case 1:
			HAL_MISC_ENABLE_SPICSN1_PINS();
			break;
		case 2:
			HAL_MISC_ENABLE_SPICSN2_PINS();
			break;
		case 3:
			HAL_MISC_ENABLE_SPICSN3_PINS();
			break;
		}
	}

	/* Disable SPI serial flash access through 0x30000000 region */
	HAL_MISC_DISABLE_SPI_SERIAL_FLASH_BANK_ACCESS();

	/* Enable SPI clock */
	HAL_PWRMGT_ENABLE_SPI_CLOCK();

	cns21xx_spi_master_device.id = id;

	spi_register_board_info(info, n);
	platform_device_register(&cns21xx_spi_master_device);
}
