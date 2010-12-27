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

#include <mach/cns21xx.h>

#if 1 // on ASIC
#define STR21XX_UART_XTAL 24000000
#else // on FPGA
#define STR21XX_UART_XTAL 13000000
#endif

#define EARLY_REGISTER_CONSOLE
#define DEVICE_REGISTER_MULTIPLE

#ifdef EARLY_REGISTER_CONSOLE
static struct uart_port cns21xx_serial_ports[] = {
	{
		.membase	= (void *)(SYSVA_UART0_BASE_ADDR),
		.mapbase	= (SYSPA_UART0_BASE_ADDR),
		.irq		= INTC_UART0_BIT_INDEX,
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
		.irq		= INTC_UART1_BIT_INDEX,
		.flags		= UPF_BOOT_AUTOCONF | UPF_SKIP_TEST | UPF_FIXED_TYPE,
		.iotype		= UPIO_MEM,
		.regshift	= 2,
		.uartclk	= STR21XX_UART_XTAL,
		.line		= 1,
		.type		= PORT_16550A,
		.fifosize	= 16
	}
};
#else
static struct resource cns21xx_uart0_resources[] = {
	[0] = {
		.start	= SYSPA_UART0_BASE_ADDR,
		.end	= SYSPA_UART0_BASE_ADDR + 0xff,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= INTC_UART0_BIT_INDEX,
		.end	= INTC_UART0_BIT_INDEX,
		.flags	= IORESOURCE_IRQ
	}
};

static struct resource cns21xx_uart1_resources[] = {
	[0] = {
		.start	= SYSPA_UART1_BASE_ADDR,
		.end	= SYSPA_UART1_BASE_ADDR + 0xff,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= INTC_UART1_BIT_INDEX,
		.end	= INTC_UART1_BIT_INDEX,
		.flags	= IORESOURCE_IRQ
	}
};

static struct plat_serial8250_port cns21xx_uart0_data[] = {
	{
		.membase	= (char*)(SYSVA_UART0_BASE_ADDR),
		.mapbase	= (SYSPA_UART0_BASE_ADDR),
		.irq		= INTC_UART0_BIT_INDEX,
		.uartclk	= STR21XX_UART_XTAL,
		.regshift	= 2,
		.iotype		= UPIO_MEM,
		.flags		= UPF_BOOT_AUTOCONF | UPF_SKIP_TEST,
	},
	{  },
};

static struct plat_serial8250_port cns21xx_uart1_data[] = {
	{
		.membase	= (char*)(SYSVA_UART1_BASE_ADDR),
		.mapbase	= (SYSPA_UART1_BASE_ADDR),
		.irq		= INTC_UART1_BIT_INDEX,
		.uartclk	= STR21XX_UART_XTAL,
		.regshift	= 2,
		.iotype		= UPIO_MEM,
		.flags		= UPF_BOOT_AUTOCONF | UPF_SKIP_TEST,
	},
	{  },
};

static struct platform_device cns21xx_uart0_device = {
	.name = "serial8250",
	.id = 0,
	.dev.platform_data = cns21xx_uart0_data,
	.num_resources = 2,
	.resource = cns21xx_uart0_resources,
};

static struct platform_device cns21xx_uart1_device = {
	.name = "serial8250",
	.id = 1,
	.dev.platform_data = cns21xx_uart1_data,
	.num_resources = 2,
	.resource = cns21xx_uart1_resources,
};
#endif


static u64 usb_dmamask = 0xffffffffULL;
static struct resource cns21xx_usb11_resources[] = {
	[0] = {
		.start	= SYSPA_USB11_CONFIG_BASE_ADDR,
		.end	= SYSPA_USB11_CONFIG_BASE_ADDR + SZ_1M - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= INTC_USB11_BIT_INDEX,
		.end	= INTC_USB11_BIT_INDEX,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device cns21xx_usb11_device = {
	.name		= "cns21xx-ohci",
	.id		= -1,
	.dev = {
		.dma_mask		= &usb_dmamask,
		.coherent_dma_mask	= 0xffffffff,
	},
	.resource	= cns21xx_usb11_resources,
	.num_resources	= ARRAY_SIZE(cns21xx_usb11_resources),
};

static struct resource cns21xx_usb20_resources[] = {
	[0] = {
		.start	= SYSPA_USB20_CONFIG_BASE_ADDR,
		.end	= SYSPA_USB20_CONFIG_BASE_ADDR + SZ_1M - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= INTC_USB20_BIT_INDEX,
		.end	= INTC_USB20_BIT_INDEX,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device cns21xx_usb20_device = {
	.name		= "cns21xx-ehci",
	.id		= -1,
	.dev		= {
		.dma_mask		= &usb_dmamask,
		.coherent_dma_mask	= 0xffffffff,
	},
	.resource	= cns21xx_usb20_resources,
	.num_resources	= ARRAY_SIZE(cns21xx_usb20_resources),
};

#ifdef DEVICE_REGISTER_MULTIPLE
static struct platform_device *cns21xx_devices[] __initdata = {
#ifndef EARLY_REGISTER_CONSOLE
	&cns21xx_uart0_device,
	&cns21xx_uart1_device,
#endif
	&cns21xx_usb11_device,
	&cns21xx_usb20_device
};
#endif

static void __init cns21xx_fixup(struct machine_desc *desc,
	struct tag *tags, char **cmdline, struct meminfo *mi)
{
	printk("DAMJAN: cns21xx_fixup start\n");
        mi->nr_banks = 1;
	mi->bank[0].start = CONFIG_SYSTEM_DRAM_BASE;
	mi->bank[0].size = CONFIG_SYSTEM_DRAM_SIZE << 20;
	mi->bank[0].highmem = 0;
	//
	printk("DAMJAN: cns21xx_fixup start\n");
}

/* ######################################################################### */
#ifdef CONFIG_CPU_ISPAD_ENABLE 
extern unsigned long __ispad_begin; 
#endif

void __init cns21xx_init(void)
{
	printk("DAMJAN: cns21xx_init start\n");
#ifdef DEVICE_REGISTER_MULTIPLE
	platform_add_devices(cns21xx_devices, ARRAY_SIZE(cns21xx_devices));
#else // DEVICE_REGISTER_MULTIPLE
#ifndef EARLY_REGISTER_CONSOLE
	platform_device_register(&cns21xx_uart0_device);
	platform_device_register(&cns21xx_uart1_device);
#endif
	platform_device_register(&cns21xx_usb11_device);
	platform_device_register(&cns21xx_usb20_device);
#endif
	printk("DAMJAN: cns21xx_init end\n");
}

void __init nsk330_map_io(void)
{
	printk("DAMJAN: nsk330_map_io start\n");
	cns21xx_map_io();
#ifdef EARLY_REGISTER_CONSOLE
	early_serial_setup(&cns21xx_serial_ports[0]);
	early_serial_setup(&cns21xx_serial_ports[1]);
#endif
	printk("DAMJAN: nsk330_map_io end \n");
}

MACHINE_START(NSK330, "NS-K330 NAS (CNS2132)")
	.fixup		= cns21xx_fixup,
	.map_io		= nsk330_map_io,
	.init_irq	= cns21xx_init_irq,
	.timer		= &cns21xx_timer,
	.boot_params	= 0x0100,
	.init_machine	= cns21xx_init,
MACHINE_END

