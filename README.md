# OS-project
Multithreaded operating system with time sharing. This operating system also provides threads, semaphores, asynchronous context switch, preemption and interrupts from timer and keyboard.

This project was task i was given at my university and it has to be runned inside educational operating system xv6.
Kernel was implemented for RISC-V processor architecture.
This xv6 system has been significantly modified for this occasion by removing the many functionalities (context switching and process scheduling, memory management,
file subsystem, disk management, etc.).
The host system has a role only to start itself and initializes the target hardware, then creates only one process with a virtual address space
which occupies the entire available physical memory, loads the code of the programs that make it realized kernel and its associated application and then starts its execution in the context that one process.
Besides that, host system also provides basic hardware services such as: periodic timer interrupt and console access.

How to run:
1. Install the xv6 virtual machine, here's the link: https://drive.google.com/file/d/1edGYFcvdnV0pbKws_1G1vePtEec0qC0G/view
password: OS
2. Open project base from clion installed on virtual machine.
3. Copy h and src files into project base.
4. In Makefile delete 12. and 13. line of code (deleting this means that student have decided to complete whole project).

syscal_c.hpp is the C API.
syscal_cpp.hpp is the CPP API.
