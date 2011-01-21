/*******************************************************************************
 *
 *  Copyright (c) 2008 Cavium Networks 
 *  Copyright (c) 2011 Damjan Marion <damjan.marion@gmail.com>
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

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/serial.h>
#include <linux/tty.h>
#include <linux/serial_8250.h>

#include <mach/hardware.h>
#include <mach/cns21xx-uart.h>
#include <mach/cns21xx-misc.h>

#define CNS21XX_UART_XTAL 24000000


static struct uart_port cns21xx_uart0_port = {
		.membase	= (void *)(CNS21XX_UART0_BASE_VIRT),
		.mapbase	= (CNS21XX_UART0_BASE),
		.irq		= CNS21XX_IRQ_UART0,
		.flags		= UPF_BOOT_AUTOCONF | UPF_SKIP_TEST | UPF_FIXED_TYPE,
		.iotype		= UPIO_MEM,
		.regshift	= 2,
		.uartclk	= CNS21XX_UART_XTAL,
		.line		= 0,
		.type		= PORT_16550A,
		.fifosize	= 16
};


static struct plat_serial8250_port cns21xx_uart0_data[] = {
	{
		.membase	= (char*)(CNS21XX_UART0_BASE_VIRT),
		.mapbase	= (CNS21XX_UART0_BASE),
		.irq		= CNS21XX_IRQ_UART0,
		.uartclk	= CNS21XX_UART_XTAL,
		.regshift	= 2,
		.iotype		= UPIO_MEM,
		.type		= PORT_16550A,
		.flags		= UPF_SKIP_TEST | UPF_FIXED_TYPE | UPF_NO_TXEN_TEST,
	},
	{}
};

static struct plat_serial8250_port cns21xx_uart1_data[] = {
	{
		.membase	= (char*)(CNS21XX_UART1_BASE_VIRT),
		.mapbase	= (CNS21XX_UART1_BASE),
		.irq		= CNS21XX_IRQ_UART1,
		.uartclk	= CNS21XX_UART_XTAL,
		.regshift	= 2,
		.iotype		= UPIO_MEM,
		.flags		= UPF_SKIP_TEST | UPF_FIXED_TYPE | UPF_NO_TXEN_TEST,
	},
	{}
};

static struct platform_device cns21xx_uart0_device = {
	.name = "serial8250",
	.id = PLAT8250_DEV_PLATFORM,
	.dev.platform_data = cns21xx_uart0_data,
};

static struct platform_device cns21xx_uart1_device = {
	.name = "serial8250",
	.id = PLAT8250_DEV_PLATFORM,
	.dev.platform_data = cns21xx_uart1_data,
};

int __init cns21xx_register_uart0(void)
{
	return platform_device_register(&cns21xx_uart0_device);
}

int __init cns21xx_register_uart1(void)
{
	HAL_MISC_ENABLE_UART1_PINS();                                                                                                              
	return platform_device_register(&cns21xx_uart1_device);
}

int __init cns21xx_register_uart0_early(void)
{
	return early_serial_setup(&cns21xx_uart0_port);
}



