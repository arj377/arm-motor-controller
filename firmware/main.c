#include <stdint.h>
#include <stdbool.h>

#include "command.h"
#include "control.h"
#include "motor.h"
#include "motor_model.h"
#include "safety.h"
#include "telemetry.h"
#include "timer.h"
#include "uart.h"

#define CONTROL_PERIOD_MS 10
#define TELEMETRY_PERIOD_MS 1000

static uint32_t missed_control_deadlines = 0;

int main(void) {
  uint32_t last_control = 0;
  uint32_t last_telemetry = 0;
  
  uart_init();
  timer_init();
  motor_init();

  control_set_target(1500);

  uart_puts("motor controller ready\n");

  while (1) {
    int c = uart_getc();

    if (c != -1) {
      uart_putc(c);
      command_process_char(c);
    }

    if (timer_ticks - last_telemetry >= TELEMETRY_PERIOD_MS) {
        uint32_t elapsed = timer_ticks - last_telemetry;
        uint32_t periods = elapsed / TELEMETRY_PERIOD_MS;

        last_telemetry += periods * TELEMETRY_PERIOD_MS;
        telemetry_print();
    }
    if (timer_ticks - last_control >= CONTROL_PERIOD_MS) {
        uint32_t elapsed = timer_ticks - last_control;
        uint32_t periods = elapsed / CONTROL_PERIOD_MS;
        if (periods > 1) {
          missed_control_deadlines += periods - 1;
        }
        last_control += periods * CONTROL_PERIOD_MS;

        motor_model_update();
        safety_update();

        if (!safety_fault_active()) {
            control_update();
        }
    }
  }
}

uint32_t get_missed_control_deadlines(void) {
    return missed_control_deadlines;
}