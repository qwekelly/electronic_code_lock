//#ifndef __I2C_H
//#define __I2C_H
//#include "stm32f10x.h"

//#define I2C1_SCL(x)      (x ?  GPIO_SetBits(GPIOB,GPIO_PIN_6) : GPIO_ResetBits(GPIOB,GPIO_PIN_6) )       
//#define I2C1_SDA(x)      (x ?  GPIO_SetBits(GPIOB,GPIO_PIN_7) : GPIO_ResetBits(GPIOB,GPIO_PIN_7) )       
//#define RD_I2C1_SDA      GPIO_ReadInputDataBit(GPIOB,GPIO_PIN_7)   

//#define I2C1_SDA_OUT()   {GPIOB->CRL &= 0x0FFFFFFF;GPIOB->CRL |= 0x30000000;} 
//#define I2C1_SDA_IN()    {GPIOB->CRL &= 0x0FFFFFFF;GPIOB->CRL |= 0x40000000;}
//   

////------------------------------------------------------------------------------
////------------------------------------------------------------------------------
////º¯ÊýÉùÃ÷
//void I2C1_Init(void);
//void I2C1_DELAY(uint16_t del);
//void I2C1_START(void);
//void I2C1_STOP(void);
//uint8_t I2C1_WAIT_ACK(void);
//void I2C1_ACK(void);
//void I2C1_NOT_ACK(void);
//uint8_t I2C1_WRITE_BYTE(uint8_t dat);
//uint8_t I2C1_READ_BYTE(uint8_t ack);

//#endif


