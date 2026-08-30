#include <stdint.h>

#include "telemetry.h"
#include "control.h"
#include "motor_model.h"
#include "safety.h"
#include "uart.h"

void telemetry_print(void) {  // target=1500 speed=1455 error=45 output=49
    uart_puts("target = ");
    uart_putint(control_get_target_speed());
    uart_puts(" | ");

    uart_puts("speed = ");
    uart_putint(motor_model_get_speed());
    uart_puts(" | ");

    uart_puts("error = ");
    uart_putint(control_get_error());
    uart_puts(" | ");

    uart_puts("output = ");
    uart_putint(control_get_output());
    uart_puts(" | ");

    uart_puts("safety state = ");
    uart_putint(get_safety_state()); // 0 = FAULT_NONE, 1 = FAULT_OVERSPEED, 2 = FAULT_EMERGENCY_STOP
    uart_puts(" | ");

    uart_puts("missed deadlines = ");
    uart_putint(get_missed_control_deadlines());
    uart_puts("\n");
}