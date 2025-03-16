//
// Created by os on 9/10/24.
//

#ifndef KSEMAPHORE_HPP
#define KSEMAPHORE_HPP
#include "kthread.hpp"
#include "queue.hpp"
#include "kscheduler.hpp"

class ksemaphore {
private:
  int val;
  queue<kthread*> suspended_threads;

public:
  void* operator new(size_t size);

  void operator delete(void* adress);

  ksemaphore(int val) : val(val) {suspended_threads.init(); }

  ~ksemaphore();

  int wait(int time);

  int try_wait();

  int signal();

  int get_number_of_blocked_threads();
};



#endif //KSEMAPHORE_HPP
