#include "PageFrameAllocator.h"

uint64_t freeMemory;
uint64_t reservedMemory;
uint64_t usedMemory;
bool Initialized = false;

void PageFrameAllocator::ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapSize, size_t mMapDescSize){
    if (Initialized) return;
    
    Initialized = true;

    void*  largestFreeMemSeg = NULL;
    size_t largestFreeMemSegSize = 0;

    uint64_t mMapEntries = mMapSize / mMapDescSize;
    for(int i =0; i < mMapEntries; i++){
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescSize));
        if (desc->type == 7){
            if(desc->numPages * 4096 > largestFreeMemSegSize){
                largestFreeMemSeg = desc->physAddr;
                largestFreeMemSegSize = desc->numPages * 4096;
            }
        }
    }
    
    uint64_t memorySize = GetMemorySize(mMap,mMapEntries,mMapDescSize);
    freeMemory = memorySize;
    uint64_t bitmapSize = memorySize / 4096 / 8 + 1;
    
    InitBitmap(bitmapSize,largestFreeMemSeg);

    lockPages(&PageBitmap.Buffer, PageBitmap.Size / 4096 +1);

    for(int i =0; i < mMapEntries; i++){
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescSize));
        if(desc->type != 7){
            reservePages(desc->physAddr,desc->numPages);
        }
    }

}
void PageFrameAllocator::InitBitmap(size_t bitmapSize, void* bufferAddress){
    PageBitmap.Size = bitmapSize;
    PageBitmap.Buffer = (uint8_t*)bufferAddress;
    for(int i = 0; i <bitmapSize;i++){
        *(uint8_t*)(PageBitmap.Buffer + i) = 0;
    }
}

void PageFrameAllocator::freePage(void* address){
    uint64_t index = (uint64_t)address / 4096;
    if (PageBitmap[index] == false) return;
    PageBitmap.Set(index, false);
    freeMemory += 4096;
    usedMemory -= 4096;
}
void PageFrameAllocator::lockPage(void* address){
    uint64_t index = (uint64_t)address / 4096;
    if (PageBitmap[index] == true) return;
    PageBitmap.Set(index, true);
    freeMemory -= 4096;
    usedMemory += 4096;
}
void PageFrameAllocator::reservePage(void* address){
    uint64_t index = (uint64_t)address / 4096;
    if (PageBitmap[index] == true) return;
    PageBitmap.Set(index, true);
    freeMemory -= 4096;
    reservedMemory += 4096;
}
void PageFrameAllocator::unreservePage(void* address){
    uint64_t index = (uint64_t)address / 4096;
    if (PageBitmap[index] == false) return;
    PageBitmap.Set(index, false);
    freeMemory += 4096;
    reservedMemory -= 4096;
}
uint64_t inc = 0; 
void* PageFrameAllocator::requestpage(){
    for (uint64_t index = 0; index < PageBitmap.Size * 8; index++){
        if (PageBitmap[index] == true) continue;
        lockPage((void*)(index * 4096));
        return (void*)(index * 4096);
    }

    return NULL; // Page Frame Swap to file
}


void PageFrameAllocator::freePages(void* address, uint64_t pageCount){
    for(int i =0; i < pageCount; i++){
        freePage((void*)((uint64_t)address + (i * 4096)));
    }
}
void PageFrameAllocator::lockPages(void* address, uint64_t pageCount){
    for(int i =0; i < pageCount; i++){
        lockPage((void*)((uint64_t)address + (i * 4096)));
    }
}
void PageFrameAllocator::reservePages(void* address, uint64_t pageCount){
    for(int i =0; i < pageCount; i++){
        reservePage((void*)((uint64_t)address + (i * 4096)));
    }
}
void PageFrameAllocator::unreservePages(void* address, uint64_t pageCount){
    for(int i =0; i < pageCount; i++){
        unreservePage((void*)((uint64_t)address + (i * 4096)));
    }
}
uint64_t PageFrameAllocator::getFreeMemory(){
    return freeMemory;
}
uint64_t PageFrameAllocator::getUsedMemory(){
    return usedMemory;
}
uint64_t PageFrameAllocator::getReservedMemory(){
    return reservedMemory;
}