//
// Created by os on 8/1/24.
//

#ifndef QUEUE_HPP
#define QUEUE_HPP
#include "kmem_alocator.hpp"
#include "../lib/hw.h"

template <typename T>
class queue {
private:

    T* array;
    int max_size;
    int size;
    int head;
    int tail;

public:

    void add(T elem);

    T get();

    int init();

    void free();

    void extend();      //kad ekstendujes, ubacujes u extendovani niz redom kako je u starom

    int get_size();

};

template<typename T>
int queue<T>::init() {
    max_size = 10;
    size_t size_of_blocks = (sizeof(T) * max_size) / MEM_BLOCK_SIZE + ((sizeof(T) * max_size) % MEM_BLOCK_SIZE > 0) ? 1 : 0;
    size = 0;
    head = 0;
    tail = -1;
    array = (T*)kmem_alocator::kalocate(size_of_blocks);
    if(array == nullptr) return -1;
    return 0;
}

template<typename T>
void queue<T>::add(T elem) {
    if(size == max_size) {
        extend();
    }
    if(size == 0) tail = head;
    array[head++] = elem;
    head %= max_size;
    size++;
}

template<typename T>
T queue<T>::get() {
    if (size == 0) return nullptr;
    size--;
    T ret = array[tail++];
    tail %= max_size;
    return ret;
}

template<typename T>
void queue<T>::extend() {
    max_size += 10;
    size_t size_of_blocks = (sizeof(T) * max_size) / MEM_BLOCK_SIZE + ((sizeof(T) * max_size) % MEM_BLOCK_SIZE > 0) ? 1 : 0;
    T* new_array;
    new_array = (T*)kmem_alocator::kalocate(size_of_blocks);
    for(int i = 0 ; i < size ; i++) {
        new_array[i] = array[(i + tail) % size];
    }
    kmem_alocator::kfree(array);
    array = new_array;
    tail = 0;
    head = size;
}

template<typename T>
int queue<T>::get_size() {
  return size;
}

template<typename T>
void queue<T>::free() {
    kmem_alocator::kfree(array);
}

#endif //QUEUE_HPP
