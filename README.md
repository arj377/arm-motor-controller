# Bare-Metal ARM Cortex-M3 Motor Controller

A bare-metal real-time motor-control system built from scratch in C for an ARM Cortex-M3, running on the QEMU-emulated TI Stellaris LM3S6965EVB.

The project implements the firmware stack from reset vector to closed-loop control without an operating system, RTOS, HAL, or runtime. It includes custom startup code, memory layout, MMIO peripheral drivers, interrupt-driven timing, PWM and GPIO motor control, UART commands and telemetry, PID speed control, and latched safety faults.

## Features

- Bare-metal ARM Cortex-M3 firmware
- Custom vector table and reset handler
- Custom linker script and FLASH/RAM initialization
- Memory-mapped peripheral drivers
- Interrupt-driven 1 ms hardware timer
- 20 kHz PWM motor output
- GPIO-based bidirectional motor control
- 100 Hz closed-loop speed controller
- Integer PID with saturation and anti-windup
- UART command interface and telemetry
- Emergency-stop handling
- Overspeed detection
- Latched faults with explicit recovery
- Missed real-time deadline detection

## Architecture

The control path is:

    UART setpoint
         |
         v
    PID Controller
         |
         v
    Motor HAL ------> PWM + Direction GPIO
         |
         v
    Motor Model
         |
         v
    Measured Speed
         |
         +----------> PID feedback

A hardware timer generates a 1 ms system tick. The main loop uses that timebase to schedule the control loop every 10 ms (100 Hz).

Interrupt handlers are kept minimal. Control, safety checking, command parsing, and telemetry execute outside interrupt context.

## Bare-Metal Startup

The firmware boots without an operating system or standard runtime.

The startup path:

1. Cortex-M3 loads the initial stack pointer and reset vector.
2. `Reset_Handler` copies initialized `.data` from FLASH to SRAM.
3. `.bss` is zero-initialized.
4. Execution enters `main()`.

The custom linker script defines a 256 KB FLASH region and 64 KB SRAM region and places the vector table, program code, initialized data, and zero-initialized data explicitly.

## Motor Control

The motor interface separates hardware-facing control from the feedback controller.

The motor HAL converts signed controller output into:

- Output magnitude -> PWM duty cycle
- Output sign -> GPIO direction

The PWM driver is configured for a 20 kHz output.

The controller accepts speed targets between:

    -3000 RPM and +3000 RPM

Values outside this range are clamped.

## Closed-Loop Controller

Speed control runs at 100 Hz using an integer PID controller.

Each iteration computes:

    error = target_speed - measured_speed

and applies proportional, integral, and derivative terms before clamping the resulting motor command to:

    -100% <= output <= +100%

The controller includes saturation-aware integral anti-windup and suppresses derivative kick during initialization.

## Real-Time Scheduling

The control loop runs every 10 ms using the millisecond hardware-timer timebase.

Rather than replaying multiple stale controller iterations when execution falls behind, the scheduler detects skipped periods, advances to the current timing phase, and executes one current control iteration.

Missed periods are recorded through a deadline counter.

Normal QEMU validation produced:

    Control frequency:          100 Hz
    Missed control deadlines:  0

## Performance

For a 1500 RPM steady-state target:

    Observed speed:             ~1495-1505 RPM
    Steady-state error:         ~±5 RPM
    Relative error:             ~0.33%

For a 1500 -> 2500 RPM step:

    Final speed:                ~2499-2501 RPM
    Final error:                ~±1 RPM
    Time to enter ±1% band:     2.10 s
    Missed deadlines:           0

The controller also successfully performs full bidirectional reversals, including +2500 RPM -> -1500 RPM.

## Safety

The firmware implements latched safety states:

    FAULT_NONE
    FAULT_OVERSPEED
    FAULT_EMERGENCY_STOP

An emergency stop immediately:

1. Sets hardware motor output to zero.
2. Sets the simulated plant command to zero.
3. Resets PID state and target speed.
4. Latches the emergency-stop fault.

While faulted, normal controller execution is blocked.

Faults require an explicit clear command. Clearing a fault leaves the target at 0 RPM, preventing the motor from unexpectedly restarting with a previous setpoint.

Overspeed protection uses a 3200 RPM safety threshold and was validated using debugger fault injection.

## UART Interface

Commands can be issued while the controller is running:

    s <rpm>     Set target speed
    x           Emergency stop
    c           Clear fault

Example:

    s 2500

Telemetry reports:

    target
    measured speed
    control error
    controller output
    safety state
    missed control deadlines

## Validation

The firmware was tested for:

- Positive and negative speed commands
- 1500 -> 2500 RPM step response
- +2500 -> -1500 RPM reversal
- ±3000 RPM target clamping
- PID saturation and anti-windup
- Emergency-stop activation
- Latched fault behavior
- Safe fault recovery
- Overspeed fault injection
- Real-time deadline tracking

## QEMU and Hardware Model

QEMU emulates the ARM Cortex-M3 processor and the LM3S6965EVB platform, allowing the firmware's startup path, memory accesses, UART, timers, interrupts, NVIC behavior, and GPIO control to execute against emulated hardware.

QEMU does not provide a physical motor or encoder, so the closed-loop plant and speed feedback are implemented with a software motor model.

The PWM driver is implemented against the LM3S6965 peripheral registers, but QEMU does not fully emulate the board's PWM peripheral. Physical PWM waveform timing and motor behavior therefore require validation on real hardware.

This distinction keeps the project focused on real bare-metal firmware architecture while avoiding claims about physical hardware behavior that cannot be measured in QEMU.

## Build

Requires the ARM GNU Toolchain and QEMU.

Build:

    arm-none-eabi-gcc -g -mcpu=cortex-m3 -mthumb -nostdlib -T linker.ld \
        startup.c main.c uart.c timer.c pwm.c motor.c motor_model.c \
        control.c command.c telemetry.c safety.c -o firmware.elf

Run:

    qemu-system-arm -M lm3s6965evb -kernel firmware.elf -nographic

Debug:

    qemu-system-arm -M lm3s6965evb -kernel firmware.elf \
        -nographic -S -gdb tcp::1234

Then connect with:

    arm-none-eabi-gdb firmware.elf

and:

    target remote :1234

## What I Learned

This project was built to understand what sits underneath embedded frameworks and RTOS abstractions: processor startup, linking and memory initialization, memory-mapped I/O, interrupts, peripheral configuration, deterministic scheduling, feedback control, and fault handling.

Building each layer directly made the hardware/software boundary part of the design rather than an abstraction hidden behind a vendor HAL.
