#include <stdint.h>

void Reset_Handler(void);
int main(void);
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sidata;

__attribute__((section(".isr_vector")))
uintptr_t vectorTable []= {0x20010000, (uintptr_t)&Reset_Handler}; //Addresses are where the SP and PC are read from

void Reset_Handler (void) {

    // Zero in uninitialized variables
    for (uint32_t *bssPtr = &_sbss; bssPtr < &_ebss; bssPtr++) {
        *bssPtr = 0;
    }

    uint32_t* flashPtr = &_sidata;
    for (uint32_t *dataPtr = &_sdata; dataPtr < &_edata; dataPtr++) {
        *dataPtr = *flashPtr;
        flashPtr++;
    }

    main();
    while(1) {}
}