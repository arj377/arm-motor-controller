#include <stdint.h>

void uart_init(void);
void timer_init(void);
void motor_init(void);
extern volatile uint32_t timer_ticks;

void uart_putc(char c);
void uart_puts(char *s);
int uart_getc(void);

void motor_set_output(int percent);

int main(void) {
  uint32_t last_print = 0;
  
  uart_init();
  timer_init();
  motor_init();

  motor_set_output(-50);
  uart_puts("Hello");

  while (1) {
    int c = uart_getc();

    if (c != -1) {
      uart_putc(c);
    }

    if (timer_ticks - last_print >= 1000) {
      uart_puts("tick\n");
      last_print += 1000;
    }
  }
}