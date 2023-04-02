/******************************************************************************
 *
 * Copyright:
 *    (C) 2006 Embedded Artists AB
 *
 * File:
 *    pca9532.c
 *
 * Description:
 *    Implements hardware specific routines
 *
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "general.h"
#include <lpc2xxx.h>
#include <printf_P.h>
#include "i2c.h"
#include "pca9532.h"
#include "eeprom.h"

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


/*****************************************************************************
 *
 * Description:
 *    Initialize the io-pins and find out if HW is ver 1.0 or ver 1.1
 *
 ****************************************************************************/
tBool
pca9532Init(void)
{
  tU8 initCommand[] = {0x12, 0x97, 0x80, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00};
  //                                                         04 = LCD_RST# low
  //                                                         10 = BT_RST# low

  //initialize PCA9532
  if (I2C_CODE_OK == pca9532(initCommand, sizeof(initCommand), NULL, 0))
    return TRUE;
  else
    return FALSE;
}


/*****************************************************************************
 *
 * Description:
 *
 ****************************************************************************/
void
setPca9532Pin(tU8 pinNum, tU8 value)
{
  tU8 command[] = {0x00, 0x00};
  tU8 regValue;
  tU8 mask;
  
  if (pinNum < 4)
    command[0] = 0x06;
  else if (pinNum < 8)
    command[0] = 0x07;
  else if (pinNum < 12)
    command[0] = 0x08;
  else
    command[0] = 0x09;
    
  pca9532(command, 1, &regValue, 1);
  
  mask = (3 << 2*(pinNum % 4));
  
  regValue &= ~mask;
  
  if (value == 0)
    command[1] = 0x01;
  else
    command[1] = 0x00;
    
  command[1] <<= 2*(pinNum % 4);
  
  command[1] |= regValue;

  pca9532(command, sizeof(command), NULL, 0);
}


/*****************************************************************************
 *
 * Description:
 *
 ****************************************************************************/
tU16
getPca9532Pin(void)
{
  tU8 command[] = {0x19};
  tU8 regValue[3];
  
  pca9532(command, 1, regValue, 3);
  
  return (tU16)regValue[1] | ((tU16)regValue[2] << 8);
}

