#ifndef DEF_H
#define DEF_H

#define RAM_BASE  0x80000000
#define RAM_END   0x81000000
#define VMC_BASE  0x1f800000
#define VMC_END   0x1f800010
#define UART_BASE 0x1f900000
#define UART_END  0x1f900008
#define NVS_BASE  0x1fa00000
#define NVS_END   0x1fa00040
#define GPU_BASE  0x1f801810
#define GPU_END   0x1f801818
#define DMA_BASE  0x1f801080
#define DMA_END   0x1f801180
#define IC_BASE   0x1f801070
#define IC_END    0x1f801078

// UART definitions
#define UART_RHR        0x00
#define UART_THR        0x00
#define UART_IER        0x01
#define UART_IIR        0x02
#define UART_FCR        0x02
#define UART_ISR        0x02
#define UART_LCR        0x03
#define UART_LSR        0x05
#define IER_RX_IRQ      0x01
#define IER_TX_IRQ      0x02
#define FCR_FIFO_ENABLE 0x01
#define FCR_FIFO_CLEAR  0x06
#define LCR_WORD_LEN    0x03
#define LCR_DLAB        0x80
#define LSR_RX_READY    0x01
#define LSR_TX_EMPTY    0x20


#endif