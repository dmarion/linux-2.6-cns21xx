/*
 *  Copyright (c) 2008 Cavium Networks
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License, Version 2, as
 *  published by the Free Software Foundation.
 */

#ifndef _CNS21XX_IRQS_H
#define _CNS21XX_IRQS_H

#define CNS21XX_IRQ_TIMER1	0
#define CNS21XX_IRQ_TIMER2	1
#define CNS21XX_IRQ_CPM		2
#define CNS21XX_IRQ_WDT		3
#define CNS21XX_IRQ_GPIO	4
#define CNS21XX_IRQ_PCI_INTA	5
#define CNS21XX_IRQ_PCI_INTB	6
#define CNS21XX_IRQ_PCI_BROKEN	7
#define CNS21XX_IRQ_AHB2PCI	8
#define CNS21XX_IRQ_UART0	9
#define CNS21XX_IRQ_UART1	10
#define CNS21XX_IRQ_GDMAC_TC	11
#define CNS21XX_IRQ_GDMAC_ERR	12
#define CNS21XX_IRQ_PCMCIA	13
#define CNS21XX_IRQ_RTC		14
#define CNS21XX_IRQ_PCM		15
#define CNS21XX_IRQ_USB_DEVICE	16
#define CNS21XX_IRQ_IDE		17
#define CNS21XX_IRQ_NIC_STATUS	18
#define CNS21XX_IRQ_NIC_TXTC	19
#define CNS21XX_IRQ_NIC_RXRC	20
#define CNS21XX_IRQ_NIC_TXQE	21
#define CNS21XX_IRQ_NIC_RXQF	22
#define CNS21XX_IRQ_OHCI	23
#define CNS21XX_IRQ_EHCI	24
#define CNS21XX_IRQ_I2S		25
#define CNS21XX_IRQ_SPI		26
#define CNS21XX_IRQ_I2C		27
#define CNS21XX_IRQ_USB_VBUS	28
#define CNS21XX_IRQ_EXT_29	29
#define CNS21XX_IRQ_EXT_30	30
#define CNS21XX_IRQ_HSDMAC	31

#define CNS21XX_GPIO_IRQ_BASE	32

#define CNS21XX_NR_INTC_IRQS	32
#define CNS21XX_NR_GPIO_IRQS	64

#define NR_IRQS			96

#endif /* _CNS21XX_IRQS_H */
