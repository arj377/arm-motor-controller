#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

extern volatile uint32_t timer_ticks;

void timer_init(void);

#endif