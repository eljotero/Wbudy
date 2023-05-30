/******************************************************************************
 *  Obsługa pomiaru jasności przy pomocy jasnościomierza TSL2561.
 *
 *  Plik z kodem źródłowym funkcji.
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/

#include "tsl2561.h"

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/

#define LUX_SCALE 14  // scale by 2 ^ 14
#define RATIO_SCALE 9 // scale ratio by 2 ^ 9

#define CH_SCALE        10      // scale channel values by 2 ^ 10
#define CHSCALE_TINT0   0x7517  // 322 / 11 * 2 ^ CH_SCALE
#define CHSCALE_TINT1   0x0FE7  // 322 / 81 * 2 ^ CH_SCALE

#define tsl2561AddressGND 0x29      // 0x29 = 0b 0010 1001
#define tsl2561ReadAddressGND 0x53  // 0x53 = 0b 0101 0011
#define tsl2561WriteAddressGND 0x52 // 0x52 = 0b 0101 0010

#define tsl2561AddressFloat 0x39      // 0x39 = 0b 0011 1001
#define tsl2561ReadAddressFloat 0x73  // 0x73 = 0b 0111 0011
#define tsl2561WriteAddressFloat 0x72 // 0x72 = 0b 0111 0010

#define tsl2561AddressVDD 0x49      // 0x39 = 0b 0100 1001
#define tsl2561ReadAddressVDD 0x93  // 0x79 = 0b 1001 0011
#define tsl2561WriteAddressVDD 0x92 // 0x78 = 0b 1001 0010

#define HIGH_GAIN   0x01
#define LOW_GAIN    0x00

#define LOW_INTEG_TIME  0x00
#define MED_INTEG_TIME  0x01
#define HIGH_INTEG_TIME 0x02

// Constants for T, FN and CL package

#define K1T 0x0040 // 0.1250 * 2 ^ RATIO_SCALE
#define B1T 0x01f2 // 0.0304 * 2 ^ LUX_SCALE
#define M1T 0x01be // 0.0272 * 2 ^ LUX_SCALE

#define K2T 0x0080 // 0.2500 * 2 ^ RATIO_SCALE
#define B2T 0x0214 // 0.0325 * 2 ^ LUX_SCALE
#define M2T 0x02d1 // 0.0440 * 2 ^ LUX_SCALE

#define K3T 0x00c0 // 0.3750 * 2 ^ RATIO_SCALE
#define B3T 0x023f // 0.0351 * 2 ^ LUX_SCALE
#define M3T 0x037b // 0.0544 * 2 ^ LUX_SCALE

#define K4T 0x0100 // 0.5000 * 2 ^ RATIO_SCALE
#define B4T 0x0270 // 0.0381 * 2 ^ LUX_SCALE
#define M4T 0x03fe // 0.0624 * 2 ^ LUX_SCALE

#define K5T 0x0138 // 0.6100 * 2 ^ RATIO_SCALE
#define B5T 0x016f // 0.0224 * 2 ^ LUX_SCALE
#define M5T 0x01fc // 0.0310 * 2 ^ LUX_SCALE

#define K6T 0x019a // 0.8000 * 2 ^ RATIO_SCALE
#define B6T 0x00d2 // 0.0128 * 2 ^ LUX_SCALE
#define M6T 0x00fb // 0.0153 * 2 ^ LUX_SCALE

#define K7T 0x029a // 1.30000 * 2 ^ RATIO_SCALE
#define B7T 0x0018 // 0.00146 * 2 ^ LUX_SCALE
#define M7T 0x0012 // 0.00112 * 2 ^ LUX_SCALE

#define K8T 0x029a // 1.300 * 2 ^ RATIO_SCALE
#define B8T 0x0000 // 0.000 * 2 ^ LUX_SCALE
#define M8T 0x0000 // 0.000 * 2 ^ LUX_SCALE

// Constants for CS package

#define K1C 0x0043 // 0.1300 * 2 ^ RATIO_SCALE
#define B1C 0x0204 // 0.0315 * 2 ^ LUX_SCALE
#define M1C 0x01ad // 0.0262 * 2 ^ LUX_SCALE

#define K2C 0x0085 // 0.2600 * 2 ^ RATIO_SCALE
#define B2C 0x0228 // 0.0337 * 2 ^ LUX_SCALE
#define M2C 0x02c1 // 0.0430 * 2 ^ LUX_SCALE

#define K3C 0x00c8 // 0.3900 * 2 ^ RATIO_SCALE
#define B3C 0x0253 // 0.0363 * 2 ^ LUX_SCALE
#define M3C 0x0363 // 0.0529 * 2 ^ LUX_SCALE

#define K4C 0x010a // 0.5200 * 2 ^ RATIO_SCALE
#define B4C 0x0282 // 0.0392 * 2 ^ LUX_SCALE
#define M4C 0x03df // 0.0605 * 2 ^ LUX_SCALE

#define K5C 0x014d // 0.6500 * 2 ^ RATIO_SCALE
#define B5C 0x0177 // 0.0229 * 2 ^ LUX_SCALE
#define M5C 0x01dd // 0.0291 * 2 ^ LUX_SCALE

#define K6C 0x019a // 0.8000 * 2 ^ RATIO_SCALE
#define B6C 0x0101 // 0.0157 * 2 ^ LUX_SCALE
#define M6C 0x0127 // 0.0180 * 2 ^ LUX_SCALE

#define K7C 0x029a // 1.30000 * 2 ^ RATIO_SCALE
#define B7C 0x0037 // 0.00338 * 2 ^ LUX_SCALE
#define M7C 0x002b // 0.00260 * 2 ^ LUX_SCALE

#define K8C 0x029a // 1.3000 * 2 ^ RATIO_SCALE
#define B8C 0x0000 // 0.0000 * 2 ^ LUX_SCALE
#define M8C 0x0000 // 0.0000 * 2 ^ LUX_SCALE

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
 * @brief   Funkcja calculateBrightness() wykorzystywana jest do przeliczenia wartości
 *          z kanałów 0. oraz 1. na prawidłową wartość jasności wyrażoną w lux-ach.
 *
 * @param   channel0
 *          Wartość odczytana z pary rejestrów po adresami 0xCh i 0xDh z urządzenia TSL2561
 *          reprezentująca 16 - bitową wartość (2 rejestry po 8 bitów). Bajtem młodzszym jest
 *          bajt z rejestru 0xCh, a starszym - z rejestru 0xDh.
 * @param   channel1
 *          Wartość odczytana z pary rejestrów po adresami 0xEh i 0xFh z urządzenia TSL2561
 *          reprezentująca 16 - bitową wartość (2 rejestry po 8 bitów). Bajtem młodzszym jest
 *          bajt z rejestru 0xDh, a starszym - z rejestru 0xFh.
 * @returns
 *          Przeliczona, 64 - bitowa wartość jasności, wyrażona w lux-ach.
 *
 * @side effects:
 *          Brak
 */

