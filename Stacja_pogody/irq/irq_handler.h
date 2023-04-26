/*************************************************************************************
 *
 * @Description:
 * Przykład definiowania procedury obsługi przerwania.
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

#ifndef IRQ_HANDLER
#define IRQ_HANDLER

/******************************************************************************
 * makro __cplusplus pozwala użyć procedury obsługi przerwań w programie
 * napisanym w C++
 * Jest ono automatycznie ustawiane jeżeli plik ma rozszerzenie cpp, cc, cxx
 ******************************************************************************/

#ifdef __cplusplus  
extern "C" {
#endif
void IRQ_Test(void) __attribute__ ((interrupt("IRQ"))) ;
#ifdef __cplusplus
}
#endif


#endif //IRQ_HANDLER
