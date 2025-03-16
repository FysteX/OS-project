//
// Created by os on 12/20/24.
//

#ifndef KCONSOLE_HPP
#define KCONSOLE_HPP

#include "syscall_c.hpp"
#include "kthread.hpp"


class kconsole {
private:

// o je oznaka za output
    static char* o_char_buffer;
    static int o_head;
    static int o_tail;
    static int o_size;
    // i je onzaka za input
    static char* i_char_buffer;
    static int i_head;
    static int i_tail;
    static int i_size;

public:

    static void init();

    static kthread* output_thread;

    static int output_buffer_put_char(char c);

    static char output_buffer_get_char();

    static bool output_buffer_empty();

    static int input_buffer_put_char(char c);

    static char input_buffer_get_char();

    static bool input_buffer_empty();

    static void print_string(const char*);
};



#endif //KCONSOLE_HPP
