//*******************************************************
// Copyright (c) MLRS project
// GPL3
// https://www.gnu.org/licenses/gpl-3.0.de.html
//*******************************************************
// SAMR34 UARTB standard library
// SERCOM USART with ISR-driven ring buffers
// Configure-by-define, header-only
//*******************************************************
#ifndef SAMR34_UARTB_H
#define SAMR34_UARTB_H
#ifdef __cplusplus
extern "C" {
#endif


#include "samr34.h"
#include "samr34-peripherals.h"


//-------------------------------------------------------
// UART protocol types (matching STM32 API)
//-------------------------------------------------------

typedef enum {
    XUART_PARITY_NO = 0,
    XUART_PARITY_EVEN = 1,
    XUART_PARITY_ODD = 2,
    XUART_PARITY_MAKEITU32 = UINT32_MAX,
} UARTPARITYENUM;

typedef enum {
    UART_STOPBIT_1 = 0,
    UART_STOPBIT_2 = 1,
    UART_STOPBIT_MAKEITU32 = UINT32_MAX,
} UARTSTOPBITENUM;


//-------------------------------------------------------
// Configuration defines expected from the board HAL:
//
// UARTB_USE_SERCOMn       — which SERCOM to use
// UARTB_BAUD              — baud rate (default 115200)
// UARTB_TXBUFSIZE         — TX buffer size (must be power of 2)
// UARTB_RXBUFSIZE         — RX buffer size (must be power of 2)
// UARTB_TX_IO             — TX pin
// UARTB_RX_IO             — RX pin
// UARTB_TX_PINMUX         — peripheral mux function number for TX
// UARTB_RX_PINMUX         — peripheral mux function number for RX
// UARTB_TX_PAD            — SERCOM pad number for TX
// UARTB_RX_PAD            — SERCOM pad number for RX
//-------------------------------------------------------

#ifndef UARTB_BAUD
#define UARTB_BAUD  115200
#endif

#ifndef UARTB_TXBUFSIZE
#define UARTB_TXBUFSIZE  256
#endif

#ifndef UARTB_RXBUFSIZE
#define UARTB_RXBUFSIZE  256
#endif


//-- Resolve SERCOM instance

#if defined UARTB_USE_SERCOM0
  #define UARTB_SERCOM          SERCOM0
  #define UARTB_SERCOM_N        0
  #ifndef UARTB_IRQn
  #define UARTB_IRQn            SERCOM0_IRQn
  #endif
  #define UARTB_GCLK_PCHCTRL   SERCOM0_GCLK_ID_CORE
#elif defined UARTB_USE_SERCOM1
  #define UARTB_SERCOM          SERCOM1
  #define UARTB_SERCOM_N        1
  #ifndef UARTB_IRQn
  #define UARTB_IRQn            SERCOM1_IRQn
  #endif
  #define UARTB_GCLK_PCHCTRL   SERCOM1_GCLK_ID_CORE
#elif defined UARTB_USE_SERCOM2
  #define UARTB_SERCOM          SERCOM2
  #define UARTB_SERCOM_N        2
  #ifndef UARTB_IRQn
  #define UARTB_IRQn            SERCOM2_IRQn
  #endif
  #define UARTB_GCLK_PCHCTRL   SERCOM2_GCLK_ID_CORE
#elif defined UARTB_USE_SERCOM3
  #define UARTB_SERCOM          SERCOM3
  #define UARTB_SERCOM_N        3
  #ifndef UARTB_IRQn
  #define UARTB_IRQn            SERCOM3_IRQn
  #endif
  #define UARTB_GCLK_PCHCTRL   SERCOM3_GCLK_ID_CORE
#elif defined UARTB_USE_SERCOM5
  #define UARTB_SERCOM          SERCOM5
  #define UARTB_SERCOM_N        5
  #ifndef UARTB_IRQn
  #define UARTB_IRQn            SERCOM5_IRQn
  #endif
  #define UARTB_GCLK_PCHCTRL   SERCOM5_GCLK_ID_CORE
#else
  #error "No UARTB_USE_SERCOMn defined!"
#endif


//-- Derive TXPO/RXPO from pad assignments
// TXPO: 0=PAD0, 1=PAD2, 2=PAD0(RTS/CTS)
// RXPO: pad number directly (0-3)

#if (UARTB_TX_PAD == 0)
  #define UARTB_TXPO  0
#elif (UARTB_TX_PAD == 2)
  #define UARTB_TXPO  1
#else
  #error "Invalid UARTB_TX_PAD for TXPO"
#endif

#define UARTB_RXPO  UARTB_RX_PAD


//-------------------------------------------------------
// Ring buffer
//-------------------------------------------------------

static volatile uint8_t uartb_txbuf[UARTB_TXBUFSIZE];
static volatile uint16_t uartb_txwritepos;
static volatile uint16_t uartb_txreadpos;

static volatile uint8_t uartb_rxbuf[UARTB_RXBUFSIZE];
static volatile uint16_t uartb_rxwritepos;
static volatile uint16_t uartb_rxreadpos;


//-------------------------------------------------------
// ISR
//-------------------------------------------------------

static inline void uartb_isr(void)
{
    // RX complete
    if (UARTB_SERCOM->USART.INTFLAG.bit.RXC) {
        uint8_t c = (uint8_t)UARTB_SERCOM->USART.DATA.reg;
        uint16_t next = (uartb_rxwritepos + 1) & (UARTB_RXBUFSIZE - 1);
        if (next != uartb_rxreadpos) { // not full
            uartb_rxbuf[uartb_rxwritepos] = c;
            uartb_rxwritepos = next;
        }
    }
    // TX data register empty
    if (UARTB_SERCOM->USART.INTFLAG.bit.DRE && UARTB_SERCOM->USART.INTENSET.bit.DRE) {
        if (uartb_txreadpos != uartb_txwritepos) {
            UARTB_SERCOM->USART.DATA.reg = uartb_txbuf[uartb_txreadpos];
            uartb_txreadpos = (uartb_txreadpos + 1) & (UARTB_TXBUFSIZE - 1);
        } else {
            UARTB_SERCOM->USART.INTENCLR.reg = SERCOM_USART_INTENCLR_DRE;
        }
    }
}


IRQHANDLER(
void UARTB_IRQHandler(void)
{
    uartb_isr();
})


//-------------------------------------------------------
// API
//-------------------------------------------------------

static inline void uartb_putc(char c)
{
    uint16_t next = (uartb_txwritepos + 1) & (UARTB_TXBUFSIZE - 1);
    while (next == uartb_txreadpos) {} // wait if full
    uartb_txbuf[uartb_txwritepos] = c;
    uartb_txwritepos = next;
    UARTB_SERCOM->USART.INTENSET.reg = SERCOM_USART_INTENSET_DRE;
}


static inline void uartb_puts(const char* s)
{
    while (*s) uartb_putc(*s++);
}


static inline void uartb_putbuf(uint8_t* buf, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) uartb_putc(buf[i]);
}


