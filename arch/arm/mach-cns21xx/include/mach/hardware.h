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

#ifndef __ASM_ARCH_HARDWARE_H__
#define __ASM_ARCH_HARDWARE_H__

//#define SYS_CLK			(87500000) // 87.5MHz, 13MHz if FPGA
//#define UART_CLK		(24000000) // 24MHz, 13Mhz if FPGA
//#define AHB_CLK			(SYS_CLK)
//#define APB_CLK			(AHB_CLK >> 1) // (AHB_CLK) if FPGA
//#define TIMER_COUNTER		(APB_CLK / HZ)

//#define DEBUG_CONSOLE		(0)

//#define FLASH_BASE_ADDR		SYSPA_FLASH_SRAM_BANK0_BASE_ADDR
//#define FLASH_SIZE		0x800000

#define PCIBIOS_MIN_IO		0x00000000
#define PCIBIOS_MIN_MEM		0x00000000

#define pcibios_assign_all_busses()	0

#ifndef __ASSEMBLY__
extern unsigned long cns21xx_get_pll_freq(void);
extern unsigned long cns21xx_get_cpu_freq(void);
extern unsigned long cns21xx_get_ahb_freq(void);
extern unsigned long cns21xx_get_apb_freq(void);                                                                                                  
#endif


#endif /* __ASM_ARCH_HARDWARE_H__ */