tU64 calculateBrightness(tU16 channel0, tU16 channel1, tU8 integTime, tU8 gainValue)
{
    tU64 chScale = 0;

    if (integTime == (tU8)LOW_INTEG_TIME)
    {
        // 13.7 ms integration time
        chScale = CHSCALE_TINT0;
    }
    else if (integTime == (tU8)MED_INTEG_TIME)
    {
        // 101 ms integration time
        chScale = CHSCALE_TINT1;
    }
    else
    {
        // Assume no scaling
        chScale = ((tU64)1 << CH_SCALE);
    }

    if (gainValue != (tU8)0x01)
    {
        chScale = chScale << 4;
    }

    // Scale the channel values
    tU64 ch0 = ((tU64)channel0 * chScale) >> CH_SCALE;
    tU64 ch1 = ((tU64)channel1 * chScale) >> CH_SCALE;

    // Find the ratio of the channel values (ch1 / ch0)
    // Protect against divide by zero
    tU64 channelsRatio = 0;
    if (channel0 != (tU16)0)
    {
        channelsRatio = (ch1 << ((tU64)RATIO_SCALE + (tU64)1)) / ch0;
    }

    // Round the ratio value
    tU64 roundedRatio = (channelsRatio + (tU64)1) >> 1;
    tU32 bValue = 0;
    tU32 mValue = 0;

    // Case for T, FN and CL package.
    /*
    if ((roundedRatio >= (tU64)0) && (roundedRatio <= (tU64)K1T))
    {
        bValue = B1T;
        mValue = M1T;
    }
    else if (roundedRatio <= (tU64)K2T)
    {
        bValue = B2T;
        mValue = M2T;
    }
    else if (roundedRatio <= (tU64)K3T)
    {
        bValue = B3T;
        mValue = M3T;
    }
    else if (roundedRatio <= (tU64)K4T)
    {
        bValue = B4T;
        mValue = M4T;
    }
    else if (roundedRatio <= (tU64)K5T)
    {
        bValue = B5T;
        mValue = M5T;
    }
    else if (roundedRatio <= (tU64)K6T)
    {
        bValue = B6T;
        mValue = M6T;
    }
    else if (roundedRatio <= (tU64)K7T)
    {
        bValue = B7T;
        mValue = M7T;
    }
    else if (roundedRatio > (tU64)K8T)
    {
        bValue = B8T;
        mValue = M8T;
    }
    else
    {
        ;
    }
    */

    // Case for CS package
    if ((roundedRatio >= (tU64)0) && (roundedRatio <= (tU64)K1C))
    {
        bValue = B1C;
        mValue = M1C;
    }
    else if (roundedRatio <= (tU64)K2C)
    {
        bValue = B2C;
        mValue = M2C;
    }
    else if (roundedRatio <= (tU64)K3C)
    {
        bValue = B3C;
        mValue = M3C;
    }
    else if (roundedRatio <= (tU64)K4C)
    {
        bValue = B4C;
        mValue = M4C;
    }
    else if (roundedRatio <= (tU64)K5C)
    {
        bValue = B5C;
        mValue = M5C;
    }
    else if (roundedRatio <= (tU64)K6C)
    {
        bValue = B6C;
        mValue = M6C;
    }
    else if (roundedRatio <= (tU64)K7C)
    {
        bValue = B7C;
        mValue = M7C;
    }
    else
    {
        ;
    }

    tU64 temporary;
    temporary = ((ch0 * (tU64)bValue) - (ch1 * (tU64)mValue));

    // Do not allow negative lux value
    if (temporary < (tU64)0)
    {
        temporary = 0;
    }

    // Round LSB (2 ^ (LUX_SCALE − 1))
    temporary = temporary + ((tU64)1 << ((tU64)LUX_SCALE - (tU64)1));

    // Strip off fractional portion
    tU64 resultInLux = temporary >> LUX_SCALE;

    return resultInLux;
}

