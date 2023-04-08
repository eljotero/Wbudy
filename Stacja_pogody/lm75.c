/*
    Obsługa pomiaru temperatury przy pomocy temometru LM75.

    Plik z kodem źródłowym funkcji.
*/

#include "lm75.h"
#include "additional.h"
#include "lcd.h"


tS8 measureTemperature(tU8 addr,
                       tU8 *pBuf,
                       tU16 len)
{
    return i2cRead(addr, pBuf, len);

}

void calculateTemperatureValue(tU8 *byteArray)
{
	tU8 i;
    tU8 charArray[6] = {};
    byteArray[1] = (byteArray[1] & (1 << 8));
    tS8 calculatedValue = byteArray[0];
    tU8 isNegative = 0;
    tU8 appendHalf = 0;
    if (calculatedValue < 0) {
    	isNegative = 1;
    }
    if (isNegative == 1 && (byteArray[1] & 0x80) == 1){
    	calculatedValue--;
    	appendHalf = 1;
    } else if (isNegative == 0 && (byteArray[1] & 0x80) == 1) {
    	appendHalf = 1;
    }

    if (isNegative == 1 && appendHalf == 1) {
    	sprintf(charArray, "%d.5", calculatedValue);
    } else if (isNegative == 1 && appendHalf == 0) {
    	sprintf(charArray, "%d", calculatedValue);
    }  else if (isNegative == 0 && appendHalf == 1) {
    	sprintf(charArray, "%d.5", calculatedValue);
    } else {
    	sprintf(charArray, "%d", calculatedValue);
    }
    lcdPuts(charArray);
}
