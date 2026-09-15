//*******************************************************
// Copyright (c) MLRS project
// GPL3
// https://www.gnu.org/licenses/gpl-3.0.de.html
//*******************************************************
// SAMR34 DELAY standard library
// Uses TC4 as free-running 1MHz counter for us-level delays
// (Cortex-M0+ has no DWT cycle counter)
//*******************************************************
#ifndef SAMR34_DELAY_H
#define SAMR34_DELAY_H


#include "samr34.h"
#include "samr34-peripherals.h"


//-------------------------------------------------------
// TC4 as free-running 16-bit counter at 1MHz
// Clocked from GCLK2 (OSC16M / 16 = 1MHz), prescaler DIV1
//-------------------------------------------------------

#define DELAY_TC        TC4
#define DELAY_TC_N      4
#define DELAY_TC_GCLK_ID  TC4_GCLK_ID


void delay_init(void)
{
    rcc_init_tc(DELAY_TC_N);
    gclk_connect_pch(DELAY_TC_GCLK_ID, 0); // GCLK0 = 16MHz

    DELAY_TC->COUNT16.CTRLA.bit.ENABLE = 0;
    while (DELAY_TC->COUNT16.SYNCBUSY.bit.ENABLE) {}

    // 16-bit mode, DIV16 prescaler (16MHz/16 = 1MHz tick = 1us)
    DELAY_TC->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16 | TC_CTRLA_PRESCALER_DIV16;

    // Enable
    DELAY_TC->COUNT16.CTRLA.bit.ENABLE = 1;
    while (DELAY_TC->COUNT16.SYNCBUSY.bit.ENABLE) {}

    // Start counter
    DELAY_TC->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_RETRIGGER;
    while (DELAY_TC->COUNT16.SYNCBUSY.bit.CTRLB) {}
}


static inline uint16_t delay_timer_count(void)
{
    DELAY_TC->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_READSYNC;
    while (DELAY_TC->COUNT16.SYNCBUSY.bit.COUNT) {}
    return DELAY_TC->COUNT16.COUNT.reg;
}


void delay_us(uint32_t us)
{
    uint16_t start = delay_timer_count();
    while ((uint16_t)(delay_timer_count() - start) < (uint16_t)us) {}
}


void delay_ms(uint16_t ms)
{
    while (ms--) {
        delay_us(1000);
    }
}


void delay_ns(uint32_t ns)
{
    uint32_t loops = ns / 125; // ~2 cycles per iteration at 16MHz = 125ns
    while (loops--) {
        __NOP();
    }
}


#endif // SAMR34_DELAY_H
