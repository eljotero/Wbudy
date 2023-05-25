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

tBool pca9532Init(void)
{
	tU8 initCommand[] =
	{ 0x12, 0x97, 0x80, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00 };
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

void setPca9532Pin(tU8 pinNum, tU8 value)
{
	tU8 command[] = { 0x00, 0x00 };
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

	mask = (3 << 2 * (pinNum % 4));

	regValue &= ~mask;

	if (value == 0)
		command[1] = 0x02;
	else
		command[1] = 0x00;

	command[1] <<= 2 * (pinNum % 4);

	command[1] |= regValue;

	pca9532(command, sizeof(command), NULL, 0);
}

/*****************************************************************************
 *
 * Description:
 *
 ****************************************************************************/

tU16 getPca9532Pin(void)
{
	tU8 command[] = { 0x19 };
	tU8 regValue[3];

	pca9532(command, 1, regValue, 3);

	return (tU16) regValue[1] | ((tU16) regValue[2] << 8);
}

/*
 * @brief   Funkcja manageLED() wykorzystywana jest do kontrolowania LED'ów oznaczonych numerami od 1 do 16
 *			znajdujących się do okoła ekranu LCD, na expansion boardzie dołączonym do mikrokontrolera LPC2138.
 *			Działanie tej funkcji sprowadza sie do zapalenia po kolei LED'ów numerowanych od 1 do 16 z 30 ms opóźnieniem
 *			pomiędzy kolejnymi zapalonymi LED'ami, a następnie jak wszystkie są zapalone to ich zgaszeniu, z 
 *		 	zastosowaniem tego samego opóźnienia.
 *
 * @param   pca9532Present 
 *          Zmienna wskazująca czy PCA9532 zostało zainicjalizowane.
 * @returns
 *          void
 * 
 * @side effects: 
 *          Funkcja dokonuje zmiany stanu rejestrów, przy czym ich pierwotna zawartość nie jest przechowywana.
 */

void manageLED(tU8 pca9532Present)
{
	IODIR |= 0x00260000; //RGB
	IOSET = 0x00260000;

	IODIR1 |= 0x000F0000; //LEDs
	IOSET1 = 0x000F0000;
	IOCLR1 = 0x00030000;
	IOCLR1 = 0x00050000;
	IOCLR1 = 0x000c0000;
	IOCLR1 = 0x00090000;
	IOSET1 = 0x000F0000;
	IODIR1 &= ~0x00F00000; //Keys

	tU16 keys;
	if (TRUE == pca9532Present)
	{
		tU8 i;
		for (i = 0; i < 16; i++) {
			setPca9532Pin(i, 0);
			mdelay(30);
		}
		for (i = 0; i < 16; i++) {
			setPca9532Pin(i, 1);
			mdelay(30);
		}
	}
}

// TODO: Rozszerzyć metodę manageLED() o PWM i może zmianę koloru LED'ów
