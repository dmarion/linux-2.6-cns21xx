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
#include <linux/major.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/serial.h>
#include <linux/tty.h>
#include <linux/serial_8250.h>


#include <asm/io.h>
#include <asm/pgtable.h>
#include <asm/page.h>
#include <asm/mach/map.h>
#include <asm/setup.h>
#include <asm/system.h>
#include <asm/memory.h>
#include <mach/hardware.h>
#include <mach/cns21xx_irq.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>

#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include <mach/cns21xx.h>

#if 1 // on ASIC
#define STR21XX_UART_XTAL 24000000
#else // on FPGA
#define STR21XX_UART_XTAL 13000000
#endif

static struct uart_port cns21xx_serial_ports[] = {
	{
		.membase	= (void *)(SYSVA_UART0_BASE_ADDR),
		.mapbase	= (SYSPA_UART0_BASE_ADDR),
		.irq		= IRQ_UART0,
		.flags		= UPF_BOOT_AUTOCONF | UPF_SKIP_TEST | UPF_FIXED_TYPE,
		.iotype		= UPIO_MEM,
		.regshift	= 2,
		.uartclk	= STR21XX_UART_XTAL,
		.line		= 0,
		.type		= PORT_16550A,
		.fifosize	= 16
	} , {
		.membase	= (void *)(SYSVA_UART1_BASE_ADDR),
		.mapbase	= (SYSPA_UART1_BASE_ADDR),
		.irq		= IRQ_UART1,
		.flags		= UPF_BOOT_AUTOCONF | UPF_SKIP_TEST | UPF_FIXED_TYPE,
		.iotype		= UPIO_MEM,
		.regshift	= 2,
		.uartclk	= STR21XX_UART_XTAL,
		.line		= 1,
		.type		= PORT_16550A,
		.fifosize	= 16
	}
};

/*
 * OHCI 
 */

static u64 cns21xx_usb_ohci_dmamask = 0xffffffffUL;

static struct resource cns21xx_usb11_resources[] = {
	[0] = {
		.start	= SYSPA_USB11_CONFIG_BASE_ADDR,
		.end	= SYSPA_USB11_CONFIG_BASE_ADDR + SZ_1M - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= IRQ_USB11,
		.end	= IRQ_USB11,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device cns21xx_usb11_device = {
	.name		= "cns21xx-ohci",
	.id		= -1,
	.dev = {
		.dma_mask		= &cns21xx_usb_ohci_dmamask,
		.coherent_dma_mask	= 0xffffffffUL,
	},
	.resource	= cns21xx_usb11_resources,
	.num_resources	= ARRAY_SIZE(cns21xx_usb11_resources),
};

/*
 * EHCI 
 */

static u64 cns21xx_usb_ehci_dmamask = 0xffffffffUL;

static struct resource cns21xx_usb20_resources[] = {
	[0] = {
		.start	= SYSPA_USB20_CONFIG_BASE_ADDR,
		.end	= SYSPA_USB20_CONFIG_BASE_ADDR + SZ_1M - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= IRQ_USB20,
		.end	= IRQ_USB20,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device cns21xx_usb20_device = {
	.name		= "cns21xx-ehci",
	.id		= -1,
	.dev		= {
		.dma_mask		= &cns21xx_usb_ehci_dmamask,
		.coherent_dma_mask	= 0xffffffffUL,
	},
	.resource	= cns21xx_usb20_resources,
	.num_resources	= ARRAY_SIZE(cns21xx_usb20_resources),
};

/*
 * SPI
 */

static u64 cns21xx_usb_spi_dmamask = 0xffffffffUL;

static struct resource cns21xx_spi_resources[] = {
	[0] = {
		.start	= SYSPA_SPI_BASE_ADDR,
		.end	= SYSPA_SPI_BASE_ADDR + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= IRQ_SPI,
		.end	= IRQ_SPI,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device cns21xx_spi_device = {
	.name		= "cns21xx-spi",
	.id		= -1,
	.dev		= {
		.dma_mask		= &cns21xx_usb_spi_dmamask,
		.coherent_dma_mask	= 0xffffffffUL,
	},
	.resource	= cns21xx_spi_resources,
	.num_resources	= ARRAY_SIZE(cns21xx_spi_resources),
};

/*
 *  SPI MTD Flash
 */

static struct mtd_partition nsk330_spi_flash_partitions[] = {
	{
		.name =		"BOOT",
		.offset =	0x00000000,
		.size =		256 * SZ_1K,
	},{
		.name =		"CFG",
		.offset =	0x00040000,
		.size =		128 * SZ_1K,
	},{
		.name =		"KERNEL",
		.offset =	0x00060000,
		.size =		960 * SZ_1K,
	},{
		.name =		"INITRD",
		.offset =	0x00150000,
		.size =		2688 * SZ_1K,
	},{
		.name =		"ALL",
		.offset =	0x00000000,
		.size =		4032 * SZ_1K,
	}
};


static struct flash_platform_data nsk330_spi_flash_data = {
	.name = "m25p80",
	.parts = nsk330_spi_flash_partitions,
	.nr_parts = ARRAY_SIZE(nsk330_spi_flash_partitions),
	.type = "m25p32",
};

static struct spi_board_info nsk330_spi_board_info[] = {
        {
		.modalias	= "m25p80",
		.chip_select	= 0,
		.max_speed_hz	= 25 * 1000 * 1000,
		.bus_num	= 1,
		.platform_data = &nsk330_spi_flash_data,
        },
};

static struct platform_device *cns21xx_devices[] __initdata = {
	&cns21xx_usb11_device,
	&cns21xx_usb20_device,
	&cns21xx_spi_device
};

static void __init cns21xx_fixup(struct machine_desc *desc,
	struct tag *tags, char **cmdline, struct meminfo *mi)
{
        mi->nr_banks = 1;
	mi->bank[0].start = CONFIG_SYSTEM_DRAM_BASE;
	mi->bank[0].size = CONFIG_SYSTEM_DRAM_SIZE << 20;
	mi->bank[0].highmem = 0;
}

/* ######################################################################### */
#ifdef CONFIG_CPU_ISPAD_ENABLE 
extern unsigned long __ispad_begin; 
#endif

void __init cns21xx_init(void)
{
	//platform_add_devices(device, size);
	platform_add_devices(cns21xx_devices, ARRAY_SIZE(cns21xx_devices));
	spi_register_board_info(nsk330_spi_board_info, ARRAY_SIZE(nsk330_spi_board_info));
}

void __init nsk330_map_io(void)
{
	cns21xx_map_io();
	early_serial_setup(&cns21xx_serial_ports[0]);
	early_serial_setup(&cns21xx_serial_ports[1]);
}

MACHINE_START(NSK330, "NS-K330 NAS (CNS2132)")
	.fixup		= cns21xx_fixup,
	.map_io		= nsk330_map_io,
	.init_irq	= cns21xx_init_irq,
	.timer		= &cns21xx_timer,
	.boot_params	= 0x0100,
	.init_machine	= cns21xx_init,
MACHINE_END

