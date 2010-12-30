/*
 *  Copyright (c) 2008 Cavium Networks
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, Version 2, as
 *  published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/io.h>
#include <linux/ioport.h>

#include <mach/cns21xx.h>

#define	INTC_INTERRUPT_RAW_STATUS_REG		0x000
#define	INTC_EDGE_INTERRUPT_SOURCE_CLEAR_REG	0x004
#define	INTC_INTERRUPT_MASK_REG			0x008
#define	INTC_INTERRUPT_MASK_CLEAR_REG		0x00c
#define	INTC_INTERRUPT_TRIGGER_MODE_REG		0x010
#define	INTC_INTERRUPT_TRIGGER_LEVEL_REG	0x014
#define	INTC_FIQ_SELECT_REG			0x018
#define	INTC_IRQ_STATUS_REG			0x01c
#define	INTC_FIQ_STATUS_REG			0x020
#define	INTC_SOFTWARE_INTERRUPT_REG		0x024
#define	INTC_SOFTWARE_INTERRUPT_CLEAR_REG	0x028
#define	INTC_SOFTWARE_PRIORITY_MASK_REG		0x02c
#define	INTC_POWER_MANAGEMENT_INTERRUPT_REG	0x034

#define	INTC_VECTOR_ADDRESS_REG(_x)		((_x) + 0x040)
#define	INTC_PRIORITY_REG(_x)			((_x) + 0x0c0)
#define	INTC_IRQ_VECTOR_ADDRESS_REG		0x140
#define	INTC_VECTOR_INTERRUPT_ENABLE_REG	0x144

#define INTC_SIZE				0x148

static unsigned int cns21xx_irq_types[CNS21XX_NR_INTC_IRQS] = {
	[CNS21XX_IRQ_TIMER1]	= IRQ_TYPE_EDGE_RISING,
	[CNS21XX_IRQ_TIMER2]	= IRQ_TYPE_EDGE_RISING,
	[CNS21XX_IRQ_CPM]	= IRQ_TYPE_EDGE_FALLING,
	[CNS21XX_IRQ_WDT]	= IRQ_TYPE_EDGE_RISING,
	[CNS21XX_IRQ_GPIO]	= IRQ_TYPE_NONE,
	[CNS21XX_IRQ_PCI_INTA]	= IRQ_TYPE_LEVEL_LOW,
	[CNS21XX_IRQ_PCI_INTB]	= IRQ_TYPE_LEVEL_LOW,
	[CNS21XX_IRQ_PCI_BROKEN] = IRQ_TYPE_LEVEL_HIGH,
	[CNS21XX_IRQ_AHB2PCI]	= IRQ_TYPE_LEVEL_HIGH,
	[CNS21XX_IRQ_UART0]	= IRQ_TYPE_LEVEL_HIGH,
	[CNS21XX_IRQ_UART1]	= IRQ_TYPE_LEVEL_HIGH,
	[CNS21XX_IRQ_GDMAC_TC]	= IRQ_TYPE_LEVEL_HIGH,
	[CNS21XX_IRQ_GDMAC_ERR]	= IRQ_TYPE_LEVEL_HIGH,
	[CNS21XX_IRQ_PCMCIA]	= IRQ_TYPE_NONE,
	[CNS21XX_IRQ_RTC]	= IRQ_TYPE_LEVEL_HIGH,
	[CNS21XX_IRQ_PCM]	= IRQ_TYPE_LEVEL_LOW,
	[CNS21XX_IRQ_USB_DEVICE] = IRQ_TYPE_LEVEL_LOW,
	[CNS21XX_IRQ_IDE]	= IRQ_TYPE_LEVEL_HIGH,
	[CNS21XX_IRQ_NIC_STATUS] = IRQ_TYPE_LEVEL_HIGH,
	[CNS21XX_IRQ_NIC_TXTC]	= IRQ_TYPE_EDGE_RISING,
	[CNS21XX_IRQ_NIC_RXRC]	= IRQ_TYPE_EDGE_RISING,
	[CNS21XX_IRQ_NIC_TXQE]	= IRQ_TYPE_EDGE_RISING,
	[CNS21XX_IRQ_NIC_RXQF]	= IRQ_TYPE_EDGE_RISING,
	[CNS21XX_IRQ_OHCI]	= IRQ_TYPE_LEVEL_LOW,
	[CNS21XX_IRQ_EHCI]	= IRQ_TYPE_LEVEL_LOW,
	[CNS21XX_IRQ_I2S]	= IRQ_TYPE_LEVEL_LOW,
	[CNS21XX_IRQ_SPI]	= IRQ_TYPE_LEVEL_LOW,
	[CNS21XX_IRQ_I2C]	= IRQ_TYPE_LEVEL_LOW,
	[CNS21XX_IRQ_USB_VBUS]	= IRQ_TYPE_EDGE_RISING,
	[CNS21XX_IRQ_EXT_29]	= IRQ_TYPE_NONE,
	[CNS21XX_IRQ_EXT_30]	= IRQ_TYPE_NONE,
	[CNS21XX_IRQ_HSDMAC]	= IRQ_TYPE_EDGE_RISING,
};

static void __iomem *cns21xx_intc_base;

static inline void cns21xx_intc_writel(u32 val, unsigned int reg)
{
	__raw_writel(val, cns21xx_intc_base + reg);
}

static inline u32 cns21xx_intc_readl(unsigned int reg)
{
	return __raw_readl(cns21xx_intc_base + reg);
}

static void cns21xx_irq_ack(unsigned int irq)
{
	cns21xx_intc_writel(1 << irq, INTC_EDGE_INTERRUPT_SOURCE_CLEAR_REG);
}

static void cns21xx_irq_mask(unsigned int irq)
{
	cns21xx_intc_writel(1 << irq, INTC_INTERRUPT_MASK_REG);
}

static void cns21xx_irq_unmask(unsigned int irq)
{
	cns21xx_intc_writel(1 << irq, INTC_INTERRUPT_MASK_CLEAR_REG);
}

static struct irq_chip cns21xx_irq_chip = {
	.name	= "INTC",
	.ack	= cns21xx_irq_ack,
	.mask	= cns21xx_irq_mask,
	.unmask	= cns21xx_irq_unmask,
};

static struct resource cns21xx_intc_resource = {
	.name	= "INTC",
	.flags	= IORESOURCE_MEM,
	.start	= CNS21XX_INTC_BASE,
	.end	= CNS21XX_INTC_BASE + INTC_SIZE - 1,
};

void __init cns21xx_init_irq(void)
{
	unsigned int mode = 0;
	unsigned int level = 0;
	int i;

	/*
	 * Disable arch_idle() by default since it is buggy
	 * For more info see arch/arm/mach-cns21xx/include/mach/system.h
	 */
	disable_hlt();

	request_resource(&iomem_resource, &cns21xx_intc_resource);
	cns21xx_intc_base = ioremap(cns21xx_intc_resource.start, INTC_SIZE);

	cns21xx_intc_writel(0xffffffff, INTC_INTERRUPT_MASK_REG);
	cns21xx_intc_writel(0xffffffff, INTC_EDGE_INTERRUPT_SOURCE_CLEAR_REG);
	cns21xx_intc_writel(0xffffffff, INTC_SOFTWARE_INTERRUPT_CLEAR_REG);
	cns21xx_intc_writel(0, INTC_SOFTWARE_PRIORITY_MASK_REG);
	cns21xx_intc_writel(0, INTC_FIQ_SELECT_REG);
	cns21xx_intc_writel(0, INTC_VECTOR_INTERRUPT_ENABLE_REG);

	for (i = 0; i < ARRAY_SIZE(cns21xx_irq_types);  i++) {
		irq_flow_handler_t handler;

		switch (cns21xx_irq_types[i]) {
		case IRQ_TYPE_EDGE_RISING:
			handler = handle_edge_irq;
			mode |= (1 << i);
			break;

		case IRQ_TYPE_EDGE_FALLING:
			handler = handle_edge_irq;
			mode |= (1 << i);
			level |= (1 << i);
			break;

		case IRQ_TYPE_LEVEL_LOW:
			handler = handle_level_irq;
			level |= (1 << i);
			break;

		case IRQ_TYPE_LEVEL_HIGH:
		case IRQ_TYPE_NONE:
			handler = handle_level_irq;
			break;

		default:
			BUG();
			break;
		}

		set_irq_chip(i, &cns21xx_irq_chip);
		set_irq_handler(i, handler);
		set_irq_flags(i, IRQF_VALID | IRQF_PROBE);
	}

	cns21xx_intc_writel(mode, INTC_INTERRUPT_TRIGGER_MODE_REG);
	cns21xx_intc_writel(level, INTC_INTERRUPT_TRIGGER_LEVEL_REG);
}
