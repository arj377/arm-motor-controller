# Bare-Metal ARM Motor Controller

A bare-metal real-time motor control system built from scratch in C for an ARM Cortex-M3, running on the TI Stellaris LM3S6965 platform in QEMU.

The project implements the firmware stack without an operating system, RTOS, standard library, or vendor HAL - including startup code, memory initialization, interrupt handling, peripheral drivers, PWM control, and closed-loop motor speed control.

## Current Features

- Custom ARM Cortex-M3 startup code and interrupt vector table
- Custom linker script defining Flash and SRAM layout
- `.data` initialization from Flash to RAM
- `.bss` zero initialization
- Memory-mapped peripheral drivers written directly against hardware registers
- Interrupt-driven UART RX with polling TX
- UART receive ring buffer
- 1 ms hardware timer interrupt
- 20 kHz PWM motor output
- GPIO-based motor direction control
- Signed motor command interface (`-100` to `+100`)
- Fixed-rate 100 Hz closed-loop control task
- Software motor/plant model for QEMU feedback
- PID speed controller using integer arithmetic
- Output saturation and integral anti-windup
- Runtime target-speed changes

## Architecture

The system is organized into hardware-facing drivers, control logic, and a simulated plant:

    +-------------------+
    |   Target Speed    |
    +---------+---------+
              |
              v
    +-------------------+
    |  PID Controller   |
    |    control.c      |
    +---------+---------+
              |
         -100 ... +100
              |
       +------+------+
       |             |
       v             v
    +--------+   +-------------+
    | Motor  |   | Motor Model |
    |  HAL   |   |   (QEMU)    |
    +---+----+   +------+------+
        |               |
     PWM + GPIO          |
                        RPM
                         |
                         +------> feedback to controller

`motor.c` contains the hardware-facing motor interface. It converts a signed motor command into PWM magnitude and GPIO direction.

`motor_model.c` represents the physical motor and encoder that are unavailable in QEMU. It converts the applied motor command into simulated RPM with a simple first-order response.

`control.c` compares the target speed against measured speed and adjusts the motor command using feedback.

## Bare-Metal Startup

The firmware boots directly on the Cortex-M3 without an operating system or runtime.

The custom startup path:

1. Loads the initial stack pointer from the interrupt vector table.
2. Enters `Reset_Handler`.
3. Copies initialized `.data` from its load address in Flash to its runtime address in SRAM.
4. Zero-initializes `.bss`.
5. Calls `main()`.

Memory layout:

    Flash: 0x00000000 - 256 KB
    SRAM:  0x20000000 - 64 KB

The initial stack pointer is placed at the top of SRAM (`0x20010000`).

## Peripheral Drivers

### UART

UART0 is controlled directly through memory-mapped registers.

RX is interrupt-driven: incoming characters generate a UART interrupt and are placed into a software ring buffer. TX currently uses polling.

### Timer

Timer0 generates a periodic interrupt every 1 ms.

The interrupt handler increments a global tick counter while higher-level scheduling remains outside the ISR.

The closed-loop controller runs every 10 ticks, producing a deterministic 100 Hz control loop.

### PWM

The PWM driver is configured for a 20 kHz motor-control signal.

With a 12 MHz system clock and `/2` PWM clock divider:

    PWM clock = 6 MHz
    6,000,000 / 20,000 = 300 ticks

Therefore:

    LOAD = 299

Duty cycle is controlled by updating the PWM comparator.

### Motor HAL

The motor interface accepts signed commands:

    +100 = full-power forward
       0 = stopped
    -100 = full-power reverse

Command sign controls a GPIO direction pin while command magnitude controls PWM duty cycle.

## Closed-Loop Speed Control

The project initially used open-loop motor commands, where software directly selected motor power without knowing the resulting speed.

The current controller instead accepts a target speed:

    target speed
         |
         v
    target - measured
         |
         v
       error
         |
         v
        PID
         |
         v
    motor command
         |
         v
       motor
         |
         +---- measured speed ----+

The controller currently runs at 100 Hz.

### PID Controller

The controller contains proportional, integral, and derivative terms using integer arithmetic rather than floating point.

The proportional term reacts to current error.

The integral term accumulates past error, allowing the controller to eliminate the steady-state error observed with proportional-only control.

The derivative term responds to changes in error and includes special handling for the first control iteration to prevent a startup derivative spike.

The controller also implements output saturation and anti-windup so the integral term does not continue growing when the actuator is already saturated.

## QEMU Motor Simulation

QEMU emulates the Cortex-M3 CPU, memory, interrupts, timers, UART, and GPIO used by the firmware.

It does not emulate the LM3S6965 PWM peripheral or a physical motor/encoder.

For that reason, the project separates the real hardware-facing motor HAL from a software plant model.

The model currently assumes a maximum motor speed of 3000 RPM and maps motor commands from `-100` to `+100` onto the corresponding target motor speed. Motor inertia is approximated by moving the simulated speed toward that value on each update.

This allows the complete real-time feedback architecture to be developed and tested while preserving a hardware abstraction that can later be connected to a physical motor and encoder.

## Validation

The firmware has been tested through QEMU and ARM GDB.

Validated behavior includes:

- Cortex-M3 startup and memory initialization
- Timer interrupt execution
- UART interrupt handling
- GPIO motor-direction control
- Fixed-rate 100 Hz controller execution
- Proportional, integral, and derivative controller state
- Integral anti-windup
- Runtime target-speed changes
- Closed-loop convergence

In one target-step test, the controller was allowed to settle near 1500 RPM before the target was changed at runtime to 2500 RPM.

After settling:

    Target speed:     2500 RPM
    Simulated speed:  2463 RPM
    Error:              37 RPM
    Controller output:  82%

The expected steady-state motor command for the model at 2500 RPM is approximately 83%, closely matching the controller output.

## Hardware Validation

The PWM implementation follows the LM3S6965 register interface, but QEMU does not emulate the PWM peripheral.

Therefore PWM waveform generation has not been electrically validated.

On physical hardware, the next validation steps would include:

- Measuring the 20 kHz PWM waveform with an oscilloscope or logic analyzer
- Verifying duty-cycle changes
- Connecting a motor driver and motor
- Replacing the software plant with encoder feedback
- Measuring real control-loop timing and jitter

## Project Status

Completed:

- Bare-metal startup and linker configuration
- UART driver
- Timer interrupts
- PWM driver
- Motor hardware abstraction
- Simulated motor feedback
- Closed-loop PID speed control
- Anti-windup and output saturation
- Runtime target-step validation

In progress:

- UART command interface and telemetry
- Fault handling and safety
- Real-time architecture cleanup
- Timing benchmarks and final validation
- Final documentation
