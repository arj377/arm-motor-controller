#define MAX_SPEED_RPM 3000 // Maximum simulated motor speed in RPM

static int motor_command = 0;
static int current_speed = 0;
static int desired_speed = 0;

void motor_model_set_command(int motor_value) {
    if (motor_value > 100) {
        motor_command = 100;
    } else if (motor_value < -100) {
        motor_command = -100;
    } else {
        motor_command = motor_value;
    }

    desired_speed = (MAX_SPEED_RPM * motor_command) / 100;
}

void motor_model_update() {
    int error = desired_speed - current_speed;
    current_speed += error / 10; 
}

int motor_model_get_speed() {
    return current_speed;
}