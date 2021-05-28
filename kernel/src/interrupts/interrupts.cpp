#include "interrupts.h"
void Panic(const char* panicMessage);

__attribute__((interrupt)) void PageFault_Handler(struct interrupt_frame* frame){
    Panic((const char*)"Page Fault Detected");
    while(true);
}