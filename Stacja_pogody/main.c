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
		
		// Address of the LM75 thermometer, when not soldered. 
		// tU8 lm75Address = 0x9F;
		// Address of the LM75 thermometer, when soldered.
		tU8 charArray[10] = {0};
		tU8 amountOfLettersCopied = 0;

		tU8 helperValue = (tU8)1;
		while (helperValue == (tU8)TRUE)
		{
			if ((IOPIN & 0x00000100) == 0)
			{
				// Key P0.8 center-key is pressed
				lcdGotoxy(0, 0);
				const tU8 textToBeWritten[] = "Aktualny czas: ";
				lcdPuts(textToBeWritten);
				lcdGotoxy(0, 15);
			 	printCurrentTime();
			 	manageLED(pca9532Present);
			}
			else if ((IOPIN & 0x00000200) == 0)
			{
				// Key P0.9 left-key is pressed
				lcdGotoxy(0, 0);
				const tU8 textToBeWritten[] = "Pomiar\ntemperatury: ";
				lcdPuts(textToBeWritten);
				lcdGotoxy(0, 30);
				measureTemperature();
				manageLED(pca9532Present);
			}
			else if ((IOPIN & 0x00000400) == 0)
			{
				// Key P0.10 up-key is pressed
				lcdGotoxy(0, 0);
				const tU8 textToBeWritten[] = "Pomiar\nwilgotnosci: ";
				lcdPuts(textToBeWritten);
				tU16 humidityValue = measureHumidity();
				retCode = sprintf(charArray, "%d", humidityValue);
				lcdGotoxy(0, 30);
				lcdPuts(charArray);
				manageLED(pca9532Present);
			}
			else if ((IOPIN & 0x00000800) == 0)
			{
				// Key P0.11 right-key is pressed
				lcdGotoxy(0, 0);
				const tU8 textToBeWritten[] = "Pomiar\njasnosci: ";
				lcdPuts(textToBeWritten);
				tU64 brightnessValue = measureBrightness();
				retCode = sprintf(charArray, "%d", brightnessValue);
				lcdGotoxy(0, 30);
				lcdPuts(charArray);
				manageLED(pca9532Present);
			}
			else if ((IOPIN & 0x00001000) == 0)
			{
				// Key P0.12 down-key is pressed
				lcdGotoxy(0, 0);
				const tU8 textToBeWritten[] = "Pomiar\ncisnienia: ";
				lcdPuts(textToBeWritten);
				tS64 pressureValue = measurePressure();
				retCode = sprintf(charArray, "%d", pressureValue);
				lcdGotoxy(0, 30);
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
