/******************************************************************************
 *
 * Copyright:
 *    (C) 2007 Embedded Artists AB
 *
 * File:
 *    pca9532.h
 *
 * Description:
 *    Expose hardware specific routines
 *
 *****************************************************************************/

#ifndef _PCA9532_H_
#define _PCA9532_H_

/******************************************************************************
 * Includes
 *****************************************************************************/

#include "general.h"
#include <lpc2xxx.h>
#include "additional.h"

/******************************************************************************
 * Typedefs and defines
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

tBool pca9532Init(void);
void  setPca9532Pin(tU8 pinNum, tU8 value);
tU16  getPca9532Pin(void);
void manageLED(tU8 pca9532Present);

#endif
