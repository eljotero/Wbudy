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
#include "lcd_hw.h"

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
 *    Send 9-bit data to LCD controller
 *
 ****************************************************************************/

void sendToLCD(tU8 firstBit, tU8 data)
{
  //disable SPI
  IOCLR = LCD_CLK;
  PINSEL0 &= 0xffffc0ff;
  
  if (1 == firstBit)
    IOSET = LCD_MOSI;   //set MOSI
  else
    IOCLR = LCD_MOSI;   //reset MOSI
  
  //Set clock high
  IOSET = LCD_CLK;
  
  //Set clock low
  IOCLR = LCD_CLK;
  
  /*
   * Enable SPI again
   */
  //initialize SPI interface
  SPI_SPCCR = 0x08;    
  SPI_SPCR  = 0x20;

  //connect SPI bus to IO-pins
  PINSEL0 |= 0x00001500;
  
  //send byte
  SPI_SPDR = data;
  while((SPI_SPSR & 0x80) == 0)
    ;
}

/*****************************************************************************
 *
 * Description:
 *    Initialize the SPI interface for the LCD controller
 *
 ****************************************************************************/

void initSpiForLcd(void)
{
  //make SPI slave chip select an output and set signal high
  IODIR |= (LCD_CS | LCD_CLK | LCD_MOSI);
  
  //deselect controller
  selectLCD(FALSE);

  //connect SPI bus to IO-pins
  PINSEL0 |= 0x00001500;
  
  //initialize SPI interface
  SPI_SPCCR = 0x08;    
  SPI_SPCR  = 0x20;
}

/*****************************************************************************
 *
 * Description:
 *    Select/deselect LCD controller (by controlling chip select signal)
 *
 ****************************************************************************/

void selectLCD(tBool select)
{
  if (TRUE == select)
    IOCLR = LCD_CS;
  else
    IOSET = LCD_CS;
}
