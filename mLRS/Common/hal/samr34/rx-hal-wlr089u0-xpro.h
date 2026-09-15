//*******************************************************
// Copyright (c) MLRS project
// GPL3
// https://www.gnu.org/licenses/gpl-3.0.de.html
//*******************************************************
// hal
//********************************************************

//-------------------------------------------------------
// SAMR34/WLR089U0, WLR089U0 Xplained Pro, RX
//-------------------------------------------------------

#define DEVICE_HAS_SINGLE_LED
#define DEVICE_HAS_NO_DEBUG


//-- UARTS
// UARTB = serial port
// UART = output port, SBus or whatever
// UARTF = debug port

#define UARTB_USE_SERCOM0
#define UARTB_BAUD                RX_SERIAL_BAUDRATE
#define UARTB_TXBUFSIZE           RX_SERIAL_TXBUFSIZE
#define UARTB_RXBUFSIZE           RX_SERIAL_RXBUFSIZE
#define UARTB_TX_IO               IO_PA04
#define UARTB_RX_IO               IO_PA05
#define UARTB_TX_PAD              0 // SERCOM0 PAD0
#define UARTB_RX_PAD              1 // SERCOM0 PAD1
#define UARTB_TX_PINMUX           3 // PMUX function D (SERCOM0)
#define UARTB_RX_PINMUX           3 // PMUX function D (SERCOM0)
#define UARTB_IRQn                SERCOM0_IRQn
#define UARTB_IRQHandler          SERCOM0_Handler


//-- SX1: SX1276 integrated radio & SPI (SERCOM4)

#define SPI_USE_SERCOM4
#define SPI_CS_IO                 IO_PB31
#define SPI_MOSI_IO               IO_PB30
#define SPI_MISO_IO               IO_PC19
#define SPI_SCK_IO                IO_PC18
#define SPI_MOSI_PAD              2 // SERCOM4 PAD2
#define SPI_MISO_PAD              0 // SERCOM4 PAD0
#define SPI_SCK_PAD               3 // SERCOM4 PAD3
#define SPI_MOSI_PINMUX           5 // PMUX function F (SERCOM4)
#define SPI_MISO_PINMUX           5 // PMUX function F
#define SPI_SCK_PINMUX            5 // PMUX function F
#define SPI_BAUDRATE              4000000

#define SX_RESET                  IO_PB15
#define SX_DIO                    IO_PB16  // DIO0
#define SX_DIO_EXTI_LINE          0        // EXTINT0
#define SX_DIO_EXTI_IRQn          EIC_IRQn

#define RF_SWITCH_IO              IO_PA13
#define TCXO_PWR_IO               IO_PA09


IRQHANDLER(void SX_DIO_EXTI_IRQHandler(void);)
IRQHANDLER(
void EIC_Handler(void)
{
    SX_DIO_EXTI_IRQHandler();
})

void sx_init_gpio(void)
{
    gpio_init(SX_RESET, IO_MODE_OUTPUT_PP_HIGH);
    gpio_init(SX_DIO, IO_MODE_INPUT_PD);
    gpio_init(RF_SWITCH_IO, IO_MODE_OUTPUT_PP_LOW);
    gpio_init(TCXO_PWR_IO, IO_MODE_OUTPUT_PP_HIGH); // enable TCXO
    delay_ms(2); // TCXO stabilization
}

void sx_post_reset_init(void)
{
    gpio_low(SPI_CS_IO);
    while (!(SERCOM4->SPI.INTFLAG.bit.DRE)) {}
    SERCOM4->SPI.DATA.reg = 0x4B | 0x80;
    while (!(SERCOM4->SPI.INTFLAG.bit.RXC)) {}
    (void)SERCOM4->SPI.DATA.reg;
    while (!(SERCOM4->SPI.INTFLAG.bit.DRE)) {}
    SERCOM4->SPI.DATA.reg = 0x19;
    while (!(SERCOM4->SPI.INTFLAG.bit.RXC)) {}
    (void)SERCOM4->SPI.DATA.reg;
    gpio_high(SPI_CS_IO);
}

