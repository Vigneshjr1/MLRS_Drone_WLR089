//*******************************************************
// Copyright (c) MLRS project
// GPL3
// https://www.gnu.org/licenses/gpl-3.0.de.html
//*******************************************************
// SAMR34 SPI standard library
// SERCOM SPI master, header-only, configure-by-define
//*******************************************************
#ifndef SAMR34_SPI_H
#define SAMR34_SPI_H
#ifdef __cplusplus
extern "C" {
#endif


#include "samr34.h"
#include "samr34-peripherals.h"


//-------------------------------------------------------
// Configuration defines expected from the board HAL:
//
// SPI_USE_SERCOMn       — which SERCOM to use (e.g. SPI_USE_SERCOM4)
// SPI_CS_IO             — chip-select pin (IO_PBxx)
// SPI_MOSI_IO           — MOSI pin
// SPI_MISO_IO           — MISO pin
// SPI_SCK_IO            — SCK pin
// SPI_MOSI_PINMUX       — peripheral mux function number for MOSI
// SPI_MISO_PINMUX       — peripheral mux function number for MISO
// SPI_SCK_PINMUX        — peripheral mux function number for SCK
// SPI_MOSI_PAD          — SERCOM pad number for MOSI
// SPI_MISO_PAD          — SERCOM pad number for MISO
// SPI_SCK_PAD           — SERCOM pad number for SCK
// SPI_BAUDRATE          — desired SPI clock in Hz (default 2000000)
//-------------------------------------------------------

#ifndef SPI_BAUDRATE
#define SPI_BAUDRATE  2000000UL
#endif


//-- Resolve SERCOM instance

#if defined SPI_USE_SERCOM0
  #define SPI_SERCOM          SERCOM0
  #define SPI_SERCOM_N        0
  #define SPI_IRQn            SERCOM0_IRQn
  #define SPI_GCLK_PCHCTRL   SERCOM0_GCLK_ID_CORE
#elif defined SPI_USE_SERCOM1
  #define SPI_SERCOM          SERCOM1
  #define SPI_SERCOM_N        1
  #define SPI_IRQn            SERCOM1_IRQn
  #define SPI_GCLK_PCHCTRL   SERCOM1_GCLK_ID_CORE
#elif defined SPI_USE_SERCOM2
  #define SPI_SERCOM          SERCOM2
  #define SPI_SERCOM_N        2
  #define SPI_IRQn            SERCOM2_IRQn
  #define SPI_GCLK_PCHCTRL   SERCOM2_GCLK_ID_CORE
#elif defined SPI_USE_SERCOM3
  #define SPI_SERCOM          SERCOM3
  #define SPI_SERCOM_N        3
  #define SPI_IRQn            SERCOM3_IRQn
  #define SPI_GCLK_PCHCTRL   SERCOM3_GCLK_ID_CORE
#elif defined SPI_USE_SERCOM4
  #define SPI_SERCOM          SERCOM4
  #define SPI_SERCOM_N        4
  #define SPI_IRQn            SERCOM4_IRQn
  #define SPI_GCLK_PCHCTRL   SERCOM4_GCLK_ID_CORE
#elif defined SPI_USE_SERCOM5
  #define SPI_SERCOM          SERCOM5
  #define SPI_SERCOM_N        5
  #define SPI_IRQn            SERCOM5_IRQn
  #define SPI_GCLK_PCHCTRL   SERCOM5_GCLK_ID_CORE
#else
  #error "No SPI_USE_SERCOMn defined!"
#endif


//-- Derive DOPO from MOSI/SCK pad assignments

#if (SPI_MOSI_PAD == 0) && (SPI_SCK_PAD == 1)
  #define SPI_DOPO  0
#elif (SPI_MOSI_PAD == 2) && (SPI_SCK_PAD == 3)
  #define SPI_DOPO  1
#elif (SPI_MOSI_PAD == 3) && (SPI_SCK_PAD == 1)
  #define SPI_DOPO  2
#elif (SPI_MOSI_PAD == 0) && (SPI_SCK_PAD == 3)
  #define SPI_DOPO  3
#else
  #error "Invalid SPI_MOSI_PAD/SPI_SCK_PAD combination for DOPO"
#endif

#define SPI_DIPO  SPI_MISO_PAD


//-------------------------------------------------------
// SPI nop byte (sent during reads)
//-------------------------------------------------------

static uint8_t spi_nop_byte = 0xFF;

static inline void spi_setnop(uint8_t nop)
{
    spi_nop_byte = nop;
}


//-------------------------------------------------------
// CS control
//-------------------------------------------------------

static inline void spi_select(void)
{
    gpio_low(SPI_CS_IO);
}


static inline void spi_deselect(void)
{
    gpio_high(SPI_CS_IO);
}


//-------------------------------------------------------
// Single-byte transfer
//-------------------------------------------------------

static inline uint8_t spi_transferbyte(uint8_t out)
{
    while (!(SPI_SERCOM->SPI.INTFLAG.bit.DRE)) {}
    SPI_SERCOM->SPI.DATA.reg = out;
    while (!(SPI_SERCOM->SPI.INTFLAG.bit.RXC)) {}
    return (uint8_t)SPI_SERCOM->SPI.DATA.reg;
}


//-------------------------------------------------------
// Bulk transfer (bidirectional)
//-------------------------------------------------------

static inline void spi_transfer(uint8_t* dataout, uint8_t* datain, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++) {
        datain[i] = spi_transferbyte(dataout[i]);
    }
}


