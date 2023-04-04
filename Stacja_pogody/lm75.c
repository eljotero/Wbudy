/*
    Obsługa pomiaru temperatury przy pomocy temometru LM75.

    Plik z kodem źródłowym funkcji.
*/

#include "lm75.h"
#include "additional.h"

tS8 measureTemperature(tU8 addr,
                       tU8 *pBuf,
                       tU16 len)
{
    tU8 retCode;
    return retCode = i2cRead(addr, pBuf, len);
}

tS8 calculateTemperatureValue(tU8 *byteArray)
{
	tU8 i;
    tS16 intValue = 0;
    tU16 readValue = ((byteArray[0] << 8) | byteArray[1]);
    tBool isNegative = (readValue & 0x8000);
    if (isNegative)
    {
        intValue = -1;
    }
    for (i = 1; i < 9; i++)
    {
        intValue = intValue * 2 + ((readValue << i) & 0x8000);
    }
    return (intValue);
}
