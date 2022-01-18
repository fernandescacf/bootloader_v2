/* Memory System Macros */
.set SCTLR_M,		(1<<0) 		// SCTLR.M bit (MMU)
.set SCTLR_A,		(1<<1) 		// SCTLR.A bit (Strict aligment checking)
.set SCTLR_C,		(1<<2) 		// SCTLR.C bit (Data cache)
.set SCTLR_W,		(1<<3) 		// SCTLR.W bit (Write buffer)
.set SCTLR_Z,		(1<<11) 	// SCTLR.Z bit (Branch prediction)
.set SCTLR_I,		(1<<12)		// SCTLR.I bit (Instruction cache)
.set SCTLR_V,		(1<<13) 	// SCTLR.V bit (Low exception vectors)
.set SCTLR_U,		(1<<22) 	// SCTLR.U bit (Unaligned data access)
.set SCTLR_XP,		(1<<23) 	// SCTLR.XP bit (Extended page tables)

/* ARM Processor Modes */
.set USR_MODE,		0x10
.set FIQ_MODE,		0x11
.set IRQ_MODE, 		0x12
.set SVC_MODE, 		0x13
.set ABT_MODE,		0x17
.set UNDEF_MODE, 	0x1B
.set SYS_MODE, 		0x1F

/* Boot Code */
.text

.global _start
_start:
	b		boot_code		// Reset			-> 0x00
	b		.				// Undefined		-> 0x04
	b		.				// Supervisor		-> 0x08
	b		.				// Pre-fetch Abort	-> 0x0c
	b		.				// Data Abort		-> 0x10
	NOP						// Hyper-visor		-> 0x14  (not support in this processor)
	b		.				// IRQ				-> 0x18
	b		.				// FIQ				-> 0x1c

boot_code:
	/* Change to SVC mode */
	cps		SVC_MODE
	/* Set SVC stack pointer */
	ldr		sp, =__bootLoader_stack
	/* Disable interrupts */
	cpsid	if
	/* Disable MMU, Caches, Branch Prediction, Write Buffer, ... */
	mrc		p15, 0, r5, c1, c0, 0
	bic		r5, r5, #SCTLR_M
	bic		r5, r5, #SCTLR_A
	bic		r5, r5, #SCTLR_C
//	bic		r5, r5, #SCTLR_W
	bic		r5, r5, #SCTLR_Z
	bic		r5, r5, #SCTLR_I
	orr		r5, r5, #SCTLR_U
	bic		r5, r5, #SCTLR_XP
	mcr		p15, 0, r5, c1, c0, 0
	isb
	dsb
	/* Initialize .bss section */
	mov		r0, #0x0
	ldr 	r1, =_bss_start
	ldr		r2, =_bss_end
1:	cmp		r1,r2
	strne	r0, [r1], #4
	bne		1b
	/* Jump to the main */
	b		main
	/* Should not get to here */
	.long 	0xffffffff
	.long 	0xffffffff
	.long 	0xffffffff
	/* Infinite loop */
	b		.

#if(CPU_CORE_COUNT > 1)

.global boot_sec
.func boot_sec
boot_sec:
	/* Change to SVC mode */
	cps		SVC_MODE
	/* Disable interrupts */
	cpsid	if
	/* Disable MMU, Caches, Branch Prediction, Write Buffer, ... */
	mrc		p15, 0, r5, c1, c0, 0
	bic		r5, r5, #SCTLR_M
	bic		r5, r5, #SCTLR_A
	bic		r5, r5, #SCTLR_C
//	bic		r5, r5, #SCTLR_W
	bic		r5, r5, #SCTLR_Z
	bic		r5, r5, #SCTLR_I
	orr		r5, r5, #SCTLR_U
	bic		r5, r5, #SCTLR_XP
	mcr		p15, 0, r5, c1, c0, 0
	isb
	dsb
	/* Fetch boot parameter boot_addr, arg0, arg1 */
	ldr		r2, =_boot_param
	ldr		r0, [r2, #4]
	ldr		r1, [r2, #8]
	ldr		pc, [r2]
.endfunc

.global _SetBoot
.func	_SetBoot
// void _SetBoot(void* _boot_addr, void* _boot_arg0, void* _boot_arg1)
_SetBoot:
	ldr		r3, =_boot_param
	str		r0, [r3]
	str		r1, [r3, #4]
	str		r2, [r3, #8]
	mov		pc, lr
.endfunc

.data
.align  2
.type   __boot_param, %object
_boot_param:
	.long   0x0		//_boot_addr
	.long	0x0		//_boot_arg0
	.long	0x0		//_boot_arg1

#endif