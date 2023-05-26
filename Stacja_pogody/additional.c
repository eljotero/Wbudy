/******************************************************************************
 *  Funkcje pomocnicze - wykorzystywane do realizacji timer'a, itp.
 * 
 *  Plik z kodem źródłowym funkcji.
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/

#include "additional.h"

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/

/*****************************************************************************
 * Global variables
 ****************************************************************************/

/*****************************************************************************
 * Local variables
 ****************************************************************************/

/*****************************************************************************
 * Local prototypes
 ****************************************************************************/

/*
 * @brief   Funkcja mdelay() służy do odmierzenia wskazanego w milisekundach okresu czasu, po upłynięciu
 *          którego funkcja się kończy. Przed upłynięciem czasu, wykonywana jest zawartość pętli while, w wyniku czego
 *          za pomocą tej funkcji można wstrzymać wykonywanie programu na pewien zadany czas.
 *            
 * @param   delayInMs  
 *          Długość okresu oczekiwania, podana w milisekundach.
 * @returns void
 * 
 * @side effects: 
 *          Brak
 */

void mdelay(tU32 delayInMs)
{
  /*
   * Setup timer #1 for delay
   */

  T1TCR = 0x02;          // 0x02 = 0b 10 -> Timer counter and Prescle counter are synchronously reset.
  T1PR  = 0x00;          // Set prescaler to zero.

  // Since PCLK = CORE_FREQ:
  // Basic delay: basic_delay[s] = (1 / CORE_FREQ)[s], then: time_delay [ms] = (delayInMs * (1 / CORE_FREQ)) / 1000; 

  // Maximum delay value that can be achieved by this function:
  // MAX_DELAY = ((2 ^ 63 - 1) / CORE_FREQ) = 9 2223 372 036 854 775 807 / 58 982 400 = 156 374 987 061
  // Max value of delayInMs: (2 ^ 32 - 1) = 4 294 967 295

  T1MR0 = (((long)delayInMs) * (long)CORE_FREQ) / 1000;

  T1IR  = 0xFF;          // Reset all interrrupt flags in interrupt register of Timer 1.
  T1MCR = 0x04;          // Stop timer on match, effectively disabling Timer Counter and Prescale Counter (setting 0 to T1TCR[0])
  T1TCR = 0x01;          // 0x01 = 0b 1 -> Timer counter and Prescale counter are enabled to count.
  
  // Wait until delay time has elapsed (This loop will be terminated when there will be a match between MR0 and TC values)

  while ((T1TCR & 0x01) == 1)
  {
    ;
  }

  T1TCR = 0x00;         // Resetting entire TCR register of Timer 1.
}

/*
 * @brief   Funkcja sdelay() służy do odmierzenia wskazanego w sekundach okresu czasu, po upłynięciu
 *          którego funkcja się kończy. Przed upłynięciem czasu, wykonywana jest zawartość pętli while, w wyniku czego
 *          za pomocą tej funkcji można wstrzymać wykonywanie programu na pewien zadany czas.
            
 * @param   delayInS 
 *          Długość okresu oczekiwania, podana w sekundach.
 * @returns void    
 * 
 * @side effects: 
 *          Brak
 */

void sdelay(tU32 delayInS)
{
  /*
   * Setup timer #1 for delay
   */

  T1TCR = 0x02;          // 0x02 = 0b 10 -> Timer counter and Prescle counter are synchronously reset.
  T1PR  = 0x00;          // Set prescaler to zero.

  // Maximum delay value that can be achieved by this function:
  // MAX_DELAY = ((2 ^ 63 - 1) / CORE_FREQ) = 9 2223 372 036 854 775 807 / 58 982 400 = 156 374 987 061
  // Max value of delayInS: (2 ^ 32 - 1) = 4 294 967 295

  T1MR0 = (((long)delayInS) * (long)CORE_FREQ);

  T1IR  = 0xFF;          // Reset all interrrupt flags in interrupt register of Timer 1.
  T1MCR = 0x04;          // Stop timer on match, effectively disabling Timer Counter and Prescale Counter (setting 0 to T1TCR[0])
  T1TCR = 0x01;          // 0x01 = 0b 1 -> Timer counter and Prescale counter are enabled to count.

  // Wait until delay time has elapsed (This loop will be terminated when there will be a match between MR0 and TC values)

  while ((T1TCR & 0x01) == 1)
  {
    ;
  }

  T1TCR = 0x00;         // Resetting entire TCR register of Timer 1.
}

// TODO: Poprawić funkcje sdelay oraz mdelay w opraciu o dokumentację.
