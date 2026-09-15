//*******************************************************
// Copyright (c) MLRS project
// GPL3
// https://www.gnu.org/licenses/gpl-3.0.de.html
//*******************************************************
// SAMR34 EEPROM standard library
// NVM flash emulation for settings storage
// Uses last rows of main flash (defined by EE_START_ADDR in HAL)
//*******************************************************
#ifndef SAMR34_EEPROM_H
#define SAMR34_EEPROM_H


#include "samr34.h"
#include <string.h>


//-------------------------------------------------------
// Status enum matching STM32 EEPROM driver API
//-------------------------------------------------------

typedef enum {
    EE_STATUS_FLASH_FAIL = 0,
    EE_STATUS_PAGE_UNDEF,
    EE_STATUS_PAGE_EMPTY,
    EE_STATUS_PAGE_FULL,
    EE_STATUS_OK
} EE_STATUS_ENUM;


//-------------------------------------------------------
// NVM helpers
//-------------------------------------------------------

static inline void nvm_wait_ready(void)
{
    while (!NVMCTRL->INTFLAG.bit.READY) {}
}


static inline bool nvm_erase_row(uint32_t addr)
{
    nvm_wait_ready();
    NVMCTRL->ADDR.reg = addr >> 1;
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER;
    nvm_wait_ready();
    return !(NVMCTRL->INTFLAG.bit.ERROR);
}


static inline bool nvm_write_page(uint32_t addr, const uint8_t* data, uint16_t len)
{
    nvm_wait_ready();

    // Clear page buffer
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_PBC;
    nvm_wait_ready();

    // Write data to page buffer as 32-bit words
    volatile uint32_t* dst = (volatile uint32_t*)addr;
    uint16_t words = (len + 3) / 4;
    uint32_t word;
    for (uint16_t i = 0; i < words; i++) {
        memcpy(&word, data + i * 4, 4);
        dst[i] = word;
    }

    // Write page
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP;
    nvm_wait_ready();
    return !(NVMCTRL->INTFLAG.bit.ERROR);
}


//-------------------------------------------------------
// EEPROM API matching mLRS expectations
// Uses a simple scheme: one row stores the data directly
//-------------------------------------------------------

// EE_START_ADDR, EE_PAGE_SIZE, EE_ROW_SIZE must be defined by HAL

EE_STATUS_ENUM ee_init(void)
{
    return EE_STATUS_OK;
}


EE_STATUS_ENUM ee_readdata(void* data, uint16_t datalen)
{
    memcpy(data, (const void*)EE_START_ADDR, datalen);
    return EE_STATUS_OK;
}


EE_STATUS_ENUM ee_writedata(void* data, uint16_t datalen)
{
    uint32_t addr = EE_START_ADDR;

    // Erase the row(s) covering the data
    uint16_t rows = (datalen + EE_ROW_SIZE - 1) / EE_ROW_SIZE;
    for (uint16_t r = 0; r < rows; r++) {
        if (!nvm_erase_row(addr + r * EE_ROW_SIZE)) return EE_STATUS_FLASH_FAIL;
    }

    // Write in page-sized chunks
    uint8_t* src = (uint8_t*)data;
    uint16_t offset = 0;
    while (offset < datalen) {
        uint16_t chunk = datalen - offset;
        if (chunk > EE_PAGE_SIZE) chunk = EE_PAGE_SIZE;
        if (!nvm_write_page(addr + offset, src + offset, chunk)) return EE_STATUS_FLASH_FAIL;
        offset += EE_PAGE_SIZE;
    }

    return EE_STATUS_OK;
}


EE_STATUS_ENUM ee_format(void)
{
    uint16_t rows = 4096 / EE_ROW_SIZE; // 4KB reserved
    for (uint16_t r = 0; r < rows; r++) {
        if (!nvm_erase_row(EE_START_ADDR + r * EE_ROW_SIZE)) return EE_STATUS_FLASH_FAIL;
    }
    return EE_STATUS_OK;
}


#endif // SAMR34_EEPROM_H