//-------------------------------------------------------
// Bulk read (sends nop bytes)
//-------------------------------------------------------

static inline void spi_read(uint8_t* datain, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++) {
        datain[i] = spi_transferbyte(spi_nop_byte);
    }
}


//-------------------------------------------------------
// Bulk write (discards received data)
//-------------------------------------------------------

static inline void spi_write(uint8_t* dataout, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++) {
        spi_transferbyte(dataout[i]);
    }
}


//-------------------------------------------------------
// Init
//-------------------------------------------------------

static inline void spi_init(void)
{
    // CS pin as output, high (deselected)
    gpio_init(SPI_CS_IO, IO_MODE_OUTPUT_PP_HIGH);

    // Configure pin muxes
    gpio_init_af(SPI_MOSI_IO, SPI_MOSI_PINMUX);
    gpio_init_af(SPI_MISO_IO, SPI_MISO_PINMUX);
    gpio_init_af(SPI_SCK_IO, SPI_SCK_PINMUX);

    // Enable bus clock
    rcc_init_sercom(SPI_SERCOM_N);

    // Connect GCLK0 (16MHz) to SERCOM
    gclk_connect_pch(SPI_GCLK_PCHCTRL, 0);

    // Disable SERCOM before configuring
    SPI_SERCOM->SPI.CTRLA.bit.ENABLE = 0;
    while (SPI_SERCOM->SPI.SYNCBUSY.bit.ENABLE) {}

    // CTRLA: SPI master, CPOL=0, CPHA=0
    SPI_SERCOM->SPI.CTRLA.reg =
        SERCOM_SPI_CTRLA_MODE(3) |           // SPI master
        SERCOM_SPI_CTRLA_DOPO(SPI_DOPO) |
        SERCOM_SPI_CTRLA_DIPO(SPI_DIPO);

    // CTRLB: 8-bit char, receiver enable
    SPI_SERCOM->SPI.CTRLB.reg = SERCOM_SPI_CTRLB_RXEN;
    while (SPI_SERCOM->SPI.SYNCBUSY.bit.CTRLB) {}

    // Baud rate: BAUD = (f_ref / 2*f_baud) - 1
    uint32_t baud = (SystemCoreClock / (2UL * SPI_BAUDRATE)) - 1;
    if (baud > 255) baud = 255;
    SPI_SERCOM->SPI.BAUD.reg = (uint8_t)baud;

    // Enable
    SPI_SERCOM->SPI.CTRLA.bit.ENABLE = 1;
    while (SPI_SERCOM->SPI.SYNCBUSY.bit.ENABLE) {}
}


//-------------------------------------------------------
#ifdef __cplusplus
}
#endif
#endif // SAMR34_SPI_H
