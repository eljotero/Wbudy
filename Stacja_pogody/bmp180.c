/*
    Plik, zawierajcy kod źródłowy funkcji przeznaczonych
    do obsługi czujnika ciśnienia BMP180.
*/

#include "general.h"

tU16 measurePressure()
{
    // BMP180 read address
    tU8 bmpReadAddress = ((0x77 << 1) | 1);
    // BMP180 write address
    tU8 bmpWriteAddress = (0x77 << 1);

    // Compensation data read from BMP180 EEPROM.

    tU16 compensationValuesArray[11] = {};
    tU8 registerContents[2] = {};
    tU8 staringRegister = 0xAA;                                                                     
    tU8 i;
    for (i = 0; i < 11; i++)
    {
        i2cRead(staringRegister, registerContents, 2);
        compensationValuesArray[i] = ((registerContents[0] << 8) | registerContents[1]);
        staringRegister += 2;
    }

    // Temperature register addresses
    tU8 tempRegVal = 0x2E;

    // Control register value
    tU8 registerAddress = 0xF4;

    // Value for getting pressure register addresses
    tU8 pressureOss = 0x01;

    // Value location
    tU8 valueLocation = 0xF6;

    tU8 commandArr[2] = {};

    // Command for getting temperature value
    commandArr[0] = registerAddress;
    commandArr[1] = tempRegVal;

    // I2C write to BMP180 for reading temperature value.
    i2cWrite(bmpWriteAddress, commandArr, 2);
    // Waiting for 5 ms to read temperature.
    mdelay(5);
    // I2C read to retrieve temperature value from slave.
    i2cWrite(bmpWriteAddress, valueLocation, 1);
    i2cRead(bmpReadAddress, registerContents, 2);
    tU16 readTemp = ((registerContents[0] << 8) | (registerContents[1] & 0xFF));

    // Getting value to write to control register
    tU8 pressRegVal = (0x34 + (pressureOss << 6));

    // Command for getting pressure value with oss = 1.
    commandArr[0] = registerAddress;
    commandArr[1] = pressureOss;

    // I2C write to BMP180 for reading temperature value.
    i2cWrite(bmpWriteAddress, commandArr, 2);
    // Waiting for 5 ms to read temperature.
    mdelay(8); // Minimum waiting time = 7.5 ms
    // I2C read to retrieve temperature value from slave.
    i2cWrite(bmpWriteAddress, valueLocation, 1);
    i2cRead(bmpReadAddress, registerContents, 2);
    tU16 readPress = ((registerContents[0] << 8) | (registerContents[1] & 0xFF));

    return calculatePressure(readPress, readTemp, compensationValuesArray, 1);    
}

tU16 calculatePressure(tU16 readPress, tU16 readTemp, tU16 *compensationData, tU8 pressureOss)
{

    // Coś na pewno jest tu źle.

    // Calculations for temperature.

    tU16 X1 = ((readTemp - (*(compensationData + 5))) * (*(compensationData + 4))) / 32768;
    tU16 X2 = ((*(compensationData + 9)) * 2048) / (X1 + (*(compensationData + 10)));
    tU16 B5 = X1 + X2;
    tU16 T = (B5 + 8) / 16;                 

    // Calculations for pressure.

    tU16 B6 = B5 - 4000;
    X1 = ((*(compensationData + 7)) * ((B6 * B6) / 4096)) / 2048;
    X2 = ((*(compensationData + 1)) * B6) / 2048;

    tU16 X3 = X1 + X2;
    tU16 B3 = (((((*compensationData) * 4) + X3) << 1) + 2) / 4;
    X1 = ((*(compensationData + 2)) * B6) / 8192;
    X2 = ((*(compensationData + 6)) * ((B6 * B6) / 4096)) / 65536;
    X3 = ((X1 + X2) + 2) / 4;

    tU16 B4 = ((*(compensationData + 3)) * ((tU64)X3 + 32768)) / 32768;
    tU16 B7 = ((tU64)readPress - B3) * (50000 >> pressureOss);

    tU16 p;
    if (B7 < 0x80000000)
    {
        p = (B7 * 2) / B4;
    }
    else
    {
        p = (B7 / B4) * 2;
    }

    X1 = (p / 256) * (p / 256);
    X1 = (X1 * 3038) / 65536;
    X2 = (-7357 * p) / 65536;

    p = p + (X1 + X2 + 3791) / 16;

    return p;
}