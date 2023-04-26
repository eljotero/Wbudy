/*************************************************************************************
 *
 * @Description:
 * Przykład definiowania uniwersalnych typów.
 * 
 * @Authors: Michał Morawski, 
 *           Daniel Arendt, 
 *           Przemysław Ignaciuk,
 *           Marcin Kwapisz
 *
 * @Change log:
 *           2016.12.01: Wersja oryginalna.
 *
 ******************************************************************************/
#ifndef GENERAL_H
#define GENERAL_H

#ifndef NULL
// Wymagana zmiana do MISRA check
#define NULL (0)
#endif


typedef unsigned char             tU8;
typedef unsigned short            tU16;
typedef unsigned int              tU32;
typedef unsigned long long        tU64;
typedef signed char               tS8;
typedef signed short              tS16;
typedef signed int                tS32;
typedef signed long long          tS64;
typedef enum{FALSE = 0, 
             TRUE  = !FALSE}      tBool;
#define BIT(n)                   (1<<(n))
// Wymagana zmiana do MISRA check
#define PIN(port,bit)            ((IOPIN##port & BIT(bit)) != 0)

#endif  //GENERAL_H

