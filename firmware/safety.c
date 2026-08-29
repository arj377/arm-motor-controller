#include <stdbool.h>

#define MAX_SAFE_SPEED_RPM 3200

enum SAFETY_STATE {
    FAULT_NONE,         // Healthy
    FAULT_OVERSPEED,    // Unsafe motor speed
    FAULT_EMERGENCY_STOP // User commanded an emergency stop
};

static enum SAFETY_STATE safety_state = FAULT_NONE;

void motor_model_set_command (int val);
void control_set_target(int val);
void motor_set_output(int val);
int motor_model_get_speed(void);

bool safety_fault_active() {

    if (safety_state == FAULT_NONE) {
        return false;
    }

    return true;
}

void safety_update() {
    if (safety_state == FAULT_NONE) {
        int cur_speed = motor_model_get_speed();
        if (cur_speed > MAX_SAFE_SPEED_RPM || cur_speed < -MAX_SAFE_SPEED_RPM) {
            safety_state = FAULT_OVERSPEED; // Latched fault
            motor_set_output(0);
            motor_model_set_command(0);
            control_set_target(0);
        }
    }
}

void emergency_stop() {
    safety_state = FAULT_EMERGENCY_STOP;
    motor_model_set_command(0);  // Simulated plant command = 0
    motor_set_output(0); // physical/HAL command = 0
    control_set_target(0); // Controller no longer wants movement
}

void clear_fault() {
    int speed = motor_model_get_speed();
    if (speed <= MAX_SAFE_SPEED_RPM && speed >= -MAX_SAFE_SPEED_RPM) {
        safety_state = FAULT_NONE;
    }
    // Else stay latched
}

enum SAFETY_STATE get_safety_state() {
    return safety_state;
}