#ifndef __AT24CXX_H
#define __AT24CXX_H
#include "stm32f10x.h"
#include "stdlib.h"


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//º¯ÊýÉùÃ÷
void I2C1_Configuration(void);
//int fputc(int ch, FILE *fp);
void start(void);

uint8_t READ_AT24C02(uint8_t addr);
void WRITE_AT24C02(uint8_t addr, uint8_t value);
void read_more(uint8_t addr, char *data, uint8_t len);
void write_more(uint8_t addr, const char *data, uint8_t len) ;

#endif


