/*
    Plik, zawierajcy kod źródłowy funkcji przeznaczonych
    do obsługi czujnika ciśnienia BMP180.
*/

#include "bmp180.h"

tS64 calculatePressure(tU64 readPress, tU64 readTemp, tU16 *calibrationArray, tU8 pressureOss)
{
    // Compensation data read from BMP180 EEPROM.
    tS16 AC1;
    tS16 AC2;
    tS16 AC3;
    tS16 B1;
    tS16 B2;
    tS16 MB;
    tS16 MC;
    tS16 MD;
    tU16 AC4; 
    tU16 AC5;
    tU16 AC6;

    // Getting coefficient value with correct types, according to documentation.

    AC1 = calibrationArray[0];
    AC2 = calibrationArray[1];
    AC3 = calibrationArray[2];
    AC4 = calibrationArray[3];
    AC5 = calibrationArray[4];
    AC6 = calibrationArray[5];
    B1 = calibrationArray[6];
    B2 = calibrationArray[7];
    MB = calibrationArray[8];
    MC = calibrationArray[9];
    MD = calibrationArray[10];

    // Calculations for temperature.

    tS64 X1 = ((tS64)(((tS64)readTemp - (tS64)AC6) * (tS64)AC5) / (tS64)((tU64)1 << 15));
    //tS64 X2 = ((tS64)MC * ((tS64)1 << 11)) / (X1 + (tS64)MD);
    tS64 X2 = ((tS64)MC * (tS64)((tU64)1 << 11)) / (X1 + (tS64)MD);
    tS64 B5 = X1 + X2;
    tS64 T = (B5 + 8) / (tS64)((tU64)1 << 4);

    // Calculations for pressure.

    tS64 B6 = B5 - 4000;
    X1 = (B2 * ((B6 * B6) / (tS64)((tU64)1 << 12)) / (tS64)((tU64)1 << 11));
    X2 = (AC2 * B6) / (tS64)((tU64)1 << 11);
    tS64 X3 = X1 + X2;

    tS64 B3 = (((tS64)((tU64)(((tU64)AC1 * (tU64)4) + (tU64)X3) << pressureOss) + (tS64)2) / (tS64)((tU64)1 << 2));
    X1 = ((tS64)AC3 * B6) / (tS64)((tU64)1 << 13);
    X2 = (B1 * ((B6 * B6) / (tS64)((tU64)1 << 12))) / (tS64)((tU64)1 << 16);
    X3 = ((X1 + X2) + 2) / (tS64)((tU64)1 << 2);

    tU64 B4 = ((tU64)AC4 * (tU64)((tU64)X3 + ((tU64)1 << 15))) / ((tU64)1 << 15);
    tU64 B7 = ((tU64)(readPress - (tU64)B3) * (tU64)((tU64)50000 >> (tU64)pressureOss));

    tS64 p;
    if (B7 < 0x80000000UL)
    {
        p = ((tS64)((tU64)B7 << 1) / (tS64)B4);
    }
    else
    {
        p = (tS64)((B7 / B4) << 1);
    }

    X1 = (p / (tS64)((tU64)1 << 8)) * (p / (tS64)((tU64)1 << 8));
    X1 = (X1 * 3038) / (tS64)((tU64)1 << 16);
    X2 = (-7357 * p) / (tS64)((tU64)1 << 16);

    p = (p + (tS64)((tS64)(X1 + X2 + (tS64)3791) / (tS64)((tU64)1 << 4)));

    return p;
}

tS64 measurePressure(void)
{
    // BMP180 read address: 7 bit address -> 1110111 and additional bit of value 1 for read operation.
    tU8 bmpReadAddress = (((tU8)0x77 << 1) | (tU8)1); // Address: 0xEF
    // BMP180 write address: 7 bit address -> 1110111 and additional bit of value 0 for write operation.
    tU8 bmpWriteAddress = ((tU8)0x77 << 1); // Address: 0xEE
    // Array of bytes used for reading compensation values.
    tU8 registerContents[3] = {0};

    // Array holding calibration values
    tU16 calibrationArray[11] = {0};

    // Address of register, where calibration data starts.
    tU8 staringRegister = (tU8)0xAA; // From 0xAA to 0xBF

    // Reading calibration value for all coefficients
    tU16 iterator;
    // Making device "aware" we are going to read compenstaion data.
    i2cWrite(bmpWriteAddress, staringRegister, 1);
    for (iterator = 0; iterator < (tU16)11; iterator = iterator + (tU16)1)
    {
        i2cRead(bmpReadAddress, registerContents, 2);
        calibrationArray[iterator] = ((registerContents[0] << 8) | registerContents[1]);
        // We increment by 2 starting register to be able to read next calibration value.
        staringRegister = (staringRegister + (tU8)2);
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ //

    // Temperature / pressure value location registers: 0xF6 (MSB), 0xF7 (LSB), 0xF8 (XLSB)
    tU8 resultLocationAddress[1];
    resultLocationAddress[0] = 0xF6;

    // Value for pressure oversampling setting.
    tU8 pressureOss = 0x03;

    // Control register value for measuring temperature
    tU8 controlRegisterTemperatureValue = 0x2E;

    // Control register value for measuring pressure
    tU8 controlRegisterPressureValue = ((tU8)0x34 + (pressureOss << 6));

    // Measurement control register address
    tU8 registerAddress = (tU8)0xF4;

    tU8 commandArr[2] = {0};

    // Command for getting temperature value
    commandArr[0] = registerAddress;
    commandArr[1] = controlRegisterTemperatureValue;

    // I2C write to BMP180 for reading temperature value.
    i2cWrite(bmpWriteAddress, commandArr, 2);
    // Waiting for 5 ms to read temperature ; Minimum waiting time - 4.5 ms.
    mdelay(5);
    // I2C read to retrieve temperature value from slave.
    i2cWrite(bmpWriteAddress, resultLocationAddress, 1);
    i2cRead(bmpReadAddress, registerContents, 2);
    tU64 readTemp = ((registerContents[0] << 8) | (registerContents[1]));

    // Command for getting pressure value with oversampling setting equal to 1.
    commandArr[0] = registerAddress;
    commandArr[1] = controlRegisterPressureValue;

    // I2C write to BMP180 for reading temperature value.
    i2cWrite(bmpWriteAddress, commandArr, 2);
    // Waiting for 26 ms to read pressure ; Minimum waiting time = 25.5 ms
    mdelay(26);
    // I2C read to retrieve presssure value from slave.
    i2cWrite(bmpWriteAddress, resultLocationAddress, 1);
    i2cRead(bmpReadAddress, registerContents, 3);

    tU64 readPress = ((registerContents[0] << 16) | (registerContents[1] << 8) | (registerContents[2]));
    readPress = (readPress >> ((tU64)8 - (tU64)pressureOss));

    tS64 calculatedPressure = calculatePressure(readPress, readTemp, calibrationArray, pressureOss);

    return calculatedPressure;
}