static inline uint16_t uartb_rx_available(void)
{
    return (uartb_rxwritepos - uartb_rxreadpos) & (UARTB_RXBUFSIZE - 1);
}


static inline char uartb_getc(void)
{
    char c = uartb_rxbuf[uartb_rxreadpos];
    uartb_rxreadpos = (uartb_rxreadpos + 1) & (UARTB_RXBUFSIZE - 1);
    return c;
}


static inline uint8_t uartb_tx_isempty(void)
{
    return (uartb_txreadpos == uartb_txwritepos) ? 1 : 0;
}


static inline void uartb_flush(void)
{
    while (!uartb_tx_isempty()) {}
}


static inline void uartb_rx_enableisr(void)
{
    UARTB_SERCOM->USART.INTENSET.reg = SERCOM_USART_INTENSET_RXC;
    nvic_irq_enable_w_priority(UARTB_IRQn, 15);
}


//-------------------------------------------------------
// Init
//-------------------------------------------------------

static inline void uartb_init(void)
{
    uartb_txwritepos = uartb_txreadpos = 0;
    uartb_rxwritepos = uartb_rxreadpos = 0;

    // Configure pin muxes
    gpio_init_af(UARTB_TX_IO, UARTB_TX_PINMUX);
    gpio_init_af(UARTB_RX_IO, UARTB_RX_PINMUX);

    // Enable bus clock
    rcc_init_sercom(UARTB_SERCOM_N);

    // Connect GCLK0 (16MHz) to SERCOM
    gclk_connect_pch(UARTB_GCLK_PCHCTRL, 0);

    // Disable SERCOM
    UARTB_SERCOM->USART.CTRLA.bit.ENABLE = 0;
    while (UARTB_SERCOM->USART.SYNCBUSY.bit.ENABLE) {}

    // CTRLA: internal clock, async mode, 8N1
    UARTB_SERCOM->USART.CTRLA.reg =
        SERCOM_USART_CTRLA_MODE(1) |         // USART with internal clock
        SERCOM_USART_CTRLA_DORD |             // LSB first
        SERCOM_USART_CTRLA_TXPO(UARTB_TXPO) |
        SERCOM_USART_CTRLA_RXPO(UARTB_RXPO) |
        SERCOM_USART_CTRLA_SAMPR(0);          // 16x arithmetic

    // CTRLB: TX and RX enable, 8-bit char
    UARTB_SERCOM->USART.CTRLB.reg =
        SERCOM_USART_CTRLB_TXEN |
        SERCOM_USART_CTRLB_RXEN;
    while (UARTB_SERCOM->USART.SYNCBUSY.bit.CTRLB) {}

    // Baud: 16x arithmetic: BAUD = 65536 * (1 - 16 * f_baud / f_ref)
    uint64_t br = (uint64_t)65536 * (SystemCoreClock - 16UL * UARTB_BAUD) / SystemCoreClock;
    UARTB_SERCOM->USART.BAUD.reg = (uint16_t)br;

    // Enable
    UARTB_SERCOM->USART.CTRLA.bit.ENABLE = 1;
    while (UARTB_SERCOM->USART.SYNCBUSY.bit.ENABLE) {}

    // Enable RX interrupt
    uartb_rx_enableisr();
}


