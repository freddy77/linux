/*
 * Copyright (C) 2012 - Virtual Open Systems and Columbia University
 * Author: Christoffer Dall <c.dall@virtualopensystems.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <linux/linkage.h>
#include <asm/unified.h>
#include <asm/asm-offsets.h>
#include <asm/kvm_asm.h>
#include <asm/kvm_arm.h>
#include <asm/kvm_mmu.h>

/********************************************************************
 * Hypervisor initialization
 *   - should be called with:
 *       r0 = top of Hyp stack (kernel VA)
 *       r1 = pointer to hyp vectors
 *       r2,r3 = Hypervisor pgd pointer
 *
 * The init scenario is:
 * - We jump in HYP with four parameters: boot HYP pgd, runtime HYP pgd,
 *   runtime stack, runtime vectors
 * - Enable the MMU with the boot pgd
 * - Jump to a target into the trampoline page (remember, this is the same
 *   physical page!)
 * - Now switch to the runtime pgd (same VA, and still the same physical
 *   page!)
 * - Invalidate TLBs
 * - Set stack and vectors
 * - Profit! (or eret, if you only care about the code).
 *
 * As we only have four registers available to pass parameters (and we
 * need six), we split the init in two phases:
 * - Phase 1: r0 = 0, r1 = 0, r2,r3 contain the boot PGD.
 *   Provides the basic HYP init, and enable the MMU.
 * - Phase 2: r0 = ToS, r1 = vectors, r2,r3 contain the runtime PGD.
 *   Switches to the runtime PGD, set stack and vectors.
 */

	.text
	.pushsection    .hyp.idmap.text,"ax"
	.align 5
__kvm_hyp_init:
	.globl __kvm_hyp_init

	@ Hyp-mode exception vector
	W(b)	.
	W(b)	.
	W(b)	.
	W(b)	.
	W(b)	.
	W(b)	__do_hyp_init
	W(b)	.
	W(b)	.

__do_hyp_init:
	@ check for special values, odd numbers can't be a stack pointer
	cmp	r0, #1
	beq	cpu_proc_fin
	cmp	r0, #3
	beq	restore

	cmp	r0, #0			@ We have a SP?
	bne	phase2			@ Yes, second stage init

	@ Set the HTTBR to point to the hypervisor PGD pointer passed
	mcrr	p15, 4, r2, r3, c2

	@ Save HSCR to be able to return it, save and restore
	mrc	p15, 4, r3, c1, c0, 0	@ HSCR

	@ Set the HTCR and VTCR to the same shareability and cacheability
	@ settings as the non-secure TTBCR and with T0SZ == 0.
	mrc	p15, 4, r0, c2, c0, 2	@ HTCR
	ldr	r2, =HTCR_MASK
	bic	r0, r0, r2
	mrc	p15, 0, r1, c2, c0, 2	@ TTBCR
	and	r1, r1, #(HTCR_MASK & ~TTBCR_T0SZ)
	orr	r0, r0, r1
	mcr	p15, 4, r0, c2, c0, 2	@ HTCR

	mrc	p15, 4, r1, c2, c1, 2	@ VTCR
	ldr	r2, =VTCR_MASK
	bic	r1, r1, r2
	bic	r0, r0, #(~VTCR_HTCR_SH)	@ clear non-reusable HTCR bits
	orr	r1, r0, r1
	orr	r1, r1, #(KVM_VTCR_SL0 | KVM_VTCR_T0SZ | KVM_VTCR_S)
	mcr	p15, 4, r1, c2, c1, 2	@ VTCR

	@ Use the same memory attributes for hyp. accesses as the kernel
	@ (copy MAIRx ro HMAIRx).
	mrc	p15, 0, r0, c10, c2, 0
	mcr	p15, 4, r0, c10, c2, 0
	mrc	p15, 0, r0, c10, c2, 1
	mcr	p15, 4, r0, c10, c2, 1

	@ Set the HSCTLR to:
	@  - ARM/THUMB exceptions: Kernel config (Thumb-2 kernel)
	@  - Endianness: Kernel config
	@  - Fast Interrupt Features: Kernel config
	@  - Write permission implies XN: disabled
	@  - Instruction cache: enabled
	@  - Data/Unified cache: enabled
	@  - Memory alignment checks: enabled
	@  - MMU: enabled (this code must be run from an identity mapping)
	mrc	p15, 4, r0, c1, c0, 0	@ HSCR
	ldr	r2, =HSCTLR_MASK
	bic	r0, r0, r2
	mrc	p15, 0, r1, c1, c0, 0	@ SCTLR
	ldr	r2, =(HSCTLR_EE | HSCTLR_FI | HSCTLR_I | HSCTLR_C)
	and	r1, r1, r2
 ARM(	ldr	r2, =(HSCTLR_M | HSCTLR_A)			)
 THUMB(	ldr	r2, =(HSCTLR_M | HSCTLR_A | HSCTLR_TE)		)
	orr	r1, r1, r2
	orr	r0, r0, r1
	isb
	mcr	p15, 4, r0, c1, c0, 0	@ HSCR

	@ Return initial HSCR register
	mov	r0, r3

	@ End of init phase-1
	eret

phase2:
	@ Set stack pointer
	mov	sp, r0

	@ Set HVBAR to point to the HYP vectors
	mcr	p15, 4, r1, c12, c0, 0	@ HVBAR

	@ Jump to the trampoline page
	ldr	r0, =TRAMPOLINE_VA
	adr	r1, target
	bfi	r0, r1, #0, #PAGE_SHIFT
	mov	pc, r0

target:	@ We're now in the trampoline code, switch page tables
	mcrr	p15, 4, r2, r3, c2
	isb

	@ Invalidate the old TLBs
	mcr	p15, 4, r0, c8, c7, 0	@ TLBIALLH
	dsb	ish

	eret

cpu_proc_fin:
	mrc	p15, 4, r0, c1, c0, 0		@ ctrl register
	bic	r0, r0, #0x1000			@ ...i............
	bic	r0, r0, #0x0006			@ .............ca.
	mcr	p15, 4, r0, c1, c0, 0		@ disable caches
	eret

restore:
	isb
	mcr	p15, 4, r1, c1, c0, 0	@ HSCR
	isb
	mcr	p15, 4, r0, c8, c7, 0	@ TLBIALLH
	isb
	dsb	ish

	@ Disable MMU, caches and Thumb in SCTLR
	mrc	p15, 0, r1, c1, c0, 0   @ SCTLR
	ldr	r0, =0x40003807
	bic	r1, r1, r0
	mcr	p15, 0, r1, c1, c0, 0

	bx	r2

	.ltorg

	.globl __kvm_hyp_init_end
__kvm_hyp_init_end:

	.popsection
