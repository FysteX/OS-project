//
// Created by os on 8/1/24.
//

#ifndef KTHREAD_HPP
#define KTHREAD_HPP
#include "../lib/hw.h"
#include "../lib/console.h"

using body = void(*)(void*);

class kthread {
private:

    struct context {
        uint64 sp;
    };

    body thread_body;
    int id;
    bool finished;
    void* stack;
    uint64 sepc;
    uint64 ret;
    uint64 sstatus;
    bool started;
    bool blocked_flag;
    int sleep_time;
    void* arg;
    context* thread_context;

public:

    void start();

    bool is_started() {return this->started;}

    void set_started() {this->started = true;}

    int get_id() {return id;}

    uint64 get_ret() {return ret;}

    void set_ret(uint64 ret) {this->ret = ret;}

    uint64 get_sepc() {return sepc;}

    void set_sepc(uint64 sepc) { this->sepc = sepc; }

    uint64 get_sstatus() {return sstatus; }

    void set_sstatus(uint64 sstatus) { this->sstatus = sstatus; }

    context* get_context() {return thread_context;}

    body get_thread_body() {return thread_body;}

    void block() { blocked_flag = true; }

    void unblock() { blocked_flag = false; }

    bool blocked() {return blocked_flag;}

    void set_sleep_time(time_t time) {sleep_time = time;}

    void dec_sleep_time() { if(sleep_time > 0) {sleep_time--;} }

    int get_sleep_time() { return sleep_time; }

    void* operator new (size_t size);

    void operator delete(void* arg);

    body get_body() {return thread_body;}

    bool is_finished() {return finished;}

    void set_finished() {finished = true;}

    void set_arg(void* arg) { this->arg = arg;}

    void* get_arg() {return this->arg;}

    static kthread* create_thread(body thread_body, uint64 sp);

    static kthread* running;

    static void switch_from_null_to_new(context* new_context);

    static void switch_from_null_to_started(context* new_context);

    static void switch_fom_started_to_new(context* old_context, context* new_context);

    static void switch_from_started_to_started(context* old_context, context* new_context);

    static void yield(kthread* old_thread, kthread* new_thread);

    kthread(body thread_body, uint64 sp);

    ~kthread();

};



#endif //KTHREAD_HPP
