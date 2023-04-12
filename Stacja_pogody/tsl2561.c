/*
    Plik z kodem źródłowym funkcji, przeznaczonych do obsługi
    czujnika oświetlenia TLS2561.
*/

#include "general.h"
#include "math.h"

tU16 measureBrightness()
{
    tU8 command;
    tU8 *readValue;
    // Write address for TLS2561
    tU8 tslWriteAddress = ((0x1C << 1) | 1);
    // Read address for TLS2561
    tU8 tslReadAddress = (0x1C << 1);
    tU16 channel0, channel1;

    // Command for reading low byte of 16 bit channel 0
    tU8 command = ((0x09 << 4) | 0x0C);
    i2cWrite(tslWriteAddress, command, 1);
    i2cRead(tslReadAddress, readValue, 1);
    channel0 |= *readValue;

    // Command for reading high byte of 16 bit channel 0
    command = ((0x09 << 4) | 0x0D);
    i2cWrite(tslWriteAddress, command, 1);
    i2cRead(tslReadAddress, readValue, 1);
    channel0 = (channel0 | (*readValue << 8));

    // Command for reading low byte of 16 bit channel 1
    command = ((0x09 << 4) | 0x0E);
    i2cWrite(tslWriteAddress, command, 1);
    i2cRead(tslReadAddress, readValue, 1);
    channel1 |= *readValue;

    // Command for reading high byte of 16 bit channel 1
    command = ((0x09 << 4) | 0x0F);
    i2cWrite(tslWriteAddress, command, 1);
    i2cRead(tslReadAddress, readValue, 1);
    channel1 = (channel1 | (*readValue << 8));

    return calculateBrightness(channel0, channel1);
}

tU16 calculateBrightness(tU16 channel0, tU16 channel1)
{
    // Conversion of result to result in lux, according to TLS2561 manual, page: 23
    tU16 finalResult;
    if ((channel1 / channel0) > 0 && (channel1 / channel0) <= 0.52) 
    {
        finalResult = 0.0315 * channel0 - 0.0593 * channel0 * pow((channel1 / channel0), 1.4);
    } 
    else if ((channel1 / channel0) > 0.52 && (channel1 / channel0) <= 0.65) 
    {
        finalResult = 0.0229 * channel0 - 0.0291 * channel1;
    }
    else if((channel1 / channel0) > 0.65 && (channel1 / channel0) <= 0.80) 
    {
        finalResult = 0.0157 * channel0 - 0.0180 * channel1;
    }
    else if ((channel1 / channel0) > 0.80 && (channel1 / channel0) <= 1.3) 
    {
        finalResult = 0.00388 * channel0 - 0.00260 * channel1;
    }
    else if ((channel1 / channel0) > 1.3) {
        finalResult = 0;
    }
    return finalResult;
}