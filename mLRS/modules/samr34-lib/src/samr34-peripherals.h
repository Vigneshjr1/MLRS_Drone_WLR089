//*******************************************************
// Copyright (c) MLRS project
// GPL3
// https://www.gnu.org/licenses/gpl-3.0.de.html
//*******************************************************
// SAMR34 PERIPHERALS standard library
// bare-metal register-level drivers for SAMR34/WLR089
//*******************************************************
#ifndef SAMR34_PERIPHERALS_H
#define SAMR34_PERIPHERALS_H
#ifdef __cplusplus
extern "C" {
#endif


#include "samr34.h"


//-------------------------------------------------------
// GCLK peripheral channel IDs (missing from our CMSIS headers)
//-------------------------------------------------------

#define EIC_GCLK_ID               3
#define SERCOM0_GCLK_ID_CORE      18
#define SERCOM1_GCLK_ID_CORE      19
#define SERCOM2_GCLK_ID_CORE      20
#define SERCOM3_GCLK_ID_CORE      21
#define SERCOM4_GCLK_ID_CORE      22
#define SERCOM5_GCLK_ID_CORE      24
#define TC0_GCLK_ID               27
#define TC3_GCLK_ID               28
#define TC4_GCLK_ID               29


//-------------------------------------------------------
// NVIC
//-------------------------------------------------------

static inline void nvic_irq_enable_w_priority(IRQn_Type IRQn, uint32_t priority)
{
    NVIC_SetPriority(IRQn, priority);
    NVIC_EnableIRQ(IRQn);
}


//-------------------------------------------------------
// Pin encoding
//-------------------------------------------------------
// IO pin = (port_group << 8) | pin_number
// port_group: 0=A, 1=B, 2=C

#define IO_PA0   ((uint16_t)(0x0000))
#define IO_PA1   ((uint16_t)(0x0001))
#define IO_PA2   ((uint16_t)(0x0002))
#define IO_PA3   ((uint16_t)(0x0003))
#define IO_PA4   ((uint16_t)(0x0004))
#define IO_PA5   ((uint16_t)(0x0005))
#define IO_PA6   ((uint16_t)(0x0006))
#define IO_PA7   ((uint16_t)(0x0007))
#define IO_PA8   ((uint16_t)(0x0008))
#define IO_PA9   ((uint16_t)(0x0009))
#define IO_PA10  ((uint16_t)(0x000A))
#define IO_PA11  ((uint16_t)(0x000B))
#define IO_PA12  ((uint16_t)(0x000C))
#define IO_PA13  ((uint16_t)(0x000D))
#define IO_PA14  ((uint16_t)(0x000E))
#define IO_PA15  ((uint16_t)(0x000F))
#define IO_PA16  ((uint16_t)(0x0010))
#define IO_PA17  ((uint16_t)(0x0011))
#define IO_PA18  ((uint16_t)(0x0012))
#define IO_PA19  ((uint16_t)(0x0013))
#define IO_PA20  ((uint16_t)(0x0014))
#define IO_PA21  ((uint16_t)(0x0015))
#define IO_PA22  ((uint16_t)(0x0016))
#define IO_PA23  ((uint16_t)(0x0017))
#define IO_PA24  ((uint16_t)(0x0018))
#define IO_PA25  ((uint16_t)(0x0019))
#define IO_PA27  ((uint16_t)(0x001B))
#define IO_PA28  ((uint16_t)(0x001C))
#define IO_PA30  ((uint16_t)(0x001E))
#define IO_PA31  ((uint16_t)(0x001F))

// Zero-padded aliases
#define IO_PA00  IO_PA0
#define IO_PA01  IO_PA1
#define IO_PA02  IO_PA2
#define IO_PA03  IO_PA3
#define IO_PA04  IO_PA4
#define IO_PA05  IO_PA5
#define IO_PA06  IO_PA6
#define IO_PA07  IO_PA7
#define IO_PA08  IO_PA8
#define IO_PA09  IO_PA9

#define IO_PB0   ((uint16_t)(0x0100))
#define IO_PB1   ((uint16_t)(0x0101))
#define IO_PB2   ((uint16_t)(0x0102))
#define IO_PB3   ((uint16_t)(0x0103))
#define IO_PB4   ((uint16_t)(0x0104))
#define IO_PB5   ((uint16_t)(0x0105))
#define IO_PB6   ((uint16_t)(0x0106))
#define IO_PB7   ((uint16_t)(0x0107))
#define IO_PB8   ((uint16_t)(0x0108))
#define IO_PB9   ((uint16_t)(0x0109))
#define IO_PB10  ((uint16_t)(0x010A))
#define IO_PB11  ((uint16_t)(0x010B))
#define IO_PB12  ((uint16_t)(0x010C))
#define IO_PB13  ((uint16_t)(0x010D))
#define IO_PB14  ((uint16_t)(0x010E))
#define IO_PB15  ((uint16_t)(0x010F))
#define IO_PB16  ((uint16_t)(0x0110))
#define IO_PB17  ((uint16_t)(0x0111))
#define IO_PB22  ((uint16_t)(0x0116))
#define IO_PB23  ((uint16_t)(0x0117))
#define IO_PB30  ((uint16_t)(0x011E))
#define IO_PB31  ((uint16_t)(0x011F))

// Zero-padded aliases
#define IO_PB00  IO_PB0
#define IO_PB01  IO_PB1
#define IO_PB02  IO_PB2
#define IO_PB03  IO_PB3
#define IO_PB04  IO_PB4
#define IO_PB05  IO_PB5
#define IO_PB06  IO_PB6
#define IO_PB07  IO_PB7
#define IO_PB08  IO_PB8
#define IO_PB09  IO_PB9

#define IO_PC18  ((uint16_t)(0x0212))
#define IO_PC19  ((uint16_t)(0x0213))

#define IO_PIN_GROUP(io)   ((uint8_t)((io) >> 8))
#define IO_PIN_NUM(io)     ((uint8_t)((io) & 0xFF))
#define IO_PIN_MASK(io)    ((uint32_t)(1UL << IO_PIN_NUM(io)))


//-------------------------------------------------------
// GPIO modes
//-------------------------------------------------------

typedef enum {
    IO_MODE_INPUT_ANALOG = 0,
    IO_MODE_INPUT_PU,
    IO_MODE_INPUT_PD,
    IO_MODE_OUTPUT_PP_LOW,
    IO_MODE_OUTPUT_PP_HIGH,
    IO_MODE_OUTPUT_OD_LOW,
    IO_MODE_OUTPUT_OD_HIGH,
} IO_MODE_ENUM;

// Peripheral mux function IDs (A=0, B=1, ..., H=7)
typedef enum {
    IO_MUX_A = 0,
    IO_MUX_B,
    IO_MUX_C,
    IO_MUX_D,
    IO_MUX_E,
    IO_MUX_F,
    IO_MUX_G,
    IO_MUX_H,
} IO_MUX_ENUM;


//-------------------------------------------------------
// GPIO functions
//-------------------------------------------------------

static inline void gpio_init(uint16_t io, IO_MODE_ENUM mode)
{
    uint8_t group = IO_PIN_GROUP(io);
    uint8_t pin = IO_PIN_NUM(io);
    uint32_t mask = (1UL << pin);

    PORT->Group[group].PINCFG[pin].reg = 0; // reset pin config

    switch (mode) {
    case IO_MODE_INPUT_ANALOG:
        PORT->Group[group].DIRCLR.reg = mask;
        break;
    case IO_MODE_INPUT_PU:
        PORT->Group[group].DIRCLR.reg = mask;
        PORT->Group[group].PINCFG[pin].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
        PORT->Group[group].OUTSET.reg = mask; // pull-up
        break;
    case IO_MODE_INPUT_PD:
        PORT->Group[group].DIRCLR.reg = mask;
        PORT->Group[group].PINCFG[pin].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
        PORT->Group[group].OUTCLR.reg = mask; // pull-down
        break;
    case IO_MODE_OUTPUT_PP_LOW:
        PORT->Group[group].DIRSET.reg = mask;
        PORT->Group[group].OUTCLR.reg = mask;
        break;
    case IO_MODE_OUTPUT_PP_HIGH:
        PORT->Group[group].DIRSET.reg = mask;
        PORT->Group[group].OUTSET.reg = mask;
        break;
    case IO_MODE_OUTPUT_OD_LOW:
        PORT->Group[group].DIRSET.reg = mask;
        PORT->Group[group].PINCFG[pin].reg = PORT_PINCFG_INEN;
        PORT->Group[group].OUTCLR.reg = mask;
        break;
    case IO_MODE_OUTPUT_OD_HIGH:
        PORT->Group[group].DIRSET.reg = mask;
        PORT->Group[group].PINCFG[pin].reg = PORT_PINCFG_INEN;
        PORT->Group[group].OUTSET.reg = mask;
        break;
    }
}


static inline void gpio_init_af(uint16_t io, uint8_t mux)
{
    uint8_t group = IO_PIN_GROUP(io);
    uint8_t pin = IO_PIN_NUM(io);

    PORT->Group[group].PINCFG[pin].reg = PORT_PINCFG_PMUXEN | PORT_PINCFG_INEN;
    if (pin & 1) {
        PORT->Group[group].PMUX[pin >> 1].bit.PMUXO = mux;
    } else {
        PORT->Group[group].PMUX[pin >> 1].bit.PMUXE = mux;
    }
}


static inline void gpio_high(uint16_t io)
{
    PORT->Group[IO_PIN_GROUP(io)].OUTSET.reg = IO_PIN_MASK(io);
}


static inline void gpio_low(uint16_t io)
{
    PORT->Group[IO_PIN_GROUP(io)].OUTCLR.reg = IO_PIN_MASK(io);
}


static inline void gpio_toggle(uint16_t io)
{
    PORT->Group[IO_PIN_GROUP(io)].OUTTGL.reg = IO_PIN_MASK(io);
}


static inline uint8_t gpio_read_activehigh(uint16_t io)
{
    return (PORT->Group[IO_PIN_GROUP(io)].IN.reg & IO_PIN_MASK(io)) ? 1 : 0;
}


static inline uint8_t gpio_read_activelow(uint16_t io)
{
    return (PORT->Group[IO_PIN_GROUP(io)].IN.reg & IO_PIN_MASK(io)) ? 0 : 1;
}


//-------------------------------------------------------
// MCLK peripheral clock enable
//-------------------------------------------------------

static inline void rcc_init_sercom(uint8_t n)
{
    switch (n) {
    case 0: MCLK->APBCMASK.reg |= MCLK_APBCMASK_SERCOM0; break;
    case 1: MCLK->APBCMASK.reg |= MCLK_APBCMASK_SERCOM1; break;
    case 2: MCLK->APBCMASK.reg |= MCLK_APBCMASK_SERCOM2; break;
    case 3: MCLK->APBCMASK.reg |= MCLK_APBCMASK_SERCOM3; break;
    case 4: MCLK->APBCMASK.reg |= MCLK_APBCMASK_SERCOM4; break;
    case 5: MCLK->APBDMASK.reg |= MCLK_APBDMASK_SERCOM5; break;
    }
}


static inline void rcc_init_tc(uint8_t n)
{
    switch (n) {
    case 0: MCLK->APBCMASK.reg |= MCLK_APBCMASK_TC0; break;
    case 1: MCLK->APBCMASK.reg |= MCLK_APBCMASK_TC1; break;
    case 2: MCLK->APBCMASK.reg |= MCLK_APBCMASK_TC2; break;
    case 3: MCLK->APBCMASK.reg |= MCLK_APBCMASK_TC3; break;
    case 4: MCLK->APBDMASK.reg |= MCLK_APBDMASK_TC4; break;
    }
}


static inline void rcc_init_eic(void)
{
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_EIC;
}


//-------------------------------------------------------
// GCLK helpers
//-------------------------------------------------------
// SAMR34 GCLK peripheral channel IDs (from samr34j18b.h)
// GCLK_PCHCTRL indices for SERCOMs, TCs, EIC, etc.

// Wait for GCLK sync after writing GENCTRL
static inline void gclk_sync_genctrl(uint8_t gen)
{
    while (GCLK->SYNCBUSY.reg & (1UL << (GCLK_SYNCBUSY_GENCTRL0_Pos + gen))) {}
}


// Connect a peripheral channel to a GCLK generator
static inline void gclk_connect_pch(uint8_t pch_id, uint8_t gen)
{
    GCLK->PCHCTRL[pch_id].reg = GCLK_PCHCTRL_GEN(gen) | GCLK_PCHCTRL_CHEN;
    while (!(GCLK->PCHCTRL[pch_id].reg & GCLK_PCHCTRL_CHEN)) {}
}


//-------------------------------------------------------
// Clock init
//-------------------------------------------------------
// Configures: OSC16M at 16MHz -> GCLK0 (CPU), XOSC32K -> GCLK1

static inline void clock_init(void)
{
    // Set flash wait states for 16MHz (1 wait state at >8MHz)
    NVMCTRL->CTRLB.bit.RWS = 1;

    // Configure OSC16M to 16MHz
    OSCCTRL->OSC16MCTRL.reg = OSCCTRL_OSC16MCTRL_ENABLE | OSCCTRL_OSC16MCTRL_FSEL_16;
    while (!(OSCCTRL->STATUS.reg & OSCCTRL_STATUS_OSC16MRDY)) {}

    // GCLK0: source from OSC16M, no prescaler -> 16MHz CPU clock
    GCLK->GENCTRL[0].reg = GCLK_GENCTRL_SRC_OSC16M | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_DIV(1);
    gclk_sync_genctrl(0);

    // GCLK1: XOSC32K for RTC and low-power peripherals
    OSC32KCTRL->XOSC32K.reg = OSC32KCTRL_XOSC32K_XTALEN |
                               OSC32KCTRL_XOSC32K_EN32K |
                               OSC32KCTRL_XOSC32K_RUNSTDBY |
                               OSC32KCTRL_XOSC32K_ENABLE;
    while (!(OSC32KCTRL->STATUS.reg & OSC32KCTRL_STATUS_XOSC32KRDY)) {}

    GCLK->GENCTRL[1].reg = GCLK_GENCTRL_SRC_XOSC32K | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_DIV(1);
    gclk_sync_genctrl(1);

    // GCLK2: OSC16M / 16 = 1MHz for microsecond timers
    GCLK->GENCTRL[2].reg = GCLK_GENCTRL_SRC_OSC16M | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_DIV(16);
    gclk_sync_genctrl(2);

    // Update SystemCoreClock
    SystemCoreClock = 16000000UL;
}


//-------------------------------------------------------
#ifdef __cplusplus
}
#endif
#endif // SAMR34_PERIPHERALS_H
