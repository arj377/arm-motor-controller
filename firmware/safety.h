#ifndef SAFETY_H
#define SAFETY_H

#include <stdbool.h>

enum SAFETY_STATE {
    FAULT_NONE,
    FAULT_OVERSPEED,
    FAULT_EMERGENCY_STOP
};

bool safety_fault_active(void);
void safety_update(void);
void emergency_stop(void);
void clear_fault(void);
enum SAFETY_STATE get_safety_state(void);

#endif