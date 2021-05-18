#pragma once
#include <stddef.h>
#include <stdint.h>

class bitmap{
    public:
    size_t Size;
    uint8_t* Buffer;
    bool operator[](uint64_t index);
    bool Set(uint64_t index, bool value);
};