#include <stdint.h>
#include "pwm.h"

#define GPIO_PORTF_BASE 0x40025000
#define GPIOAFSEL (*(volatile uint32_t *)(GPIO_PORTF_BASE + 0x420)) // GPIO alternate function
#define GPIODEN (*(volatile uint32_t *)(GPIO_PORTF_BASE + 0x51C))

#define PWM0_BASE 0x40028000
#define PWMENABLE (*(volatile uint32_t *)(PWM0_BASE + 0x008))
#define PWM0CTL (*(volatile uint32_t *)(PWM0_BASE + 0x040))
#define PWM0LOAD (*(volatile uint32_t *)(PWM0_BASE + 0x050))
#define PWM0CMPA (*(volatile uint32_t *)(PWM0_BASE + 0x058))
#define PWM0GENA (*(volatile uint32_t *)(PWM0_BASE + 0x060))

#define SYSTEM_CONTROL_BASE 0x400FE000
#define RCC (*(volatile uint32_t *)(SYSTEM_CONTROL_BASE + 0x060))   // Run-mode clock configuration
#define RCGC0 (*(volatile uint32_t *)(SYSTEM_CONTROL_BASE + 0x100)) // Controls clocks for peripherals
#define RCGC2 (*(volatile uint32_t *)(SYSTEM_CONTROL_BASE + 0x108))

void pwm_init(void) {
  RCGC0 |= (1U << 20); // Enable clock for PWM
  RCGC2 |= (1U << 5);  // Enable clock for GPIO Port F

  PWM0CTL &= ~(1U); // Disable generator
  GPIOAFSEL |= 1U;  // Allow PWM0 (hardware) to be primary function
  GPIODEN |= 1U;    // Enable PF0 digital function

  RCC |= (1U << 20);  // PWM clock configs
  RCC &= ~(7U << 17); // Divisor = 2

  PWM0GENA = 0x0000008C; // LOAD event → PWM0 goes HIGH, CMPA while counting down → PWM0 goes LOW

  PWM0LOAD = 299; // 300 ticks per cycle
  PWM0CMPA = 149; // 50% duty cycle

  PWM0CTL |= 1U;   // Turn on timerw
  PWMENABLE |= 1U; // Enable PWM outputs
}

void pwm_set_duty(int percent) {
  if (percent <= 0) {
    PWM0GENA = 0x00000008; // Force LOW at LOAD
  } else if (percent >= 100) {
    PWM0GENA = 0x0000000C; // Always stay HIGH
  } else {
    PWM0GENA = 0x0000008C; // LOAD event → PWM0 goes HIGH, CMPA while counting down → PWM0 goes LOW
    int high_ticks = ((PWM0LOAD + 1) * percent) / 100;
    PWM0CMPA = PWM0LOAD - high_ticks;
  }
}