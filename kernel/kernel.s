#include "def.h"

.org 0x00000180

.equ SYS_KMEM_BASE,         0x90000000
.equ SYS_KMEM_END,          0x90010000
.equ SYS_IRQ_HANDLER_PTR,   0x90008000
.equ SYS_EXC_HANDLER_PTR,   0x90008004
.equ SYS_HEAP_BASE_PAGE,    0x90008008 # End of OS executable
.equ SYS_XCONTEXT_BASE,     0x90009000

.macro xsave
.set noat
    la   $k0, SYS_XCONTEXT_BASE
    sw   $at, 0($k0)
    sw   $v0, 4($k0)
    sw   $v1, 8($k0)
    sw   $a0, 12($k0)
    sw   $a1, 16($k0)
    sw   $a2, 20($k0)
    sw   $a3, 24($k0)
    sw   $t0, 28($k0)
    sw   $t1, 32($k0)
    sw   $t2, 36($k0)
    sw   $t3, 40($k0)
    sw   $t4, 44($k0)
    sw   $t5, 48($k0)
    sw   $t6, 52($k0)
    sw   $t7, 56($k0)
    sw   $s0, 60($k0)
    sw   $s1, 64($k0)
    sw   $s2, 68($k0)
    sw   $s3, 72($k0)
    sw   $s4, 76($k0)
    sw   $s5, 80($k0)
    sw   $s6, 84($k0)
    sw   $s7, 88($k0)
    sw   $t8, 92($k0)
    sw   $t9, 96($k0)
    sw   $at, 100($k0)
    sw   $gp, 104($k0)
    sw   $sp, 108($k0)
    sw   $fp, 112($k0)
    sw   $ra, 116($k0)
    mflo $k1
    sw   $k1, 120($k0)
    mfhi $k1
    sw   $k1, 124($k0)
.set at
.endm

.macro xrestore
.set noat
    la   $k0, SYS_XCONTEXT_BASE
    lw   $at, 0($k0)
    lw   $v0, 4($k0)
    lw   $v1, 8($k0)
    lw   $a0, 12($k0)
    lw   $a1, 16($k0)
    lw   $a2, 20($k0)
    lw   $a3, 24($k0)
    lw   $t0, 28($k0)
    lw   $t1, 32($k0)
    lw   $t2, 36($k0)
    lw   $t3, 40($k0)
    lw   $t4, 44($k0)
    lw   $t5, 48($k0)
    lw   $t6, 52($k0)
    lw   $t7, 56($k0)
    lw   $s0, 60($k0)
    lw   $s1, 64($k0)
    lw   $s2, 68($k0)
    lw   $s3, 72($k0)
    lw   $s4, 76($k0)
    lw   $s5, 80($k0)
    lw   $s6, 84($k0)
    lw   $s7, 88($k0)
    lw   $t8, 92($k0)
    lw   $t9, 96($k0)
    lw   $at, 100($k0)
    lw   $gp, 104($k0)
    lw   $sp, 108($k0)
    lw   $fp, 112($k0)
    lw   $ra, 116($k0)
    lw   $k1, 120($k0)
    mtlo $k1
    lw   $k1, 124($k0)
    mthi $k1
.set at
.endm

exception_handler:
    xsave

    # Get Cause register
    mfc0    $t0, $13

    # Get excode
    srl     $t0, 2
    andi    $t0, 0x1f

    # 0: INT
    beqz    $t0, irq_handler
    nop

    # 8: SYS
    li      $t1, 8
    beq     $t0, $t1, syscall_handler
    nop

    # Anything else, call user handle
    j       exc_handler
    nop

unhandled_exception:
    # Unhandled exception (crash kernel)
    la      $t1, unhandled_exception_msg
    jal     kputs
    nop
    mfc0    $k0, $13
    j       halt
    nop

# To-do: Implement jumptable
syscall_handler:
    # # Initialize IRQ handler
    # la      $t0, SYS_IRQ_HANDLER_PTR
    # sw      $a0, 0($t0)

    # # Acknowledge pending IRQs
    # la      $k1, 0x9f801070
    # sw      $zero, 0($k1)

    # # Enable VBLANK IRQ
    # li      $k0, 0x1
    # la      $k1, 0x9f801074
    # sw      $k0, 0($k1)

    # Can't enable interrupts within exception handler
    # li      $k0, 0xff03
    # mtc0    $k0, $12

    # Restore saved state
    xrestore

    # la      $k0, SYS_HEAP_BASE_PAGE
    # lw      $a0, 0($k0)

    # Jump to EPC (+4 to skip syscall instr)
    mfc0    $k0, $14
    nop

.set noreorder
    addi    $k0, 4
    jr      $k0
    rfe
.set reorder

irq_handler:
    # Acknowledge pending IRQs
    la      $k1, 0x9f801070
    sw      $zero, 0($k1)

    # Call registered IRQ handler
    la      $t0, SYS_KMEM_BASE
    lw      $t0, 0($t0)
    beqz    $t0, 0f                 # Skip if no handler is registered
    nop
    jalr    $t0
    nop

0:
    # Restore saved state
    xrestore

    # Jump to EPC
    mfc0    $k0, $14
    nop

.set noreorder
    jr      $k0
    rfe
.set reorder

exc_handler:
    # Assume TLB refill needed
    # Load mapping for unmapped page from kernel memory
    # Context was setup so it automatically points to the
    # correct mapping.
    mfc0    $k0, $4

    # Load mapping from kernel memory
    # Note: EntryHi is loaded by the CPU
    lw      $k1, 0($k0)
    mtc0    $k1, $2

    # Write mapping to TLB (random entry)
    tlbwr

    # mtc0    $0, $0
    # tlbwi
 
    # # Call registered EXC handler
    # la      $t0, SYS_KMEM_BASE
    # lw      $t0, 4($t0)

    # # Crash kernel if no handler is registered
    # beqz    $t0, unhandled_exception
    # nop
    # jalr    $t0
    # nop

    # Restore saved state
    xrestore

    # Jump to EPC
    mfc0    $k0, $14
    nop

.set noreorder
    jr      $k0
    rfe
.set reorder

kputs:
    la      $t0, 0x9f900000

0:
    lb      $t2, 0($t1)
    beqz    $t2, 1f
    sb      $t2, 0($t0)
    b       0b
    addi    $t1, 1

1:
    jr      $ra
    nop

halt:
    la      $t1, system_halted_msg
    jal     kputs
    nop

0:
    b       0b
    nop

unhandled_exception_msg:
    .asciiz "kernel: Unhandled exception! Check $k0 for exception cause.\n"

system_halted_msg:
    .asciiz "kernel: System halted\n"

#include "syscall.s"
