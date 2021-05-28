#include "panic.h"

void panic(const char* panicMessage){
    globalrenderer->clear(0x00ff0000);
    globalrenderer->resetpos();
    globalrenderer->setcolor(0);
    globalrenderer->println("Kernel Panic");
    globalrenderer->println("");
    globalrenderer->println(panicMessage);
}