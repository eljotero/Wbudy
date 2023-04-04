/*
    Plik z kodem źródłowym funkcji do obsługi
    czujnika wilgotności HTU21D(F)
*/

#include "general.h"
#include "i2c.h"
#include "additional.h"

tU16 measureTemperature(tU8 address,
                        tU8 *pBuf,
                        tU16 length)
{
    tS8 retCode;
    /*
        Komenda odczytu wilgotności w trybie "No Hold Master".
    */
    tU8 *commandNoHoldMaster;
    *commandNoHoldMaster = 0xF5;
    /*
        Adres rejestru do którego trzeba wpisać komendę: (0x40 << 1)
        Adres rejestru z którego trzeba odczytać wynik pomiaru: ((0x40 << 1) | 1)
    */
    retCode = i2cWrite(address << 1, commandNoHoldMaster, 1);
    mdelay(16);
    retCode = i2cRead(((address << 1) | 1), pBuf, 2);
    
    return retCode;
}

tS16 calculateHumidity(tU8 *byteArray)
{
    /*
        Konwersja tablicy 2 bajtów na jedną zmienną 16 bitową.
        Wyłączenie bitów stanu - dla wilgotności są to ostatnie 4 bity.
    */

    tU16 readData = ((byteArray[0] << 8) | byteArray[1]);
    readData |= ~(0x000F);

    return (((125 * (readData)) / 65536.0) - 6.0);
}
