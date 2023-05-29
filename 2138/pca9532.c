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

#define NUM_OF_LEDS 0x10 // 0x10 = 16d

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

#define INPUT0 0x00 // 0x00 = 0b 0000	(Read only)		Input Register 0
#define INPUT1 0x01 // 0x01 = 0b 0001	(Read only)		Input Register 1
#define PSC0 0x02	// 0x02 = 0b 0010	(Read / Write)	Frequency Prescaler 0
#define PWM0 0x03	// 0x03 = 0b 0011	(Read / Write)	PWM Register 0
#define PSC1 0x04	// 0x04 = 0b 0100	(Read / Write)	Frequency Prescaler 1
#define PWM1 0x05	// 0x05 = 0b 0101	(Read / Write)	PWM Register 1
#define LS0 0x06	// 0x06 = 0b 0110	(Read / Write)	LED 0 - 3 Selector
#define LS1 0x07	// 0x07 = 0b 0111	(Read / Write)	LED 4 - 7 Selector
#define LS2 0x08	// 0x08 = 0b 1000	(Read / Write)	LED 8 - 11 Selector
#define LS3 0x09	// 0x09 = 0b 1001	(Read / Write)	LED 12 - 15 Selector

#define BASE_ADDR 0x00
#define AUTO_INC_ADDR 0x01

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
		{0x12, 0x97, 0x80, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00};
	//                                                         04 = LCD_RST# low
	//                                                         10 = BT_RST# low

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
	// Buffer holding command executed with I2C (that command is send to PCA9532 device).
	tU8 command[] = {0x00, 0x00};
	tU8 regValue = 0;
	tU8 mask = 0;

	tS8 retCode = 0;

	if (pinNum < (tU8)4)
	{
		// Case for the pins from 0 to 3.
		command[0] = 0x06;
	}
	else if (pinNum < (tU8)8)
	{
		// Case for the pins from 4 to 7.
		command[0] = 0x07;
	}
	else if (pinNum < (tU8)12)
	{
		// Case for the pins from 8 to 11.
		command[0] = 0x08;
	}
	else
	{
		// Case for the pins from 12 to 15.
		command[0] = 0x09;
	}

	retCode = pca9532(command, 1, &regValue, 1);

	// 3 << 2 * (pinNum % 4);
	/*
		pinNum:

		0  -> 3 << (2 * 0) = 3				// 3   = 0b 0000 0011
		1  -> 3 << (2 * 1) = 3 << 2 = 12	// 12  = 0b 0000 1100
		2  -> 3 << (2 * 2) = 3 << 4 = 48 	// 48  = 0b 0011 0000
		3  -> 3 << (2 * 3) = 3 << 6 = 192	// 192 = 0b 1100 0000
	*/

	mask = (tU8)3 << ((tU8)2 * (pinNum % (tU8)4));

	/*
		pinNum:

		0  -> ~ 0b 0000 0011 = 0b 1111 1100
		1  -> ~ 0b 0000 1100 = 0b 1111 0011
		2  -> ~ 0b 0011 0000 = 0b 1100 1111
		3  -> ~ 0b 1100 0000 = 0b 0011 1111
	*/

	// Mask is XOR-ed with "current" state of LED register.

	// This instruction does put ones on every single bit for other pins (other that pinNum)
	// So that when bitwise AND is performed the current state of the rest of the pins is preserved.
	// But for pinNum value is zeroed - two bits with zero value.

	regValue &= ~mask;

	if (value == (tU8)0)
	{
		// Passing a value that is 0, means turning on a pin, but it does use PWM0 rate of blink.
		command[1] = 0x02;
	}
	else
	{
		// Passing value different that 0 means turning off a certain pin.
		command[1] = 0x00;
	}

	// Value == 0: 0x02 << (2 * (pinNum % 4)):
	/*
		0 -> 0x02 << (2 * 0) = 0b 0000 0010;
		1 -> 0x02 << (2 * 1) = 0b 0000 1000;
		2 -> 0x02 << (2 * 2) = 0b 0010 0000;
		3 -> 0x02 << (2 * 3) = 0b 1000 0000;
	*/
	// Value != 0: 0x00 << (2 * (pinNum % 4));
	/*
		0 -> 0x00 << (2 * 0) = 0b 0000 0000;
		1 -> 0x00 << (2 * 1) = 0b 0000 0000;
		2 -> 0x00 << (2 * 2) = 0b 0000 0000;
		3 -> 0x00 << (2 * 3) = 0b 0000 0000;
	*/

	command[1] = command[1] << ((tU8)2 * (pinNum % (tU8)4));

	command[1] = command[1] | regValue;

	retCode = pca9532(command, sizeof(command), NULL, 0);
}

/*****************************************************************************
 *
 * Description:
 *
 ****************************************************************************/

tU16 getPca9532Pin(void)
{
	tU8 command[] = {0x19}; // 0x19 = 0b 0001 1001
	tU8 regValue[3] = {0};
	tS8 retCode = 0;

	retCode = pca9532(command, 1, regValue, 3);

	return (tU16)regValue[1] | ((tU16)regValue[2] << 8);
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

	tU8 redLight = (tU8)224;
	tU8 greenLight = (tU8)28;
	tU8 blueLight = (tU8)3;

	tU8 PWM0Address = (((tU8)BASE_ADDR << 4) | (tU8)PWM0);
	// tU8 PSC0Address = (((tU8)BASE_ADDR << 4) | (tU8)PSC0);

	tU8 commandToPCA[2] = {0};

	tU8 regValue;
	tS8 retCode;

	if ((tU8)TRUE == pca9532Present)
	{
		tU8 i = 0;
		tU8 j = 0;
		for (j = 0; j < 3; j++)
		{
			for (i = (tU8)0; i < (tU8)NUM_OF_LEDS; i = i + (tU8)1)
			{
				commandToPCA[0] = PWM0Address;
				if (((i + j) % (tU8)3) == (tU8)0)
				{
					commandToPCA[1] = redLight;
				}
				else if (((i + j) % (tU8)3) == (tU8)1)
				{
					commandToPCA[1] = greenLight;
				}
				else
				{
					commandToPCA[1] = blueLight;
				}
				retCode = pca9532(commandToPCA, 2, NULL, 0);
				setPca9532Pin(i, 0);
				mdelay(30);
			}
			mdelay(200);
		}
		for (i = (tU8)0; i < (tU8)NUM_OF_LEDS; i = i + (tU8)1)
		{
			setPca9532Pin(i, 1);
			retCode = pca9532(commandToPCA, 2, NULL, 0);
			mdelay(30);
		}
	}
}
