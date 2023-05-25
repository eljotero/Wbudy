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
   * setup timer #1 for delay
   */
  T1TCR = 0x02;          //stop and reset timer
  T1PR  = 0x00;          //set prescaler to zero
  T1MR0 = (((long)delayInMs - 1) * (long)CORE_FREQ / 1000);
  T1IR  = 0xff;          //reset all interrrupt flags
  T1MCR = 0x04;          //stop timer on match
  T1TCR = 0x01;          //start timer
  
  //wait until delay time has elapsed
  while ((T1TCR & 0x01) == 1)
  {
    ;
  }
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
   * setup timer #1 for delay
   */
  T1TCR = 0x02;          //stop and reset timer
  T1PR  = 0x00;          //set prescaler to zero
  T1MR0 = (((long)delayInS) * (long)CORE_FREQ);
  T1IR  = 0xff;          //reset all interrrupt flags
  T1MCR = 0x04;          //stop timer on match
  T1TCR = 0x01;          //start timer

  //wait until delay time has elapsed
  while ((T1TCR & 0x01) == 1)
  {
    ;
  }
}

// TODO: Poprawić funkcje sdelay oraz mdelay w opraciu o dokumentację.
