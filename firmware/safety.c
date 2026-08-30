#include <stdbool.h>

#include "safety.h"
#include "control.h"
#include "motor.h"
#include "motor_model.h"

#define MAX_SAFE_SPEED_RPM 3200

enum SAFETY_STATE {
    FAULT_NONE,         // Healthy
    FAULT_OVERSPEED,    // Unsafe motor speed
    FAULT_EMERGENCY_STOP // User commanded an emergency stop
};

static enum SAFETY_STATE safety_state = FAULT_NONE;

bool safety_fault_active(void) {

    if (safety_state == FAULT_NONE) {
        return false;
    }

    return true;
}

void safety_update(void) {
    if (safety_state == FAULT_NONE) {
        int cur_speed = motor_model_get_speed();
        if (cur_speed > MAX_SAFE_SPEED_RPM || cur_speed < -MAX_SAFE_SPEED_RPM) {
            safety_state = FAULT_OVERSPEED; // Latched fault
            motor_set_output(0);
            motor_model_set_command(0);
            control_reset();
        }
    }
}

void emergency_stop(void) {
    safety_state = FAULT_EMERGENCY_STOP;
    motor_model_set_command(0);  // Simulated plant command = 0
    motor_set_output(0); // physical/HAL command = 0
    control_reset();; // Controller no longer wants movement
}

void clear_fault(void) {
    int speed = motor_model_get_speed();
    if (speed <= MAX_SAFE_SPEED_RPM && speed >= -MAX_SAFE_SPEED_RPM) {
        safety_state = FAULT_NONE;
    }
    // Else stay latched
}

enum SAFETY_STATE get_safety_state(void) {
    return safety_state;
}