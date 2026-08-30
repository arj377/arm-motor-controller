#include <stdint.h>
#include "motor.h"
#include "pwm.h"

#define GPIO_PORTF_BASE 0x40025000
#define GPIODATA (*(volatile uint32_t *)(GPIO_PORTF_BASE + 0x008))  // can GPIO send?
#define GPIODIR (*(volatile uint32_t *)(GPIO_PORTF_BASE + 0x400))   // can GPIO send?
#define GPIOAFSEL (*(volatile uint32_t *)(GPIO_PORTF_BASE + 0x420)) // GPIO alternate function
#define GPIODEN (*(volatile uint32_t *)(GPIO_PORTF_BASE + 0x51C))   // GPIO alternate function

void motor_init(void) {
  pwm_init();
  GPIOAFSEL &= ~(1U << 1); // PF1 uses normal GPIO, not alternate hardware
  GPIODIR |= (1U << 1);    // Pin sends data, doesn't receive
  GPIODEN |= (1U << 1);    // PF1 digital enabled
  GPIODATA = 0;            // Write LOW to PF1
}

void motor_set_output(int percent) {
  if (percent < 0) {
    pwm_set_duty(percent * -1);
    GPIODATA = 0; // Write LOW to PF1
  } else {
    pwm_set_duty(percent);
    GPIODATA = (1U << 1); // Write HIGH to PF1
  }
}