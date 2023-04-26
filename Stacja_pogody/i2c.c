

/******************************************************************************
 * Includes
 *****************************************************************************/

#include "i2c.h"

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/

#define I2C_CONSET (*((volatile unsigned char *)0xE001C000U))
#define I2C_STAT (*((volatile unsigned char *)0xE001C004U))
#define I2C_DATA (*((volatile unsigned char *)0xE001C008U))
#define I2C_ADDR (*((volatile unsigned char *)0xE001C00CU))
#define I2C_SCLH (*((volatile unsigned short *)0xE001C010U))
#define I2C_SCLL (*((volatile unsigned short *)0xE001C014U))
#define I2C_CONCLR (*((volatile unsigned char *)0xE001C018U))

#define I2C_REG_CONSET 0x00000040U      /* Control Set Register         */
#define I2C_REG_CONSET_MASK 0x0000007CU /* Used bits                    */
#define I2C_REG_DATA 0x00000000U        /* Data register                */
#define I2C_REG_DATA_MASK 0x000000FFU   /* Used bits                    */
#define I2C_REG_ADDR 0x00000000U        /* Slave address register       */
#define I2C_REG_ADDR_MASK 0x000000FFU   /* Used bits                    */
#define I2C_REG_SCLH 0x00000100U        /* SCL Duty Cycle high register */
#define I2C_REG_SCLH_MASK 0x0000FFFFU   /* Used bits                    */
#define I2C_REG_SCLL 0x00000100U        /* SCL Duty Cycle low register  */
#define I2C_REG_SCLL_MASK 0x0000FFFFU   /* Used bits                    */

/******************************************************************************
 *
 * Description:
 *    Checks the I2C status.
 *
 *
 *  Returns:
 *      00h Bus error
 *      08h START condition transmitted
 *      10h Repeated START condition transmitted
 *      18h SLA + W transmitted, ACK received
 *      20h SLA + W transmitted, ACK not received
 *      28h Data byte transmitted, ACK received
 *      30h Data byte transmitted, ACK not received
 *      38h Arbitration lost
 *      40h SLA + R transmitted, ACK received
 *      48h SLA + R transmitted, ACK not received
 *      50h Data byte received in master mode, ACK transmitted
 *      58h Data byte received in master mode, ACK not transmitted
 *      60h SLA + W received, ACK transmitted
 *      68h Arbitration lost, SLA + W received, ACK transmitted
 *      70h General call address received, ACK transmitted
 *      78h Arbitration lost, general call addr received, ACK transmitted
 *      80h Data byte received with own SLA, ACK transmitted
 *      88h Data byte received with own SLA, ACK not transmitted
 *      90h Data byte received after general call, ACK transmitted
 *      98h Data byte received after general call, ACK not transmitted
 *      A0h STOP or repeated START condition received in slave mode
 *      A8h SLA + R received, ACK transmitted
 *      B0h Arbitration lost, SLA + R received, ACK transmitted
 *      B8h Data byte transmitted in slave mode, ACK received
 *      C0h Data byte transmitted in slave mode, ACK not received
 *      C8h Last byte transmitted in slave mode, ACK received
 *      F8h No relevant status information, SI=0
 *      FFh Channel error
 *
 *****************************************************************************/
tU8 i2cCheckStatus(void)
{
  tU8 status = 0;

  /* wait for I2C Status changed */
  while ((I2C_CONSET & 0x08) == 0) /* while SI == 0 */
  {
    ;
  }

  /* Read I2C State */
  status = I2C_STAT;

  /* NOTE! SI flag is not cleared here */

  return status;
}

/******************************************************************************
 *
 * Description:
 *    Reset the I2C module.
 *
 *****************************************************************************/
void i2cInit(void)
{
  PINSEL0 |= 0x50;

  /* clear flags */
  I2C_CONCLR = 0x6c;

  /* reset registers */
  I2C_SCLL = (I2C_SCLL & ~I2C_REG_SCLL_MASK) | I2C_REG_SCLL;
  I2C_SCLH = (I2C_SCLH & ~I2C_REG_SCLH_MASK) | I2C_REG_SCLH;
  I2C_ADDR = (I2C_ADDR & ~I2C_REG_ADDR_MASK) | I2C_REG_ADDR;
  I2C_CONSET = (I2C_CONSET & ~I2C_REG_CONSET_MASK) | I2C_REG_CONSET;
}

