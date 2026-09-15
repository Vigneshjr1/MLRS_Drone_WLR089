//*******************************************************
// Copyright (c) MLRS project
// GPL3
// https://www.gnu.org/licenses/gpl-3.0.de.html
//*******************************************************
// SAMR34 RxClock
// Uses TC3 for RX period timing (two compare channels)
// CC0 = short delay (1ms after frame receipt) -> doPostReceive
// CC1 = frame period (20ms) -> reschedules CC0 for missed frames
//*******************************************************
#ifndef SAMR34_RXCLOCK_H
#define SAMR34_RXCLOCK_H
#pragma once


#define CLOCK_SHIFT_US  1000 // 1ms shift like STM32


volatile bool doPostReceive = false;
volatile uint32_t tc3_fire_count = 0;

uint16_t CLOCK_PERIOD_US; // frame period in us


IRQHANDLER(
void TC3_Handler(void)
{
    if (TC3->COUNT16.INTFLAG.bit.MC0) {
        TC3->COUNT16.INTFLAG.reg = TC_INTFLAG_MC0;
        doPostReceive = true;
        tc3_fire_count++;
    }
    if (TC3->COUNT16.INTFLAG.bit.MC1) {
        TC3->COUNT16.INTFLAG.reg = TC_INTFLAG_MC1;
        // frame boundary: schedule next doPostReceive (for missed frame case)
        uint16_t cc1 = TC3->COUNT16.CC[1].reg;
        TC3->COUNT16.CC[0].reg = cc1 + CLOCK_SHIFT_US;
        while (TC3->COUNT16.SYNCBUSY.bit.CC0) {}
        TC3->COUNT16.CC[1].reg = cc1 + CLOCK_PERIOD_US;
        while (TC3->COUNT16.SYNCBUSY.bit.CC1) {}
    }
})


class tRxClock
{
  public:
    void Init(uint16_t period_ms)
    {
        CLOCK_PERIOD_US = (uint16_t)((uint32_t)period_ms * 1000UL);

        // Enable TC3 bus clock
        MCLK->APBCMASK.reg |= MCLK_APBCMASK_TC3;

        // Connect GCLK0 (16MHz) to TC3
        GCLK->PCHCTRL[TC3_GCLK_ID].reg = GCLK_PCHCTRL_GEN(0) | GCLK_PCHCTRL_CHEN;
        while (!(GCLK->PCHCTRL[TC3_GCLK_ID].reg & GCLK_PCHCTRL_CHEN)) {}

        // Disable TC3 before configuring
        TC3->COUNT16.CTRLA.bit.ENABLE = 0;
        while (TC3->COUNT16.SYNCBUSY.bit.ENABLE) {}

        // 16-bit mode, prescaler DIV16 => 16MHz/16 = 1MHz = 1us per tick
        // Free-running (no MFRQ/MPWM), count up continuously
        TC3->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16 | TC_CTRLA_PRESCALER_DIV16;

        // Set initial compare values
        TC3->COUNT16.CC[0].reg = CLOCK_SHIFT_US; // 1ms: first doPostReceive
        while (TC3->COUNT16.SYNCBUSY.bit.CC0) {}
        TC3->COUNT16.CC[1].reg = CLOCK_PERIOD_US; // 20ms: frame boundary
        while (TC3->COUNT16.SYNCBUSY.bit.CC1) {}

        // Enable MC0 and MC1 interrupts
        TC3->COUNT16.INTENSET.reg = TC_INTENSET_MC0 | TC_INTENSET_MC1;
        NVIC_SetPriority(TC3_IRQn, 2);
        NVIC_EnableIRQ(TC3_IRQn);

        // Enable and start
        TC3->COUNT16.CTRLA.bit.ENABLE = 1;
        while (TC3->COUNT16.SYNCBUSY.bit.ENABLE) {}

        TC3->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_RETRIGGER;
        while (TC3->COUNT16.SYNCBUSY.bit.CTRLB) {}

        doPostReceive = false;
    }

    void SetPeriod(uint16_t period_ms)
    {
        CLOCK_PERIOD_US = (uint16_t)((uint32_t)period_ms * 1000UL);
    }

    void Reset(void)
    {
        doPostReceive = false;

        __disable_irq();

        // Retrigger to restart count from zero
        TC3->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_RETRIGGER;
        while (TC3->COUNT16.SYNCBUSY.bit.CTRLB) {}

        // Schedule doPostReceive at 1ms from now
        TC3->COUNT16.CC[0].reg = CLOCK_SHIFT_US;
        while (TC3->COUNT16.SYNCBUSY.bit.CC0) {}

        // Schedule frame boundary at period from now
        TC3->COUNT16.CC[1].reg = CLOCK_PERIOD_US;
        while (TC3->COUNT16.SYNCBUSY.bit.CC1) {}

        // Clear any pending flags
        TC3->COUNT16.INTFLAG.reg = TC_INTFLAG_MC0 | TC_INTFLAG_MC1;

        __enable_irq();
    }
};


#endif // SAMR34_RXCLOCK_H
