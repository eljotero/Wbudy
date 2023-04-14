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
#include <stdio.h>

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

int main(void)
{
	tU8 pca9532Present = FALSE;
	// check if connection with PCA9532
	i2cInit();
	pca9532Present = pca9532Init();
	printf_init();
	if (TRUE == pca9532Present)
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
		tU8 lm75Address = (0x48 << 1) | 1;
		tU8 htu21dfaddress = 0x40;
		tU8 readTemperature[2] = {};
		tU8 readHumidity[2] = {};
		tU8 charArray[10] = {};
		tU16 humidityValue;
		tU16 brightnessValue;
		tS64 pressureValue;

		while (TRUE)
		{

			// Trzeba będzie napisać jak ustawiony ma być PINSEL.

			// Check if P0.8 center-key is pressed
			if ((IOPIN & 0x00000100) == 0)
			{
				// Key P0.8 center-key is pressed
				lcdClrscr();
				lcdGotoxy(0, 0);
				lcdPuts("Stacja pogody");
			}
			else if ((IOPIN & 0x00000200) == 0 && measureTemperature(lm75Address, readTemperature, 2) != I2C_CODE_ERROR)
			{
				// Key P0.9 left-key is pressed
				lcdClrscr();
				lcdGotoxy(0, 0);
				lcdPuts("Pomiar");
				lcdGotoxy(0, 15);
				lcdPuts("temperatury: ");
				lcdGotoxy(0, 30);
				calculateTemperatureValue(readTemperature);
			}
			else if ((IOPIN & 0x00000400) == 0 && measureHumidity(htu21dfaddress, readHumidity, 2) != I2C_CODE_ERROR)
			{
				// Key P0.10 up-key is pressed
				lcdClrscr();
				lcdGotoxy(0, 0);
				lcdPuts("Pomiar");
				lcdGotoxy(0, 15);
				lcdPuts("wilgotnosci: ");
				humidityValue = calculateHumidity(readHumidity);
				sprintf(charArray, "%d", humidityValue);
				lcdGotoxy(0, 30);
				lcdPuts(charArray);
			}
			else if ((IOPIN & 0x00000800) == 0)
			{
				// Key P0.11 right-key is pressed
				lcdClrscr();
				lcdGotoxy(0, 0);
				lcdGotoxy(0, 0);
				lcdPuts("Pomiar");
				lcdGotoxy(0, 15);
				lcdPuts("jasnosci: ");
				brightnessValue = measureBrightness();
				sprintf(charArray, "%d", brightnessValue);
				lcdGotoxy(0, 30);
				lcdPuts(charArray);
			}
			else if ((IOPIN & 0x00001000) == 0)
			{
				// Key P0.12 down-key is pressed
				lcdClrscr();
				lcdGotoxy(0, 0);
				lcdGotoxy(0, 0);
				lcdPuts("Pomiar");
				lcdGotoxy(0, 15);
				lcdPuts("cisnienia: ");
				pressureValue = measurePressure();
				sprintf(charArray, "%d", pressureValue);
				lcdGotoxy(0, 30);
				lcdPuts(charArray);
			}
			else
			{
				// Joystick is not pressed at all.
				lcdClrscr();
				lcdGotoxy(0, 0);
				lcdGotoxy(0, 0);
				lcdPuts("Aktualny");
				lcdGotoxy(0, 15);
				lcdPuts("czas: ");
			}
			sdelay(1);
		}
	}
}
