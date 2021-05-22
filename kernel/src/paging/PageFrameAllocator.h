#pragma once
#include "../efiMemory.h"
#include "../Bitmap.h"
#include <stdint.h>
#include "../memory.h"
#include "../cstr.h"

class PageFrameAllocator{
    public:
    void ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapSize, size_t mMapdescSize);
    Bitmap PageBitmap;
    void freePage(void* address);
    void lockPage(void* address);
    void freePages(void* address, uint64_t pageCount);
    void lockPages(void* address, uint64_t pageCount);
    void* requestpage();
    uint64_t getFreeMemory();
    uint64_t getUsedMemory();
    uint64_t getReservedMemory();

    private:
    void InitBitmap(size_t bitmapSize, void* bufferAddress);
    void reservePage(void* address);
    void unreservePage(void* address);
    void reservePages(void* address, uint64_t pageCount);
    void unreservePages(void* address, uint64_t pageCount);
};
