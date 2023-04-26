/*
    Plik z kodem źródłowym funkcji, przeznaczonych do obsługi
    czujnika oświetlenia TLS2561.
*/

#include "tsl2561.h"

tU16 calculateBrightness(tU16 channel0, tU16 channel1)
{
    // Conversion of result to result in lux, according to TLS2561 manual, page: 23
    tU16 finalResult;
    // Below we are using float value, since in MISRA set of rules, it is forbidden to cast
    // values to floats like that.
    float divisionResult = ((float)channel1 / (float)channel0);
    if ((divisionResult > 0.0) && (divisionResult <= 0.52))
    {
        finalResult = (0.0315 * channel0) - ((0.0593 * channel0) * pow((channel1 / channel0), 1.4));
    }
    else if ((divisionResult > 0.52) && (divisionResult <= 0.65))
    {
        finalResult = (0.0229 * channel0) - (0.0291 * channel1);
    }
    else if ((divisionResult > 0.65) && (divisionResult <= 0.80))
    {
        finalResult = (0.0157 * channel0) - (0.0180 * channel1);
    }
    else if ((divisionResult > 0.80) && (divisionResult <= 1.3))
    {
        finalResult = (0.00388 * channel0) - (0.00260 * channel1);
    } else 
    {
        finalResult = 0;
    }
    return finalResult;
}

tU16 measureBrightness(void)
{
    tU8 command[2] = {0};
    tU8 readValue[1] = {0};
    // tS8 returnValue = 0;

    // TSL2561 device address ; Chosen address: VDD = 0b1001001 = 0x49
    tU8 tsl2561Address = 0x49;
    // Write address for TSL2561
    tU8 tslReadAddress = ((tU8)(tsl2561Address << 1) | (tU8)1);
    // Read address for TSL2561
    tU8 tslWriteAddress = (tU8)(tsl2561Address << 1);
    tU16 channel0 = 0;
    tU16 channel1 = 0;

    // Sending a command to the device to power it up.
    command[0] = (((tU8)0x09 << 4) | (tU8)0x00);  // Change target register to control register
    command[1] = ((tU8)0x03);  // Write 0x03 (so power up) the device
    i2cWrite(tslWriteAddress, command, 2);

    mdelay(403); // Waiting for 403 ms, since it takes 402 ms for both channels to integrate

    // Command for reading low byte of 16 bit channel 0
    command[0] = (((tU8)0x09 << 4) | (tU8)0x0C);
    command[1] = (command[0] & (tU8)0);
    i2cWrite(tslWriteAddress, command, 1);
    i2cRead(tslReadAddress, readValue, 1);
    channel0 = (channel0 | (tU16)readValue[0]);

    // Command for reading high byte of 16 bit channel 0
    command[0] = (((tU8)0x09 << 4) | (tU8)0x0D);
    i2cWrite(tslWriteAddress, command, 1);
    i2cRead(tslReadAddress, readValue, 1);
    channel0 = (channel0 | ((tU16)readValue[0] << 8));

    // Command for reading low byte of 16 bit channel 1
    command[0] = (((tU8)0x09 << 4) | (tU8)0x0E);
    i2cWrite(tslWriteAddress, command, 1);
    i2cRead(tslReadAddress, readValue, 1);
    channel1 = (channel1 | (tU16)readValue[0]);

    // Command for reading high byte of 16 bit channel 1
    command[0] = (((tU8)0x09 << 4) | (tU8)0x0F);
    i2cWrite(tslWriteAddress, command, 1);
    i2cRead(tslReadAddress, readValue, 1);
    channel1 = (channel1 | ((tU16)readValue[0] << 8));

    // Sending a command to the device to power it down.
    command[0] = (((tU8)0x09 << 4) | (tU8)0x00);  // Change target register to control register
    command[1] = ((tU8)0x00);  // Wrtie 0x00 (so power down) the device
    i2cWrite(tslWriteAddress, command, 2);

    tU16 calculatedBrightness = calculateBrightness(channel0, channel1);

    return calculatedBrightness;
}