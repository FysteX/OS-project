//
// Created by os on 7/28/24.
//

#ifndef SYSCALL_C_HPP
#define SYSCALL_C_HPP
#include "../lib/hw.h"
#include "riscv.hpp"
#include "ksemaphore.hpp"

static const uint64 MEM_ALLOC_CODE = 0x01;
static const uint64 MEM_FREE_CODE = 0x02;

static const uint64 THREAD_CREATE_CODE = 0x11;
static const uint64 THREAD_EXIT_CODE = 0x12;
static const uint64 THREAD_DISPATCH_CODE = 0x13;

static const uint64 SEMAPHORE_OPEN_CODE = 0x21;
static const uint64 SEMAPHORE_CLOSE_CODE = 0x22;
static const uint64 SEMAPHORE_WAIT_CODE = 0x23;
static const uint64 SEMAPHORE_SIGNAL_CODE = 0x24;
static const uint64 SEMAPHORE_TIMEDWAIT_CODE = 0x25;
static const uint64 SEMAPHORE_TRYWAIT_CODE = 0x26;

static const uint64 TIME_SLEEP_CODE = 0x31;

static const uint64 GETC_CODE = 0x41;
static const uint64 PUTC_CODE = 0x42;

uint64 syscall(uint64 code, uint64 arg0, uint arg1, uint arg2);

void init_syscall(void(*user_main_start_routine)(void*));

void* mem_alloc(size_t size);

int mem_free (void*);

typedef kthread _thread;
typedef _thread* thread_t;
int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg, void* stack_space);
int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg);

int thread_exit();

void thread_dispatch();

typedef ksemaphore _sem;
typedef _sem* sem_t;
int sem_open (sem_t* handle, unsigned init);

int sem_close (sem_t handle);

int sem_wait (sem_t id);

int sem_signal (sem_t id);

int sem_timedwait(sem_t id, time_t timeout);

int sem_trywait(sem_t id);

typedef unsigned long time_t;
int time_sleep (time_t);

const int EOF = -1;
char getc ();

int putc (char);

#endif //SYSCALL_C_HPP
