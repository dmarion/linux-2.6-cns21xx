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

#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/leds.h>
#include <linux/gpio_keys.h>
#include <linux/input.h>

#include <asm/setup.h>
#include <mach/hardware.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>

#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include <mach/cns21xx-intc.h>
#include <mach/cns21xx-misc.h>
#include <mach/irqs.h>

#include "common.h"
#include "nic.h"

#define NSK330_GPIO_LED_LINK		1
#define NSK330_GPIO_LED_USB1		16
#define NSK330_GPIO_LED_USB2		17
#define NSK330_GPIO_LED_ETH_GREEN	22
#define NSK330_GPIO_LED_ETH_ORANGE	23

#define NSK330_GPIO_BTN_RESET		13
#define NSK330_GPIO_BTN_USB1		14
#define NSK330_GPIO_BTN_USB2		15

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

/*
 * GPIO LED
 */

static struct gpio_led nsk330_gpio_leds[] = {
	{
		.name		= "ns-k330:red:link",
		.gpio		= NSK330_GPIO_LED_LINK,
		.active_low	= 1,
	}, {
		.name		= "ns-k330:green:usb1",
		.gpio		= NSK330_GPIO_LED_USB1,
		.active_low	= 1,
	}, {
		.name		= "ns-k330:green:usb2",
		.gpio		= NSK330_GPIO_LED_USB2,
		.active_low	= 1,
	}, {
		.name		= "ns-k330:green:eth",
		.gpio		= NSK330_GPIO_LED_ETH_GREEN,
		.active_low	= 1,
	}, {
		.name		= "ns-k330:orange:eth",
		.gpio		= NSK330_GPIO_LED_ETH_ORANGE,
		.active_low	= 1,
	}
};

static struct gpio_led_platform_data nsk330_gpio_leds_data = {
	.num_leds		= ARRAY_SIZE(nsk330_gpio_leds),
	.leds			= nsk330_gpio_leds,
};

static struct platform_device nsk330_gpio_leds_device = {
	.name			= "leds-gpio",
	.id			= -1,
	.dev.platform_data	= &nsk330_gpio_leds_data,
};


/*
 * GPIO Keys
 */

static struct gpio_keys_button nsk330_gpio_keys[] = {
	{
		.code		= KEY_RESTART,
		.gpio		= NSK330_GPIO_BTN_RESET,
		.desc		= "Reset Button",
		.active_low	= 1,
	},
	{
		.code		= BTN_0,
		.gpio		= NSK330_GPIO_BTN_USB1,
		.desc		= "USB1 Button",
		.active_low	= 1,
	},
	{
		.code		= BTN_1,
		.gpio		= NSK330_GPIO_BTN_USB2,
		.desc		= "USB2 Button",
		.active_low	= 0,
	},
};

static struct gpio_keys_platform_data nsk330_gpio_keys_data = {
	.buttons	= nsk330_gpio_keys,
	.nbuttons	= ARRAY_SIZE(nsk330_gpio_keys),
};

static struct platform_device nsk330_gpio_keys_device = {
	.name		= "gpio-keys",
	.id		= -1,
	.num_resources	= 0,
	.dev		= {
		.platform_data	= &nsk330_gpio_keys_data,
	},
};

/*
 * Functions
 */

static void __init nsk330_fixup(struct machine_desc *desc,
	struct tag *tags, char **cmdline, struct meminfo *mi)
{
        mi->nr_banks = 1;
	mi->bank[0].start = 0;
	mi->bank[0].size = SZ_32M;
}

void __init nsk330_init(void)
{
	cns21xx_gpio_init();
	
	HAL_MISC_DISABLE_LED012_PINS();
	HAL_MISC_DISABLE_I2C_PINS();
	HAL_MISC_DISABLE_I2S_PINS();

#ifndef ARCH_CNS21XX_EARLY_CONSOLE
	cns21xx_register_uart0();
#endif
	cns21xx_register_usb();
	
	cns21xx_register_spi_master(-1, nsk330_spi_board_info,
				    ARRAY_SIZE(nsk330_spi_board_info));

	cns21xx_gec_data.phy_type = CNS21XX_GEC_PHY_TYPE_INTERNAL;
	cns21xx_register_gec();
	
	platform_device_register(&nsk330_gpio_leds_device);
	platform_device_register(&nsk330_gpio_keys_device);
}

void __init nsk330_map_io(void)
{
	cns21xx_map_io();
#ifdef ARCH_CNS21XX_EARLY_CONSOLE
	cns21xx_register_uart0_early();
#endif
}

/*
 *  Machine definition
 */

MACHINE_START(NSK330, "NS-K330 NAS (CNS2132)")
	.fixup		= nsk330_fixup,
	.map_io		= nsk330_map_io,
	.init_irq	= cns21xx_init_irq,
	.timer		= &cns21xx_timer,
	.boot_params	= 0x0100,
	.init_machine	= nsk330_init,
MACHINE_END

