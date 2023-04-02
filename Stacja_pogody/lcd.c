/******************************************************************************
 *
 * Copyright:
 *    (C) 2006 Embedded Artists AB
 *
 * File:
 *    lcd.c
 *
 * Description:
 *    Implements routines for the LCD.
 *
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "general.h"
#include "lcd.h"
#include "lcd_hw.h"
#include "ascii.h"

/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/
#define LCD_CMD_SWRESET   0x01
#define LCD_CMD_BSTRON    0x03
#define LCD_CMD_SLEEPIN   0x10
#define LCD_CMD_SLEEPOUT  0x11
#define LCD_CMD_INVON     0x21
#define LCD_CMD_SETCON    0x25
#define LCD_CMD_DISPON    0x29
#define LCD_CMD_CASET     0x2A
#define LCD_CMD_PASET     0x2B
#define LCD_CMD_RAMWR     0x2C
#define LCD_CMD_RGBSET    0x2D
#define LCD_CMD_MADCTL    0x36
#define LCD_CMD_COLMOD    0x3A

#define MADCTL_HORIZ      0x48
#define MADCTL_VERT       0x68


/*****************************************************************************
 * Local variables
 ****************************************************************************/
static tU8 lcd_x;
static tU8 lcd_y;
static tU8 bkgColor;
static tU8 textColor;
static tU8 setcolmark;

/*****************************************************************************
 * Local prototypes
 ****************************************************************************/
static void lcdWindow1(tU8 xp, tU8 yp, tU8 xe, tU8 ye);


/*****************************************************************************
 *
 * Description:
 *    Initialize the LCD controller and SPI interface
 *    (0,0) is top left and (129,129) is bottom right
 *    8 bits Color mode uses RRRGGGBB layout
 *
 ****************************************************************************/
void
lcdInit(void)
{
  bkgColor  = 0;
  textColor = 0;
  
  //init SPI interface
  initSpiForLcd();
  
  //select controller
  selectLCD(TRUE);

	lcdWrcmd(LCD_CMD_SWRESET);

	sdelay(1);
	lcdWrcmd(LCD_CMD_SLEEPOUT);
	lcdWrcmd(LCD_CMD_DISPON);
	lcdWrcmd(LCD_CMD_BSTRON);
	sdelay(1);
		
	lcdWrcmd(LCD_CMD_MADCTL);   //Memory data acces control
	lcdWrdata(MADCTL_HORIZ);    //X Mirror and BGR format
	lcdWrcmd(LCD_CMD_COLMOD);   //Colour mode
	lcdWrdata(0x02);            //256 colour mode select
	lcdWrcmd(LCD_CMD_INVON);    //Non Invert mode

	lcdWrcmd(LCD_CMD_RGBSET);   //LUT write
  lcdWrdata(0);               //Red
  lcdWrdata(2);
  lcdWrdata(4);
  lcdWrdata(6);
  lcdWrdata(9);
  lcdWrdata(11);
  lcdWrdata(13);
  lcdWrdata(15);
  lcdWrdata(0);               //Green
  lcdWrdata(2);
  lcdWrdata(4);
  lcdWrdata(6);
  lcdWrdata(9);
  lcdWrdata(11);
  lcdWrdata(13);
  lcdWrdata(15);
  lcdWrdata(0);               //Blue
  lcdWrdata(6);
	lcdWrdata(10);
	lcdWrdata(15);

  //deselect controller
  selectLCD(FALSE);

	lcdContrast(56);

	lcdClrscr();
}


/*****************************************************************************
 *
 * Description:
 *    Clear screen (with current background color)
 *
 ****************************************************************************/
void
lcdClrscr(void)
{
  tU32 i;

	lcd_x = 0;
  lcd_y = 0;

  //select controller
  selectLCD(TRUE);   

  lcdWindow1(255,255,128,128);
  
  lcdWrcmd(LCD_CMD_RAMWR);    //write memory
  
  for(i=0; i<16900; i++)
    lcdWrdata(bkgColor);

  //deselect controller
  selectLCD(FALSE);
}


