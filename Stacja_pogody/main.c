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

/************************************************************************
 * @Description: uruchomienie obsługi przerwań
 * @Parameter:
 *    [in] period    : okres generatora przerwań
 *    [in] duty_cycle: wypełnienie w %
 * @Returns: Nic
 * @Side effects:
 *    przeprogramowany timer #1
 *************************************************************************/
/*
static void init_irq (tU32 period, tU8 duty_cycle)
{
	//Zainicjuj VIC dla przerwań od Timera #1
	VICIntSelect &= ~TIMER_1_IRQ;           //Przerwanie od Timera #1 przypisane do IRQ (nie do FIQ)
	VICVectAddr5  = (tU32)IRQ_Test;         //adres procedury przerwania
	VICVectCntl5  = VIC_ENABLE_SLOT | TIMER_1_IRQ_NO;
	VICIntEnable  = TIMER_1_IRQ;            // Przypisanie i odblokowanie slotu w VIC od Timera #1

	T1TCR = TIMER_RESET;                    //Zatrzymaj i zresetuj
	T1PR  = 0;                              //Preskaler nieużywany
	T1MR0 = ((tU64)period)*((tU64)PERIPHERAL_CLOCK)/1000;
	T1MR1 = (tU64)T1MR0 * duty_cycle / 100; //Wypełnienie
	T1IR  = TIMER_ALL_INT;                  //Resetowanie flag przerwań
	T1MCR = MR0_I | MR1_I | MR0_R;          //Generuj okresowe przerwania dla MR0 i dodatkowo dla MR1
	T1TCR = TIMER_RUN;                      //Uruchom timer
}
*/

void printCurrentTime(void);

int main(void)
{
	tU8 pca9532Present = FALSE;
	// check if connection with PCA9532
	i2cInit();
	pca9532Present = pca9532Init();
	printf_init();
	if ((tU8)TRUE == pca9532Present)
	{

		lcdInit();
		lcdColor(0xff, 0x00);
		lcdClrscr();

		/*
			Inicjalizacja I2C
		*/

		/*
			Main loop containg enitre program logic. With every iteration, joystick position is checked
			and based on that, appropriate measurement value.
		*/
		
		// Address of the LM75 thermometer, when not soldered. 
		// tU8 lm75Address = 0x9F;
		// Address of the LM75 thermometer, when soldered.
		tU8 charArray[10] = {0};

		tU8 helperValue = (tU8)1;
		while (helperValue == (tU8)TRUE)
		{

			// Trzeba będzie napisać jak ustawiony ma być PINSEL.

			// Check if P0.8 center-key is pressed
			if ((IOPIN & 0x00000100) == 0)
			{
				// Key P0.8 center-key is pressed
				lcdGotoxy(0, 0);
				lcdPuts("Aktualny czas: ");
				lcdGotoxy(0, 15);
			 	printCurrentTime();
			 	manageLED(pca9532Present);
			}
			else if ((IOPIN & 0x00000200) == 0)
			{
				// Key P0.9 left-key is pressed
				lcdGotoxy(0, 0);
				lcdPuts("Pomiar\ntemperatury: ");
				lcdGotoxy(0, 30);
				measureTemperature();
				manageLED(pca9532Present);
			}
			else if ((IOPIN & 0x00000400) == 0)
			{
				// Key P0.10 up-key is pressed
				lcdGotoxy(0, 0);
				lcdPuts("Pomiar\nwilgotnosci: ");
				tU16 humidityValue = measureHumidity();
				sprintf(charArray, "%d", humidityValue);
				lcdGotoxy(0, 30);
				lcdPuts(charArray);
				manageLED(pca9532Present);
			}
			else if ((IOPIN & 0x00000800) == 0)
			{
				// Key P0.11 right-key is pressed
				lcdGotoxy(0, 0);
				lcdPuts("Pomiar\njasnosci: ");
				tU64 brightnessValue = measureBrightness();
				sprintf(charArray, "%d", brightnessValue);
				lcdGotoxy(0, 30);
				lcdPuts(charArray);
				manageLED(pca9532Present);
			}
			else if ((IOPIN & 0x00001000) == 0)
			{
				// Key P0.12 down-key is pressed
				lcdGotoxy(0, 0);
				lcdPuts("Pomiar\ncisnienia: ");
				tS64 pressureValue = measurePressure();
				sprintf(charArray, "%d", pressureValue);
				lcdGotoxy(0, 30);
				lcdPuts(charArray);
				manageLED(pca9532Present);
			}
			else
			{
				// Joystick is not pressed at all.
				lcdGotoxy(0, 0);
				lcdPuts("Stacja pogody.\nWybor funkcji\nprzy pomocy\njoystick'a:\nL - temperatura\nR - jasnosc\nG - wilgotnosc\nD - cisnienie");
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
		fullHourTable[6] = '0'; 			// Ascii code for '0'.
		fullHourTable[7] = RTC_SEC + '0'; 	// Ascii code for '0'.
	} else {
		tU8 div = RTC_SEC / 10;
		fullHourTable[6] = div + '0';
		fullHourTable[7] = ((tU8)RTC_SEC - (div * (tU8)10)) + '0';
	}
	
	if (RTC_MIN < 10) {
		fullHourTable[3] = '0';				// Ascii code for '0'.
		fullHourTable[4] = RTC_MIN + '0';	// Ascii code for '0'.
	} else {
		tU8 div = RTC_MIN / 10;
		fullHourTable[3] = div + '0';
		fullHourTable[4] = ((tU8)RTC_MIN - (div * (tU8)10)) + '0';
	}
	
	if (RTC_HOUR < 10){
		fullHourTable[0] = '0';			// Ascii code for '0'.
		fullHourTable[1] = RTC_HOUR + '0';	// Ascii code for '0'.
	} else {
		tU8 div = RTC_HOUR / 10;
		fullHourTable[0] = div + '0';
		fullHourTable[1] = ((tU8)RTC_HOUR - (div * (tU8)10)) + '0';
	}

	lcdPuts(fullHourTable);
}
