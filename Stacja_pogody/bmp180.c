/*
    Plik, zawierajcy kod źródłowy funkcji przeznaczonych
    do obsługi czujnika ciśnienia BMP180.
*/

#include "bmp180.h"

tS64 calculatePressure(tS16 readPress, tS32 readTemp, tU16 *calibrationArray, tU8 pressureOss)
{
    // Compensation data read from BMP180 EEPROM.
    tS16 AC1, AC2, AC3, B1, B2, MB, MC, MD;
    tU16 AC4, AC5, AC6;

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

    tS64 X1 = ((readTemp - AC6) * AC5) / (1 << 15);
    tS64 X2 = (MC * (1 << 11)) / (X1 + MD);
    tS64 B5 = X1 + X2;
    tS64 T = (B5 + 8) / (1 << 4);

    // Calculations for pressure.

    tS64 B6 = B5 - 4000;
    X1 = (B2 * ((B6 * B6) / (1 << 12)) / (1 << 11));
    X2 = (AC2 * B6) / (1 << 11);
    tS64 X3 = X1 + X2;

    tS64 B3 = (((AC1 * 4 + X3) << pressureOss) + 2) / 4;
    X1 = (AC3 * B6) / (1 << 13);
    X2 = (B1 * ((B6 * B6) / (1 << 12))) / (1 << 16);
    X3 = ((X1 + X2) + 2) / (1 << 2);

    tU64 B4 = AC4 * (tU64)(X3 + (1 << 15)) / (1 << 15);
    tU64 B7 = ((tU64)readPress - B3) * (50000 >> pressureOss);

    tS64 p;
    if (B7 < 0x80000000)
    {
        p = (B7 * 2) / B4;
    }
    else
    {
        p = (B7 / B4) * 2;
    }

    X1 = (p / (1 << 8)) * (p / (1 << 8));
    X1 = (X1 * 3038) / (1 << 16);
    X2 = (-7357 * p) / (1 << 16);

    p = p + (X1 + X2 + 3791) / (1 << 4);

    return p;
}

tS64 measurePressure()
{
    // BMP180 read address: 7 bit address -> 1110111 and additional bit of value 1 for read operation.
    tU8 bmpReadAddress = ((0x77 << 1) | 1); // Address: 0xEF
    // BMP180 write address: 7 bit address -> 1110111 and additional bit of value 0 for write operation.
    tU8 bmpWriteAddress = (0x77 << 1); // Address: 0xEE
    // Array of bytes used for reading compensation values.
    tU8 registerContents[3] = {};

    // Array holding calibration values
    tU16 calibrationArray[11] = {};

    // Address of register, where calibration data starts.
    tU8 staringRegister = 0xAA; // From 0xAA to 0xBF

    // Reading calibration value for all coefficients
    tU8 iterator;
    // Making device "aware" we are going to read compenstaion data.
    i2cWrite(bmpWriteAddress, staringRegister, 1);
    for (iterator = 0; iterator < 11; iterator++)
    {
        i2cRead(bmpReadAddress, registerContents, 2);
        calibrationArray[iterator] = ((registerContents[0] << 8) | registerContents[1]);
        // We increment by 2 starting register to be able to read next calibration value.
        staringRegister += 2;
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
    tU8 controlRegisterPressureValue = (0x34 + (pressureOss << 6));

    // Measurement control register address
    tU8 registerAddress = 0xF4;

    tU8 commandArr[2] = {};

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
    tS16 readTemp = ((registerContents[0] << 8) | (registerContents[1]));

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
    tS32 readPress = ((registerContents[0] << 8) | (registerContents[1]) | (registerContents[2] >> (8 - pressureOss)));

    tS64 calculatedPressure = calculatePressure(readPress, readTemp, calibrationArray, pressureOss);

    return calculatedPressure;
}
