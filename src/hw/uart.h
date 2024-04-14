#ifndef UART_H
#define UART_H

#define UART_BASE       0x9f900000
#define UART_RHR        0x9f900000
#define UART_THR        0x9f900000
#define UART_IER        0x9f900001
#define UART_IIR        0x9f900002
#define UART_FCR        0x9f900002
#define UART_ISR        0x9f900002
#define UART_LCR        0x9f900003
#define UART_LSR        0x9f900005
#define IER_RX_IRQ      0x01
#define IER_TX_IRQ      0x02
#define FCR_FIFO_ENABLE 0x01
#define FCR_FIFO_CLEAR  0x06
#define LCR_WORD_LEN    0x03
#define LCR_DLAB        0x80
#define LSR_RX_READY    0x01
#define LSR_TX_EMPTY    0x20

void uart_init();
void uart_send_byte(int c);
int uart_recv_byte(void);

#endif