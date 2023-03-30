/******************************************************************************
 *
 * Copyright:
 *    (C) 2006 Embedded Artists AB
 *
 * File:
 *    lcd.h
 *
 * Description:
 *    Expose public functions related to LCD functionality.
 *
 *****************************************************************************/
#ifndef _LCD_H_
#define _LCD_H_

void lcdInit(void);
void lcdOff(void);
void lcdContrast(tU8 contr);
void lcdClrscr(void);
void lcdPutchar(tU8 data);
void lcdPuts(char s[]);
void lcdGotoxy(tU8 x, tU8 y);
void lcdWindow(tU8 xp, tU8 yp, tU8 xe, tU8 ye);
void lcdColor(tU8 bkg, tU8 text);
void lcdRect(tU8 x, tU8 y, tU8 xLen, tU8 yLen, tU8 color);
void lcdRectBrd(tU8 x, tU8 y, tU8 xLen, tU8 yLen, tU8 color1, tU8 color2, tU8 color3);
void lcdIcon(tU8 x, tU8 y, tU8 xLen, tU8 yLen, tU8 compressionOn, tU8 escapeChar, const tU8* pData);

void lcdWrdata(tU8 data);
void lcdWrcmd(tU8 cmd);

#endif
