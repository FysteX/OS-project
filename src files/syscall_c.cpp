//
// Created by os on 7/28/24.
//

#include "../h/syscall_c.hpp"

uint64 syscall(uint64 code, uint64 arg0, uint64 arg1, uint64 arg2, uint64 arg3) {
    uint64 ret;
    __asm__ volatile("mv a0, %[code]" : : [code] "r" (code));
    __asm__ volatile("mv a1, %[arg0]" : : [arg0] "r" (arg0));
    __asm__ volatile("mv a2, %[arg1]" : : [arg1] "r" (arg1));
    __asm__ volatile("mv a3, %[arg2]" : : [arg2] "r" (arg2));
    __asm__ volatile("mv a4, %[arg3]" : : [arg3] "r" (arg3));
    __asm__ volatile("ecall");
    __asm__ volatile("mv %[ret], a0" : [ret] "=r" (ret));
    return ret;
}

void init_syscall(void(*user_main_start_routine)(void*)) {
    syscall(0x0, (uint64)user_main_start_routine, (uint64)0, (uint64)0, (uint64)0);
}

void *mem_alloc(size_t size) {
    size_t size_in_blocks;
    size_in_blocks = size / MEM_BLOCK_SIZE + ((size % MEM_BLOCK_SIZE > 0 ) ? 1 : 0);
    return (void*)syscall(MEM_ALLOC_CODE, size_in_blocks, (uint64)0, (uint64)0, (uint64)0);
}

int mem_free(void* adress) {
    return syscall(MEM_FREE_CODE, (uint64) adress, (uint64)0, (uint64)0, (uint64)0);
}

int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg, void* stack_space) {
    return syscall(THREAD_CREATE_CODE, (uint64)handle, (uint64)start_routine, (uint64)arg, (uint64)stack_space);
}

int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg) {
    void* stack_space = mem_alloc(DEFAULT_STACK_SIZE);
    return thread_create(handle, start_routine, arg, (void*)((uint64)stack_space + DEFAULT_STACK_SIZE));
}

int thread_exit() {
	return syscall(THREAD_EXIT_CODE, (uint64)0, (uint64)0, (uint64)0, (uint64)0);
}

void thread_dispatch() {
	syscall(THREAD_DISPATCH_CODE, (uint64)0, (uint64)0, (uint64)0, (uint64)0);
}

int sem_open (sem_t* handle, unsigned init) {
	return syscall(SEMAPHORE_OPEN_CODE, (uint64) handle, (uint64) init, (uint64)0, (uint64)0);
}

int sem_close (sem_t handle) {
    return syscall(SEMAPHORE_CLOSE_CODE, (uint64) handle, (uint64)0, (uint64)0, (uint64)0);
}

int sem_wait (sem_t id) {
    return syscall(SEMAPHORE_WAIT_CODE, (uint64) id, (uint64)0, (uint64)0, (uint64)0);
}

int sem_signal (sem_t id) {
    return syscall(SEMAPHORE_SIGNAL_CODE, (uint64) id, (uint64)0, (uint64)0, (uint64)0);
}

int sem_timedwait(sem_t id, time_t timeout) {
    return syscall(SEMAPHORE_TIMEDWAIT_CODE, (uint64) id, (uint64) timeout, (uint64)0, (uint64)0);
}

int sem_trywait(sem_t id) {
    return syscall(SEMAPHORE_TRYWAIT_CODE, (uint64) id, (uint64)0, (uint64)0, (uint64)0);
}

int time_sleep(time_t time) {
    return syscall(TIME_SLEEP_CODE, (uint64)time, (uint64)0, (uint64)0, (uint64)0);
}

char getc() {
    return (char)syscall(GETC_CODE, (uint64)0, (uint64)0, (uint64)0, (uint64)0);
}

int putc(char c) {
    return syscall(PUTC_CODE, (uint64)c, (uint64)0, (uint64)0, (uint64)0);
}

