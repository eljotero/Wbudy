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

#define NUM_OF_LEDS 			0x10 // 0x10 = 16d
// Definitions of PCA9532 Addresses (not needed since function pca9532 will be used)

/*

 #define pca9532Address000 		0x60
 #define pca9532ReadAddress000 	0xC1
 #define pca9532WriteAddress000 	0xC0

 #define pca9532Address001 		0x61
 #define pca9532ReadAddress001 	0xC3
 #define pca9532WriteAddress001 	0xC2

 #define pca9532Address010 		0x62
 #define pca9532ReadAddress010 	0xC5
 #define pca9532WriteAddress010 	0xC4

 #define pca9532Address100 		0x64
 #define pca9532ReadAddress100 	0xC9
 #define pca9532WriteAddress100 	0xC8

 #define pca9532Address011 		0x63
 #define pca9532ReadAddress011 	0xC7
 #define pca9532WriteAddress011 	0xC6

 #define pca9532Address101 		0x65
 #define pca9532ReadAddress101 	0xCB
 #define pca9532WriteAddress101 	0xCA

 #define pca9532Address110 		0x66
 #define pca9532ReadAddress110 	0xCD
 #define pca9532WriteAddress110 	0xCC

 #define pca9532Address111 		0x67
 #define pca9532ReadAddress111 	0xCF
 #define pca9532WriteAddress111 	0xCE

 */

#define INPUT0 	0x00	// 0x00 = 0b 0000	(Read only)		Input Register 0
#define INPUT1 	0x01	// 0x01 = 0b 0001	(Read only)		Input Register 1
#define PSC0	0x02	// 0x02 = 0b 0010	(Read / Write)	Frequency Prescaler 0
#define PWM0	0x03	// 0x03 = 0b 0011	(Read / Write)	PWM Register 0
#define PSC1	0x04	// 0x04 = 0b 0100	(Read / Write)	Frequency Prescaler 1
#define PWM1	0x05	// 0x05 = 0b 0101	(Read / Write)	PWM Register 1
#define LS0		0x06	// 0x06 = 0b 0110	(Read / Write)	LED 0 - 3 Selector
#define LS1		0x07	// 0x07 = 0b 0111	(Read / Write)	LED 4 - 7 Selector
#define LS2		0x08	// 0x08 = 0b 1000	(Read / Write)	LED 8 - 11 Selector
#define LS3		0x09	// 0x09 = 0b 1001	(Read / Write)	LED 12 - 15 Selector
#define BASE_ADDR 		0x00
#define AUTO_INC_ADDR 	0x01

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

	// Init Command:

	// 0x12 -> Address for PSC0 register.
	// 0x97 -> Value that will be written PSC0 register.
	// 0x80 -> Value that will be written PWM0 register.
	// 0x00 -> Value that will be written PSC1 register.
	// 0x40 -> Value that will be written PWM1 register.
	// 0x00 -> Value that will be written LS0 register.
	// 0x00 -> Value that will be written LS1 register.
	// 0x00 -> Value that will be written LS2 register.
	// 0x00 -> Value that will be written LS3 register.

	tBool result;

	// Initialize PCA9532
	if (I2C_CODE_OK == pca9532(initCommand, sizeof(initCommand), NULL, 0))
	{
		result = TRUE;
	}
	else
	{
		result = FALSE;
	}
	return result;
}

/*****************************************************************************
 *
 * Description:
 *
 ****************************************************************************/

void setPca9532Pin(tU8 pinNum, tU8 value)
{
	tS8 returnValue = 0;
	tU8 command[] =
	{ 0x00, 0x00 };
	tU8 regValue;
	tU8 mask;

	if (pinNum < (tU8)4)
	{
		command[0] = 0x06;
	}
	else if (pinNum < (tU8)8)
	{
		command[0] = 0x07;
	}
	else if (pinNum < (tU8)12)
	{
		command[0] = 0x08;
	}
	else 
	{
		command[0] = 0x09;
	}

	returnValue = pca9532(command, 1, &regValue, 1);

	mask = (tU8)((tU8)3 << ((tU8)2 * (pinNum % (tU8)4)));

	regValue &= ~mask;

	if (value == (tU8) 0)
	{
		// Passing a value that is 0, means turning on a pin, but it does use PWM0 rate of blink.
		command[1] = 0x02;
	}
	else if (value == (tU8) 1)
	{
		// Passing a value that is 1, means turning on a pin, but id does use PWM1 rate of blink.
		command[1] = 0x03;
	}
	else if (value == (tU8) 2)
	{
		// Passing a value that is 1, means turning on a pin, without blinking with any rate.
		command[1] = 0x01;
	}
	else
	{
		// Passing value different that 0 means turning off a certain pin.
		command[1] = 0x00;
	}

	command[1] = command[1] << (tU8)((tU8)2 * (pinNum % (tU8)4));

	command[1] |= regValue;

	returnValue = pca9532(command, sizeof(command), NULL, 0);
}

