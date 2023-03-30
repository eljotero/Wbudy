/*
    Plik zawierający kod źródłowy, dla funkcji obsługujących
    czujnik wilgotności HTU21D(F).
*/

#include "general.h"
#include "i2c.h"
#include "additional.h"

tU8 measureHumidity1(tU8 addr,
                    tU8 *pBuf,
                    tU16 len)
{
    tU8 status;
    tU8 retCode;
    tU8 command = 0xF5;

    retCode = i2cStart();
    if (retCode == I2C_CODE_OK)
    {
        retCode = i2cPutChar(addr << 1);
        while (retCode == I2C_CODE_BUSY)
        {
            retCode = i2cPutChar(addr << 1);
        }

        if (retCode == I2C_CODE_OK)
        {
            retCode = i2cPutChar(command);
            while (retCode == I2C_CODE_BUSY)
            {
                retCode = i2cPutChar(command);
            }

            if (retCode == I2C_CODE_OK)
            {
                retCode = i2cPutChar((addr << 1) ^ 1);
                while (retCode == I2C_CODE_BUSY)
                {
                    retCode = i2cPutChar((addr << 1) ^ 1);
                }
            }

            if (retCode == I2C_CODE_OK)
            {
                /* Tu gdzieś powinno się wstrzymać, aby można odczytać prawidłowo pomiar. */

                for (tU8 i = 0; i < len; i++)
                {
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
    }

    i2cStop();
    return retCode;
}

tU8 measureHumidity(tU8 addr,
                    tU8 *pBuf,
                    tU16 len)
{
    tU8 retCode;
    tU8 command = 0xF5;
    retCode = i2cStart();

    if (retCode == I2C_CODE_OK)
    {
        retCode = i2cWrite(addr << 1, command, 1);
        if (retCode == I2C_CODE_OK)
        {
            mdelay(50);
            retCode = i2cRead((addr << 1) | 1, pBuf, 2);
        }
    }
    return retCode;
}

tU8 calculateHumidityValue(tU8 *byteArray)
{
    tU8 intValue = 0;
    tU16 firstMask = ~(1 << 1) | 1;

    tU16 readValue = ((byteArray[0] << 8) | byteArray[1]);
    readValue &= firstMask;

    for (tU8 i = 0; i < 16; i++)
    {
        intValue = intValue * 2 + ((readValue << i) & 0x8000);
    }
    return (125 * (intValue / 65536.0) - 6.0);
}