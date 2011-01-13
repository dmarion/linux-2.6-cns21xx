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

/*  Cavium patch
--- a/arch/arm/kernel/entry-armv.S
+++ b/arch/arm/kernel/entry-armv.S
@@ -31,6 +31,11 @@
        .macro  irq_handler
        get_irqnr_preamble r5, lr
+#ifdef CONFIG_VIC_INTERRUPT
+       get_irqnr_and_base r0, r6, r5, lr
+       mov     r1, sp
+       bl      asm_do_IRQ
+#else
 1:     get_irqnr_and_base r0, r6, r5, lr
        movne   r1, sp
        @
	@ routine called with r0 = irq number, r1 = struct pt_regs *
        @
        adrne   lr, BSYM(1b)
        bne     asm_do_IRQ
+#endif
*/


#include <mach/hardware.h>
#include <mach/cns21xx-regs.h>

	.macro	disable_fiq
	.endm

	.macro	get_irqnr_preamble, base, tmp
	.endm

	.macro	arch_ret_to_user, tmp1, tmp2
	.endm

/*
#ifdef CONFIG_VIC_INTERRUPT
	.macro	get_fiqnr_and_base, irqnr, irqstat, base, tmp
	ldr	\base, =(CNS21XX_VIC_BASE_VIRT + 0x140)
	ldr	\irqnr, [\base]
	.endm

	.macro	get_irqnr_and_base, irqnr, irqstat, base, tmp
	ldr	\base, =(CNS21XX_VIC_BASE_VIRT + 0x140)
	ldr	\irqnr, [\base]
	.endm
#else
	.macro	get_fiqnr_and_base, irqnr, irqstat, base, tmp
	ldr	\base, =(CNS21XX_VIC_BASE_VIRT + 0x20)
	ldr	\irqstat, [\base]
	mov	\irqnr, #0
9001:
	tst	\irqstat, #1
	bne	9002f
	add	\irqnr, \irqnr, #1
	mov	\irqstat, \irqstat, lsr #1
	cmp	\irqnr, #32
	bcc	9001b
9002:
	.endm

	.macro	get_irqnr_and_base, irqnr, irqstat, base, tmp
	ldr	\base, =(CNS21XX_VIC_BASE_VIRT + 0x1C)
	ldr	\irqstat, [\base]
	mov	\irqnr, #0
9003:
	tst	\irqstat, #1
	bne	9004f
	add	\irqnr, \irqnr, #1
	mov	\irqstat, \irqstat, lsr #1
	cmp	\irqnr, #32
	bcc	9003b
9004:
	.endm
#endif

	.macro	irq_prio_table
	.endm
*/

	.macro  get_irqnr_and_base, irqnr, irqstat, base, tmp
	ldr     \irqstat, =(CNS21XX_VIC_BASE_VIRT + 0x1C)
	ldr     \irqnr, [\irqstat]
	cmp     \irqnr, #0
	beq     2313f
	mov     \tmp, \irqnr
	mov     \irqnr, #0
2312:
	tst     \tmp, #1
	bne     2313f
	add     \irqnr, \irqnr, #1
	mov     \tmp, \tmp, lsr #1
	cmp     \irqnr, #31
	bcc     2312b
2313:
	.endm





