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
#ifndef VIC_H
#define VIC_H

#include "general.h"

#define WATCHDOG_IRQ_NO (0)
#define WATCHDOG_IRQ    BIT(WATCHDOG_IRQ_NO)
#define TIMER_0_IRQ_NO  (4)
#define TIMER_0_IRQ     BIT(TIMER_0_IRQ_NO)
#define TIMER_1_IRQ_NO  (5)
#define TIMER_1_IRQ     BIT(TIMER_1_IRQ_NO)
#define UART_0_IRQ_NO   (6)
#define UART_0_IRQ      BIT(UART_0_NO)
#define UART_1_IRQ_NO   (7)
#define UART_1_IRQ      BIT(UART_1_NO)
#define PWM_0_IRQ_NO    (8)
#define PWM_0_IRQ       BIT(PWM_0_NO)
#define I2C_0_IRQ_NO    (9)
#define I2C_0_IRQ       BIT(I2C_0_NO)
#define SPI_0_IRQ_NO    (10)
#define SPI_0_IRQ       BIT(SPI_0_NO)
#define SPI_1_IRQ_NO    (11)
#define SPI_1_IRQ       BIT(SPI_1_NO)
#define PLL_IRQ_NO      (12)
#define PLL_IRQ         BIT(PLL_NO)
#define RTC_IRQ_NO      (13)
#define RTC_IRQ         BIT(RTC_NO)
#define EINT_0_IRQ_NO   (14)
#define EINT_0_IRQ      BIT(EINT_0_NO)
#define EINT_1_IRQ_NO   (15)
#define EINT_1_IRQ      BIT(EINT_1_NO)
#define EINT_2_IRQ_NO   (16)
#define EINT_2_IRQ      BIT(EINT_2_NO)
#define EINT_3_IRQ_NO   (17)
#define EINT_3_IRQ      BIT(EINT_3_NO)
#define ADC_0_IRQ_NO    (18)
#define ADC_0_IRQ       BIT(ADC_0_NO)
#define I2C_1_IRQ_NO    (19)
#define I2C_1_IRQ       BIT(I2C_1_NO)
#define BOD_IRQ_NO      (20)
#define BOD_IRQ         BIT(BOD_NO)
#define ADC_1_IRQ_NO    (21)
#define ADC_1_IRQ       BIT(ADC_1_NO)
#define USB_IRQ_NO      (22)
#define USB_IRQ         BIT(USB_NO)


#define VIC_ENABLE_SLOT BIT(5)


typedef void (__attribute__ ((interrupt("IRQ"))) *IRQ_Handler)(void) ;

// Listę stałych wartości rozszerzyć
#endif //VIC_H