/*****************************************************************************
 *
 * Description:
 *    Shut down the LCD controller
 *
 ****************************************************************************/
void
lcdOff(void)
{
  lcdClrscr();

  //select controller
  selectLCD(TRUE);   

  lcdWrcmd(LCD_CMD_SLEEPIN);

  //deselect controller
  selectLCD(FALSE);
}


/*****************************************************************************
 *
 * Description:
 *    Set current foreground and background color
 *
 ****************************************************************************/
void
lcdColor(tU8 bkg, tU8 text)
{
  bkgColor  = bkg;
  textColor = text;
}


/*****************************************************************************
 *
 * Description:
 *    Set display contrast
 *
 ****************************************************************************/
void
lcdContrast(tU8 cont) //vary between 0 - 127
{
  //select controller
  selectLCD(TRUE);

  //set contrast cmd.
  lcdWrcmd(LCD_CMD_SETCON);
	lcdWrdata(cont);

  //deselect controller
  selectLCD(FALSE);
}


/*****************************************************************************
 *
 * Description:
 *    Draw a rectangular area with specified color.
 *
 ****************************************************************************/
void
lcdRect(tU8 x, tU8 y, tU8 xLen, tU8 yLen, tU8 color)
{
  tU32 i;
  tU32 len;

  //select controller
  selectLCD(TRUE);   

  lcdWindow1(x,y,x+xLen-1,y+yLen-1);
  
  lcdWrcmd(LCD_CMD_RAMWR);    //write memory
  
  len = xLen*yLen;
  for(i=0; i<len; i++)
    lcdWrdata(color);

  //deselect controller
  selectLCD(FALSE);
}


/*****************************************************************************
 *
 * Description:
 *    Draw rectangular area with different boardser colors. Currently used
 *    by example game.
 *
 ****************************************************************************/
void
lcdRectBrd(tU8 x, tU8 y, tU8 xLen, tU8 yLen, tU8 color1, tU8 color2, tU8 color3)
{
  tU32 i,j;

  //select controller
  selectLCD(TRUE);   

  lcdWindow1(x,y,x+xLen-1,y+yLen-1);
  
  lcdWrcmd(LCD_CMD_RAMWR);    //write memory
  
  for(i=0; i<xLen; i++)
    lcdWrdata(color2);
  for(j=1; j<(yLen-2); j++)
  {
    lcdWrdata(color2);
    for(i=0; i<(xLen-2); i++)
      lcdWrdata(color1);
    lcdWrdata(color3);
  }
  for(i=0; i<xLen; i++)
    lcdWrdata(color3);

  //deselect controller
  selectLCD(FALSE);
}


/*****************************************************************************
 *
 * Description:
 *    Draw rectangular area from bitmap. Specify xy-position and xy-length.
 *    Compressed format is supported.
 *
 *    In uncompressed color mode, pData points to an area of xLen * yLen
 *    bytes with the icon.
 *    In compressed mode the escapeChar is used to denote that the next
 *    two bytes contain a length and a color (run length encoding)
 *    Note that is is still possible to specify the color value that
 *    equals the escape value in a compressed string.
 *
 ****************************************************************************/
void
lcdIcon(tU8 x, tU8 y, tU8 xLen, tU8 yLen, tU8 compressionOn, tU8 escapeChar, const tU8* pData)
{
  tU32 i,j;
  tS32 len;

  //select controller
  selectLCD(TRUE);   

  lcdWindow1(x,y,x+xLen-1,y+yLen-1);
  
  lcdWrcmd(LCD_CMD_RAMWR);    //write memory
  
  len = xLen*yLen;
  if (compressionOn == FALSE)
  {
    for(i=0; i<len; i++)
      lcdWrdata(*pData++);
  }
  else
    while(len > 0)
    {
      if(*pData == escapeChar)
      {
        pData++;
        j = *pData++;
        for(i=0;i<j;i++)
          lcdWrdata(*pData);
        pData++;
        len -= j;
      }
      else
      {
        lcdWrdata(*pData++);
        len--;
      }
    }

  //deselect controller
  selectLCD(FALSE);
}


