#include "kernelUtil.h"
#include "panic.h"

extern "C" int main(BootInfo* bootInfo){

    KernelInfo KernelInfo = InitializeKernel(bootInfo);

    PageTableManager* pageTableManager = KernelInfo.pageTableManager;
    globalrenderer->resetpos();

    globalrenderer->println("Kernel Initialized Successfully");

	while(true);
}