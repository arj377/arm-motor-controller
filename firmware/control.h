#ifndef CONTROL_H
#define CONTROL_H

void control_set_target(int target);
void control_update(void);

int control_get_target_speed(void);
int control_get_output(void);
int control_get_error(void);
void control_reset(void);

#endif