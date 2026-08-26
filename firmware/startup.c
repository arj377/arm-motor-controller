#include <stdint.h>

void Reset_Handler(void);
int main(void);
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sidata;
extern void UART0_Handler(void);
extern void Timer0_Handler(void);

__attribute__((section(".isr_vector")))

// Stores inital SP value and handler addresses
uintptr_t vectorTable[36] = {[0] = 0x20010000,
                             [1] = (uintptr_t)&Reset_Handler,
                             [21] = (uintptr_t)&UART0_Handler,
                             [35] = (uintptr_t)&Timer0_Handler};

void Reset_Handler(void) {

  // Zero in uninitialized variables
  for (uint32_t *bssPtr = &_sbss; bssPtr < &_ebss; bssPtr++) {
    *bssPtr = 0;
  }

  uint32_t *flashPtr = &_sidata;
  for (uint32_t *dataPtr = &_sdata; dataPtr < &_edata; dataPtr++) {
    *dataPtr = *flashPtr;
    flashPtr++;
  }

  main();
  while (1) {
  }
}