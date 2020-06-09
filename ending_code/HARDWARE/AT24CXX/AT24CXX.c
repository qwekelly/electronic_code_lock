#include "stdlib.h"
#include "AT24CXX.h"
#include "usart.h"
#include "delay.h"
#include "stm32f10x.h"
#include "stm32f10x_i2c.h"


/**
  * @brief  I2C_Configuration，初始化硬件IIC引脚
  * @param  无
  * @retval 无
  */
void I2C1_Configuration(void)
{
	I2C_InitTypeDef  I2C_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure; 

	/*I2C1外设时钟使能 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	
	/*I2C1外设GPIO时钟使能 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	 /* I2C_SCL、I2C_SDA*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	       // 开漏输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	
	/* I2C 配置 */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;	
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;	/* 高电平数据稳定，低电平数据变化 SCL 时钟线的占空比 */
  //I2C_InitStructure.I2C_OwnAddress1 =OLED_ADDRESS;    //主机的I2C地址
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;	
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;	/* I2C的寻址模式 */
  I2C_InitStructure.I2C_ClockSpeed = 100000;	                            /* 通信速率 */
  
  I2C_Init(I2C1, &I2C_InitStructure);	                                      /* I2C1 初始化 */
	I2C_Cmd(I2C1, ENABLE);  	                                                /* 使能 I2C1 */
	
}

// 发送开始信号和从机地址(传输模式)
void start(void)
{
	if (I2C1->SR1 || I2C1->SR2)
		printf("error!\n");
	
restart:
	I2C_GenerateSTART(I2C1, ENABLE);
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR); // 等待开始信号发送完毕
	I2C_SendData(I2C1, 0xa0); // 从机地址(传输模式)
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR) // 等待从机确认
	{
		if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET)
		{
			// 若从机未响应, 则重试
			// 执行了写操作后需要等待一段时间才能执行其他命令
			I2C_ClearFlag(I2C1, I2C_FLAG_AF);
			//printf("NACK!\n");
			goto restart;
		}
	}
}
 
uint8_t READ_AT24C02(uint8_t addr)
{
	start();
	I2C_SendData(I2C1, addr); // 发送存储器地址
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
	
	I2C_GenerateSTART(I2C1, ENABLE); // RESTART
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	I2C_SendData(I2C1, 0xa1); // 从机地址(接收模式)
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR);
	
	I2C_GenerateSTOP(I2C1,ENABLE); // 接收最后一字节数据前先准备好STOP信号
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR); // 等待数据接收完毕
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET); // 等待停止信号发送完毕
	return I2C_ReceiveData(I2C1);
}
 
void WRITE_AT24C02(uint8_t addr, uint8_t value)
{
	start();
	I2C_SendData(I2C1, addr); // 前两个数据可连发, 无需等待TXE置位
	I2C_SendData(I2C1, value);
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR); // 等待数据完全发送完毕
	I2C_GenerateSTOP(I2C1,ENABLE);
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET); // 等待停止信号发送完毕
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
			I2C_GenerateSTOP(I2C1,ENABLE); // 接收最后一字节数据前先准备好STOP信号
		}
		while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR); // 等待当前数据接收完毕
		*data++ = I2C_ReceiveData(I2C1);
	}
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET); // 等待停止信号发送完毕
}
 
void write_more(uint8_t addr, const char *data, uint8_t len) 
{
	start();
	I2C_SendData(I2C1, addr);
	while (len--)
	{
		I2C_SendData(I2C1, *data++);
		while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR); // 等待TXE
	}
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR); // 等待全部数据发送完毕
	I2C_GenerateSTOP(I2C1,ENABLE);
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET); // 等待停止信号发送完毕
}



