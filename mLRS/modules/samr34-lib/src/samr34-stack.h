//*******************************************************
// Copyright (c) MLRS project
// GPL3
// https://www.gnu.org/licenses/gpl-3.0.de.html
//*******************************************************
// SAMR34 STACK standard library
// Stack usage monitoring (fill pattern check)
//*******************************************************
#ifndef SAMR34_STACK_H
#define SAMR34_STACK_H
#ifdef __cplusplus
extern "C" {
#endif


#include "samr34.h"


// Linker symbols
extern uint32_t _sstack;
extern uint32_t _estack;


uint32_t stack_check_used(void)
{
    uint32_t ptr = (uint32_t)&_sstack;
    uint32_t end = (uint32_t)&_estack;
    while (ptr < end) {
        if (*(uint8_t*)ptr != 0xAA) break;
        ptr++;
    }
    return (end - ptr);
}


void stack_check_init(void)
{
    uint32_t ptr = (uint32_t)&_sstack;
    uint32_t end = __get_MSP() - 1;
    while (ptr < end) {
        *(uint8_t*)ptr = 0xAA;
        ptr++;
    }
}


//-------------------------------------------------------
#ifdef __cplusplus
}
#endif
#endif // SAMR34_STACK_H
