#include "general.h"
#include <lpc2xxx.h>
#include <printf_P.h>
#include <printf_init.h>
#include <consol.h>
#include <config.h>
#include "irq/irq_handler.h"
#include "timer.h"
#include "VIC.h"
#include "i2c.h"
#include <stdio.h>
#include <string.h>

// Include'y do urządzeń peryferyjnych

#include "lm75.h"
#include "htu21df.h"
#include "bmp180.h"
#include "tsl2561.h"

// End

#include "lcd.h"
#include "pca9532.h"
#include "additional.h"

#include "Common_Def.h"

/*!
 *  @brief    Uruchomienie obsługi przerwań
 *  @param period
 *             Okres generatora przerwań
 *  @param duty_cycle
 *             Wypełnienie w %
 *
 *  @returns  Nic
 *  @side effects:
 *            Przeprogramowany timer #1
 */

/*!
 *  @brief Main function of the entire program - responsible for managing all user
 * 		   interaction and showing them results of these actions.
 *  @returns Technically, this function is executed constantly, so in fact it does not return
 * 			 any value.
 *  @side effects: As for now, they are possible, but not yet known.
 */

int main(void)
{
	tU8 pca9532Present = FALSE;
	// I2C initalization
	i2cInit();
	pca9532Present = pca9532Init();
	printf_init();
	// LCD display initialization
	lcdInit();
	// Setting colors for background and text.
	lcdColor(0xff, 0x00);
	lcdClrscr();

	/*
		Main loop containg enitre program logic. With every iteration, joystick position is checked
		and based on that, appropriate measurement value.
	*/
	tU8 *charPtr;
	tS8 returnedValue = 0;
	tU8 charArray[10] = {0};
	// Variables for holding messages to display with lcdPuts
	tU8 *messagePointer;
	tU8 messageHolder[128];

	tS8 humidityValue;
	tU16 brightnessValue;
	tS64 pressureValue;

	tU8 helperVariable = TRUE;

	while (helperVariable == (tU8)TRUE)
	{

		// Trzeba będzie napisać jak ustawiony ma być PINSEL.

		// Check if P0.8 center-key is pressed
		if ((IOPIN & 0x00000100) == 0)
		{
			// Key P0.8 center-key is pressed
			lcdGotoxy(0, 0);
			messagePointer = strcpy(messageHolder, "Aktualny \nczas: ");
			lcdPuts(messagePointer);
			lcdGotoxy(0, 28);
			// lcdGotoxy(48, 0);
		}
		else if ((IOPIN & 0x00000200) == 0)
		{
			// Key P0.9 left-key is pressed
			lcdGotoxy(0, 0);
			messagePointer = strcpy(messageHolder, "Pomiar \temperatury: \n");
			lcdPuts(messagePointer);
			lcdGotoxy(0, 30);
			measureTemperature(charArray);
		}
		else if ((IOPIN & 0x00000400) == 0)
		{
			// Key P0.10 up-key is pressed
			lcdGotoxy(0, 0);
			messagePointer = strcpy(messageHolder, "Pomiar \nwilgotnosci: \n");
			lcdPuts(messagePointer);
			humidityValue = measureHumidity();
			returnedValue = sprintf(charArray, "%d '%'", humidityValue);
			if (returnedValue == (tS8)(-1))
			{
				charPtr = strncpy(charArray, "Error", 6);
			}
			lcdGotoxy(0, 30);
			lcdPuts(charPtr);
		}
		else if ((IOPIN & 0x00000800) == 0)
		{
			// Key P0.11 right-key is pressed
			lcdGotoxy(0, 0);
			messagePointer = strcpy(messageHolder, "Pomiar \njasnosci: \n");
			lcdPuts(messagePointer);
			brightnessValue = measureBrightness();
			returnedValue = sprintf(charArray, "%d Lux", brightnessValue);
			if (returnedValue == (tS8)(-1))
			{
				charPtr = strncpy(charArray, "Error", 6);
			}
			lcdGotoxy(0, 30);
			lcdPuts(charPtr);
		}
		else if ((IOPIN & 0x00001000) == 0)
		{
			// Key P0.12 down-key is pressed
			lcdGotoxy(0, 0);
			messagePointer = strcpy(messageHolder, "Pomiar \ncisnienia: \n");
			lcdPuts(messagePointer);
			pressureValue = measurePressure();
			returnedValue = sprintf(charArray, "%d hPa", pressureValue / 100);
			if (returnedValue == (tS8)(-1))
			{
				charPtr = strncpy(charArray, "Error", 6);
			}
			lcdGotoxy(0, 30);
			lcdPuts(charPtr);
		}
		else
		{
			// Joystick is not pressed at all.
			lcdGotoxy(0, 0);
			messagePointer = strcpy(messageHolder, "Stacja pogody! \nPoruszaj joystick'iem \nw celu wybrania \nfunkcjonalności.");
			lcdPuts(messagePointer);
		}
		mdelay(300);
		lcdClrscr();
	}
}

void printCurrentTime(void)
{
	// TODO: Write comments to below code
	RTC_CCR = 0x00000010;
	RTC_ILR = 0x00000000;
	RTC_CIIR = 0x00000000;
	RTC_AMR = 0x00000000;
	mdelay(100);
	RTC_SEC = 50;
	RTC_MIN = 59;
	RTC_HOUR = 23;
	mdelay(100);
	RTC_CCR = 0x00000011;
	// Till here

	tU8 secTable[2] = {0};
	tU8 minTable[2] = {0};
	tU8 hourTable[2] = {0};
	if (RTC_SEC < 10)
	{
		secTable[0] = '0';
		secTable[1] = RTC_SEC + '0';
	}
	else
	{
		tU8 div = RTC_SEC / 10;
		secTable[0] = div;
		secTable[1] = (RTC_SEC - (div * 10));
	}
	if (RTC_MIN < 10)
	{
		minTable[0] = '0';
		minTable[1] = RTC_MIN + '0';
	}
	else
	{
		tU8 div = RTC_MIN / 10;
		minTable[0] = div;
		minTable[1] = (RTC_MIN - (div * 10));
	}
	if (RTC_HOUR < 10)
	{
		hourTable[0] = '0';
		hourTable[1] = RTC_HOUR + '0';
	}
	else
	{
		tU8 div = RTC_HOUR / 10;
		hourTable[0] = div;
		hourTable[1] = (RTC_HOUR - (div * 10));
	}

	int length = sizeof(secTable) / sizeof(secTable[0]) +
				 sizeof(minTable) / sizeof(minTable[0]) +
				 sizeof(hourTable) / sizeof(hourTable[0] + 2);

	tU8 fullHourTable[length];

	memcpy(fullHourTable, secTable, sizeof(secTable));
	memcpy(fullHourTable + sizeof(secTable) / sizeof(secTable[0]), ":", sizeof(":"));
	memcpy(fullHourTable + sizeof(secTable) / sizeof(secTable[0]) + 1, minTable, sizeof(minTable));
	memcpy(fullHourTable + (sizeof(secTable) / sizeof(secTable[0]) + sizeof(minTable) / sizeof(minTable[0]) + 1), ":", sizeof(":"));
	memcpy(fullHourTable + (sizeof(secTable) / sizeof(secTable[0]) + sizeof(minTable) / sizeof(minTable[0]) + 2), hourTable, sizeof(hourTable));

	lcdPuts(fullHourTable);
}

void printCurrentDate(void)
{
	;
}
