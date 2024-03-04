#include <iostream>
#include <unistd.h>

void* smalloc(size_t size) {
    if(size == 0 || size>100000000) {
        return NULL;
    }

    void* prog_break = sbrk(size);
    if(prog_break == (void*)(-1))
        return NULL;
    return prog_break;
}
