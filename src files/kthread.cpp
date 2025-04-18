//
// Created by os on 8/1/24.
//

#include "../h/kthread.hpp"
#include "../h/kscheduler.hpp"
#include "../lib/console.h"

static int br = 0;

kthread* kthread::running;

void kthread::start() {
    kscheduler::put_thread(this);
}

void* kthread::operator new(size_t size) {
    return kmem_alocator::kalocate(size / MEM_BLOCK_SIZE + (size % MEM_BLOCK_SIZE > 0 ) ? 1 : 0);
}

void kthread::operator delete(void* adress) {
    kmem_alocator::kfree(adress);
}

kthread* kthread::create_thread(body thread_body, uint64 sp) {
    return new kthread(thread_body, sp);
}

kthread::kthread(body thread_body, uint64 sp):
    thread_body(thread_body),
    id(br++),
    finished(false),
    stack((void*)(sp - DEFAULT_STACK_SIZE)),
    sepc((uint64)thread_body),
    ret(-1),
    sstatus(-1),
    started(false),
    blocked_flag(false),
    sleep_time(-1)
{
    thread_context = (context*)kmem_alocator::kalocate(sizeof(context) / MEM_BLOCK_SIZE + (sizeof(context) % MEM_BLOCK_SIZE) ? 1 : 0);
    thread_context->sp = sp;
}

void kthread::yield(kthread* old_thread, kthread* new_thread) {
    running = new_thread;
    if (old_thread == nullptr && new_thread->is_started() == false) {
     	new_thread->set_started();
    	switch_from_null_to_new(new_thread->get_context());
    } else if(old_thread == nullptr && new_thread->is_started() == true) {
        switch_from_null_to_started(new_thread->get_context());
    } else if(old_thread != nullptr && new_thread->is_started() == false) {
        new_thread->set_started();
        switch_fom_started_to_new(old_thread->get_context(), new_thread->get_context());
    } else if(old_thread != nullptr && new_thread->is_started() == true) {
        switch_from_started_to_started(old_thread->get_context(), new_thread->get_context());
    }
}

kthread::~kthread() {
    kmem_alocator::kfree(stack);
    kmem_alocator::kfree(thread_context);
}

