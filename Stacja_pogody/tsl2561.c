/*
    Plik z kodem źródłowym funkcji, przeznaczonych do obsługi
    czujnika oświetlenia TLS2561.
*/

#include "general.h"
#include "math.h"

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

tU16 measureBrightness()
{
    tU8 command[2] = {};
    tU8 *readValue;
    // Write address for TLS2561
    tU8 tslWriteAddress = ((0x1C << 1) | 1);
    // Read address for TLS2561
    tU8 tslReadAddress = (0x1C << 1);
    tU16 channel0, channel1;

    // Sending a command to the device to power it up.
    command[0] = ((0xA0 << 4) | 0x00);  // Change target register to control register
    command[1] = (0x03);  // Write 0x03 (so power up) the device
    i2cWrite(tslWriteAddress, command, 2);

    command[0] = ((0x09 << 4) | 0x00);  // Change target register to control register
    i2cWrite(tslWriteAddress, command, 1);
    tU8 super = 0;
    tU8 readByte[1] = {};
    i2cRead(tslReadAddress, readByte, 1);
    if (readByte[0] == 0x03) {
    	super = 1;
    }

    mdelay(403); // Waiting for 403 ms, since it takes 402 ms for both channels to integrate

    // Command for reading low byte of 16 bit channel 0
    command[0] = ((0x09 << 4) | 0x0C);
    i2cWrite(tslWriteAddress, command, 1);
    i2cRead(tslReadAddress, readValue, 1);
    channel0 = (channel0 | (*readValue));

    // Command for reading high byte of 16 bit channel 0
    command[0] = ((0x09 << 4) | 0x0D);
    i2cWrite(tslWriteAddress, command, 1);
    i2cRead(tslReadAddress, readValue, 1);
    channel0 = (channel0 | (*readValue << 8));

    // Command for reading low byte of 16 bit channel 1
    command[0] = ((0x09 << 4) | 0x0E);
    i2cWrite(tslWriteAddress, command, 1);
    i2cRead(tslReadAddress, readValue, 1);
    channel1 = (channel1 | (*readValue));

    // Command for reading high byte of 16 bit channel 1
    command[0] = ((0x09 << 4) | 0x0F);
    i2cWrite(tslWriteAddress, command, 1);
    i2cRead(tslReadAddress, readValue, 1);
    channel1 = (channel1 | (*readValue << 8));

    // Sending a command to the device to power it down.
    command[0] = ((0xA0 << 4) | 0x00);  // Change target register to control register
    command[1] = (0x00);  // Wrtie 0x00 (so power down) the device
    i2cWrite(tslWriteAddress, command, 2);

    return calculateBrightness(channel0, channel1);
}
