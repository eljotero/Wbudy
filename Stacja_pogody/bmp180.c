/******************************************************************************
 *  Obsługa pomiaru ciśnienia przy pomocy czujnika BMP180.
 * 
 *  Plik z kodem źródłowym funkcji.
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/

#include "bmp180.h"

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/

// 7 - bit address of BMP180 device.
#define bmp180Address       0x77 // 0x77 = 0b 0111 0111

// ((bmp180Address << 1) | 1) (east significant bit is 1 for read operation).
#define bmp180ReadAddress   0xEF // 0xEE = 0b 1110 1111

// (bmp180Address << 1) (least significant bit is 0 for write operation).
#define bmp180WriteAddress  0xEE // 0xEE = 0b 1110 1110

// Defines oversampling ratio of the pressure measurement
#define pressureOss 0x03    // 0x03 = 0b 11

/*****************************************************************************
 * Global variables
 ****************************************************************************/

/*****************************************************************************
 * Local variables
 ****************************************************************************/

/*****************************************************************************
 * Local prototypes
 ****************************************************************************/

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
 * @returns
 *          Przeliczona wartość ciśnienia, która wyrażona jest w paskalach (Pa).
 * 
 * @side effects: 
 *          Brak
 */

tS32 calculatePressure(tU32 readPress, tU16 readTemp, tU16 *calibrationArray)
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

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    // Calculations for temperature.
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

    tS64 X1 = ((tS64)(((tS64)readTemp - (tS64)AC6) * (tS64)AC5) / (tS64)32768);
    tS64 X2 = ((tS64)((tS64)MC * (tS64)2048) / (tS64)(X1 + (tS64)MD));
    tS64 B5 = X1 + X2;
    tS64 T = ((tS64)(B5 + (tS64)8) / (tS64)16);

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    // Calculations for pressure.
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

    tU64 helperValue = 0;
    tS64 helperValueSigned = 0;

    tS64 B6 = B5 - 4000;
    X1 = ((tS64)((tS64)B2 * ((tS64)(B6 * B6) / (tS64)4096)) / (tS64)2048);
    X2 = ((tS64)((tS64)AC2 * B6) / (tS64)2048);
    tS64 X3 = X1 + X2;

    helperValue = ((tU64)1 << (tU64)pressureOss);
    tS64 B3 = (tS64)(((tS64)((tS64)((tS64)AC1 * (tS64)4) + X3) / (tS64)helperValue) + (tS64)2) / (tS64)4;
    X1 = (tS64)((tS64)AC3 * B6) / (tS64)8192;
    X2 = (tS64)(((tS64)B1 * (tS64)((B6 * B6) / (tS64)4096))) / (tS64)65536;
    X3 = (((X1 + X2) + (tS64)2) / (tS64)4);

    helperValueSigned = (tS64)((tS64)((tS64)((tS64)AC4 * (X3 + (tS64)32768))) / (tS64)32768); 
    tU64 B4 = (tU64)helperValueSigned;
    helperValue = ((tU64)50000 >> (tU64)pressureOss);
    helperValueSigned = (((tS64)readPress - B3) * (tS64)helperValue);
    tU64 B7 = (tU64)helperValueSigned;

    tS64 p;
    if (B7 < 0x80000000UL)
    {
        helperValue = (B7 * (tU64)2) / B4;
        p = (tS64)helperValue;
    }
    else
    {
        helperValue = (B7 / B4) * (tU64)2;
        p = (tS64)helperValue;
    }

    X1 = (tS64)(p / (tS64)256) * (tS64)(p / (tS64)256);
    X1 = ((tS64)(X1 * (tS64)3038) / (tS64)65536);
    X2 = ((tS64)((tS64)-7357 * p) / (tS64)65536);

    p = p + ((tS64)(X1 + X2 + (tS64)3791) / (tS64)16);

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
 *          Stan czujnika BMP180 ulega zmianie jako, że są do niego zapisywane wartości np. wartości próbkowania 
 *          (oversampling).
 */

tS32 measurePressure(void)
{
    // Return code for I2C operations.
    tS8 retCode = 0;

    // Array of bytes used for reading compensation values.
    tU8 registerContents[3] = {0};

    // Array holding calibration values
    tS16 calibrationArray[11] = {0};

    // Address of register, where calibration data starts.
    tU8 staringRegister = (tU8)0xAA; // From 0xAA to 0xBF

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    // Reading calibration value for all coefficients
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

    //  Definition of iterator used later in a for loop.
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

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

    // Temperature / pressure value location registers: 0xF6 (MSB), 0xF7 (LSB), 0xF8 (XLSB)
    tU8 resultLocationAddress[1];
    resultLocationAddress[0] = 0xF6;

    // Control register value for measuring temperature
    tU8 controlRegisterTemperatureValue = 0x2E;

    // Control register value for measuring pressure
    tU8 controlRegisterPressureValue = ((tU8)0x34 + (tU8)((tU8)pressureOss << (tU8)6));

    // Measurement control register address
    tU8 registerAddress = (tU8)0xF4;

    tU8 commandArr[2] = {0};

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    // Setting oversampling ratio of pressure measurement to 3. (Which means that pressure will be
    // measure 8 times before estimating final measured value).
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

    // Command for setting oversampling value to 0x03.
    commandArr[0] = registerAddress;
    commandArr[1] = (tU8)((tU8)pressureOss << (tU8)6);

    retCode = i2cWrite(bmp180WriteAddress, commandArr, 2);

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    // Getting temperature value 
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

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

    // Creating a single 16 bit unsigned variable out of read 2 bytes of data.
    tU16 readTemp = ((registerContents[0] << 8) | (registerContents[1]));

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    // Getting pressure value
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

    // Command for getting pressure value..
    commandArr[0] = registerAddress;
    commandArr[1] = controlRegisterPressureValue;

    // I2C write to BMP180 for reading temperature value.
    retCode = i2cWrite(bmp180WriteAddress, commandArr, 2);

    // Waiting for 26 ms to read pressure ; Minimum waiting time = 25.5 ms
    mdelay(26);

    // I2C read to retrieve presssure value from slave.
    retCode = i2cWrite(bmp180WriteAddress, resultLocationAddress, 1);
    retCode = i2cRead(bmp180ReadAddress, registerContents, 3);

    // Creating a single 32 bit unsigned variable out of 3 read bytes of data.
    tU32 readPress = ((registerContents[0] << 16) | (registerContents[1] << 8) | (registerContents[2]));
    readPress = (readPress >> ((tU32)8 - (tU32)pressureOss));

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    // Calculating value of real pressure
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

    tS32 calculatedPressure = calculatePressure(readPress, readTemp, calibrationArray);

    return calculatedPressure;
}
