/*******************************************************************************
 *
 *  Copyright (c) 2008 Cavium Networks 
 *  Copyright (c) 2010 Damjan Marion
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

#ifndef	_MACH_CNS21XX_IRQS_H_
#define	_MACH_CNS21XX_IRQS_H_

#include <mach/hardware.h>

/*
 *  IRQ Mapping
 */
#define	CNS21XX_IRQ_TIMER1		0
#define	CNS21XX_IRQ_TIMER2		1
#define	CNS21XX_IRQ_FREQ_SCALING	2
#define CNS21XX_IRQ_WATCHDOG		3
#define CNS21XX_IRQ_GPIO		4
#define CNS21XX_IRQ_PCI_EXT0		5
#define CNS21XX_IRQ_PCI_EXT1		6
#define CNS21XX_IRQ_PCI_BROKEN		7
#define CNS21XX_IRQ_HOST_BRIDGE		8
#define CNS21XX_IRQ_UART0		9
#define CNS21XX_IRQ_UART1		10
#define CNS21XX_IRQ_DMA_TERM_COUNT	11
#define CNS21XX_IRQ_DMA_ERR		12
// 13 reserved
#define CNS21XX_IRQ_RTC			14
#define CNS21XX_IRQ_PCM_CTRL		15
#define CNS21XX_IRQ_IDE_HOST_CTRL	16
#define CNS21XX_IRQ_IDE_DEVICE		17
#define CNS21XX_IRQ_NIC_CTRL		18
#define CNS21XX_IRQ_NIC_DMA_TNTC	19
#define CNS21XX_IRQ_NIC_DMA_FNRC	20
#define CNS21XX_IRQ_NIC_DMA_TNQE	21
#define CNS21XX_IRQ_NIC_DMA_FNQF	22
#define CNS21XX_IRQ_USB11		23
#define CNS21XX_IRQ_USB20		24
#define CNS21XX_IRQ_I2S			25
#define CNS21XX_IRQ_SPI			26
#define CNS21XX_IRQ_TWI			27
#define CNS21XX_IRQ_VBUS_STATE_CHG	28
#define CNS21XX_IRQ_EXT_INT29		29
#define CNS21XX_IRQ_EXT_INT30		30
#define CNS21XX_IRQ_HSDMA		31

#define CNS21XX_GPIO_IRQ_BASE		32

#define NR_IRQS				96

#ifdef CONFIG_VIC_INTERRUPT
#define irq_finish(irq) do { INTC_IRQ_VECTOR_ADDRESS_REG = 0; } while (0)
#endif

#endif /* _MACH_CNS21XX_IRQS_H_ */
