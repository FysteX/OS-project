//
// Created by os on 7/28/24.
//

#ifndef RISCV_HPP
#define RISCV_HPP

#include "../lib/console.h"
#include "../lib/hw.h"
#include "kscheduler.hpp"
#include "syscall_c.hpp"
#include "ksemaphore.hpp"

extern "C" void handle_supervisor_trap();

extern "C" void supervisor_trap();

enum BitMasks {
    MASK_0 = (1 << 0),
    MASK_1 = (1 << 1),
    MASK_5 = (1 << 5),
    MASK_8 = (1 << 8),
    MASK_9 = (1 << 9),
};

inline uint64 read_sepc() {
    uint64 volatile sepc;
    __asm__ volatile("csrr %[sepc], sepc" : [sepc] "=r" (sepc));
    return sepc;
}

inline void write_sepc(uint64 sepc) {
    __asm__ volatile ("csrw sepc, %[sepc]" : : [sepc] "r" (sepc));
}

inline uint64 read_scause() {
    uint64 volatile scause;
    __asm__ volatile("csrr %[scause], scause" : [scause] "=r" (scause));
    return scause;
}

inline void write_scause(uint64 scause) {
    __asm__ volatile ("csrw scause, %[scause]" : : [scause] "r" (scause));
}

inline uint64 read_sstatus() {
    uint64 volatile sstatus;
    __asm__ volatile("csrr %[sstatus], sstatus" : [sstatus] "=r" (sstatus));
    return sstatus;
}

inline void write_sstatus(uint64 sstatus) {
    __asm__ volatile ("csrw sstatus, %[sstatus]" : : [sstatus] "r" (sstatus));
}
//mask clear
inline void mc_sstatus(uint64 mask) {
    __asm__ volatile ("csrc sstatus, %[mask]" : : [mask] "r" (mask));
}
//mask set
inline void ms_sstatus(uint64 mask) {
    __asm__ volatile ("csrs sstatus, %[mask]" : : [mask] "r"(mask));
}

inline void ms_sie(uint64 mask) {
    __asm__ volatile ("csrs sie, %[mask]" : : [mask] "r"(mask));
}

inline void mc_sie(uint64 mask) {
    __asm__ volatile ("csrc sie, %[mask]" : : [mask] "r"(mask));
}

inline void ms_sip(uint64 mask) {
    __asm__ volatile ("csrs sip, %[mask]" : : [mask] "r"(mask));
}

inline void mc_sip(uint64 mask) {
    __asm__ volatile ("csrc sip, %[mask]" : : [mask] "r"(mask));
}

inline uint64 read_sip() {
    uint64 volatile sip;
    __asm__ volatile ("csrr %[sip], sip" : [sip] "=r"(sip));
    return sip;
}

inline void write_sip(uint64 sip) {
    __asm__ volatile ("csrw sip, %[sip]" : : [sip] "r"(sip));
}
inline uint64 read_a0() {
    uint64 volatile a0;
    __asm__ volatile("mv %[a0], a0" : [a0] "=r" (a0));
    return a0;
}

inline void write_a0 (uint64 val) {
    __asm__ volatile("mv a0, %[a0]" : : [a0] "r" (val));
}

inline uint64 read_a1() {
    uint64 val;
    __asm__ volatile("mv %[val], a1" : [val] "=r" (val));
    return val;
}

inline uint64 read_a2() {
    uint64 val;
    __asm__ volatile("mv %[val], a2" : [val] "=r" (val));
    return val;
}

inline uint64 read_a3() {
    uint64 val;
    __asm__ volatile("mv %[val], a3" : [val] "=r" (val));
    return val;
}

inline uint64 read_a4() {
    uint64 val;
    __asm__ volatile("mv %[val], a4" : [val] "=r" (val));
    return val;
}
#endif //RISCV_HPP
