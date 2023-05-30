#include "general.h"
#include <lpc2xxx.h>
#include <printf_P.h>
#include <printf_init.h>
#include <consol.h>
#include <config.h>
#include "i2c.h"

#include "lm75.h"
#include "htu21df.h"
#include "bmp180.h"
#include "tsl2561.h"

#include "lcd.h"
#include "pca9532.h"
#include "additional.h"

#include "Common_Def.h"

void printCurrentTime(void);

int main(void)
{
	tS8 retCode = 0;
	tU8 pca9532Present = FALSE;
	// Check if connection with PCA9532 is established
	i2cInit();
	pca9532Present = pca9532Init();
	printf_init();
	if ((tU8)TRUE == pca9532Present)
	{
		/*
			Inicjalizacja I2C
		*/

		lcdInit();
		lcdColor(0xff, 0x00);
		lcdClrscr();

		/*
			Main loop containg enitre program logic. With every iteration, joystick position is checked
			and based on that, appropriate measurement value.
		*/
		
		tU8 amountOfLettersCopied = 0;

		tU8 helperValue = (tU8)1;

		IODIR1 &= ~0x00F00000;
		PINSEL0 &= ~0x30000000;

		while (helperValue == (tU8)TRUE)
		{
			tU8 charArray[20] = {0};

			//  IOPIN0 -> gets values from GPIO Port 0 register.
			// 	IOPIN1 -> gets values from GPIO Port 0 register.

			if (((IOPIN0 & 0x00000100) == 0) || ((IOPIN1 & 0x00400000) == 0))
			{
				// Key P0.8 center-key is pressed
				// 0x00000100 = 0b 0000 0000 0000 0000 0000 0001 0000 0000
				// Key P1.22 is pressed
				// 0x00400000 = 0b 0000 0000 0100 0000 0000 0000 0000 0000
				lcdGotoxy(0, 0);
				const tU8 textToBeWritten[] = "Aktualny czas: ";
				lcdPuts(textToBeWritten);
				lcdGotoxy(0, 15);
			 	printCurrentTime();
			}
			else if (((IOPIN0 & 0x00000200) == 0) || ((IOPIN1 & 0x00100000) == 0))
			{
				// Key P0.9 left-key is pressed
				// 0x00000100 = 0b 0000 0000 0000 0000 0000 0010 0000 0000
				// Key P1.20 is pressed
				// 0x00100000 = 0b 0000 0000 0001 0000 0000 0000 0000 0000
				lcdGotoxy(0, 0);
				const tU8 textToBeWritten[] = "Pomiar\ntemperatury: ";
				lcdPuts(textToBeWritten);
				lcdGotoxy(0, 30);
				measureTemperature();
				manageLED(pca9532Present);
			}
			else if (((IOPIN0 & 0x00000400) == 0) || ((IOPIN1 & 0x00200000) == 0))
			{
				// Key P0.10 up-key is pressed
				// 0x00000100 = 0b 0000 0000 0000 0000 0000 0100 0000 0000
				// Key P1.21 is pressed
				// 0x00200000 = 0b 0000 0000 0010 0000 0000 0000 0000 0000
				lcdGotoxy(0, 0);
				const tU8 textToBeWritten[] = "Pomiar\nwilgotnosci: ";
				lcdPuts(textToBeWritten);
				tU16 humidityValue = measureHumidity();
				retCode = sprintf(charArray, "%d%s", humidityValue, " %");
				lcdGotoxy(0, 30);
				lcdPuts(charArray);
				manageLED(pca9532Present);
			}
			else if (((IOPIN0 & 0x00000800) == 0) || ((IOPIN0 & 0x00004000) == 0))
			{
				// Key P0.11 right-key is pressed
				// 0x00000100 = 0b 0000 0000 0000 0000 0000 1000 0000 0000
				// Key P0.14 is pressed
				// 0x00004000 = 0b 0000 0000 0000 0000 0100 0000 0000 0000
				lcdGotoxy(0, 0);
				const tU8 textToBeWritten[] = "Pomiar\njasnosci: ";
				lcdPuts(textToBeWritten);
				tU64 brightnessValue = measureBrightness();
				retCode = sprintf(charArray, "%d", brightnessValue);
				lcdGotoxy(0, 30);
				charArray[retCode] = ' ';
				charArray[retCode + 1] = 'l';
				charArray[retCode + 2] = 'u';
				charArray[retCode + 3] = 'x';
				lcdPuts(charArray);
				manageLED(pca9532Present);
			}
			else if (((IOPIN0 & 0x00001000) == 0) || ((IOPIN1 & 0x00800000) == 0))
			{
				// Key P0.12 down-key is pressed
				// 0x00000100 = 0b 0000 0000 0000 0000 0001 0000 0000 0000
				// Key P1.23 is pressed
				// 0x00800000 = 0b 0000 0000 1000 0000 0000 0000 0000 0000
				lcdGotoxy(0, 0);
				const tU8 textToBeWritten[] = "Pomiar\ncisnienia: ";
				lcdPuts(textToBeWritten);
				tS64 pressureValue = measurePressure();
				retCode = sprintf(charArray, "%d", pressureValue);
				lcdGotoxy(0, 30);
				charArray[retCode] = ' ';
				charArray[retCode + 1] = 'P';
				charArray[retCode + 2] = 'a';
				lcdPuts(charArray);
				manageLED(pca9532Present);
			}
			else
			{
				// Joystick is not pressed at all.
				lcdGotoxy(0, 0);
				const tU8 textToBeWritten[] = "Stacja pogody.\nWybor funkcji\nprzy pomocy\njoystick'a:\nL - temperatura\nR - jasnosc\nG - wilgotnosc\nD - cisnienie";
				lcdPuts(textToBeWritten);
			}
			sdelay(1);
			lcdClrscr();
		}
	}
}

