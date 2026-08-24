#include <stdint.h>

#define UART0_BASE 0x4000C000
#define UARTDR (*(volatile uint32_t *)(UART0_BASE + 0x000)) // Data Register
#define UARTFR (*(volatile uint32_t *)(UART0_BASE + 0x018)) // Status Flags Register
#define UARTIBRD (*(volatile uint32_t *)(UART0_BASE + 0x024)) // Integer Baud Rate Divisor
#define UARTFBRD (*(volatile uint32_t *)(UART0_BASE + 0x028)) // Fractional Baud Rate Divisor
#define UARTLCRH (*(volatile uint32_t *)(UART0_BASE + 0x02C)) // UART data format
#define UARTCTL (*(volatile uint32_t *)(UART0_BASE + 0x030)) // Enable/Disable UART
