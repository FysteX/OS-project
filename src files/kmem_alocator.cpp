//
// Created by os on 7/24/24.
//

#include "../h/kmem_alocator.hpp"
#include "../lib/console.h"

mem_block* kmem_alocator::first_alocated_block_;

void kmem_alocator::init_alocator() {
    first_alocated_block_ = nullptr;
}

mem_block *kmem_alocator::init_mem_block(uint8 *adress, size_t size) {
    mem_block* new_block = (mem_block*)adress;
    *new_block = {(uint8*)new_block + MEM_BLOCK_SIZE, size, nullptr};
    return new_block;
}


void* kmem_alocator::kalocate(size_t size) {
    if(first_alocated_block_ == nullptr) {
        first_alocated_block_ = (mem_block*)HEAP_START_ADDR;
        *first_alocated_block_ = {(uint8*)first_alocated_block_ + MEM_BLOCK_SIZE, size, nullptr};
        return first_alocated_block_->adress;
    }
    mem_block* new_block;
    if(first_alocated_block_ != HEAP_START_ADDR) {
        if((size_t)(first_alocated_block_ - (mem_block*)HEAP_START_ADDR) >= size * MEM_BLOCK_SIZE) {
            new_block = init_mem_block((uint8*)HEAP_START_ADDR, size);
            new_block->next = first_alocated_block_;
            first_alocated_block_ = new_block;
            return new_block->adress;
        }
    }
    mem_block* temp;
    for(temp = first_alocated_block_ ; temp->next != nullptr ; temp = temp->next ){
        /*if((size_t)((uint8*)temp->next - (temp->adress + temp->size * MEM_BLOCK_SIZE)) >= size * MEM_BLOCK_SIZE ) {
            new_block = init_mem_block(temp->adress + temp->size * MEM_BLOCK_SIZE, size);
            new_block->next = temp->next;
            temp->next = new_block;
            //__putc('d');
            return (void*)new_block->adress;
        }*/
    }
    if((size_t)((uint8*)HEAP_END_ADDR - (temp->adress + temp->size * MEM_BLOCK_SIZE)) >= size * MEM_BLOCK_SIZE ) {
        new_block = init_mem_block(temp->adress + temp->size * MEM_BLOCK_SIZE, size);
        temp->next = new_block;
        return (void*)new_block->adress;
    } else {
        return nullptr;
    }
}

int kmem_alocator::kfree(void *adress) {
    if(first_alocated_block_->adress == (uint8*)adress) {
        first_alocated_block_ = first_alocated_block_->next;
        return 0;
    }
    mem_block* prev = first_alocated_block_;
    for(mem_block* temp = first_alocated_block_->next ; temp != nullptr ; temp = temp->next ) {
        if(temp->adress == adress) {
            prev->next = temp->next;
            return 0;
        }
        prev = temp;
    }
    return -1;
}
