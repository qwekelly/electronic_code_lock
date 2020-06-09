#include "key.h"
#include "delay.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK Mini STM32开发板
//按键输入 驱动代码		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/3/06
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									   
//////////////////////////////////////////////////////////////////////////////////	 
 	    
//按键初始化函数 
//PA0.15和PC5 设置成输入
void KEY_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;   
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE);
   
	 /********4???*********/
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	 
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOB, &GPIO_InitStructure);
	 
	 /********4???*********/
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	 
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOA, &GPIO_InitStructure);	

} 
u8 keyscan(void)
{ 
   u8 KeyVal;
   GPIO_Write(GPIOB, (GPIOB->ODR & 0xf0ff | 0x0f00));	 //
	  
	 if((GPIOB->IDR & 0xf000)==0x0000)   								 //
		  return -1;
	 else
	 {	
	    delay_ms(5);    //??5ms???
	    if((GPIOB->IDR & 0xf000)==0x0000)
	    return -1;
	 }
	 
	 GPIO_Write(GPIOB,(GPIOB->ODR & 0xf0ff | 0x0100));	//??PB8??	 
	 switch(GPIOB->IDR & 0xf000)	//???,????,??,???
	 {
     	case 0x1000: KeyVal=0; break;
			case 0x2000: KeyVal=1;	break;
			case 0x4000: KeyVal=2;	break;
			case 0x8000: KeyVal=3;	break;
   }
	 while((GPIOB->IDR & 0xf000)	> 0);     //??????
	 
	 GPIO_Write(GPIOB,(GPIOB->ODR & 0xf0ff | 0x0200));	
		switch(GPIOB->IDR & 0xf000)		        
		{
			case 0x1000: KeyVal=4;	break;
			case 0x2000: KeyVal=5;	break;
			case 0x4000: KeyVal=6;	break;
			case 0x8000: KeyVal=7;	break;
		}
    while((GPIOB->IDR & 0xf000)	> 0);
		
	  GPIO_Write(GPIOB,(GPIOB->ODR & 0xf0ff | 0x0400));	//??PB10??
		switch(GPIOB->IDR & 0xf000)		        
		{
			case 0x1000: KeyVal=8;	break;
			case 0x2000: KeyVal=9;	break;
			case 0x4000: KeyVal=10;	break;
			case 0x8000: KeyVal=11;	break;
		}
    while((GPIOB->IDR & 0xf000)	> 0);
		
	  GPIO_Write(GPIOB,(GPIOB->ODR & 0xf0ff | 0x0800));	//??PB11??
		switch(GPIOB->IDR & 0xf000)		        
		{
			case 0x1000: KeyVal=12;	break;
			case 0x2000: KeyVal=13;	break;
			case 0x4000: KeyVal=14;	break;
			case 0x8000: KeyVal=15;	break;
		}
		while((GPIOB->IDR & 0xf000)	> 0);
		return KeyVal;
}

uint8_t key_do()
{
    int num;
	  num = keyscan();
	  switch(num)
	  { 
        case 0: printf("0\n"); break;					  				      
				case 1: printf("1\n"); break;					  				       
				case 2: printf("2\n"); break;					  				     
				case 3: printf("3\n"); break;					  				     
				case 4: printf("4\n"); break;				 	       
				case 5: printf("5\n"); break;					  				      
				case 6: printf("6\n"); break;					  				      
				case 7: printf("7\n"); break;					 			       
				case 8: printf("8\n"); break;								 		       
				case 9: printf("9\n"); break;							 				     	
			  case 10: printf("10\n"); break;						 				      		
				case 11: printf("11\n"); break;					 				      
				case 12: printf("12\n"); break;							 				      	
			  case 13: printf("13\n"); break;							 				       	
				case 14: printf("14\n"); break;					 				      
				case 15: printf("15\n"); break;					 			      
    }
}

