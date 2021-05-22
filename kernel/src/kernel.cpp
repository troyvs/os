#include "basicRenderer.h"
#include "cstr.h"
#include "efiMemory.h"
#include "memory.h"
#include "Bitmap.h"
#include "PageFrameAllocator.h"

struct bootInfo{
	FrameBuffer* framebuffer;
	PSF1_FONT* psf1_Font;
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescriptorSize;
};

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

extern "C" int main(bootInfo* bootinfo){
	BasicRenderer Renderer = BasicRenderer(bootinfo->framebuffer,bootinfo->psf1_Font,0xffffff,15,50);
	PageFrameAllocator Allocator;

	uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
	uint64_t kernelPages = (uint64_t) kernelSize / 4096 + 1;
	Allocator.lockPages(&_KernelStart, kernelPages);

	Allocator.ReadEFIMemoryMap(bootinfo->mMap, bootinfo->mMapSize, bootinfo->mMapDescriptorSize);

	uint64_t mMapEntries = bootinfo->mMapSize / bootinfo->mMapDescriptorSize;

	Renderer.print("free memory: "); Renderer.print(to_string(Allocator.getFreeMemory() / 1024)); Renderer.println(" Kb");
	Renderer.print("used memory: "); Renderer.print(to_string(Allocator.getUsedMemory() / 1024)); Renderer.println(" Kb");
	Renderer.print("reserved memory: "); Renderer.print(to_string(Allocator.getReservedMemory() / 1024)); Renderer.println(" Kb");
	for(int i = 0; i < 20; i++){
		void* address = Allocator.requestpage();
		Renderer.println(to_hstring((uint64_t)address));
	}

// 	for(uint64_t index=0; index < 1000;index++){
// 		Renderer.println(to_string(index));
// 	}
}