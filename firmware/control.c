#include "control.h"
#include "motor.h"
#include "motor_model.h"

static int first_update = 1; // For derivative calculation

static int target_speed = 0;

// Proportional (PID)
static int Kp_divisor = 20;
static int P;

// Integral (PID)
static int Ki_divisor = 1000;
static int I;
static int integral = 0;

// Derivative (PID)
static int Kd_divisor = 10;
static int D;
static int previous_error = 0;

static int error = 0;
static int output = 0;


void control_set_target(int target) {
    if (target > 3000) {
        target_speed = 3000;
    } else if (target < -3000) {
        target_speed = -3000;
    } else {
        target_speed = target;
    }
}

void control_update(void) {
    int current_speed = motor_model_get_speed();
    
    error = target_speed - current_speed;
    P = error / Kp_divisor;

    int proposed_integral = integral + error;
    int proposed_I = proposed_integral / Ki_divisor;

    if (first_update) {
        previous_error = error;
    }
    int derivative = error - previous_error;
    D = derivative / Kd_divisor;

    int proposed_output = P + proposed_I + D;

    if (!((proposed_output > 100 && error > 0) ||
          (proposed_output < -100 && error < 0))) {
        integral = proposed_integral;
        I = proposed_I;
    } else {
        I = integral / Ki_divisor;
    }

    output = P + I + D;

    if (output > 100) {
        output = 100;
    } else if (output < -100) {
        output = -100;
    }

    motor_model_set_command(output);
    motor_set_output(output);

    previous_error = error;
    first_update = 0;
}

void control_reset(void) {
    target_speed = 0;
    integral = 0;
    previous_error = 0;
    error = 0;
    output = 0;

    P = 0;
    I = 0;
    D = 0;

    first_update = 1;
}

int control_get_target_speed(void) {
    return target_speed;
}

int control_get_output(void) {
    return output;
}

int control_get_error(void) {
    return error;
}