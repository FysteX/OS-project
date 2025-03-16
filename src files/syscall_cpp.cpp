//
// Created by os on 9/11/24.
//

#include "../h/syscall_cpp.hpp"

void* operator new (size_t size) {
    return mem_alloc(size);
}

void operator delete (void* obj) {
    mem_free(obj);
}

Thread::Thread(void (*body)(void*), void* arg): body(body), arg(arg) {}

Thread::Thread():body(nullptr) {}

Thread::~Thread() {}

int Thread::start() {
    return thread_create(&myHandle, thread_start, this);
}

void Thread::dispatch() {
	thread_dispatch();
}

int Thread::sleep(time_t time) {
	time_sleep(time);
    return 0;
}

void Thread::thread_start(void *arg) {
    Thread *thread = (Thread*) arg;
    if (thread->body) {
        thread->body(thread->arg);
    } else {
        thread->run();
    }
    delete thread;
}


Semaphore::Semaphore(unsigned init) {
	sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::wait() {
	return sem_wait(myHandle);
}

int Semaphore::signal() {
	return sem_signal(myHandle);
}

int Semaphore::timedWait(time_t timeout) {
    return sem_timedwait(myHandle, timeout);
}

int Semaphore::tryWait() {
    return sem_trywait(myHandle);
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}