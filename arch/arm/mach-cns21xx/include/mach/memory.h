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

#ifndef __MACH_MEMORY_H
#define __MACH_MEMORY_H

#define CONFIG_SYSTEM_DRAM_BASE	0x00000000

#if defined(CONFIG_CNS21XX_DRAM_64M)
#define CONFIG_SYSTEM_DRAM_SIZE 64
#elif defined(CONFIG_CNS21XX_DRAM_32M)
#define CONFIG_SYSTEM_DRAM_SIZE 32
#elif defined(CONFIG_CNS21XX_DRAM_16M)
#define CONFIG_SYSTEM_DRAM_SIZE 16
#endif

#define MEM_SIZE		(CONFIG_SYSTEM_DRAM_SIZE)
#define END_MEM			(CONFIG_SYSTEM_DRAM_BASE + CONFIG_SYSTEM_DRAM_SIZE)
#define DMA_SIZE		0xffffffff
#define PHYS_OFFSET		(CONFIG_SYSTEM_DRAM_BASE) // physical start address of memory

#define __phys_to_bus(x)        ((x) + PHYS_OFFSET)
#define __bus_to_phys(x)        ((x) - PHYS_OFFSET)

#define __virt_to_bus(x)	((x) - PAGE_OFFSET)
#define __bus_to_virt(x)	((x) + PAGE_OFFSET)
#define __pfn_to_bus(p)         __phys_to_bus(__pfn_to_phys(p))
#define __bus_to_pfn(b)         __phys_to_pfn(__bus_to_phys(b))

#endif
