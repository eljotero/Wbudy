#ifndef EEPROM_H
#define EEPROM_H

#include "general.h"
#include "i2c.h"

tS8 eepromWrite(tU16 addr,
                tU8* pData,
                tU16 len);

tS8 eepromPageRead(tU16 address, 
                   tU8* pBuf, 
                   tU16 len);

tS8 eepromPoll(void);

tS8 pca9532(tU8* pBuf, tU16 len, tU8* pBuf2, tU16 len2);
tS8 eepromStartRead(tU8 devAddr,
                    tU16 address);

#endif
