#pragma once
#include <stdint.h>

namespace QWERTKeyboard{

    #define LeftShift 0x2A
    #define RightShift 0x36
    #define Enter 0x1C
    #define BackSpace 0x0E 
    #define SpaceBar 0x39

    extern const char ASCIITable[];
    char Translate(uint8_t scancode, bool uppercase);
}