/*****************************************************************************
 *
 * Description:
 *
 ****************************************************************************/

tU16 getPca9532Pin(void)
{
	tU8 command[] =
	{ 0x19 };
	tU8 regValue[3];
	tS8 retCode;

	retCode = pca9532(command, 1, regValue, 3);

	return (tU16) regValue[1] | ((tU16) regValue[2] << 8);
}

/*
 * @brief   Funkcja manageLED() wykorzystywana jest do kontrolowania LED'ów oznaczonych numerami od 1 do 16
 *			znajdujących się do okoła ekranu LCD, na expansion boardzie dołączonym do mikrokontrolera LPC2138.
 *			Działanie tej funkcji sprowadza sie do zapalenia po kolei LED'ów numerowanych od 1 do 16 z 30 ms opóźnieniem
 *			pomiędzy kolejnymi zapalonymi LED'ami, gdzie piny o numerach parzystych swiecą jaśniej niż piny o numerach nieparzystych.
 *			Następnie jak wszystkie są zapalone to następuje 3 sekundowe odczekiwanie, a po jego zakończeniu, ich zgaszeniu, z 
 *		 	zastosowaniem 30 ms opóźnienia. Ponadto piny, parzyste jak i nieparzyste "mrugają" co 0,5 sekundy.
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
	IODIR |= 0x00260000; // RGB
	IOSET = 0x00260000;

	IODIR1 |= 0x000F0000; // LEDs
	IOSET1 = 0x000F0000;
	IOCLR1 = 0x00030000;
	IOCLR1 = 0x00050000;
	IOCLR1 = 0x000c0000;
	IOCLR1 = 0x00090000;
	IOSET1 = 0x000F0000;
	IODIR1 &= ~0x00F00000; // Keys

	tU8 dutyCycleOne = (tU8)224;
	tU8 dutyCycleTwo = (tU8)28;

	tU8 PWM0Address = (((tU8)BASE_ADDR << 4) | (tU8)PWM0);
	tU8 PWM1Address = (((tU8)BASE_ADDR << 4) | (tU8)PWM1);
	// tU8 PSC0Address = (((tU8)BASE_ADDR << 4) | (tU8)PSC0);

	tU8 commandToPCA[2] = {0};

	tU8 regValue;
	tS8 retCode;

	if ((tU8)TRUE == pca9532Present)
	{
		tU8 i = 0;
		tU8 j = 0;
		for (j = 0; j < (tU8)3; j = j + (tU8)1)
		{
			for (i = (tU8)0; i < (tU8)NUM_OF_LEDS; i = i + (tU8)1)
			{
				if (((i + j) % (tU8)2) == (tU8)0)
				{
					commandToPCA[0] = PWM0Address;
					commandToPCA[1] = dutyCycleOne;
					retCode = pca9532(commandToPCA, 2, NULL, 0);
					setPca9532Pin(i, 0);
				}
				else
				{
					commandToPCA[0] = PWM1Address;
					commandToPCA[1] = dutyCycleTwo;
					retCode = pca9532(commandToPCA, 2, NULL, 0);
					setPca9532Pin(i, 1);
				}
				mdelay(30);
			}
			mdelay(200);
		}
		for (i = (tU8)0; i < (tU8)NUM_OF_LEDS; i = i + (tU8)1)
		{
			setPca9532Pin(i, 3);
			retCode = pca9532(commandToPCA, 2, NULL, 0);
			mdelay(30);
		}
	}
}
