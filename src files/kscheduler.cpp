//
// Created by os on 8/2/24.
//

#include "../h/kscheduler.hpp"

queue<kthread*> kscheduler::threads;
queue<kthread*> kscheduler::sleeping_threads;
kthread* kscheduler::main_thread = nullptr;
kthread* kscheduler::idle_thread;

void kscheduler::init() {
    threads.init();
}

void kscheduler::put_thread(kthread *thread) {
    if(!thread->is_finished()) {
        threads.add(thread);
    }
}

kthread *kscheduler::get_thread() {
    return threads.get();
}

int kscheduler::size() {
    return threads.get_size();
}

int kscheduler::put_to_sleep(time_t time) {
    if(kthread::running == nullptr) {return -1;}
    sleeping_threads.add(kthread::running);
    kthread::running->set_sleep_time(time);
    return 0;
}

void kscheduler::update_sleeping_threads() {
    kthread* temp;
    for(int i = 0; i < size_sleeping(); i++) {
         temp = sleeping_threads.get();
         temp->dec_sleep_time();
         if(temp->get_sleep_time() == 0) {
             put_thread(temp);
         }else {
             sleeping_threads.add(temp);
         }
    }
}

int kscheduler::size_sleeping() {
    return sleeping_threads.get_size();
}
