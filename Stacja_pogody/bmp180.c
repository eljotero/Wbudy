/*
    Plik, zawierajcy kod źródłowy funkcji przeznaczonych
    do obsługi czujnika ciśnienia BMP180.
*/

#include "bmp180.h"

#define bmp180Address       0x77 // 0x77 = 0b 0111 0111
#define bmp180ReadAddress   0xEE // 0xEE = 0b 1110 1110
#define bmp180WriteAddress  0xEF // 0xEF = 0b 1110 1111

/*
 * @brief   Funkcja calculatePressure() wykorzystywana jest do przeliczenia wartości ciśnienia, odczytanej z czujnika
 *          ciśnienia BMP180 w oparciu o podany w dokumentacji algorytm przeliczający. W tym celu wykorzystywane są 
 *          również wartości korygujące, odczytane z pamięci EEPROM urządzenia, a także wartość pomiaru temperatury.
 *           
 * @param   readPress
 *          Nieprzeliczona wartość ciśnienia, odczytana z czujnika BMP180.
 * @param   readTemp
 *          Nieprzeliczona wartość tempertury, odczytana z czujnika BMP180.
 * @param   calibrationArray
 *          Wskaźnik do jedenastoelementowej tablicy, zawierajacej wartości korygujące, potrzebne do przeliczenia
 *          wartości ciśnienia.
 * @param   pressureOss
 *          Jest to wartość oznaczająca liczbę próbek, które wykonuje czujnik przy pomiarze ciśnienia, w wyniku czego 
 *          możliwe jest osiągnięcie większej dokładności pomiaru.
 * @returns
 *          Przeliczona wartość ciśnienia, która wyrażona jest w paskalach (Pa).
 * 
 * @side effects: 
 *          Brak
 */

tS32 calculatePressure(tU32 readPress, tU16 readTemp, tU16 *calibrationArray, tU8 pressureOss)
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

    tS32 X1 = (((readTemp - AC6) * AC5) >> 15);
    tS32 X2 = ((MC << 11) / (X1 + MD));
    tS32 B5 = X1 + X2;
    tS32 T = ((B5 + 8) >> 4);

    // Calculations for pressure.

    tS32 B6 = B5 - 4000;
    X1 = (((tS32)B2 * ((B6 * B6) >> 12)) >> 11);
    X2 = (((tS32)AC2 * B6) >> 11);
    tS32 X3 = X1 + X2;

    tS32 B3 = ((((tS32)AC1 * 4 + X3) << pressureOss) + 2) >> 2;
    X1 = (tS32)AC3 * B6 >> 13;
    X2 = (((tS32)B1 * (B6 * B6) >> 12)) >> 16;
    X3 = (((X1 + X2) + 2) >> 2);

    tU32 B4 = (((tS32)AC4 * (tU32)(X3 + 32768)) >> 15);
    tU32 B7 = (((tU32)readPress - B3) * (tS32)(50000 >> (tU64)pressureOss));

    tS32 p;
    if (B7 < 0x80000000UL)
    {
        p = ((B7 * 2) / B4);
    }
    else
    {
        p = ((B7 / B4) * 2);
    }

    X1 = (p >> 8) * (p >> 8);
    X1 = ((X1 * 3038) >> 16);
    X2 = ((-7357 * p) >> 16);

    p = (p + (X1 + X2 + 3791)) >> 4;

    return p;
}

/*
 * @brief   Funkcja measurePressure() wykorzystywana jest do odczytania z czujnika ciśnienia BMP180 wartości
 *          ciśnienia. Bazowo wartość ta jest nieprzeliczona, lecz w wyniku przekazania jej do funkcji calculatePressure
 *          jest ona przeliczana na wartość wyrażoną w paskalach (Pa), która jest następnie zwracana przez tą funkcję.
 *           
 * @param   void
 *          
 * @returns
 *          Przeliczona wartość ciśnienia, wyrażona w paskalach (Pa).
 * 
 * @side effects: 
 *          Brak
 */

tS32 measurePressure(void)
{
    // Return code for I2C operations.
    tS8 retCode = 0;

    // Array of bytes used for reading compensation values.
    tU8 registerContents[3] = {0};

    // Array holding calibration values
    tU16 calibrationArray[11] = {0};

    // Address of register, where calibration data starts.
    tU8 staringRegister = (tU8)0xAA; // From 0xAA to 0xBF

    // Reading calibration value for all coefficients
    tU8 iterator;
    // Making device "aware" we are going to read compenstaion data.
    retCode = i2cWrite(bmp180WriteAddress, staringRegister, 1);
    for (iterator = 0; iterator < (tU8)11; iterator = iterator + (tU8)1)
    {
        retCode = i2cRead(bmp180ReadAddress, registerContents, 2);
        calibrationArray[iterator] = ((registerContents[0] << 8) | registerContents[1]);
        // We increment by 2 starting register to be able to read next calibration value.
        staringRegister = staringRegister + (tU8)2;
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
    retCode = i2cWrite(bmp180WriteAddress, commandArr, 2);
    // Waiting for 5 ms to read temperature ; Minimum waiting time - 4.5 ms.
    mdelay(5);
    // I2C read to retrieve temperature value from slave.
    retCode = i2cWrite(bmp180WriteAddress, resultLocationAddress, 1);
    retCode = i2cRead(bmp180ReadAddress, registerContents, 2);
    tU16 readTemp = ((registerContents[0] << 8) | (registerContents[1]));

    // Command for getting pressure value with oversampling setting equal to 1.
    commandArr[0] = registerAddress;
    commandArr[1] = controlRegisterPressureValue;

    // I2C write to BMP180 for reading temperature value.
    retCode = i2cWrite(bmp180WriteAddress, commandArr, 2);
    // Waiting for 26 ms to read pressure ; Minimum waiting time = 25.5 ms
    mdelay(26);
    // I2C read to retrieve presssure value from slave.
    retCode = i2cWrite(bmp180WriteAddress, resultLocationAddress, 1);
    retCode = i2cRead(bmp180ReadAddress, registerContents, 3);

    tU32 readPress = ((registerContents[0] << 16) | (registerContents[1] << 8) | (registerContents[2]));
    readPress = (readPress >> ((tU32)8 - (tU32)pressureOss));

    tS32 calculatedPressure = calculatePressure(readPress, readTemp, calibrationArray, pressureOss);

    return calculatedPressure;
}
