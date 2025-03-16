#include "../h/syscall_cpp.hpp"

void userMain(void* arg) {
    putc('H');putc('e');putc('l');putc('l');putc('o');putc(' ');putc('w');putc('o');putc('r');putc('l');putc('d');putc('!');putc('\n');
}

void main() {
    __asm__ volatile("csrw stvec, %[vector]" : : [vector] "r" (&supervisor_trap));
    init_syscall(userMain);
}


