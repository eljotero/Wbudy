/*
    Plik, zawierający kod źródłowy funkcji
    pomocniczych.
*/

#include "additional.h"

/******************************************************************************
** Function name:		mdelay
**
** Descriptions:
**
** parameters:			delay length
** Returned value:		None
**
******************************************************************************/
void mdelay(unsigned int delayInMs)
{
  /*
   * setup timer #1 for delay
   */
  T1TCR = 0x02; // stop and reset timer
  T1PR = 0x00;  // set prescaler to zero
  T1MR0 = (((long)delayInMs - 1) * (long)CORE_FREQ / 1000);
  T1IR = 0xff;  // reset all interrrupt flags
  T1MCR = 0x04; // stop timer on match
  T1TCR = 0x01; // start timer

  // wait until delay time has elapsed
  while (T1TCR & 0x01)
    ;
}

void sdelay(unsigned int delayInS)
{
  /*
   * setup timer #1 for delay
   */
  T1TCR = 0x02; // stop and reset timer
  T1PR = 0x00;  // set prescaler to zero
  T1MR0 = (((long)delayInS) * (long)CORE_FREQ);
  T1IR = 0xff;  // reset all interrrupt flags
  T1MCR = 0x04; // stop timer on match
  T1TCR = 0x01; // start timer

  // wait until delay time has elapsed
  while (T1TCR & 0x01)
    ;
}