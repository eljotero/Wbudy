/*
    Obsługa pomiaru temperatury przy pomocy temometru LM75.

    Plik z kodem źródłowym funkcji.
*/

#include "lm75.h"
#include "additional.h"

tS8 measureTemperature1(tU8 addr,
                        tU8 *pBuf,
                        tU16 len)
{
    tS8 status;
    tS8 retCode = i2cStart();

    if (retCode == I2C_CODE_OK)
    {
        retCode = i2cPutChar(addr);
        while (retCode == I2C_CODE_BUSY)
        {
            retCode = i2cPutChar(addr);
        }

        if (retCode == I2C_CODE_OK)
        {

            for (tU8 i = 0; i < len; i++)
            {
                /* wait until data transmitted */
                while (TRUE)
                {
                    /* Get new status */
                    status = i2cCheckStatus();

                    if ((status == 0x40) || (status == 0x48) || (status == 0x50))
                    {
                        /* Data received */

                        if (i == len)
                        {
                            /* Set generate NACK */
                            retCode = i2cGetChar(I2C_MODE_ACK1, pBuf);
                        }
                        else
                        {
                            retCode = i2cGetChar(I2C_MODE_ACK0, pBuf);
                        }

                        /* Read data */
                        retCode = i2cGetChar(I2C_MODE_READ, pBuf);
                        while (retCode == I2C_CODE_EMPTY)
                        {
                            retCode = i2cGetChar(I2C_MODE_READ, pBuf);
                        }
                        pBuf++;

                        break;
                    }
                    else if (status != 0xf8)
                    {
                        /* ERROR */
                        i = len;
                        retCode = I2C_CODE_ERROR;
                        break;
                    }
                }
            }
        }
    }

    i2cStop();
    return retCode;
}

tS8 measureTemperature(tU8 addr,
                       tU8 *pBuf,
                       tU16 len)
{
    tU8 retCode;
    retCode = i2cStart();
    
    if (retCode == I2C_CODE_OK) {
        retCode = i2cWrite(addr, 0x00, 1);
        if (retCode == I2C_CODE_OK) {
            mdelay(300);
            retCode = i2cRead(addr, pBuf, len);
        }
    }
    return retCode;
}

tU8 calculateTemperatureValue(tU8 *byteArray)
{
    tS16 intValue = 0;
    tU16 readValue = ((byteArray[0] << 8) | byteArray[1]);
    tBool isNegative = (readValue & 0x8000);
    if (isNegative)
    {
        intValue = -1;
    }
    for (tU8 i = 1; i < 9; i++)
    {
        intValue = intValue * 2 + ((readValue << i) & 0x8000);
    }
    return (intValue / 2.0);
}