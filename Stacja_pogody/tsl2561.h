/******************************************************************************
 *  Obsługa pomiaru jasności przy pomocy jasnościomierza TSL2561.
 * 
 *  Plik z defnicjami funkcji.
 *****************************************************************************/

#ifndef TLS2561_H
#define TLS2561_H

/******************************************************************************
 * Includes
 *****************************************************************************/

#include "general.h"
#include "i2c.h"
#include "math.h"

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

tU64 measureBrightness(void);
tU64 calculateBrightness(tU16 channel0, tU16 channel1);

#endif