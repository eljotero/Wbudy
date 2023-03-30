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

#include "Common_Def.h"
#include <stdio.h>
#include "i2c.h"
#include "lcd.h"

/************************************************************************
 * @Description: opóźnienie wyrażone w liczbie sekund
 * @Parameter:
 *    [in] seconds: liczba sekund opĂłĹşnienia
 * @Returns: Nic
 * @Side effects:
 *    przeprogramowany Timer #0
 *************************************************************************/
static void sdelay(tU32 seconds)
{
    T0TCR = TIMER_RESET;         // Zatrzymaj i zresetuj
    T0PR = PERIPHERAL_CLOCK - 1; // jednostka w preskalerze
    T0MR0 = seconds;
    T0IR = TIMER_ALL_INT; // Resetowanie flag przerwaĹ„
    T0MCR = MR0_S;        // Licz do wartości w MR0 i zatrzymaj się
    T0TCR = TIMER_RUN;    // Uruchom timer

    // sprawdź czy timer działa
    // nie ma wpisanego ogranicznika liczby pętli, ze względu na charakter procedury
    while (T0TCR & TIMER_RUN)
    {
    }
}

/************************************************************************
 * @Description: uruchomienie obsługi przerwań
 * @Parameter:
 *    [in] period    : okres generatora przerwań
 *    [in] duty_cycle: wypełnienie w %
 * @Returns: Nic
 * @Side effects:
 *    przeprogramowany timer #1
 *************************************************************************/
static void init_irq(tU32 period, tU8 duty_cycle)
{
    // Zainicjuj VIC dla przerwań od Timera #1
    VICIntSelect &= ~TIMER_1_IRQ;  // Przerwanie od Timera #1 przypisane do IRQ (nie do FIQ)
    VICVectAddr5 = (tU32)IRQ_Test; // adres procedury przerwania
    VICVectCntl5 = VIC_ENABLE_SLOT | TIMER_1_IRQ_NO;
    VICIntEnable = TIMER_1_IRQ; // Przypisanie i odblokowanie slotu w VIC od Timera #1

    T1TCR = TIMER_RESET; // Zatrzymaj i zresetuj
    T1PR = 0;            // Preskaler nieużywany
    T1MR0 = ((tU64)period) * ((tU64)PERIPHERAL_CLOCK) / 1000;
    T1MR1 = (tU64)T1MR0 * duty_cycle / 100; // Wypełnienie
    T1IR = TIMER_ALL_INT;                   // Resetowanie flag przerwań
    T1MCR = MR0_I | MR1_I | MR0_R;          // Generuj okresowe przerwania dla MR0 i dodatkowo dla MR1
    T1TCR = TIMER_RUN;                      // Uruchom timer
}

int main(void)
{
    // uruchomienie 'simple printf'
    printf_init();

    // powitanie
    simplePrintf("\n\n\n\n");
    simplePrintf("\n*********************************************************");
    simplePrintf("\n*");
    simplePrintf("\n* Systemy Wbudowane");
    simplePrintf("\n* Wydzial FTIMS");
    simplePrintf("\n* Moj pierwszy program");
    simplePrintf("\n*");
    simplePrintf("\n*********************************************************");

    /*********************************************************************
     * Ta część inicjuje działanie programy
     *********************************************************************/
    // uruchomienie GPIO na nodze P1.16 i P1.17.
    // PINSEL2 &= ~(1 << 3);
    // Kierunek out na nodze P1.16
    // IODIR1 |= DIODA_LEWA; //(1 << 16);
    // Kierunek out na nodze P1.17
    // IODIR1 |= DIODA_PRAWA; //(1 << 17);
    // Uruchomienie przerwań co 1/2 s.
    // init_irq(500, 20);

    /**********************************************************************
     * Ta część jest nieskończoną pętlą realizującą działania programu
     * Należy jednak pamiętać, że część jego funkcjonalności realizowana
     * jest za pomocą przerwań
     **********************************************************************/
    // Aktywne "mruganie" diodą
    /*
    tBool dioda_swieci = FALSE;
    while (TRUE)
    {
        if (dioda_swieci)
        {
            IOSET1 = DIODA_LEWA; // (1 << 16);
            simplePrintf("Nie swieci\n");
        }
        else
        {
            IOCLR1 = DIODA_LEWA; //(1 << 16);
            simplePrintf("Swieci\n");
        }
        dioda_swieci = !dioda_swieci;
        sdelay(1);  // czekaj 1 s
    }
    */

    /*
         Inicjalizacja wyświetlacza OLED.
    */

    lcdInit();
    lcdColor(0xff, 0x00);
    lcdClrscr();

    /*
        Inicjalizacja I2C
    */

    i2cInit();

    /*
        Główna pętla zawierająca całą logikę programu - tj. w okresach 2 sekudnowych następuje
        pomiar temperatury, który jest wyświetlany na wyświetlaczu.
    */

    /*
         Adres termometru LM75 po drugiej stronie interfejsu I2C.
         Jak nie zadzaiała to 3 przedostatnie bity zamienić i 1 i sprawdzić.
    */

    tU8 lm75Address = 0b10010000;
    tU8 htu21dfaddress = 0b01000000;
    tU8 readTemperature[2];
    tU8 readHumidity[2];
    tU8 temperatureInChar[51];
    tU8 humidityInChar[43];
    tS8 temperatureValue;
    tS8 humidityValue;

    while (TRUE)
    {
        if (measureTemperature(lm75Address, readTemperature, 2) != I2C_CODE_ERROR)
        {
            temperatureValue = calculateTemperatureValue(readTemperature);
            lcdClrscr();
            sprintf(temperatureInChar, "Obecna wartość temperatury: %f stopni Celsjusza", temperatureValue);
            lcdGotoxy(10, 10);
            lcdPuts(temperatureInChar);
        }
        if (measureHumidity(htu21dfaddress, readHumidity, 2) != I2C_CODE_ERROR)
        {
            humidityValue = calculateHumidityValue(readHumidity);
            lcdClrscr();
            sprintf(humidityInChar, "Obecna wartość wilgotności: %f procent", humidityValue);
            lcdGotoxy(20, 10);
            lcdPuts(humidityInChar);
        }
        sdelay(2);
    }
}