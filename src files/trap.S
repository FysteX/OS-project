# 1 "src/trap.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/riscv64-linux-gnu/include/stdc-predef.h" 1 3
# 32 "<command-line>" 2
# 1 "src/trap.S"
.global supervisor_trap
.extern handle_supervisor_trap
.align 4
supervisor_trap:

    addi sp, sp, -256
    .irp index, 1,2,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8(sp)
    .endr

    call handle_supervisor_trap

    .irp index, 1,2,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8(sp)
    .endr

    addi sp, sp, 256

    sret

.global _ZN7kthread23switch_from_null_to_newEPNS_7contextE
.type _ZN7kthread23switch_from_null_to_newEPNS_7contextE, @function
_ZN7kthread23switch_from_null_to_newEPNS_7contextE:

    ld sp, 0 * 8(a0)

    sret

.global _ZN7kthread27switch_from_null_to_startedEPNS_7contextE
.type _ZN7kthread27switch_from_null_to_startedEPNS_7contextE, @function
_ZN7kthread27switch_from_null_to_startedEPNS_7contextE:

    ld sp, 0 * 8(a0)

    ret

.global _ZN7kthread25switch_fom_started_to_newEPNS_7contextES1_
.type _ZN7kthread25switch_fom_started_to_newEPNS_7contextES1_, @function
_ZN7kthread25switch_fom_started_to_newEPNS_7contextES1_:

    sd sp, 0 * 8(a0)

    ld sp, 0 * 8(a1)

    sret

.global _ZN7kthread30switch_from_started_to_startedEPNS_7contextES1_
.type _ZN7kthread30switch_from_started_to_startedEPNS_7contextES1_, @function
_ZN7kthread30switch_from_started_to_startedEPNS_7contextES1_:

    sd sp, 0 * 8(a0)

    ld sp, 0 * 8(a1)

    ret
