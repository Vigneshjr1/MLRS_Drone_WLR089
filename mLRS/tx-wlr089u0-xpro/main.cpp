//*******************************************************
// Copyright (c) MLRS project
// GPL3
// https://www.gnu.org/licenses/gpl-3.0.de.html
//*******************************************************
// mLRS TX — WLR089U0 Xplained Pro (SAMR34J18B)
//*******************************************************

#include "samr34.h"

extern int main_main(void);


void SystemClock_Config(void)
{
    // Set flash wait states for 16MHz (1 WS at >8MHz)
    NVMCTRL->CTRLB.bit.RWS = 1;

    // Configure OSC16M to 16MHz
    OSCCTRL->OSC16MCTRL.reg = OSCCTRL_OSC16MCTRL_ENABLE | OSCCTRL_OSC16MCTRL_FSEL_16;
    while (!OSCCTRL->STATUS.bit.OSC16MRDY);

    // GCLK0: OSC16M -> 16MHz CPU clock
    GCLK->GENCTRL[0].reg = GCLK_GENCTRL_SRC_OSC16M | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_DIV(1);
    while (GCLK->SYNCBUSY.bit.GENCTRL0);

    SystemCoreClock = 16000000UL;

    // GCLK1: XOSC32K for RTC
    OSC32KCTRL->XOSC32K.reg =
        OSC32KCTRL_XOSC32K_XTALEN |
        OSC32KCTRL_XOSC32K_EN32K |
        OSC32KCTRL_XOSC32K_RUNSTDBY |
        OSC32KCTRL_XOSC32K_STARTUP(6) |
        OSC32KCTRL_XOSC32K_ENABLE;
    while (!OSC32KCTRL->STATUS.bit.XOSC32KRDY);

    GCLK->GENCTRL[1].reg = GCLK_GENCTRL_SRC_XOSC32K | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_DIV(1);
    while (GCLK->SYNCBUSY.bit.GENCTRL1);

    // GCLK2: OSC16M / 16 = 1MHz for microsecond timers (TC4 delay)
    GCLK->GENCTRL[2].reg = GCLK_GENCTRL_SRC_OSC16M | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_DIV(16);
    while (GCLK->SYNCBUSY.bit.GENCTRL2);

    // Clear any pending interrupt flags from oscillator startup to prevent
    // spurious SYSTEM_Handler entry (IRQ3: OSCCTRL/OSC32KCTRL combined)
    OSCCTRL->INTFLAG.reg = OSCCTRL->INTFLAG.reg;
    OSC32KCTRL->INTFLAG.reg = OSC32KCTRL->INTFLAG.reg;
    SUPC->INTFLAG.reg = SUPC->INTFLAG.reg;
}


int main(void)
{
    SystemClock_Config();
    return main_main();
}
