#include "uart.h"
#include "libc/stdio.h"
#include "util/mmio.h"

void uart_init() {
    mmio_write_8(UART_IER, 0);
    mmio_write_8(UART_LCR, LCR_DLAB);
    mmio_write_8(UART_BASE + 0, 0x03);
    mmio_write_8(UART_BASE + 1, 0x00);
    mmio_write_8(UART_LCR, 3);
    mmio_write_8(UART_FCR, FCR_FIFO_ENABLE | FCR_FIFO_CLEAR);

    // Interrupts not yet implemented
    // mmio_write_8(UART_IER, IER_TX_ENABLE | IER_RX_ENABLE);
}

void uart_send_byte(int c) {
    while (!(mmio_read_8(UART_LSR) & LSR_TX_EMPTY));

    mmio_write_8(UART_THR, c);
}

int uart_recv_byte(void) {
    while (!(mmio_read_8(UART_LSR) & LSR_RX_READY));

    return mmio_read_8(UART_RHR);
}