//*******************************************************
// Copyright (c) MLRS project
// GPL3
// https://www.gnu.org/licenses/gpl-3.0.de.html
//*******************************************************
// SAMR34 Timer
// SysTick-based 1ms tick + TC4-based microsecond counter
//*******************************************************
#ifndef SAMR34_TIMER_H
#define SAMR34_TIMER_H
#pragma once


volatile uint32_t uwTick = 0;

IRQHANDLER(
void SysTick_Handler(void)
{
    uwTick++;
})


uint32_t doSysTask_done = 0;

void resetSysTask(void)
{
    doSysTask_done = uwTick;
}

volatile bool doSysTask(void)
{
    if (uwTick != doSysTask_done) {
        doSysTask_done++;
        return true;
    }
    return false;
}


volatile uint32_t millis32(void)
{
    return uwTick;
}


// micros16: 16-bit microsecond counter from TC4
uint16_t micros16(void)
{
    TC4->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_READSYNC;
    while (TC4->COUNT16.SYNCBUSY.bit.COUNT) {}
    return TC4->COUNT16.COUNT.reg;
}


// Converts ms to SysTick tick count (1ms tick = 1:1 mapping)
#define SYSTICK_DELAY_MS(x)  ((uint16_t)(x))


void timer_init(void)
{
    uwTick = 0;
    // SysTick at 1ms (SystemCoreClock / 1000)
    SysTick_Config(SystemCoreClock / 1000);
    NVIC_SetPriority(SysTick_IRQn, 0);
    resetSysTask();
}


#endif // SAMR34_TIMER_H
