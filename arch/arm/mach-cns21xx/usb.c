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
#include <mach/cns21xx-regs.h>
#include <mach/irqs.h>

/*
 * OHCI 
 */

static u64 cns21xx_usb_ohci_dmamask = 0xffffffffUL;

static struct resource cns21xx_usb11_resources[] = {
	[0] = {
		.start	= CNS21XX_USB11_OPERATION_BASE,
		.end	= CNS21XX_USB11_OPERATION_BASE + SZ_1M - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= CNS21XX_IRQ_USB11,
		.end	= CNS21XX_IRQ_USB11,
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
		.start	= CNS21XX_USB20_OPERATION_BASE,
		.end	= CNS21XX_USB20_OPERATION_BASE + SZ_1M - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= CNS21XX_IRQ_USB20,
		.end	= CNS21XX_IRQ_USB20,
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

void __init cns21xx_register_usb(void)
{
	platform_device_register(&cns21xx_usb20_device);
	platform_device_register(&cns21xx_usb11_device);
}
