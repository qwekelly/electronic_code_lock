#include "stdlib.h"
#include "AT24CXX.h"
#include "usart.h"
#include "delay.h"
#include "stm32f10x.h"
#include "stm32f10x_i2c.h"


/**
  * @brief  I2C_Configuration����ʼ��Ӳ��IIC����
  * @param  ��
  * @retval ��
  */
void I2C1_Configuration(void)
{
	I2C_InitTypeDef  I2C_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure; 

	/*I2C1����ʱ��ʹ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	
	/*I2C1����GPIOʱ��ʹ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	 /* I2C_SCL��I2C_SDA*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	       // ��©���
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	
	/* I2C ���� */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;	
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;	/* �ߵ�ƽ�����ȶ����͵�ƽ���ݱ仯 SCL ʱ���ߵ�ռ�ձ� */
  //I2C_InitStructure.I2C_OwnAddress1 =OLED_ADDRESS;    //������I2C��ַ
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;	
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;	/* I2C��Ѱַģʽ */
  I2C_InitStructure.I2C_ClockSpeed = 100000;	                            /* ͨ������ */
  
  I2C_Init(I2C1, &I2C_InitStructure);	                                      /* I2C1 ��ʼ�� */
	I2C_Cmd(I2C1, ENABLE);  	                                                /* ʹ�� I2C1 */
	
}

// ���Ϳ�ʼ�źźʹӻ���ַ(����ģʽ)
void start(void)
{
	if (I2C1->SR1 || I2C1->SR2)
		printf("error!\n");
	
restart:
	I2C_GenerateSTART(I2C1, ENABLE);
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR); // �ȴ���ʼ�źŷ������
	I2C_SendData(I2C1, 0xa0); // �ӻ���ַ(����ģʽ)
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR) // �ȴ��ӻ�ȷ��
	{
		if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET)
		{
			// ���ӻ�δ��Ӧ, ������
			// ִ����д��������Ҫ�ȴ�һ��ʱ�����ִ����������
			I2C_ClearFlag(I2C1, I2C_FLAG_AF);
			//printf("NACK!\n");
			goto restart;
		}
	}
}
 
uint8_t READ_AT24C02(uint8_t addr)
{
	start();
	I2C_SendData(I2C1, addr); // ���ʹ洢����ַ
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
	
	I2C_GenerateSTART(I2C1, ENABLE); // RESTART
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	I2C_SendData(I2C1, 0xa1); // �ӻ���ַ(����ģʽ)
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR);
	
	I2C_GenerateSTOP(I2C1,ENABLE); // �������һ�ֽ�����ǰ��׼����STOP�ź�
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR); // �ȴ����ݽ������
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET); // �ȴ�ֹͣ�źŷ������
	return I2C_ReceiveData(I2C1);
}
 
void WRITE_AT24C02(uint8_t addr, uint8_t value)
{
	start();
	I2C_SendData(I2C1, addr); // ǰ�������ݿ�����, ����ȴ�TXE��λ
	I2C_SendData(I2C1, value);
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR); // �ȴ�������ȫ�������
	I2C_GenerateSTOP(I2C1,ENABLE);
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET); // �ȴ�ֹͣ�źŷ������
}
 
void read_more(uint8_t addr, char *data, uint8_t len)
{
	start();
	I2C_SendData(I2C1, addr);
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
	
	I2C_GenerateSTART(I2C1, ENABLE);
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	I2C_SendData(I2C1, 0xa1);
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR);
	
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	while (len--)
	{
		if (len == 0)
		{
			I2C_AcknowledgeConfig(I2C1, DISABLE);
			I2C_GenerateSTOP(I2C1,ENABLE); // �������һ�ֽ�����ǰ��׼����STOP�ź�
		}
		while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR); // �ȴ���ǰ���ݽ������
		*data++ = I2C_ReceiveData(I2C1);
	}
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET); // �ȴ�ֹͣ�źŷ������
}
 
void write_more(uint8_t addr, const char *data, uint8_t len) 
{
	start();
	I2C_SendData(I2C1, addr);
	while (len--)
	{
		I2C_SendData(I2C1, *data++);
		while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR); // �ȴ�TXE
	}
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR); // �ȴ�ȫ�����ݷ������
	I2C_GenerateSTOP(I2C1,ENABLE);
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET); // �ȴ�ֹͣ�źŷ������
}



