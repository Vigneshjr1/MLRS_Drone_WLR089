//*******************************************************
// Copyright (c) MLRS project
// GPL3
// https://www.gnu.org/licenses/gpl-3.0.de.html
//*******************************************************
// SAMR34 Glue
//*******************************************************
#ifndef SAMR34_GLUE_H
#define SAMR34_GLUE_H
#pragma once


#include "samr34.h"


// IRAM_ATTR is ESP-specific, not needed on SAMR34
#ifndef IRAM_ATTR
#define IRAM_ATTR
#endif


#ifndef IRQHANDLER
#ifdef __cplusplus
#  define IRQHANDLER(__Declaration__)  extern "C" {__Declaration__}
#else
#  define IRQHANDLER(__Declaration__)  __Declaration__
#endif
#endif


typedef enum {
    DISABLE = 0,
    ENABLE = !DISABLE
} FunctionalState;


typedef enum
{
    HAL_OK       = 0x00U,
    HAL_ERROR    = 0x01U,
    HAL_BUSY     = 0x02U,
    HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;


// __NOP, __disable_irq, __enable_irq, __REV, __REV16, __REVSH
// are provided by CMSIS core_cm0plus.h included via samr34.h


// Controller restart state machine (same pattern as ESP/STM32)
static uint8_t restart_controller = 0;

void main_loop(void);

int main_main(void)
{
    while (1) main_loop();
    return 0;
}

#define INITCONTROLLER_ONCE \
    if(restart_controller <= 1){ \
    if(restart_controller == 0){
#define RESTARTCONTROLLER \
    }
#define INITCONTROLLER_END \
    restart_controller = UINT8_MAX; \
    }
#define GOTO_RESTARTCONTROLLER \
    restart_controller = 1; \
    return;


#endif // SAMR34_GLUE_H
