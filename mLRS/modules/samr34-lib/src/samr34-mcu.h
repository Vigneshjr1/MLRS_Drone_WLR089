//*******************************************************
// Copyright (c) MLRS project
// GPL3
// https://www.gnu.org/licenses/gpl-3.0.de.html
//*******************************************************
// SAMR34 MCU standard library
// UID, reset, bootloader entry
//*******************************************************
#ifndef SAMR34_MCU_H
#define SAMR34_MCU_H
#ifdef __cplusplus
extern "C" {
#endif


#include "samr34.h"


//-------------------------------------------------------
// Unique device ID
// SAMR34 serial number stored at 0x008061FC, 0x00806010, 0x00806014, 0x00806018
//-------------------------------------------------------

#define SAMR34_UID_BASE0  (*(volatile uint32_t*)0x008061FCUL)
#define SAMR34_UID_BASE1  (*(volatile uint32_t*)0x00806010UL)
#define SAMR34_UID_BASE2  (*(volatile uint32_t*)0x00806014UL)
#define SAMR34_UID_BASE3  (*(volatile uint32_t*)0x00806018UL)


static inline uint32_t mcu_uid(void)
{
    return SAMR34_UID_BASE0 ^ SAMR34_UID_BASE1 ^ SAMR34_UID_BASE2 ^ SAMR34_UID_BASE3;
}


static inline void mcu_uid3(uint32_t uid[3])
{
    uid[0] = SAMR34_UID_BASE0 ^ SAMR34_UID_BASE1;
    uid[1] = SAMR34_UID_BASE2;
    uid[2] = SAMR34_UID_BASE3;
}


//-------------------------------------------------------
// System reset
//-------------------------------------------------------

static inline void mcu_reset(void)
{
    NVIC_SystemReset();
}


//-------------------------------------------------------
// Bootloader entry
//-------------------------------------------------------

static inline void mcu_enter_bootloader(void)
{
    NVIC_SystemReset();
}


void BootLoaderInit(void)
{
    __disable_irq();
    NVIC_SystemReset();
}


//-------------------------------------------------------
#ifdef __cplusplus
}
#endif
#endif // SAMR34_MCU_H
