sys_register_irq_handler:
    jr      $ra
    nop

sys_register_exc_handler:
    jr      $ra
    nop

syscall_table:
    .word sys_register_irq_handler
    .word sys_register_exc_handler