#include <stdbool.h>

enum STATE {
    WAIT_COMMAND,
    WAIT_VALUE,
    READ_VALUE
};

void control_set_target(int target);

static enum STATE state = WAIT_COMMAND;
static int value = 0;
static bool positive = true;

void uart_puts(char* c);
void uart_putint(int num);

void emergency_stop(void);
void clear_fault(void);

void command_process_char(char c) {
    switch(state) {
        case WAIT_COMMAND:
            if (c == 's') {
                value = 0;
                positive = true;
                state = WAIT_VALUE;
            } else if (c == 'x') {
                emergency_stop();
            } else if (c == 'c') {
                clear_fault();
            }
            break;
        case WAIT_VALUE:
            if (c == '-') {
                positive = false;
            } else if (c >= '0' && c <= '9') {
                value = value * 10 + (c - '0');
                state = READ_VALUE;
            } else if (c != ' ') {
                state = WAIT_COMMAND;
            }
            break;
        case READ_VALUE:
            if (c >= '0' && c <= '9') {
                value = value * 10 + (c - '0');
            } else if (c == '\n' || c == '\r') {
                if (positive == false) {
                    value *= -1;
                }

                control_set_target(value);
                state = WAIT_COMMAND;
            } else {
                state = WAIT_COMMAND;
            }
            break;
    }
}