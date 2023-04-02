/*************************************************************************************
 *
 * @Description:
 * Program przykładowy - odpowiednik "Hello World" dla systemów wbudowanych
 * Rekomendujemy wkopiowywanie do niniejszego projektu nowych funkcjonalności
 *
 *
 * UWAGA! Po zmianie rozszerzenia na cpp program automatycznie będzie używać
 * kompilatora g++. Oczywiście konieczne jest wprowadzenie odpowiednich zmian w
 * pliku "makefile"
 *
 *
 * Program przykładowy wykorzystuje Timer #0 i Timer #1 do "mrugania" diodami
 * Dioda P1.16 jest zapalona i gaszona, a czas pomiędzy tymi zdarzeniami
 * odmierzany jest przez Timer #0.
 * Program aktywnie oczekuje na upłynięcie odmierzanego czasu (1s)
 *
 * Druga z diod P1.17 jest gaszona i zapalana w takt przerwań generowanych
 * przez timer #1, z okresem 500 ms i wypełnieniem 20%.
 * Procedura obsługi przerwań zdefiniowana jest w innym pliku (irq/irq_handler.c)
 * Sama procedura MUSI być oznaczona dla kompilatora jako procedura obsługi 
 * przerwania odpowiedniego typu. W przykładzie jest to przerwanie wektoryzowane.
 * Odpowiednia deklaracja znajduje się w pliku (irq/irq_handler.h)
 * 
 * Prócz "mrugania" diodami program wypisuje na konsoli powitalny tekst.
 * 
 * @Authors: Michał Morawski,
 *           Daniel Arendt, 
 *           Przemysław Ignaciuk,
 *           Marcin Kwapisz
 *
 * @Change log:
 *           2016.12.01: Wersja oryginalna.
 *
 ******************************************************************************/

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
#include "lm75.h"
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
		//check if connection with PCA9532
	    i2cInit();
		pca9532Present = pca9532Init();
		printf_init();
		while(1) {
		if (TRUE == pca9532Present)
		{

				lcdInit();
			    lcdColor(0xff, 0x00);
			    lcdClrscr();

			    /*
			        Inicjalizacja I2C
			    */


			    /*
			        Główna pętla zawierająca całą logikę programu - tj. w okresach 2 sekudnowych następuje
			        pomiar temperatury, który jest wyświetlany na wyświetlaczu.
			    */

			    /*
			         Adres termometru LM75 po drugiej stronie interfejsu I2C.
			         Jak nie zadzaiała to 3 przedostatnie bity zamienić i 1 i sprawdzić.
			    */
			    tU8 lm75Address = 0x9F;
			    // dla zalutowanego	tU8 lm75Address = (0x48<<1)|1;
			    //tU8 htu21dfaddress = 0x40;
			    tU8 readTemperature[2];
			    //tU8 readHumidity[2];
			    tU8 temperatureInChar[20];
			    //tU8 humidityInChar[43];
			    tS8 temperatureValue;
			    //tS8 humidityValue;

			    while (TRUE)
			    {
			    	lcdGotoxy(0,0);
			    	if (measureTemperature(lm75Address, readTemperature, 2) != I2C_CODE_ERROR)
			    	        {
			    	            temperatureValue = calculateTemperatureValue(readTemperature);
			    	            lcdClrscr();
			    	            sprintf(temperatureInChar, "Temp: %d ", temperatureValue);
			    	            lcdGotoxy(10, 10);
			    	            lcdPuts(temperatureInChar);
			    	        }
			        sdelay(2);
			    }



		    sdelay(10);
		}
		}
} 


