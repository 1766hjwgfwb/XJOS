#include <xjos/syscall.h>


static _inline u32 _syscall0(u32 nr) {
    u32 ret;
    asm volatile (
        "int $0x80\n"
        : "=a"(ret) // output               // ! bug "a="
        : "a"(nr)   // input
    );

    return ret;
}


u32 test() {
    return _syscall0(SYS_NR_TEST);
}


void yield() {
    _syscall0(SYS_NR_YIELD);
}