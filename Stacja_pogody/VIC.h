/*************************************************************************************
 *
 * @Description:
 * Lista stałych używanych do programowania 
 * Wektoryzowanego kontrolera przerwań (VIC)
 * Wskazane jest uzupełnienie listy
 *
 * @Authors: Michał Morawski, 
 *           Daniel Arendt, 
 *           Przemysław Ignaciuk,
 *           Marcin Kwapisz
 *
 * @Change log:
 *           2016.12.01: Wersja oryginalna.
 *
 ******************************************************************************/

#ifndef __VIC_H__
#define __VIC_H__

/******************************************************************************
 * Includes
 *****************************************************************************/

#include "general.h"

/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/

#define WATCHDOG_IRQ_NO (0)
#define WATCHDOG_IRQ    _BIT(WATCHDOG_IRQ_NO)
#define TIMER_0_IRQ_NO  (4)
#define TIMER_0_IRQ     _BIT(TIMER_0_IRQ_NO)
#define TIMER_1_IRQ_NO  (5)
#define TIMER_1_IRQ     _BIT(TIMER_1_IRQ_NO)
#define UART_0_IRQ_NO   (6)
#define UART_0_IRQ      _BIT(UART_0_NO)
#define UART_1_IRQ_NO   (7)
#define UART_1_IRQ      _BIT(UART_1_NO)
#define PWM_0_IRQ_NO    (8)
#define PWM_0_IRQ       _BIT(PWM_0_NO)
#define I2C_0_IRQ_NO    (9)
#define I2C_0_IRQ       _BIT(I2C_0_NO)
#define SPI_0_IRQ_NO    (10)
#define SPI_0_IRQ       _BIT(SPI_0_NO)
#define SPI_1_IRQ_NO    (11)
#define SPI_1_IRQ       _BIT(SPI_1_NO)
#define PLL_IRQ_NO      (12)
#define PLL_IRQ         _BIT(PLL_NO)
#define RTC_IRQ_NO      (13)
#define RTC_IRQ         _BIT(RTC_NO)
#define EINT_0_IRQ_NO   (14)
#define EINT_0_IRQ      _BIT(EINT_0_NO)
#define EINT_1_IRQ_NO   (15)
#define EINT_1_IRQ      _BIT(EINT_1_NO)
#define EINT_2_IRQ_NO   (16)
#define EINT_2_IRQ      _BIT(EINT_2_NO)
#define EINT_3_IRQ_NO   (17)
#define EINT_3_IRQ      _BIT(EINT_3_NO)
#define ADC_0_IRQ_NO    (18)
#define ADC_0_IRQ       _BIT(ADC_0_NO)
#define I2C_1_IRQ_NO    (19)
#define I2C_1_IRQ       _BIT(I2C_1_NO)
#define BOD_IRQ_NO      (20)
#define BOD_IRQ         _BIT(BOD_NO)
#define ADC_1_IRQ_NO    (21)
#define ADC_1_IRQ       _BIT(ADC_1_NO)
#define USB_IRQ_NO      (22)
#define USB_IRQ         _BIT(USB_NO)


#define VIC_ENABLE_SLOT _BIT(5)


typedef void (__attribute__ ((interrupt("IRQ"))) *IRQ_Handler)(void) ;

/*****************************************************************************
 * Global variables
 ****************************************************************************/

// None

/*****************************************************************************
 * Local variables
 ****************************************************************************/

// None

/*****************************************************************************
 * Local prototypes
 ****************************************************************************/

// Listę stałych wartość rozszerzyć

#endif //__VIC_H__
