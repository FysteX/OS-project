//
// Created by os on 7/28/24.
//

#include "../h/riscv.hpp"
#include "../h/kconsole.hpp"
//#include "main.cpp"

bool kernel_initialized = false;

kthread* kconsole::output_thread;

static bool kernel_finished = false;

static bool output_thread_blocked_on_buffer_sem = true;

static bool output_thread_blocked_on_status_sem = true;

static ksemaphore* koutput_buffer_sem;   // semafor koji sluzi da se "output thread" blokira ako je buffer prazan

static ksemaphore* koutput_status_sem;  // semafor koji sluzi da se "output thread" blokira ako kontrol periferije nije spreman da prima podatke

static ksemaphore* kinput_buffer_empty_sem; // semafor koji sluzi da blokira pozivajucu nit ako je buffer prazan

struct args {
    uint64 arg0;
    uint64 arg1;
    uint64 arg2;
    uint64 arg3;
  };

void read_arguments(args* arguments) {
    arguments->arg0 = read_a1();
    arguments->arg1 = read_a2();
    arguments->arg2 = read_a3();
    arguments->arg3 = read_a4();
}

void kill_thread() {
    delete kthread::running;
    kthread::running = nullptr;
    if(kscheduler::size() == 0 && kscheduler::size_sleeping() == 0 && kinput_buffer_empty_sem->get_number_of_blocked_threads() == 0) {
        kscheduler::put_thread(kscheduler::main_thread);
        kernel_finished = true; //treba se dealociraju alocirane stvari u init()
    }
}

void context_switch(uint64);

void wrapper() {
    void (*func_ptr)(void*);
    func_ptr = kthread::running->get_thread_body();
    func_ptr(kthread::running->get_arg());
    //thread_dispatch();
    kill_thread(); // nakon ove linije koda, tok kontrole se vraca na yield koji je ranije pozvao funkciju wraper
    thread_dispatch();
}

void context_switch(uint64 ret) {

    if(kthread::running != nullptr) {
        kthread::running->set_sstatus(read_sstatus());
        if (read_scause() == 0x8 || read_scause() == 0x9) {
            kthread::running->set_sepc(read_sepc() + 4);
        } else {
            kthread::running->set_sepc(read_sepc());
        }
        //kthread::running->set_ret(ret);
        if(!kthread::running->blocked() && (kthread::running->get_sleep_time() == -1 || kthread::running->get_sleep_time() == 0)
            && kthread::running != kscheduler::main_thread && kthread::running != kscheduler::idle_thread) {
            kscheduler::put_thread(kthread::running);
        }
    }
    kthread* new_thread = kscheduler::get_thread();

    if (new_thread == nullptr) {
        new_thread = kscheduler::idle_thread;
    }
    // output_thread mora da radi u sistemskom rezimu i to kontrolise ovaj if
    if (!new_thread->is_started()) {
        if (new_thread == kconsole::output_thread) {
            ms_sstatus(MASK_8);
        } else {
            mc_sstatus(MASK_8);
        }
    }

    //ako je nit vec startovana, onda se sepc uveca za 4 jer sepc inace pokazuje na ecall koji je pozvan kako bi se uslo u kernel rezim
    if((uint64)new_thread->get_thread_body() != new_thread->get_sepc()) {
        write_sepc(new_thread->get_sepc() + 4);
    }
    //ako nit treba da se startuje onda se njeno telo poziva iz wraper metode kako bi nakon izvrsavanj tela niti mogao da je "ubije"
    else {
        write_sepc((uint64)&wrapper);
    }
    if (kernel_finished) {
        mc_sie(MASK_9);     //bez ove linije se umesto "Kernel finished" ispisuje samo K
    }
    if (kthread::running != new_thread) {
        kthread::yield(kthread::running, new_thread);
    }
    write_a0(kthread::running->get_ret());
    write_sepc(kthread::running->get_sepc());
    write_sstatus(kthread::running->get_sstatus());
}

void timer_interrupt() {
    uint64 volatile sstatus = read_sstatus();
    kscheduler::update_sleeping_threads();
    mc_sip(MASK_1);
    write_sstatus(sstatus);
}

void idle_thread_body() {
    int i = 1;
    while (i > 0) {
        i = i + 1 - 1;
    }
    thread_dispatch();
}

