/*******************************************************************************
 *
 *  Copyright (c) 2008 Cavium Networks 
 * 
 *  This file is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License, Version 2, as 
 *  published by the Free Software Foundation. 
 *
 *  This file is distributed in the hope that it will be useful, 
 *  but AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or 
 *  NONINFRINGEMENT.  See the GNU General Public License for more details. 
 *
 *  You should have received a copy of the GNU General Public License 
 *  along with this file; if not, write to the Free Software 
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA or 
 *  visit http://www.gnu.org/licenses/. 
 *
 *  This file may also be available under a different license from Cavium. 
 *  Contact Cavium Networks for more information
 *
 ******************************************************************************/

#include <linux/mm.h>
#include <linux/init.h>

#include <asm/mach/map.h>

#include <mach/hardware.h>
#include <mach/cns21xx-regs.h>


static struct map_desc cns21xx_std_desc[] __initdata = {
	{
		.virtual	= CNS21XX_FLASH_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_FLASH_SRAM_BANK0_BASE),
		.length		= SZ_8M,
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
		.virtual	= CNS21XX_DDRC_SDRC_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_DDRC_SDRC_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_SMC_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_SMC_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_IDE_CONTROLLER_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_IDE_CONTROLLER_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_MISC_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_MISC_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_POWER_MANAGEMENT_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_POWER_MANAGEMENT_BASE),
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
		.virtual	= CNS21XX_WATCHDOG_TIMER_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_WATCHDOG_TIMER_BASE),
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
		.virtual	= CNS21XX_PCI_CONFIG_DATA_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_PCI_CONFIG_DATA_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_PCI_CONFIG_ADDR_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_PCI_CONFIG_ADDR_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_USB11_CONFIG_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_USB11_CONFIG_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_USB11_OPERATION_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_USB11_OPERATION_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_USB20_CONFIG_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_USB20_CONFIG_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_USB20_OPERATION_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_USB20_OPERATION_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_USB20_DEVICE_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_USB20_DEVICE_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= CNS21XX_VIC_BASE_VIRT,
		.pfn		= __phys_to_pfn(CNS21XX_VIC_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}
};

void __init cns21xx_map_io(void)
{
	iotable_init(cns21xx_std_desc, ARRAY_SIZE(cns21xx_std_desc));
	//cns21xx_register_map_desc(cns21xx_std_desc, ARRAY_SIZE(cns21xx_std_desc));
}