/*****************************************************************************
 *
 * Description:
 *    Update xy-position (by also creating a window).
 *
 ****************************************************************************/
void
lcdGotoxy(tU8 x, tU8 y)
{
  lcd_x = x;
  lcd_y = y;
  lcdWindow(x, y, 129, 129);
}


/*****************************************************************************
 *
 * Description:
 *    Initialize LCD controller for a window (to write in).
 *    Set start xy-position and xy-length.
 *    Selects/deselects LCD controller.
 *
 ****************************************************************************/
void
lcdWindow(tU8 xp, tU8 yp, tU8 xe, tU8 ye)
{
  //select controller
  selectLCD(TRUE);

	lcdWindow1(xp, yp, xe, ye);

  //deselect controller
  selectLCD(FALSE);
}


/*****************************************************************************
 *
 * Description:
 *    Initialize LCD controller for a window (to write in).
 *    Set start xy-position and xy-length
 *    No select/deselect of LCD controller.
 *
 ****************************************************************************/
static void
lcdWindow1(tU8 xp, tU8 yp, tU8 xe, tU8 ye)
{
  lcdWrcmd(LCD_CMD_CASET);    //set X
  lcdWrdata(xp+2);
	lcdWrdata(xe+2);

	lcdWrcmd(LCD_CMD_PASET);    //set Y
	lcdWrdata(yp+2);
	lcdWrdata(ye+2);
}


/*****************************************************************************
 *
 * Description:
 *    Update xy-position for newline situation.
 *
 ****************************************************************************/
static void
lcdNewline(void)
{
  lcd_x  = 0;
  lcd_y	+= 14;
  if (lcd_y >= 126)
    lcd_y = 126;
}


/*****************************************************************************
 *
 * Description:
 *    Draw one character withc current foreground and background color
 *    at current xy position on display. Update x-position (+8).
 *
 ****************************************************************************/
void
lcdData(tU8 data)
{
  //select controller
  selectLCD(TRUE);
  
  if (data <= 127)
  {
    tU32 mapOffset;
    tU8 i,j,byteToShift;

    data -= 30;
    mapOffset = 14*data;

    lcdWrcmd(LCD_CMD_CASET);
    lcdWrdata(lcd_x+2);
    lcdWrdata(lcd_x+9);
    lcdWrcmd(LCD_CMD_PASET);
    lcdWrdata(lcd_y+2);
    lcdWrdata(lcd_y+15);
    lcdWrcmd(LCD_CMD_RAMWR);
    
    for(i=0; i<14; i++)
    {
      byteToShift = charMap[mapOffset++];
      for(j=0; j<8; j++)
      {
        if (byteToShift & 0x80)
          lcdWrdata(textColor);
        else
          lcdWrdata(bkgColor);
        byteToShift <<= 1;
      }
    }
  }

  //deselect controller
  selectLCD(FALSE);

  lcd_x += 8;
}


/*****************************************************************************
 *
 * Description:
 *    Write/draw one character at current xy-position.
 *    The xy-position is updated afterwards
 *
 ****************************************************************************/
void
lcdPutchar(tU8 data)
{
  if (data == '\n')
    lcdNewline();
  else if (data != '\r')
  {
    if (setcolmark == TRUE)
    {
      textColor = data;
      setcolmark = FALSE;
    }
    else if (data == 0xff)
      setcolmark = TRUE;
    else if (lcd_x <= 124)
    {
      lcdData(data);
    }
  }
}

/*****************************************************************************
 *
 * Description:
 *    Write/draw (null-terminated) string of character at current xy-position
 *
 ****************************************************************************/
void
lcdPuts(char *s)
{
  while(*s != '\0')
    lcdPutchar(*s++);
}


/*****************************************************************************
 *
 * Description:
 *    Send command data to LCD controller
 *
 ****************************************************************************/
void
lcdWrcmd(tU8 data)
{
  sendToLCD(0, data);
}


/*****************************************************************************
 *
 * Description:
 *    Send data byte to LCD controller
 *
 ****************************************************************************/
void
lcdWrdata(tU8 data)
{
  sendToLCD(1, data);
}


