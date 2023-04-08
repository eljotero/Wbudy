/*
    Plik z kodem źródłowym funkcji do obsługi
    czujnika wilgotności HTU21D(F)
*/

#include "general.h"
#include "i2c.h"
#include "additional.h"

tS8 measureHumidity(tU8 address,
                        tU8 *pBuf,
                        tU16 length)
{
    tS8 retCode;
    /*
        Komenda odczytu wilgotności w trybie "No Hold Master".
    */
    tU8 commandNoHoldMaster[1] = {};
    commandNoHoldMaster[0] = 0xF5;
    /*
        Adres rejestru do którego trzeba wpisać komendę: (0x40 << 1)
        Adres rejestru z którego trzeba odczytać wynik pomiaru: ((0x40 << 1) | 1)
    */
    tU8 finalAddress = (address << 1);
    retCode = i2cWrite(finalAddress, commandNoHoldMaster, 1);
    mdelay(16);
    finalAddress = ((address << 1) | 1);
    retCode = i2cRead(finalAddress, pBuf, 2);
    return retCode;
}

tS8 calculateHumidity(tU8 *byteArray)
{
    /*
        Konwersja tablicy 2 bajtów na jedną zmienną 16 bitową.
        Wyłączenie bitów stanu - dla wilgotności są to ostatnie 4 bity.
    */

	tU8 mask = ~(0x0F);
	byteArray[1] = (byteArray[1] & mask);
    tU16 readData = ((byteArray[0] << 8) | byteArray[1]);

    return (((125 * (readData)) / 65536.0) - 6.0);
}