/******************************************************************************
 *
 * Description:
 *    Generates a start condition on I2C when bus is free.
 *    Master mode will also automatically be entered.
 *
 *    Note: After a stop condition, you may need a bus free time before you
 *          can generate a new start condition.
 *
 * Returns:
 *    I2C_CODE_OK or I2C status code
 *
 *****************************************************************************/
tS8 i2cStart(void)
{
  tU8 status = 0;
  tS8 retCode = 0;

  /* issue a start condition */
  I2C_CONSET |= 0x20; /* STA = 1, set start flag */

  /* wait until START transmitted */
  while (1)
  {
    status = i2cCheckStatus();

    /* start transmitted */
    if ((status == (tU8)0x08) || (status == (tU8)0x10))
    {
      retCode = I2C_CODE_OK;
      break;
    }

    /* error */
    else if (status != (tU8)0xf8)
    {
      retCode = (tS8)status;
      break;
    }

    else
    {
      /* clear SI flag */
      I2C_CONCLR = 0x08;
    }
  }

  /* clear start flag */
  I2C_CONCLR = 0x20;

  return retCode;
}

/******************************************************************************
 *
 * Description:
 *    Generates a start condition on I2C when bus is free.
 *    Master mode will also automatically be entered.
 *
 *    Note: After a stop condition, you may need a bus free time before you
 *          can generate a new start condition.
 *
 * Returns:
 *    I2C_CODE_OK or I2C status code
 *
 *****************************************************************************/
tS8 i2cRepeatStart(void)
{
  tU8 status = 0;
  tS8 retCode = 0;

  /* issue a start condition */
  I2C_CONSET |= 0x20; /* STA = 1, set start flag */
  I2C_CONCLR = 0x08;  /* clear SI flag           */

  /* wait until START transmitted */
  while (1)
  {
    status = i2cCheckStatus();

    /* start transmitted */
    if ((status == (tU8)0x08) || (status == (tU8)0x10))
    {
      retCode = I2C_CODE_OK;
      break;
    }

    /* error */
    else if (status != (tU8)0xf8)
    {
      retCode = (tS8)status;
      break;
    }

    else
    {
      /* clear SI flag */
      I2C_CONCLR = 0x08;
    }
  }

  /* clear start flag */
  I2C_CONCLR = 0x20;

  return retCode;
}

/******************************************************************************
 *
 * Description:
 *    Generates a stop condition in master mode or recovers from an error
 *    condition in slave mode.
 *
 *    Note: After this function is run, you may need a bus free time before
 *          you can generate a new start condition.
 *
 * Returns:
 *    I2C_CODE_OK
 *
 *****************************************************************************/
tS8 i2cStop(void)
{
  I2C_CONSET |= 0x10; /* STO = 1, set stop flag */
  I2C_CONCLR = 0x08;  /* clear SI flag          */

  /* wait for STOP detected (while STO = 1) */
  while ((I2C_CONSET & 0x10) == 0x10)
  {
    /* do nothing */
    ;
  }

  return I2C_CODE_OK;
}

/******************************************************************************
 *
 * Description:
 *    Sends a character on the I2C network
 *
 * Params:
 *    [in] data - the character to send
 *
 * Returns:
 *    I2C_CODE_OK   - successful
 *    I2C_CODE_BUSY - data register is not ready -> byte was not sent
 *
 *****************************************************************************/
tS8 i2cPutChar(tU8 data)
{
  tS8 retCode = 0;

  /* check if I2C Data register can be accessed */
  if ((I2C_CONSET & 0x08) != 0) /* if SI = 1 */
  {
    /* send data */
    I2C_DATA = data;
    I2C_CONCLR = 0x08; /* clear SI flag */
    retCode = I2C_CODE_OK;
  }
  else
  {
    /* data register not ready */
    retCode = I2C_CODE_BUSY;
  }

  return retCode;
}

