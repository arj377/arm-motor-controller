#include <stdint.h>

#define UART0_BASE 0x4000C000
#define UARTDR (*(volatile uint32_t *)(UART0_BASE + 0x000)) // Data Register
#define UARTFR (*(volatile uint32_t *)(UART0_BASE + 0x018)) // Status Flags Register
#define UARTIBRD (*(volatile uint32_t *)(UART0_BASE + 0x024)) // Integer Baud Rate Divisor
#define UARTFBRD (*(volatile uint32_t *)(UART0_BASE + 0x028)) // Fractional Baud Rate Divisor
#define UARTLCRH (*(volatile uint32_t *)(UART0_BASE + 0x02C)) // UART data format
#define UARTCTL (*(volatile uint32_t *)(UART0_BASE + 0x030)) // Enable/Disable UART

#define SYSTEM_CONTROL_BASE 0x400FE000
#define RCGC1 (*(volatile uint32_t *)(SYSTEM_CONTROL_BASE + 0x104)) // Controls clocks for peripherals

void uart_init() {
    RCGC1 |= 1U; // Enable the peripheral clock for UART0
    UARTCTL &= ~1U; // Disable UART
    UARTIBRD = 27; // BRD = system clock / (16 × baud rate)
    UARTFBRD = 8;
    UARTLCRH |= (3U << 5); // 8 bits
    UARTLCRH &= ~(1U << 1); // No parity
    UARTLCRH &= ~(1U << 3); // 1 stop bit
    UARTLCRH |= (1U << 4); // Enable FIFO
    UARTCTL |= 1U; // Enable UART
    UARTCTL |= (3U << 8); // Enable TX and RX
}

void uart_putc(char c) {
    while (UARTFR & (1U << 5) != 0) {} // Wait while TX FIFO is full
    UARTDR = c;
}

void uart_puts(char *s) {
    while (*s != '\0') {
        uart_putc(*s);
        s += 1;
    } 
}