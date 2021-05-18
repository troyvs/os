#include "basicRenderer.h"
#include "cstr.h"
#include "efiMemory.h"
#include "memory.h"

struct bootInfo{
	FrameBuffer* framebuffer;
	PSF1_FONT* psf1_Font;
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescriptorSize;
};

extern "C" int main(bootInfo* bootinfo){
	BasicRenderer newRenderer = BasicRenderer(bootinfo->framebuffer,bootinfo->psf1_Font,0xffffff,15,50);

	uint64_t mMapEntries = bootinfo->mMapSize / bootinfo->mMapDescriptorSize;

	// for(int i =0;i < mMapEntries; i++){
	//	EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)bootinfo->mMap + (i * bootinfo->mMapDescriptorSize));
	// 	newRenderer.print(EFI_MEMORY_TYPE_STRINGS[desc->type]);
	// 	newRenderer.setcolor(0xffff00ff);
	// 	newRenderer.print(": ");
	// 	newRenderer.print(to_string(desc->numPages * 4096 /1024));
	// 	newRenderer.println(" kb");
	// 	newRenderer.setcolor(0xffffffff);
	// }
	uint64_t result = GetMemorySize(bootinfo->mMap,mMapEntries,bootinfo->mMapDescriptorSize);
	newRenderer.println(to_string(result));
}