/******************************************************************************
 *
 * Description:
 *    Read a character. I2C master mode is used.
 *    This function is also used to prepare if the master shall generate
 *    acknowledge or not acknowledge.
 *
 * Params:
 *    [in]  mode  - I2C_MODE_ACK0 Set ACK=0. Slave sends next byte
 *                  I2C_MODE_ACK1 Set ACK=1. Slave sends last byte
 *                  I2C_MODE_READ Read data from data register
 *    [out] pData - a pointer to where the data shall be saved.
 *
 * Returns:
 *    I2C_CODE_OK    - successful
 *    I2C_CODE_EMPTY - no data is available
 *
 *****************************************************************************/
tS8 i2cGetChar(tU8 mode,
               tU8 *pData)
{
  tS8 retCode = I2C_CODE_OK;

  if (mode == (tU8)I2C_MODE_ACK0)
  {
    /* the operation mode is changed from master transmit to master receive */

    /* set ACK=0 (informs slave to send next byte) */

    I2C_CONSET |= 0x04; /* AA=1          */
    I2C_CONCLR = 0x08;  /* clear SI flag */
  }
  else if (mode == (tU8)I2C_MODE_ACK1)
  {
    /* set ACK=1 (informs slave to send last byte) */
    I2C_CONCLR = 0x04;
    I2C_CONCLR = 0x08; /* clear SI flag */
  }
  else if (mode == (tU8)I2C_MODE_READ)
  {
    /* check if I2C Data register can be accessed */
    if ((I2C_CONSET & 0x08) != 0) /* SI = 1 */
    {
      /* read data  */
      *pData = (tU8)I2C_DATA;
    }
    else
    {
      /* No data available */
      retCode = I2C_CODE_EMPTY;
    }
  }

  return retCode;
}

/******************************************************************************
 *
 * Description:
 *    Sends data on the I2C network
 *
 *    Note: After this function is run, you may need a bus free time before a
 *          new data transfer can be initiated.
 *
 * Params:
 *    [in] addr  - address
 *    [in] pData - data to transmit
 *    [in] len   - number of bytes to transmit
 *
 * Returns:
 *    I2C_CODE_OK    - successful
 *    I2C_CODE_ERROR - an error occured
 *
 *****************************************************************************/
tS8 i2cWrite(tU8 addr,
             tU8 *pData,
             tU16 len)
{
  tS8 retCode = 0;
  tU8 status = 0;
  tU16 i = 0;

  /* generate Start condition */
  retCode = i2cStart();

  /* Transmit address */
  if (retCode == I2C_CODE_OK)
  {
    /* write SLA+W */
    retCode = i2cPutChar(addr);
    while (retCode == I2C_CODE_BUSY)
    {
      retCode = i2cPutChar(addr);
    }
  }

  if (retCode == I2C_CODE_OK)
  {
    /* wait until address transmitted and transmit data */
    for (i = (tU16)0; i < len; i = i + (tU16)1)
    {
      /* wait until data transmitted */

      while (TRUE == (tBool)1)
      {
        /* get new status */
        status = i2cCheckStatus();

        /*
         * SLA+W transmitted, ACK received or
         * data byte transmitted, ACK received
         */
        if ((status == (tU8)0x18) || (status == (tU8)0x28))
        {
          /* Data transmitted and ACK received */

          /* write data */
          retCode = i2cPutChar(*pData);
          while (retCode == I2C_CODE_BUSY)
          {
            retCode = i2cPutChar(*pData);
          }
          pData++;

          break;
        }

        /* no relevant status information */
        else if (status != (tU8)0xf8)
        {
          /* error */
          i = len;
          retCode = I2C_CODE_ERROR;
          break;
        }
      }
    }
  }

  /* wait until data transmitted */
  while (1)
  {
    /* get new status */
    status = i2cCheckStatus();

    /*
     * SLA+W transmitted, ACK received or
     * data byte transmitted, ACK received
     */
    if ((status == (tU8)0x18) || (status == (tU8)0x28))
    {
      /* data transmitted and ACK received */
      break;
    }

    /* no relevant status information */
    else if (status != (tU8)0xf8)
    {
      /* error */
      i = len;
      retCode = I2C_CODE_ERROR;
      break;
    }
  }

  /* generate Stop condition */
  i2cStop();

  return retCode;
}

