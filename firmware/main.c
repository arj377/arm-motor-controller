#include <stdint.h>

void uart_init(void);
void uart_putc(char c);
void uart_puts(char *s);
int uart_getc(void);

int main() {
  uart_init();
  uart_puts("Hello");

  while (1) {
    int c = uart_getc();
    if (c == -1) {
      continue;
    } else {
      uart_putc(c);
    }
  }

  return 1;
}