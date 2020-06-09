#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

#define  PortB   (GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12)

#define KEY0  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)//读取按键0
#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)//读取按键1
#define KEY2  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)//读取按键2  
#define KEY3  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)//读取按键0
#define KEY4  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)//读取按键1
#define KEY5  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)//读取按键2 
#define KEY6  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)//读取按键0
#define KEY7  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)//读取按键1

 

#define KEY0_PRES 1	//KEY0按下
#define KEY1_PRES	2	//KEY1按下
#define KEY2_PRES	3	//KEY2按下
#define KEY3_PRES 4	//KEY3按下
#define KEY4_PRES	5	//KEY4按下
#define KEY5_PRES	6	//KEY5按下
#define KEY6_PRES 7	//KEY6按下
#define KEY7_PRES	8	//KEY7按下

 


void KEY_Init(void);//IO初始化
u8 keyscan(void);  	//按键扫描函数			
uint8_t key_do(void);
#endif
