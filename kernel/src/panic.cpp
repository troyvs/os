#include "panic.h"

void Panic(const char* panicMessage){
    globalrenderer->setClrcolor(0x00ff0000);
    globalrenderer->clear();
    globalrenderer->resetpos();
    globalrenderer->setcolor(0);
    globalrenderer->println("Kernel Panic");
    globalrenderer->println("");
    globalrenderer->println(panicMessage);
}