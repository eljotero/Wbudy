/*
    Plik z kodem źródłowym funkcji do obsługi
    czujnika wilgotności HTU21D(F)
*/

#include "htu21df.h"

/*
 * @brief   Funkcja measureHumidity() wykorzystywana jest do odczytania z czujnika wilgotności
 *          nieprzeliczonej wartości wilgotności i następnie jej przeliczenie, tak aby reprezentowała
 *          wilgotność wyrażoną w procentach.
 *         
 * @param   void  
 *          
 * @returns 
 *          Przeliczona wartość ciśnienia, która wyrażona jest w procentach.
 * 
 * @side effects: 
 *          Brak
 */

tS8 measureHumidity(void)
{
    // Adres urządzenia HTU21DF
    tU8 htu21Address = 0x40;

    // Adres do zapisu do urządzenia HTU21DF
    tU8 htu21WriteAddress = ((tU8)htu21Address << 1);

    // Adres do odczytu do urządzenia HTU21DF
    tU8 htu21ReadAddress = ((tU8)htu21Address << 1 | (tU8)1);

    // Bufor 2-bajtowy do przechowania wartości odczytanej z czujnika HTU21DF 
    tU8 readHumidity[2] = {0};

    /*
        Adres rejestru do którego trzeba wpisać komendę: (0x40 << 1)
        Adres rejestru z którego trzeba odczytać wynik pomiaru: ((0x40 << 1) | 1)
    */

    tS8 retCode;

    // Komenda odczytu wilgotności w trybie "No Hold Master".
    tU8 commandNoHoldMaster[1] = {0};
    commandNoHoldMaster[0] = 0xF5;
    retCode = i2cWrite(htu21WriteAddress, commandNoHoldMaster, 1);
    mdelay(16);
    retCode = i2cRead(htu21ReadAddress, readHumidity, 2);
    
    /*
        Konwersja tablicy 2 bajtów na jedną zmienną 16 bitową.
        Wyłączenie bitów stanu - dla wilgotności są to ostatnie 4 bity.
    */

    tU8 mask = ~(0x0F);
	readHumidity[1] = (readHumidity[1] & mask);
    tU16 readData = ((readHumidity[0] << 8) | readHumidity[1]);

    tU32 upperValue = ((tU32)125 * (tU32)readData);
    tS8 humidityValue = ((tS8)(upperValue / (tU32)65536) - (tS8)6);

    if (humidityValue < 0) {
        humidityValue = 0;
    }
    else if (humidityValue > 100)
    {
        humidityValue = 100;
    }
    

    return humidityValue;
}
