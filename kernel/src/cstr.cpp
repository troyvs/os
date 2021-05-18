#include "cstr.h"

char uintto_stringoutput[128];

const char* to_string(uint64_t value){
    uint8_t size;
    uint64_t sizetest = value;
    while(sizetest /10 > 0){
        sizetest /=10;
        size++;
    }
    uint8_t index = 0;
    while(value/10 > 0){
        uint8_t remainder = value % 10;
        value /= 10;
        uintto_stringoutput[size - index] = remainder+ '0';
        index++;
    }
    uint8_t remainder = value % 10;
    uintto_stringoutput[size - index] = remainder + '0';
    uintto_stringoutput[size + 1] = 0;
    return uintto_stringoutput;
}
char intto_stringoutput[128];
const char* to_string(int64_t value){
    uint8_t isNegative = 0;
    uint8_t size;
    if (value < 0){
        isNegative = 1;
        value *= -1;
        intto_stringoutput[0] = '-';
    }
    
    uint64_t sizetest = value;
    while(sizetest /10 > 0){
        sizetest /=10;
        size++;
    }
    uint8_t index = 0;
    while(value/10 > 0){
        uint8_t remainder = value % 10;
        value /= 10;
        intto_stringoutput[isNegative + size - index] = remainder+ '0';
        index++;
    }
    uint8_t remainder = value % 10;
    intto_stringoutput[isNegative + size - index] = remainder + '0';
    intto_stringoutput[isNegative + size + 1] = 0;
    return intto_stringoutput;
}

char hexto_stringoutput[128];

const char* to_hstring(uint64_t value){
    uint64_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 8 * 2 - 1;
    for (uint8_t i = 0; i < size; i++){
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexto_stringoutput[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexto_stringoutput[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexto_stringoutput[size + 1] = 0;
    return hexto_stringoutput;
}
char hexto_stringoutput32[128];

const char* to_hstring(uint32_t value){
    uint32_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 4 * 2 - 1;
    for (uint8_t i = 0; i < size; i++){
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexto_stringoutput32[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexto_stringoutput32[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexto_stringoutput32[size + 1] = 0;
    return hexto_stringoutput32;
}
char hexto_stringoutput16[128];

const char* to_hstring(uint16_t value){
    uint16_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 2 * 2 - 1;
    for (uint8_t i = 0; i < size; i++){
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexto_stringoutput16[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexto_stringoutput16[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexto_stringoutput16[size + 1] = 0;
    return hexto_stringoutput16;
}
char hexto_stringoutput8[128];

const char* to_hstring(uint8_t value){
    uint8_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 1 * 2 - 1;
    for (uint8_t i = 0; i < size; i++){
        ptr = ((uint8_t*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexto_stringoutput8[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexto_stringoutput8[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexto_stringoutput8[size + 1] = 0;
    return hexto_stringoutput8;
}


char doubleto_stringoutput[128];
const char* to_string(double value, uint8_t decimalPlaces){

    if (decimalPlaces > 20) decimalPlaces = 20;
    char* intPtr = (char*)to_string((int64_t)value);
    char* doublePtr = doubleto_stringoutput;

    if (value < 0){
        value *= -1;
    }
    while (*intPtr !=0){
        *doublePtr = *intPtr;
        intPtr++;
        doublePtr++;
    }
    *doublePtr += '.';
    doublePtr++;

    double newValue = value - (int)value;
    for (uint8_t i = 0; i < decimalPlaces; i++){
        newValue *= 10;
        *doublePtr = (int)newValue + '0';
        newValue -= (int)newValue;
        doublePtr++;
    }

    *doublePtr = 0;
    return doubleto_stringoutput;
}
const char* to_string(double value){
    uint8_t decimalPlaces = 2;

    char* intPtr = (char*)to_string((int64_t)value);
    char* doublePtr = doubleto_stringoutput;

    if (value < 0){
        value *= -1;
    }
    while (*intPtr !=0){
        *doublePtr = *intPtr;
        intPtr++;
        doublePtr++;
    }
    *doublePtr += '.';
    doublePtr++;

    double newValue = value - (int)value;
    for (uint8_t i = 0; i < decimalPlaces; i++){
        newValue *= 10;
        *doublePtr = (int)newValue + '0';
        newValue -= (int)newValue;
        doublePtr++;
    }

    *doublePtr = 0;
    const char* nm="54.67";
    return nm;
}

