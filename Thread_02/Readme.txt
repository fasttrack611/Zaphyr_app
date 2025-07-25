Zephyr Boot Flow: Step-by-Step
1. Reset and Early Boot
The processor starts at the reset vector.
Zephyr’s architecture-specific startup code (assembly + C) initializes:
Stack pointer
BSS and data sections
CPU features (MMU, FPU, etc.)
Basic hardware setup

2. Kernel Initialization
Zephyr initializes its core kernel subsystems:
Interrupts
Memory management
Scheduler
System clocks
Threading infrastructure

3. System Initialization Levels
Zephyr uses a multi-stage init system to control when components are initialized. These stages are:

Init Level	Description
PRE_KERNEL_1	Early hardware setup (e.g., clocks, timers)
PRE_KERNEL_2	Device drivers that don’t need kernel services
POST_KERNEL	Drivers needing kernel services (e.g., I2C, UART)
APPLICATION	Application-level init (e.g., user threads, app logic)
SMP	Multi-core setup (if enabled)
Each level runs in order, and within each level, components are initialized by priority (lower number = higher priority).