/*
 * @brief   Funkcja printCurrentTime() służy do wyświetlenia aktualnego czasu względem zegara
 * 			czasu rzeczywistego (RTC) w formacie HH:MM:SS (gdzie H - oznacza godzinę, M - minutę,
 *			a S - sekundę).
 *
 * @param   void
 *
 * @returns void
 *
 * @side effects:
 *          Wyświetlany czas jest zależy od wartości inicjalizującej zegar czasu reczywistego (RTC),
 * 			gdzie w tym przypadku nie jest on inicjalizowany.
 */

void printCurrentTime(void)
{
	// TODO: Write comments to below code
	RTC_CCR = 0x00000010;
	RTC_ILR = 0x00000000;
	RTC_CIIR = 0x00000000;
	RTC_AMR = 0x00000000;
	mdelay(100);
	RTC_CCR = 0x00000011;
	// Till here

	tU8 fullHourTable[9] = {0};
	fullHourTable[2] = ':';
	fullHourTable[5] = ':';
	if (RTC_SEC < 10) {
		fullHourTable[6] = (tU8)0x30; 				// Ascii code for '0'.
		fullHourTable[7] = RTC_SEC + (tU8)0x30; 	// Ascii code for '0'.
	} else {
		tU8 div = RTC_SEC / 10;
		fullHourTable[6] = div + (tU8)0x30;
		fullHourTable[7] = ((tU8)RTC_SEC - (div * (tU8)10)) + (tU8)0x30;
	}
	
	if (RTC_MIN < 10) {
		fullHourTable[3] = (tU8)0x30;				// Ascii code for '0'.
		fullHourTable[4] = RTC_MIN + (tU8)0x30;		// Ascii code for '0'.
	} else {
		tU8 div = RTC_MIN / 10;
		fullHourTable[3] = div + (tU8)0x30;
		fullHourTable[4] = ((tU8)RTC_MIN - (div * (tU8)10)) + (tU8)0x30;
	}
	
	if (RTC_HOUR < 10){
		fullHourTable[0] = (tU8)0x30;				// Ascii code for '0'.
		fullHourTable[1] = RTC_HOUR + (tU8)0x30;	// Ascii code for '0'.
	} else {
		tU8 div = RTC_HOUR / 10;
		fullHourTable[0] = div + (tU8)0x30;
		fullHourTable[1] = ((tU8)RTC_HOUR - (div * (tU8)10)) + (tU8)0x30;
	}

	lcdPuts(fullHourTable);
}