//-------------------------------------------------------
// Additional API matching mLRS expectations
//-------------------------------------------------------

static inline uint16_t uartb_tx_notfull(void)
{
    uint16_t next = (uartb_txwritepos + 1) & (UARTB_TXBUFSIZE - 1);
    return (uartb_txreadpos != next) ? 1 : 0;
}


static inline void uartb_tx_flush(void)
{
    uartb_txwritepos = uartb_txreadpos = 0;
}


static inline uint16_t uartb_rx_bytesavailable(void)
{
    return (uartb_rxwritepos - uartb_rxreadpos) & (UARTB_RXBUFSIZE - 1);
}


static inline void uartb_rx_flush(void)
{
    uartb_rxwritepos = uartb_rxreadpos = 0;
}


static inline void uartb_setprotocol(uint32_t baud, UARTPARITYENUM parity, UARTSTOPBITENUM stopbits)
{
    // Disable USART
    UARTB_SERCOM->USART.CTRLA.bit.ENABLE = 0;
    while (UARTB_SERCOM->USART.SYNCBUSY.bit.ENABLE) {}

    // Reconfigure baud
    uint64_t br = (uint64_t)65536 * (SystemCoreClock - 16UL * baud) / SystemCoreClock;
    UARTB_SERCOM->USART.BAUD.reg = (uint16_t)br;

    // Re-enable
    UARTB_SERCOM->USART.CTRLA.bit.ENABLE = 1;
    while (UARTB_SERCOM->USART.SYNCBUSY.bit.ENABLE) {}
}


static inline void uartb_setbaudrate(uint32_t baud)
{
    uartb_setprotocol(baud, XUART_PARITY_NO, UART_STOPBIT_1);
}


static inline uint8_t uartb_has_systemboot(void)
{
    return 0;
}


//-------------------------------------------------------
#ifdef __cplusplus
}
#endif
#endif // SAMR34_UARTB_H