void sx_amp_transmit(void)
{
    gpio_low(RF_SWITCH_IO); // PA_BOOST path
}

void sx_amp_receive(void)
{
    gpio_high(RF_SWITCH_IO); // RFO_HF/LNA path
}

void sx_dio_init_exti_isroff(void)
{
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_EIC;

    EIC->CTRLA.reg = EIC_CTRLA_SWRST;
    while (EIC->SYNCBUSY.bit.SWRST);

    EIC->CTRLA.bit.CKSEL = 1;

    PORT->Group[IO_PIN_GROUP(SX_DIO)].PINCFG[IO_PIN_NUM(SX_DIO)].reg = PORT_PINCFG_PMUXEN | PORT_PINCFG_INEN;
    PORT->Group[IO_PIN_GROUP(SX_DIO)].PMUX[IO_PIN_NUM(SX_DIO) >> 1].bit.PMUXE = 0;

    EIC->CONFIG[0].reg = EIC_CONFIG_SENSE0_RISE;

    EIC->CTRLA.bit.ENABLE = 1;
    while (EIC->SYNCBUSY.bit.ENABLE);

    EIC->INTFLAG.reg = (1 << SX_DIO_EXTI_LINE);
    NVIC_SetPriority(EIC_IRQn, SX_DIO_EXTI_IRQ_PRIORITY);
}

void sx_dio_enable_exti_isr(void)
{
    EIC->INTFLAG.reg = (1 << SX_DIO_EXTI_LINE);
    EIC->INTENSET.reg = (1 << SX_DIO_EXTI_LINE);
    NVIC_EnableIRQ(EIC_IRQn);
}

void sx_dio_exti_isr_clearflag(void)
{
    EIC->INTFLAG.reg = (1 << SX_DIO_EXTI_LINE);
}

void sx_dio_poll(void) {}


//-- Button

#define BUTTON                    IO_PA28

void button_init(void)
{
    gpio_init(BUTTON, IO_MODE_INPUT_PU);
}

bool button_pressed(void)
{
    return gpio_read_activelow(BUTTON) ? true : false;
}


//-- LEDs

#define LED_RED                   IO_PA19  // LED0 active low

void leds_init(void)
{
    gpio_init(LED_RED, IO_MODE_OUTPUT_PP_HIGH); // start OFF (active low)
}

void led_red_off(void) { gpio_high(LED_RED); }
void led_red_on(void) { gpio_low(LED_RED); }
void led_red_toggle(void) { gpio_toggle(LED_RED); }


//-- Timers

#define MICROS_TC                 TC4
#define MICROS_TC_IRQn            TC4_IRQn

#define CLOCK_TC                  TC3
#define CLOCK_IRQn                TC3_IRQn
#define CLOCK_IRQHandler          TC3_Handler


//-- EEPROM

#define EE_USE_FLASH
#define EE_PAGE_SIZE              64
#define EE_ROW_SIZE               256
// Last 4KB of 256KB flash = page 3968 (0x3F000)
#define EE_START_PAGE             3968
#define EE_START_ADDR             (0x00000000 + EE_START_PAGE * EE_PAGE_SIZE)


//-- POWER

#define POWER_GAIN_DBM            0
#define POWER_SX1276_MAX          SX1276_OUTPUT_POWER_MAX
#define POWER_USE_DEFAULT_RFPOWER_CALC

#define RFPOWER_DEFAULT           1 // index into rfpower_list array

const rfpower_t rfpower_list[] = {
    { .dbm = POWER_0_DBM, .mW = 1 },
    { .dbm = POWER_10_DBM, .mW = 10 },
    { .dbm = POWER_17_DBM, .mW = 50 },
};
