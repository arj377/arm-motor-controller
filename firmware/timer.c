#include <stdint.h>

#define TIMER0_BASE 0x40030000
#define GPTMCFG (*(volatile uint32_t *)(TIMER0_BASE + 0x000))   // Timer config
#define GPTMTAMR (*(volatile uint32_t *)(TIMER0_BASE + 0x004))  // TimerA mode
#define GPTMCTL (*(volatile uint32_t *)(TIMER0_BASE + 0x00C))   // Control for timers
#define GPTMIMR (*(volatile uint32_t *)(TIMER0_BASE + 0x018))   // Enable/disable interrupts
#define GPTMICR (*(volatile uint32_t *)(TIMER0_BASE + 0x024))   // GPTM interrupt clear
#define GPTMTAILR (*(volatile uint32_t *)(TIMER0_BASE + 0x028)) // TimerA interval

#define SYSTEM_CONTROL_BASE 0x400FE000
#define RCGC1 (*(volatile uint32_t *)(SYSTEM_CONTROL_BASE + 0x104)) // Controls clocks for peripherals

#define NVIC_BASE 0xE000E000
#define EN0 (*(volatile uint32_t *)(NVIC_BASE + 0x100)) // Interrupts 0-31 Set/Enable

volatile uint32_t timer_ticks = 0;

void timer_init() {
  RCGC1 |= (1U << 16); // Enable the peripheral clock for Timer0

  GPTMCTL &= ~(1U);  // Disable TimerA
  GPTMCFG = 0U;      // 32-bit config
  GPTMTAMR = 2U;     // Periodic
  GPTMTAILR = 11999; // reload = timer_frequency (12 Mhz) × 0.001 - 1; Handler is called every 1ms
  GPTMIMR |= 1U;     // Enable timeout interrupts
  GPTMCTL |= 1U;     // Enable TimerA

  EN0 |= (1U << 19); // Allow Timer0A interrupt 19 through to the CPU
}

void Timer0_Handler() {
  GPTMICR = 1U;  // Clear this timeout
  timer_ticks++; // 1000 ticks = 1 second
}