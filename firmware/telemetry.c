int control_get_target_speed(void);
int motor_model_get_speed(void);
int control_get_error(void);
int control_get_output(void);

void uart_puts(char *s);
void uart_putint(int num);

void telemetry_print() {  // target=1500 speed=1455 error=45 output=49
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
    uart_puts("\n");

}