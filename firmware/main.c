#include <stdint.h>

void uart_init(void);
void timer_init(void);
void motor_init(void);
extern volatile uint32_t timer_ticks;

void uart_putc(char c);
void uart_puts(char *s);
int uart_getc(void);

void motor_set_output(int percent);

void control_set_target(int target);
void control_update(void);
void motor_model_update(void);

void command_process_char(char c);

void telemetry_print(void);

int main(void) {
  uint32_t last_control = 0;
  uint32_t last_telemetry = 0;
  
  uart_init();
  timer_init();
  motor_init();

  control_set_target(1500);

  uart_puts("Hello");

  while (1) {
    int c = uart_getc();

    if (c != -1) {
      uart_putc(c);
      command_process_char(c);
    }

    if (timer_ticks - last_telemetry >= 5000) { // Adjustable based on demands
      telemetry_print();
      last_telemetry += 5000;
    }
    if (timer_ticks - last_control >= 10) {
      motor_model_update();
      control_update();
      last_control += 10;
    }
  }
}