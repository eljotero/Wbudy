/*************************************************************************************
 *
 * @Description:
 * Lista sta�ych u�ywanych do programowania timer�w/licznik�w
 * Wskazane jest uzupe�nienie listy
 *
 * @Authors: Micha� Morawski, 
 *           Daniel Arendt, 
 *           Przemys�aw Ignaciuk,
 *           Marcin Kwapisz
 *
 * @Change log:
 *           2016.12.01: Wersja oryginalna.
 *
 ******************************************************************************/
#ifndef TIMER_H
#define TIMER_H

#include "general.h"

//Sta�e dla Timer�w
#define TIMER_RESET     _BIT(1)
#define TIMER_RUN       _BIT(0)
#define MR0_I           _BIT(0)
#define MR0_R           _BIT(1)
#define MR0_S           _BIT(2)
#define MR1_I           _BIT(3)
#define MR1_R           _BIT(4)
#define MR1_S           _BIT(5)
#define MR2_I           _BIT(6)
#define MR2_R           _BIT(7)
#define MR2_S           _BIT(8)
#define MR3_I           _BIT(9)
#define MR3_R           _BIT(10)
#define MR3_S           _BIT(11)

#define TIMER_MR0_INT	_BIT(0)
#define TIMER_MR1_INT	_BIT(1)
#define TIMER_MR2_INT	_BIT(2)
#define TIMER_MR3_INT	_BIT(3)
#define TIMER_CR0_INT	_BIT(4)
#define TIMER_CR1_INT	_BIT(5)
#define TIMER_CR2_INT	_BIT(6)
#define TIMER_CR3_INT	_BIT(7)
#define TIMER_ALL_INT	(TIMER_MR0_INT | TIMER_MR1_INT | TIMER_MR2_INT | TIMER_MR3_INT | TIMER_CR0_INT | TIMER_CR1_INT | TIMER_CR2_INT | TIMER_CR3_INT)

// List� sta�ych warto rozszerzy�

#endif //TIMER_H
