#include <stdint.h>

void uart_init(void);
void uart_putc(char c);
void uart_puts(char* s);

uint32_t x = 123;
uint32_t y;

int main() {
    uart_init();
    uart_puts("Hello");
    while (1) {}
    return 1;
}