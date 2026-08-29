#include <stdint.h>
#include <stdbool.h>

#define UART0_BASE 0x4000C000
#define UARTDR (*(volatile uint32_t *)(UART0_BASE + 0x000))   // Data Register
#define UARTFR (*(volatile uint32_t *)(UART0_BASE + 0x018))   // Status Flags Register
#define UARTIBRD (*(volatile uint32_t *)(UART0_BASE + 0x024)) // Integer Baud Rate Divisor
#define UARTFBRD (*(volatile uint32_t *)(UART0_BASE + 0x028)) // Fractional Baud Rate Divisor
#define UARTLCRH (*(volatile uint32_t *)(UART0_BASE + 0x02C)) // UART data format
#define UARTCTL (*(volatile uint32_t *)(UART0_BASE + 0x030))  // Enable/Disable UART
#define UARTIM (*(volatile uint32_t *)(UART0_BASE + 0x038))   // Interrupt Mask Set/Clear Register
#define UARTMIS (*(volatile uint32_t *)(UART0_BASE + 0x040))  // Masked Interrupt Status Register
#define UARTICR (*(volatile uint32_t *)(UART0_BASE + 0x044))  // Interrupt Clear Register

#define SYSTEM_CONTROL_BASE 0x400FE000
#define RCGC1 (*(volatile uint32_t *)(SYSTEM_CONTROL_BASE + 0x104)) // Controls clocks for peripherals

#define NVIC_BASE 0xE000E000
#define EN0 (*(volatile uint32_t *)(NVIC_BASE + 0x100)) // Interrupts 0-31 Set/Enable

#define RX_BUFFER_SIZE 8

// For ring buffer
uint8_t rx_buffer[RX_BUFFER_SIZE];
volatile int rx_head = 0, rx_tail = 0;

void uart_init(void) {
  RCGC1 |= 1U;    // Enable the peripheral clock for UART0
  UARTCTL &= ~1U; // Disable UART
  UARTIBRD = 27;  // BRD = system clock / (16 × baud rate)
  UARTFBRD = 8;
  UARTLCRH |= (3U << 5);  // 8 bits
  UARTLCRH &= ~(1U << 1); // No parity
  UARTLCRH &= ~(1U << 3); // 1 stop bit
  UARTLCRH |= (1U << 4);  // Enable FIFO
  UARTCTL |= 1U;          // Enable UART
  UARTCTL |= (3U << 8);   // Enable TX and RX

  UARTIM |= (1U << 4); // Allow RX events to generate an interrupt
  UARTIM |= (1U << 6); // Enable UART0 RX timeout
  EN0 |= (1U << 5);    // Allow UART0 interrupt 5 through to the CPU
}

void uart_putc(char c) {
  while ((UARTFR & (1U << 5)) != 0) {
  } // Wait while TX FIFO is full
  UARTDR = c;
}

int uart_getc(void) {
  if (rx_head == rx_tail) {
    return -1;
  } else {
    uint8_t ret = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % RX_BUFFER_SIZE;
    return ret;
  }
}

void uart_puts(char *s) {
  while (*s != '\0') {
    uart_putc(*s);
    s += 1;
  }
}

void uart_putint(int num) {
  bool negative = false;
  if (num == 0) {
    uart_putc('0');
    return;
  } else if (num < 0) {
    negative = true;
    num *= -1;
  }
  
  int temp = num;
  int counter = 0;
  while (temp != 0) {
    temp /= 10;
    counter++;
  }
  int digits[counter];

  temp = num;
  for (int i = 0; i < counter; i++) {
    digits[i] = temp % 10;
    temp /= 10;
  }

  if (negative) {
    uart_putc('-');
  }
  for (int i = counter - 1; i >= 0; i--) {
    uart_putc(digits[i] + '0');
  }
}

void UART0_Handler(void) {
  if ((UARTMIS & (1U << 4)) != 0 || (UARTMIS & (1U << 6)) != 0) { // If RX interrupt or RX-timeout interrupt
    while ((UARTFR & (1U << 4)) == 0) {                           // Keep reading until RX FIFO is empty
      int next_head = (rx_head + 1) % RX_BUFFER_SIZE;

      if (next_head != rx_tail) {
        rx_buffer[rx_head] = UARTDR;
        rx_head = next_head;
      }
    }
  }

  UARTICR = (1U << 4) | (1U << 6); // Clear interrupts
}