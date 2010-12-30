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
#include <mach/cns21xx.h>


static struct map_desc cns21xx_std_desc[] __initdata = {
	{
		.virtual	= SYSVA_FLASH_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_FLASH_SRAM_BANK0_BASE_ADDR),
		.length		= SZ_8M,
		.type		= MT_DEVICE
	}, 	{
		.virtual	= SYSVA_IDE_DEVICE_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_IDE_DEVICE_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_GDMAC_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_GDMAC_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_NIC_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_NIC_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_SPI_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_SPI_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_PCM_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_PCM_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_I2C_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_I2C_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_I2S_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_I2S_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_DDRC_SDRC_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_DDRC_SDRC_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_SMC_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_SMC_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_IDE_CONTROLLER_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_IDE_CONTROLLER_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_MISC_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_MISC_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_POWER_MANAGEMENT_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_POWER_MANAGEMENT_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_UART0_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_UART0_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_UART1_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_UART1_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_TIMER_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_TIMER_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_WATCHDOG_TIMER_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_WATCHDOG_TIMER_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_RTC_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_RTC_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_GPIOA_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_GPIOA_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_GPIOB_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_GPIOB_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_PCI_BRIDGE_CONFIG_DATA_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_PCI_BRIDGE_CONFIG_DATA_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_PCI_BRIDGE_CONFIG_ADDR_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_PCI_BRIDGE_CONFIG_ADDR_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_USB11_CONFIG_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_USB11_CONFIG_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_USB11_OPERATION_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_USB11_OPERATION_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_USB20_CONFIG_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_USB20_CONFIG_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_USB20_OPERATION_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_USB20_OPERATION_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_USB20_DEVICE_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_USB20_DEVICE_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}, {
		.virtual	= SYSVA_VIC_BASE_ADDR,
		.pfn		= __phys_to_pfn(SYSPA_VIC_BASE_ADDR),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	}
};

void __init cns21xx_map_io(void)
{
	iotable_init(cns21xx_std_desc, ARRAY_SIZE(cns21xx_std_desc));
	//cns21xx_register_map_desc(cns21xx_std_desc, ARRAY_SIZE(cns21xx_std_desc));
}