tS8 i2cWaitTransmit(void)
{
  tU8 status = 0;

  /* wait until data transmitted */
  while (1)
  {
    /* get new status */
    status = i2cCheckStatus();

    /*
     * SLA+W transmitted, ACK received or
     * data byte transmitted, ACK received
     */
    if ((status == (tU8)0x18) || (status == (tU8)0x28))
    {
      /* data transmitted and ACK received */
      return I2C_CODE_OK;
    }

    /* no relevant status information */
    else if (status != (tU8)0xf8)
    {
      /* error */

      return I2C_CODE_ERROR;
    }
  }
}

tS8 i2cWriteWithWait(tU8 data)
{
  tS8 retCode = 0;

  retCode = i2cPutChar(data);
  while (retCode == I2C_CODE_BUSY)
  {
    retCode = i2cPutChar(data);
  }

  if (retCode == I2C_CODE_OK)
    retCode = i2cWaitTransmit();

  return retCode;
}

tS8 i2cMyWrite(tU8 addr,
               tU8 *pData,
               tU16 len)
{
  tS8 retCode = 0;
  tU8 i = 0;

  do
  {
    /* generate Start condition */
    retCode = i2cStart();
    if (retCode != I2C_CODE_OK)
      break;

    /* write address */
    retCode = i2cWriteWithWait(addr);
    if (retCode != I2C_CODE_OK)
      break;

    for (i = 0; i < len; i++)
    {
      retCode = i2cWriteWithWait(*pData);
      if (retCode != I2C_CODE_OK)
        break;

      pData++;
    }

  } while (0);

  /* generate Stop condition */
  i2cStop();

  return retCode;
}

/******************************************************************************
 *
 * Description:
 *    Read a specified number of bytes from the I2C network.
 *
 *    Note: After this function is run, you may need a bus free time before a
 *          new data transfer can be initiated.
 *
 * Params:
 *    [in] addr - address
 *    [in] pBuf - receive buffer
 *    [in] len  - number of bytes to receive
 *
 * Returns:
 *    I2C_CODE_OK or I2C status code
 *
 *****************************************************************************/
tS8 i2cRead(tU8 addr,
            tU8 *pBuf,
            tU16 len)
{
  tS8 retCode = 0;
  tU8 status = 0;
  tU16 i = 0;

  /* Generate Start condition */
  retCode = i2cStart();

  /* Transmit address */
  if (retCode == I2C_CODE_OK)
  {
    /* write SLA+R */
    retCode = i2cPutChar(addr);
    while (retCode == I2C_CODE_BUSY)
    {
      retCode = i2cPutChar(addr);
    }
  }

  if (retCode == I2C_CODE_OK)
  {
    /* wait until address transmitted and receive data */
    for (i = (tU16)1; i <= len; i = i + (tU16)1)
    {
      /* wait until data transmitted */
      while (TRUE == (tBool)1)
      {
        /* get new status */
        status = i2cCheckStatus();

        /*
         * SLA+R transmitted, ACK received or
         * SLA+R transmitted, ACK not received
         * data byte received in master mode, ACK transmitted
         */
        if ((status == (tU8)0x40) || (status == (tU8)0x48) || (status == (tU8)0x50))
        {
          /* data received */

          if (i == len)
          {
            /* Set generate NACK */
            retCode = i2cGetChar(I2C_MODE_ACK1, pBuf);
          }
          else
          {
            retCode = i2cGetChar(I2C_MODE_ACK0, pBuf);
          }

          /* Read data */
          retCode = i2cGetChar(I2C_MODE_READ, pBuf);
          while (retCode == (tS8)I2C_CODE_EMPTY)
          {
            retCode = i2cGetChar(I2C_MODE_READ, pBuf);
          }
          pBuf++;

          break;
        }

        /* no relevant status information */
        else if (status != (tU8)0xf8)
        {
          /* error */
          i = len;
          retCode = I2C_CODE_ERROR;
          break;
        }
      }
    }
  }

  /*--- Generate Stop condition ---*/
  i2cStop();

  return retCode;
}
