#include "basicRenderer.h"
#include "cstr.h"
#include "efiMemory.h"
#include "memory.h"
#include "Bitmap.h"
#include "paging/PageFrameAllocator.h"
#include "paging/PageMapIndexer.h"
#include "paging/paging.h"
#include "paging/PageTableManager.h"

struct BootInfo{
	FrameBuffer* framebuffer;
	PSF1_FONT* psf1_Font;
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescriptorSize;
};

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

extern "C" int main(BootInfo* bootInfo){
	BasicRenderer Renderer = BasicRenderer(bootInfo->framebuffer,bootInfo->psf1_Font,0xffffff,15,50);

   uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescriptorSize;

    globalAllocator = PageFrameAllocator();
    globalAllocator.ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mMapDescriptorSize);

    uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;

    globalAllocator.LockPages(&_KernelStart, kernelPages);

    PageTable* PML4 = (PageTable*)globalAllocator.RequestPage();
    memset(PML4, 0, 0x1000);

    PageTableManager pageTableManager = PageTableManager(PML4);

    for (uint64_t t = 0; t < GetMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mMapDescriptorSize); t+= 0x1000){
        pageTableManager.MapMemory((void*)t, (void*)t);
    }

    uint64_t fbBase = (uint64_t)bootInfo->framebuffer->baseAddress;
    uint64_t fbSize = (uint64_t)bootInfo->framebuffer->Size + 0x1000;
    for (uint64_t t = fbBase; t < fbBase + fbSize; t += 4096){
        pageTableManager.MapMemory((void*)t, (void*)t);
    }

    asm ("mov %0, %%cr3" : : "r" (PML4));
    pageTableManager.MapMemory((void*)0x600000000, (void*)0x80000);

    uint64_t* test = (uint64_t*)0x600000000;
    *test = 26;

    Renderer.println(to_string(*test));

	return 0;
}