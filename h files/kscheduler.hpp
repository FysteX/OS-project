//
// Created by os on 8/2/24.
//

#ifndef KSCHEDULER_HPP
#define KSCHEDULER_HPP
#include "queue.hpp"
#include "kthread.hpp"

class kscheduler {
private:
    static queue<kthread*> threads;
    static queue<kthread*> sleeping_threads;

public:

    static kthread* main_thread;

    static kthread* idle_thread;

    static kthread* get_thread();

    static void put_thread(kthread* thread);

    static int put_to_sleep(time_t time);

    static void update_sleeping_threads();

    static void init();

    static int size();

    static int size_sleeping();
};



#endif //KSCHEDULER_HPP
