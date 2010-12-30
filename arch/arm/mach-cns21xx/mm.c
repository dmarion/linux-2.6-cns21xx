/*
 *  Copyright (c) 2008 Cavium Networks
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, Version 2, as
 *  published by the Free Software Foundation.
 */

#include <linux/mm.h>
#include <linux/init.h>

#include <asm/mach/map.h>

#include <mach/hardware.h>
#include <mach/cns21xx.h>

#include "common.h"

/*
 * Standard IO mapping
 */
static struct map_desc cns21xx_io_desc[] __initdata = {
	{
		.virtual	= CNS21XX_FLASH_BANK0_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_FLASH_BANK0_BASE),
		.length		= SZ_256M,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_IDE_DEVICE_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_IDE_DEVICE_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_GDMAC_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_GDMAC_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_NIC_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_NIC_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_SPI_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_SPI_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_PCM_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_PCM_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_I2C_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_I2C_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_I2S_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_I2S_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_DDRC_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_DDRC_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_SMC_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_SMC_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_IDE_CTRL_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_IDE_CTRL_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_MISC_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_MISC_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_CPM_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_CPM_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_UART0_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_UART0_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_UART1_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_UART1_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_TIMER_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_TIMER_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_WDT_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_WDT_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_RTC_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_RTC_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_GPIOA_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_GPIOA_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_GPIOB_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_GPIOB_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_PCI_CFGDATA_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_PCI_CFGDATA_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_PCI_CFGADDR_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_PCI_CFGADDR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_OHCI_CONFIG_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_OHCI_CONFIG_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_OHCI_CTRL_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_OHCI_CTRL_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_EHCI_CONFIG_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_EHCI_CONFIG_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_EHCI_CTRL_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_EHCI_CTRL_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_USB_DEVICE_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_USB_DEVICE_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_INTC_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_INTC_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}
};

void __init cns21xx_map_io(void)
{
	iotable_init(cns21xx_io_desc, ARRAY_SIZE(cns21xx_io_desc));
}
