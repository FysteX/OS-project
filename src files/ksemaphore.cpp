//
// Created by os on 9/10/24.
//

#include "../h/ksemaphore.hpp"

void* ksemaphore::operator new(size_t size) {
    return kmem_alocator::kalocate(size / MEM_BLOCK_SIZE + (size % MEM_BLOCK_SIZE > 0 ) ? 1 : 0);
}

void ksemaphore::operator delete(void* adress) {
    kmem_alocator::kfree(adress);
}

ksemaphore::~ksemaphore() {
    for(int i = 0; i < suspended_threads.get_size(); i++) {
        kscheduler::put_thread(suspended_threads.get());
    }
    suspended_threads.free();
}

//ako je -1 onda ne treba da ceka
int ksemaphore::wait(int time) {
    if(--val < 0) {
        kthread::running->block();
        suspended_threads.add(kthread::running);
        if(time >= 0) {
			kscheduler::put_to_sleep(time);
		}
    }
    return 0; // treba da se doda ako se desi greska
}

int ksemaphore::try_wait() {
    if(--val < 1) {
        return 0;
    } else {
        return 1;
    }
}

int ksemaphore::signal() {
    //ako nema niti koje cekaju na semaforu onda funkcija signal nema efekta
    if (suspended_threads.get_size() == 0) {
        val++;
        return 0;
    }
    if(++val < 1) {
        kthread* temp = suspended_threads.get();
        while(temp->blocked() == false) {
            temp = suspended_threads.get();
            if (temp == nullptr) return 0;
        }
        temp->unblock();
        if (temp->get_sleep_time() >= 0) {
            //treba se izbaci iz sleeping_threads       p
            temp->set_sleep_time(-1);
        }
        kscheduler::put_thread(temp);
    }
    return 0; // treba da se doda ako se desi greska
}

int ksemaphore::get_number_of_blocked_threads() {
    if (val >= 0) return 0;
    return -val;
}
