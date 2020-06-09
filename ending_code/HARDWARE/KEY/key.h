#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

#define  PortB   (GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12)

#define KEY0  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)//��ȡ����0
#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)//��ȡ����1
#define KEY2  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)//��ȡ����2  
#define KEY3  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)//��ȡ����0
#define KEY4  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)//��ȡ����1
#define KEY5  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)//��ȡ����2 
#define KEY6  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)//��ȡ����0
#define KEY7  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)//��ȡ����1

 

#define KEY0_PRES 1	//KEY0����
#define KEY1_PRES	2	//KEY1����
#define KEY2_PRES	3	//KEY2����
#define KEY3_PRES 4	//KEY3����
#define KEY4_PRES	5	//KEY4����
#define KEY5_PRES	6	//KEY5����
#define KEY6_PRES 7	//KEY6����
#define KEY7_PRES	8	//KEY7����

 


void KEY_Init(void);//IO��ʼ��
u8 keyscan(void);  	//����ɨ�躯��			
uint8_t key_do(void);
#endif
