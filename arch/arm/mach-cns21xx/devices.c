/*
 *  Copyright (c) 2008 Cavium Networks
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, Version 2, as
 *  published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/serial_8250.h>
#include <linux/fa_wdt.h>

#include <mach/irqs.h>
#include <mach/hardware.h>
#include <mach/cns21xx.h>
#include <mach/cns21xx_misc.h>

#define CNS21XX_UART_CLOCK	24000000

#define CNS21XX_UART_FLAGS (UPF_SKIP_TEST | UPF_FIXED_TYPE | UPF_NO_TXEN_TEST)

static struct plat_serial8250_port cns21xx_uart0_data[] = {
	{
		.mapbase	= CNS21XX_UART0_BASE,
		.membase	= (void *) CNS21XX_UART0_BASE_VIRT,
		.irq		= CNS21XX_IRQ_UART0,
		.uartclk	= CNS21XX_UART_CLOCK,
		.regshift	= 2,
		.iotype		= UPIO_MEM,
		.type		= PORT_16550A,
		.flags		= CNS21XX_UART_FLAGS,
	}, {
		/* terminating entry */
	},
};

static struct platform_device cns21xx_uart0_device = {
	.name	= "serial8250",
	.id	= PLAT8250_DEV_PLATFORM,
	.dev	= {
		.platform_data = cns21xx_uart0_data,
	},
};

int __init cns21xx_register_uart0(void)
{
	return platform_device_register(&cns21xx_uart0_device);
}

static struct plat_serial8250_port cns21xx_uart1_data[] = {
	{
		.mapbase	= CNS21XX_UART1_BASE,
		.membase	= (void *) CNS21XX_UART1_BASE_VIRT,
		.irq		= CNS21XX_IRQ_UART1,
		.uartclk	= CNS21XX_UART_CLOCK,
		.regshift	= 2,
		.iotype		= UPIO_MEM,
		.type		= PORT_16550A,
		.flags		= CNS21XX_UART_FLAGS,
	}, {
		/* terminating entry */
	},
};

static struct platform_device cns21xx_uart1_device = {
	.name	= "serial8250",
	.id	= PLAT8250_DEV_PLATFORM1,
	.dev	= {
		.platform_data = cns21xx_uart1_data,
	},
};

int __init cns21xx_register_uart1(void)
{
	HAL_MISC_ENABLE_UART1_PINS();
	return platform_device_register(&cns21xx_uart1_device);
}

static struct resource cns21xx_wdt_resources[] = {
	{
		.start	= CNS21XX_WDT_BASE,
		.end	= CNS21XX_WDT_BASE + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	},
};

#define CNS21XX_WDT_CLOCK	10	/* 10 Hz */

static struct fa_wdt_platform_data cns21xx_wdt_data = {
	.clock	= CNS21XX_WDT_CLOCK,
};

static struct platform_device cns21xx_wdt_device = {
	.name			= "fa-wdt",
	.id			= -1,
	.resource 		= cns21xx_wdt_resources,
	.num_resources		= ARRAY_SIZE(cns21xx_wdt_resources),
	.dev = {
		.platform_data	= &cns21xx_wdt_data,
	},
};

int __init cns21xx_register_wdt(void)
{
	return platform_device_register(&cns21xx_wdt_device);
}
