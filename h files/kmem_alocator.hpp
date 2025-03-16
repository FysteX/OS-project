//
// Created by os on 7/28/24.
//

#ifndef KMEM_ALOCATOR_HPP
#define KMEM_ALOCATOR_HPP
#include "../lib/hw.h"

struct mem_block  {
    uint8* adress;
    size_t size;
    mem_block* next;
};

class kmem_alocator {
private:

    static mem_block* init_mem_block(uint8* adress, size_t size);

    static mem_block* first_alocated_block_;

public:

    static void init_alocator();

    static void* kalocate(size_t size);

    static int kfree(void* adress);
};

#endif //KMEM_ALOCATOR_HPP