/*
 * @brief   Funkcja measureBrightness() służy do odczytania wartości jasności z czujnika jasności TSL2561
 *          co sprowadza się do odczytania wartości z obu kanałów urządzenia, a następnie wywołując funkcję
 *          calculateBrightness(), ta funkcja otrzmuje prawidłową wartość jasności, która jest przez nią zwracana.
 *
 * @param   void
 *
 * @returns
 *          Wartość jasności obliczona przez funkcję calculateBrightness().
 *
 * @side effects:
 *          Zmiana stanu czujnika TSL2561 (poprzez jego włączanie i wyłączanie), a także zapis 
 *          adresu rejestrów, z których czytana jest zawartość.
 */

tU64 measureBrightness(void)
{
    // Return code value for I2C operations.
    tS8 retCode = 0;
    tU8 command[2] = {0};
    tU8 readValue[1] = {0};

    // Variables for each channel value.
    tU16 channel0 = 0;
    tU16 channel1 = 0;

    // Sending a command to the device to power it up.
    command[0] = 0x80;      // Change target register to control register (0x80 = 0b 1000 0000)
    command[1] = 0x03;      // Write 0x03 (so power up) the device
    retCode = i2cWrite(tsl2561WriteAddressFloat, command, 2);

    // Debug:
    // retCode = i2cRead(tsl2561ReadAddressFloat, readValue, 1);
    // if (readValue[0] == 0x03) {
    //    lcdPuts("OK.");
    // } else {
    //    lcdPuts("Not OK.");
    // }

    // Setting gain and integration time value.

    tU8 integTime = LOW_INTEG_TIME;
    tU8 gainValue = HIGH_GAIN;

    command[0] = (((tU8)0x08 << 4) | (tU8)0x01);
    command[1] = ((gainValue << 4) | integTime); // 0x10 = 0b 0001 0000

    retCode = i2cWrite(tsl2561WriteAddressFloat, command, 2);

    if (integTime == (tU8)LOW_INTEG_TIME)
    {
        mdelay(14);  // Waiting for 14 ms, since it takes 13.7 ms for both channels to integrate
    }
    else if (integTime == (tU8)MED_INTEG_TIME)
    {
        mdelay(102); // Waiting for 102 ms, since it takes 101 ms for both channels to integrate
    }
    else
    {
        mdelay(403); // Waiting for 403 ms, since it takes 402 ms for both channels to integrate
    }

    // Command for reading low byte of 16 bit channel 0
    command[0] = (((tU8)0x08 << 4) | (tU8)0x0C);
    retCode = i2cWrite(tsl2561WriteAddressFloat, command, 1);
    retCode = i2cRead(tsl2561ReadAddressFloat, readValue, 1);
    channel0 = channel0 + (tU16)readValue[0];

    // Command for reading high byte of 16 bit channel 0
    command[0] = (((tU8)0x08 << 4) | (tU8)0x0D);
    retCode = i2cWrite(tsl2561WriteAddressFloat, command, 1);
    retCode = i2cRead(tsl2561ReadAddressFloat, readValue, 1);
    channel0 = channel0 + ((tU16)256 * (tU16)readValue[0]);

    // Command for reading low byte of 16 bit channel 1
    command[0] = (((tU8)0x08 << 4) | (tU8)0x0E);
    retCode = i2cWrite(tsl2561WriteAddressFloat, command, 1);
    retCode = i2cRead(tsl2561ReadAddressFloat, readValue, 1);
    channel1 = channel1 + (tU16)readValue[0];

    // Command for reading high byte of 16 bit channel 1
    command[0] = (((tU8)0x08 << 4) | (tU8)0x0F);
    retCode = i2cWrite(tsl2561WriteAddressFloat, command, 1);
    retCode = i2cRead(tsl2561ReadAddressFloat, readValue, 1);
    channel1 = channel1 + ((tU16)256 * (tU16)readValue[0]);

    // Sending a command to the device to power it down.
    command[0] = 0x80;      // Change target register to control register (0x80 = 0b 1000 0000)
    command[0] = 0x00;      // Write 0x00 (so power down) the device
    retCode = i2cWrite(tsl2561WriteAddressFloat, command, 2);

    // tU64 brightnessValue = calculateBrightnessFloat(channel0, channel1);
    tU64 brightnessValue = calculateBrightness(channel0, channel1, integTime, gainValue);

    return brightnessValue;
}
