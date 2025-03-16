//
// Created by os on 12/20/24.
//

#include "../h/kconsole.hpp"

char* kconsole::o_char_buffer;
int kconsole::o_head;
int kconsole::o_tail;
int kconsole::o_size;

char* kconsole::i_char_buffer;
int kconsole::i_head;
int kconsole::i_tail;
int kconsole::i_size;

void kconsole::init() {
    o_char_buffer = (char*)kmem_alocator::kalocate((64*sizeof(char)) / MEM_BLOCK_SIZE + ((100*sizeof(char) % MEM_BLOCK_SIZE > 0 ) ? 1 : 0));
    o_head = 0;
    o_tail = 0;
    o_size = 0;
    i_char_buffer = (char*)kmem_alocator::kalocate((64*sizeof(char)) / MEM_BLOCK_SIZE + ((100*sizeof(char) % MEM_BLOCK_SIZE > 0 ) ? 1 : 0));
    i_head = 0;
    i_tail = 0;
    i_size = 0;

}

int kconsole::output_buffer_put_char(char c) {
    if (o_size == 64) return 1;
    o_char_buffer[o_head++] = c;
    o_head %= 100;
    o_size++;
    return 0;
}

bool kconsole::output_buffer_empty() {
    if (o_size == 0) {
        return true;
    }
    return false;
}

char kconsole::output_buffer_get_char() {
    o_size--;
    char c =  o_char_buffer[o_tail++];
    o_tail %= 100;
    return c;
}

int kconsole::input_buffer_put_char(char c) {
    if (i_size == 64) return 1;
    i_char_buffer[i_head++] = c;
    i_head %= 100;
    i_size++;
    return 0;
}

bool kconsole::input_buffer_empty() {
    if (i_size == 0) {
        return true;
    }
    return false;
}


char kconsole::input_buffer_get_char() {
    i_size--;
    char c =  i_char_buffer[i_tail++];
    i_tail %= 100;
    return c;
}

void kconsole::print_string(const char* string) {
    while (*string != '\0') {
        output_buffer_put_char(*string);
        string++;
    }
}