void output_thread_body() {
    while (true) {
        while (kconsole::output_buffer_empty()) {
            //mc_sie(MASK_9);// ako nema sta da se cita onda se maskiraju prekidi "vljd"
            output_thread_blocked_on_buffer_sem = true;
            sem_wait(koutput_buffer_sem);
            /*koutput_buffer_sem->wait(-1);
            thread_dispatch();*/
        }
        while ((*(char*)CONSOLE_STATUS & MASK_5) == 0) {
            //mc_sie(MASK_9);
            output_thread_blocked_on_status_sem = true;
            sem_wait(koutput_status_sem);
            /*koutput_status_sem->wait(-1);
            thread_dispatch();*/
        }
        *(char*)CONSOLE_RX_DATA = kconsole::output_buffer_get_char();
    }
}

void init(body user_main_start_routine) {
    koutput_buffer_sem = new ksemaphore(0);
    koutput_status_sem = new ksemaphore(0);
    kinput_buffer_empty_sem = new ksemaphore(0);

    kconsole::init();

    kthread* user_main_thread;

    //alokacija stekova
    uint64 main_thread_sp = (uint64)kmem_alocator::kalocate(DEFAULT_STACK_SIZE / MEM_BLOCK_SIZE + ((DEFAULT_STACK_SIZE % MEM_BLOCK_SIZE > 0)  ? 1 : 0));
    uint64 output_thread_sp = (uint64)kmem_alocator::kalocate(DEFAULT_STACK_SIZE / MEM_BLOCK_SIZE + ((DEFAULT_STACK_SIZE % MEM_BLOCK_SIZE > 0 ) ? 1 : 0));
    uint64 user_main_thread_sp = (uint64)kmem_alocator::kalocate(DEFAULT_STACK_SIZE / MEM_BLOCK_SIZE + ((DEFAULT_STACK_SIZE % MEM_BLOCK_SIZE > 0 ) ? 1 : 0));
    uint64 idle_thread_sp = (uint64)kmem_alocator::kalocate(DEFAULT_STACK_SIZE / MEM_BLOCK_SIZE + ((DEFAULT_STACK_SIZE % MEM_BLOCK_SIZE > 0 ) ? 1 : 0));

    kscheduler::main_thread = kthread::create_thread(nullptr, main_thread_sp + DEFAULT_STACK_SIZE);
    kscheduler::main_thread->set_started();
    kscheduler::main_thread->set_arg(0);
    kthread::running = kscheduler::main_thread;

    kconsole::output_thread = kthread::create_thread((body)&output_thread_body, output_thread_sp + DEFAULT_STACK_SIZE);
    kconsole::output_thread->set_arg(0);
    kconsole::output_thread->start();

    user_main_thread = kthread::create_thread(user_main_start_routine, user_main_thread_sp + DEFAULT_STACK_SIZE);
    user_main_thread->set_arg(0);
    user_main_thread->start();

    kscheduler::idle_thread = kthread::create_thread((body)&idle_thread_body, idle_thread_sp + DEFAULT_STACK_SIZE);
    kscheduler::idle_thread->set_arg(0);

}
// treba da vidim da li mi ne radi zbog sie
void handle_supervisor_trap() {
    uint64 ret = 0;
    args arguments;
    uint64 code = read_a0();
    read_arguments(&arguments);

    if (!kernel_initialized) {
        mc_sstatus(MASK_8);
        //mc_sie(MASK_9);
        init((body)arguments.arg0);
        kernel_initialized = true;
        ret = 0;
    }
    uint64 scause = read_scause();
    if(scause == 0x8000000000000001) {
        timer_interrupt();
    }
    else if(scause == 0x8000000000000009) {
        if (plic_claim() == CONSOLE_IRQ) {
            if ((*(char*)CONSOLE_STATUS & MASK_5) != 0 && output_thread_blocked_on_status_sem == true) {
                output_thread_blocked_on_status_sem = false;
                koutput_status_sem->signal();
            }
            if ((*(char*)CONSOLE_STATUS & MASK_0) != 0) {
                kconsole::input_buffer_put_char(*(char*)CONSOLE_TX_DATA);
                if (kinput_buffer_empty_sem->get_number_of_blocked_threads() > 0) {
                    kinput_buffer_empty_sem->signal();
                }
            }
            mc_sip(MASK_9);
            plic_complete(CONSOLE_IRQ);
        }
    }
    else if (scause == 0x8 || scause == 0x9) {
        switch (code) {
            case 0x1:
                ret = (uint64)kmem_alocator::kalocate(arguments.arg0);
            kthread::running->set_ret(ret);
            break;
            case 0x2:
                ret = (uint64)kmem_alocator::kfree((void*)arguments.arg0);
            kthread::running->set_ret(ret);
            break;
            case 0x11:
                *(thread_t*)arguments.arg0 = kthread::create_thread((body)arguments.arg1, arguments.arg3);
            (*(thread_t*)arguments.arg0)->set_arg((void*) arguments.arg2);
            (*(thread_t*)arguments.arg0)->start();
            ret = 0; // treba za gresku da se doda
            kthread::running->set_ret(ret);
            break;
            case 0x12:
                kill_thread();
            break;
            case 0x13:
                break;
            case 0x21:
                *(sem_t*)arguments.arg0 = new ksemaphore((int)arguments.arg1);
            ret = 0; // treba za gresku da se doda
            kthread::running->set_ret(ret);
            break;
            case 0x22:
                delete (ksemaphore*)arguments.arg1;
            break;
            case 0x23:
                ((sem_t)arguments.arg0)->wait(-1);
            break;
            case 0x24:
                ((sem_t)arguments.arg0)->signal();
            break;
            case 0x25:
                ((sem_t)arguments.arg0)->wait(arguments.arg1);
            break;
            case 0x26:
                ret = ((sem_t)arguments.arg0)->try_wait();
            kthread::running->set_ret(ret);
            break;
            case 0x31:
                ret = kscheduler::put_to_sleep((time_t)arguments.arg0);
            kthread::running->set_ret(ret);
            break;
            case 0x41:
                //ms_sie(MASK_9);
                while (kconsole::input_buffer_empty()) {
                    kinput_buffer_empty_sem->wait(-1);
                    context_switch(0);
                }
            ret = kconsole::input_buffer_get_char();
            kthread::running->set_ret(ret);
            break;
            case 0x42:
                ret = kconsole::output_buffer_put_char((char)arguments.arg0);
            kthread::running->set_ret(ret);
            //mc_sip(MASK_9);
            if (output_thread_blocked_on_buffer_sem) {
                koutput_buffer_sem->signal();
                output_thread_blocked_on_buffer_sem = false;
            }
        }
    } else {
        if (scause == 0x2) {
            kconsole::print_string("GRESKA! Ilegalna instrukcija.\n");
        }
        else if (scause == 0x5) {
            kconsole::print_string("GRESKA! Nedozvoljena adresa čitanja.\n");
        } else if (scause == 0x7) {
            kconsole::print_string("GRESKA! Nedozvoljena adresa upisa.\n");
        }
        //mc_sip(MASK_9);
        if (output_thread_blocked_on_buffer_sem) {
            koutput_buffer_sem->signal();
            output_thread_blocked_on_buffer_sem = false;
        }
    }
    if((kthread::running == nullptr && kscheduler::size() == 0)) {
        write_a0(ret);
    	write_sepc(read_sepc() + 4);
        return;
    }
    /*//ovaj if sluzi kako se ne bi uslo u context_switch jer je running nit ustvari main_thread nit koja ne treba nikako da se smesti u scheduler
    //vec main_thread sluzi kako bi se ocuvao kontekst funkcije main i kasnije restaurirao kad treba da se ugasi kernel
    if((kscheduler::main_thread == nullptr || kthread::running == kscheduler::main_thread) && (kscheduler::size() == 0)
        && kernel_finished == false) {
        kscheduler::main_thread = kthread::running;
        write_a0(ret);
    	write_sepc(read_sepc() + 4);
        return;
    }*/
    context_switch(ret);
    /*if (kthread::running == kconsole::output_thread) {
        ms_sstatus(MASK_8);
    } else {
        mc_sstatus(MASK_8);
    }*/
    if(kthread::running->get_sleep_time() > 0 && kthread::running->blocked()) {
        kthread::running->unblock();
        kthread::running->set_sleep_time(-1);
        //treba izbaciti iz sleeping threads
        write_a0(-1);
    }
    else if(kthread::running->blocked()) {
        kthread::running->unblock();
        write_a0(-2); // ?
    } else if(kthread::running->get_sleep_time() == 0) {
        kthread::running->dec_sleep_time();
    }
    //treba da se ubaci fleg koji ce da odredi da li treba da se radi context switch zbog semafora
}