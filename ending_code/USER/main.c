#include "led.h"
#include "delay.h"

#include "usart.h"
#include "oled.h"
#include "key.h"
#include "stmflash.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_spi.h"
#include "AT24CXX.h"
#include "RC522.h"
#include "spi_driver.h"


char AT24C02_BUFF[6]={""}; // 24C02�洢����������
char CARD_ID[4]; // 24C02�洢��ic����
char NEW_KEYWORD[6]={""}; // ������
char MAKE_SURE_KEYWORD[6]={""}; // ȷ������


#define Read_Lie1 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12)
#define Read_Lie2 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)
#define Read_Lie3 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)


static u8 KEY_Scan(void);


char cmp(char keyword1[],char keyword2[]){ // �ȽϺ���
	int i;
	char flag=0;
	for(i=0;i<6;i++)
	{
		if(keyword1[i]!=keyword2[i]) {
			flag=1;
			break;
		}
	}
	return flag;
}

void new_card() { // �¿� ¼��IC��Ϣ
	int i;
	uint8_t Card_Type1[2];  
	uint8_t Card_ID[4];  // 536870942
	uint8_t Card_KEY[6] = {0xff,0xff,0xff,0xff,0xff,0xff};    //{0x11,0x11,0x11,0x11,0x11,0x11};   //����
	uint8_t Card_Data[16];
	uint8_t status;
	
	OLED_ShowCN(0, 0, 59); // ¼��IC��������
	OLED_ShowCN(16, 0, 60);
	OLED_ShowStr(32,0,"IC",2);
	OLED_ShowCN(48, 0, 57);
	OLED_ShowCN(64, 0, 58);
	OLED_ShowCN(80, 0, 3);
	OLED_ShowCN(96, 0, 4);
	while(1) {
		delay_ms(10);
		if(MI_OK==PcdRequest(0x52, Card_Type1))  //Ѱ������������ɹ�����MI_OK  ��ӡ��ο���
		{
			uint16_t cardType = (Card_Type1[0]<<8)|Card_Type1[1];//����ͬ�������� 0x0400
			printf("###########################  �¿��洢������Ϣ:\r\n");
			printf("�����ͣ�(0x%04X)\r\n",cardType);  //"Card Type(0x%04X):"
			switch(cardType){
			case 0x4400:
					printf("Mifare UltraLight\r\n");
					break;
			case 0x0400:
					printf("Mifare One(S50)\r\n");
					break;
			case 0x0200:
					printf("Mifare One(S70)\r\n");
					break;
			case 0x0800:
					printf("Mifare Pro(X)\r\n");
					break;
			case 0x4403:
					printf("Mifare DESFire\r\n");
					break;
			default:
					printf("Unknown Card\r\n");
					break;
			}
			
			//printf("###########################:%d%d%d%d\r\n",Card_ID[0],Card_ID[1],Card_ID[2],Card_ID[3]);
			status = PcdAnticoll(Card_ID);//����ײ ����ɹ�����MI_OK
			if(status != MI_OK){
					printf("Anticoll Error\r\n");
			} else {
					printf("Serial Number:%02X%02X%02X%02X\r\n",Card_ID[0],Card_ID[1],Card_ID[2],Card_ID[3]);
			}
			printf("**************:%d%d%d%d\r\n",Card_ID[0],Card_ID[1],Card_ID[2],Card_ID[3]);
			
			write_more(8, Card_ID, 6);
			read_more(8, CARD_ID, 6);
			
			printf("CARD_ID @@@@@@@@@@@:%d%d%d%d\r\n",CARD_ID[0],CARD_ID[1],CARD_ID[2],CARD_ID[3]);
			
			status = PcdSelect(Card_ID);  //ѡ�� ����ɹ�����MI_OK
			if(status != MI_OK) {
					printf("Select Card Error\r\n");
			} else printf("Select Card OK\r\n");
			
			status = PcdHalt();  //��Ƭ��������״̬
			if(status != MI_OK){
					printf("PcdHalt Error\r\n");			
			} else{
					printf("PcdHalt OK\r\n");	
			}
			printf("##############################################  �¿��洢������Ϣ:\r\n");
			if (strlen(CARD_ID) == 0) { // ����¼�벻�ɹ�
				OLED_ShowCN(0, 2, 57); // ����¼��ʧ��
				OLED_ShowCN(16, 2, 58);
				OLED_ShowCN(32, 2, 59);
				OLED_ShowCN(48, 2, 60);
				OLED_ShowCN(64, 2, 52);
				OLED_ShowCN(80, 2, 53);
				GPIO_SetBits(GPIOB,GPIO_Pin_5);						 //����������
				delay_ms(1500);
				GPIO_ResetBits(GPIOB,GPIO_Pin_5);
			} else {
				OLED_ShowCN(0, 2, 57); // ����¼��ɹ�
				OLED_ShowCN(16, 2, 58);
				OLED_ShowCN(32, 2, 59);
				OLED_ShowCN(48, 2, 60);
				OLED_ShowCN(64, 2, 61);
				OLED_ShowCN(80, 2, 62);
				GPIO_SetBits(GPIOB,GPIO_Pin_5);						 //����������
				delay_ms(1500);
				GPIO_ResetBits(GPIOB,GPIO_Pin_5);
				break;
			}
		}
	}
}

void login_by_card() { // ˢ������
	int i;
	u8 flag=0;
	uint8_t Card_Type1[2];  
	uint8_t Card_ID[4];  // 536870942
	uint8_t Card_KEY[6] = {0xff,0xff,0xff,0xff,0xff,0xff};    //{0x11,0x11,0x11,0x11,0x11,0x11};   //����
	uint8_t Card_Data[16];
	uint8_t status;
	while(1) {
		delay_ms(10);
		if(MI_OK==PcdRequest(0x52, Card_Type1))  //Ѱ������������ɹ�����MI_OK  ��ӡ��ο���
		{
			uint16_t cardType = (Card_Type1[0]<<8)|Card_Type1[1];//����ͬ�������� 0x0400
			printf("###########################  ˢ����¼ :\r\n");
			printf("�����ͣ�(0x%04X)\r\n",cardType);  //"Card Type(0x%04X):"
			switch(cardType){
			case 0x4400:
					printf("Mifare UltraLight\r\n");
					break;
			case 0x0400:
					printf("Mifare One(S50)\r\n");
					break;
			case 0x0200:
					printf("Mifare One(S70)\r\n");
					break;
			case 0x0800:
					printf("Mifare Pro(X)\r\n");
					break;
			case 0x4403:
					printf("Mifare DESFire\r\n");
					break;
			default:
					printf("Unknown Card\r\n");
					break;
			}
			
			//printf("###########################:%d%d%d%d\r\n",Card_ID[0],Card_ID[1],Card_ID[2],Card_ID[3]);
			status = PcdAnticoll(Card_ID);//����ײ ����ɹ�����MI_OK
			if(status != MI_OK){
					printf("Anticoll Error\r\n");
			} else {
					printf("Serial Number:%02X%02X%02X%02X\r\n",Card_ID[0],Card_ID[1],Card_ID[2],Card_ID[3]);
			}
			printf("**************:%d%d%d%d\r\n",Card_ID[0],Card_ID[1],Card_ID[2],Card_ID[3]);
			printf("CARD_ID @@@@@@@@@@@:%d%d%d%d\r\n",CARD_ID[0],CARD_ID[1],CARD_ID[2],CARD_ID[3]);

			status = PcdSelect(Card_ID);  //ѡ�� ����ɹ�����MI_OK
			if(status != MI_OK) {
					printf("Select Card Error\r\n");
			} else printf("Select Card OK\r\n");
			
			status = PcdHalt();  //��Ƭ��������״̬
			if(status != MI_OK){
					printf("PcdHalt Error\r\n");			
			} else{
					printf("PcdHalt OK\r\n");	
			}
			printf("######################################  ˢ����¼ :\r\n");
			flag=0;
			for(i=0;i<4;i++){
				if(Card_ID[i]!=CARD_ID[i]) {
					flag=1;
					break;
				}
			}
			if (flag == 0) { // ������ͬ �����ɹ�
				OLED_ShowCN(0, 6, 20); // �����ɹ�
				OLED_ShowCN(16, 6, 21);
				OLED_ShowCN(32, 6, 22);
				OLED_ShowCN(48, 6, 23);
				GPIO_SetBits(GPIOB,GPIO_Pin_5);						 //����������
				delay_ms(1500);
				GPIO_ResetBits(GPIOB,GPIO_Pin_5);
				break;
			} else {
				OLED_ShowCN(0, 6, 20); // ����ʧ��
				OLED_ShowCN(16, 6, 21);
				OLED_ShowCN(32, 6, 52);
				OLED_ShowCN(48, 6, 53);
				GPIO_SetBits(GPIOB,GPIO_Pin_5);						 //����������
				delay_ms(1500);
				GPIO_ResetBits(GPIOB,GPIO_Pin_5);
				break;
			}
		}
	}
}

void login_lock(){ //���뿪��
	u8 times=0;
	u8 key=12;
	u8 flag=0;
	char i=0,j=0;
	char keyword[6];
	
  read_more(0, AT24C02_BUFF, 6);
	while(1){     //��������
		key=KEY_Scan();
//		printf("login_lock^^^key:%d\r\n", key);
		OLED_ShowCN(0, 0, 0); // ���������룺
		OLED_ShowCN(16, 0, 1);
		OLED_ShowCN(32, 0, 2);
		OLED_ShowCN(48, 0, 3);
		OLED_ShowCN(64, 0, 4);
		OLED_ShowCN(80, 0, 14);
		
		if(key!=12 && i<=5){
			if(key>=0 && key<=9){
				keyword[i]=key + '0';
				OLED_ShowStr(i*16, 2, "*", 2);
				i++;
				key = 12;
			}
		}
		if(key==11)  //����ɾ������
		{
			i--;
			OLED_ShowStr(i*16,2," ",2);
		}
		if(key==10)  //����ȷ������
		{
			//printf("keyword:%s\r\n", keyword);
			flag=0;
			for(j=0;j<6;j++)
			{
				if(keyword[j]!=AT24C02_BUFF[j])
					flag=1;
			}
			if(flag==0){  //����������ȷ
				OLED_ShowCN(0, 4, 3); // ������ȷ�����ɹ�
				OLED_ShowCN(16, 4, 4);
				OLED_ShowCN(32, 4, 18);
				OLED_ShowCN(48, 4, 19);
				OLED_ShowCN(64, 4, 20);
				OLED_ShowCN(80, 4, 21);
				OLED_ShowCN(96, 4, 22);
				OLED_ShowCN(112, 4, 23);
				GPIO_SetBits(GPIOB,GPIO_Pin_5);						 //����������
				delay_ms(1000);
				GPIO_ResetBits(GPIOB,GPIO_Pin_5);
				key = 12;
				break;
			} else {    //�����������
				key = 12;
				OLED_ShowCN(0, 4, 3); //���������������
				OLED_ShowCN(16, 4, 4);
				OLED_ShowCN(32, 4, 24);
				OLED_ShowCN(48, 4, 25);
				OLED_ShowCN(64, 4, 26);
				OLED_ShowCN(80, 4, 15);
				OLED_ShowCN(96, 4, 1);
				OLED_ShowCN(112, 4, 2);
				GPIO_SetBits(GPIOB,GPIO_Pin_5);						 //����������
				delay_ms(1000);
				GPIO_ResetBits(GPIOB,GPIO_Pin_5);
				OLED_CLS();
				i=0;
				times++;
			}
		}
		if(times>=3) { // ����
			key = 12;
			OLED_CLS();
			OLED_ShowCN(0, 0, 1); // �����������
			OLED_ShowCN(16, 0, 2);
			OLED_ShowCN(32, 0, 28);
			OLED_ShowCN(48, 0, 29);
			OLED_ShowCN(64, 0, 30);
			OLED_ShowCN(80, 0, 31);
			OLED_ShowCN(0, 2, 32); // ˢIC�����н���
			OLED_ShowStr(16,2,"IC",2);
			OLED_ShowCN(32, 2, 33);
			OLED_ShowCN(48, 2, 34);
			OLED_ShowCN(64, 2, 35);
			OLED_ShowCN(80, 2, 20);
			OLED_ShowCN(96, 2, 21);
			GPIO_SetBits(GPIOB,GPIO_Pin_5);						 //����������
			delay_ms(1500);
			GPIO_ResetBits(GPIOB,GPIO_Pin_5);
			
			login_by_card();//break; ˢIC���н���
			break;
		}
	}
}
void set_newkey(int flag){ // ����������
	u8 i=0;
	u8 key=12;
	while(1){     //��������
		key=KEY_Scan();
		if(key!=12 && i<=5){
			if(key>=0 && key<=9){
				printf("i = %d\r\n", i);
				printf("new keyword XXX");
				if (flag == 1) {
					NEW_KEYWORD[i] = key + '0';
					OLED_ShowStr(i*16, 2, "*", 2);
				} else {
					MAKE_SURE_KEYWORD[i] = key + '0';
					OLED_ShowStr(i*16, 6, "*", 2);
				}
				i++;
				key = 12;
			}
		}
		if(key==11)  //����ɾ������
		{
			i--;
			if (flag == 1) {
				OLED_ShowStr(i*16, 2, "  ", 2);
			} else {
				OLED_ShowStr(i*16, 6, "  ", 2);
			}
		}
		if(key == 10 && i<6 && flag == 1) {
			OLED_ShowCN(0, 4, 3); // ����������λ��
			OLED_ShowCN(16, 4, 4);
			OLED_ShowCN(32, 4, 39);
			OLED_ShowCN(48, 4, 40);
			OLED_ShowCN(64, 4, 41);
			OLED_ShowCN(80, 4, 42);
			OLED_ShowCN(96, 4, 43);
			delay_ms(1500);
			OLED_ShowStr(0, 4, "                  ", 2);
		}
		if(i==6&&key==10)
			break;
  }			
}

void new_login() { 	 // ���û���¼
	u8 key=12;
	while(1) {
		key=KEY_Scan();
		OLED_ShowCN(0, 0, 9); // ��������
		OLED_ShowCN(16, 0, 10);
		OLED_ShowCN(32, 0, 12);
		OLED_ShowCN(48, 0, 13);
		
		OLED_ShowCN(0, 2, 45); // ��*����һ��
		OLED_ShowStr(16,2,"*",2);
		OLED_ShowCN(32, 2, 44);
		OLED_ShowCN(48, 2, 54);
		OLED_ShowCN(64, 2, 55);
		OLED_ShowCN(80, 2, 56);
		
		if(key==10)  //ȷ����������
		{
			printf("new_key:%d\r\n", key);
			OLED_CLS();
			OLED_ShowCN(0, 0, 15); // �����룺
			OLED_ShowCN(16, 0, 3);
			OLED_ShowCN(32, 0, 4);
			OLED_ShowCN(48, 0, 14);
			set_newkey(1);
			printf("new_keyword:%s\r\n", NEW_KEYWORD);
			
			OLED_ShowCN(0, 4, 0); // ��ȷ�����룺
			OLED_ShowCN(16, 4, 16);
			OLED_ShowCN(32, 4, 17);
			OLED_ShowCN(48, 4, 3);
			OLED_ShowCN(64, 4, 4);
			OLED_ShowCN(80, 4, 14);
			set_newkey(2);
			printf("make_sure_keyword:%s\r\n", MAKE_SURE_KEYWORD);
			
			if(cmp(NEW_KEYWORD,MAKE_SURE_KEYWORD) == 0)  //����������ͬ
			{ 
				OLED_CLS();
				OLED_ShowCN(0, 0, 9); // ���óɹ�
				OLED_ShowCN(16, 0, 10);
				OLED_ShowCN(32, 0, 22);
				OLED_ShowCN(48, 0, 23);
				write_more(0, NEW_KEYWORD, 6);
				GPIO_SetBits(GPIOB,GPIO_Pin_5);						 //����������
				delay_ms(1500);
				GPIO_ResetBits(GPIOB,GPIO_Pin_5);
				break;
			}
			else
			{
				OLED_CLS();
				OLED_ShowCN(0, 0, 36); // �������벻ͬ
				OLED_ShowCN(16, 0, 28);
				OLED_ShowCN(32, 0, 3);
				OLED_ShowCN(48, 0, 4);
				OLED_ShowCN(64, 0, 37);
				OLED_ShowCN(80, 0, 38);
				GPIO_SetBits(GPIOB,GPIO_Pin_5);						 //����������
				delay_ms(1500);
				GPIO_ResetBits(GPIOB,GPIO_Pin_5);
				OLED_CLS();
			}
		}
	}
}

void change_keyword() { // �޸�����
	//get_auth_by_card();
	int i;
	u8 flag=0;
	uint8_t Card_Type1[2];  
	uint8_t Card_ID[4];  // 536870942
	uint8_t Card_KEY[6] = {0xff,0xff,0xff,0xff,0xff,0xff};    //{0x11,0x11,0x11,0x11,0x11,0x11};   //����
	uint8_t Card_Data[16];
	uint8_t status;
	
		// ˢIC����ȡ�޸�Ȩ��
	OLED_ShowCN(0, 0, 32); // ˢIC��������֤
	OLED_ShowStr(16,0,"IC",2);
	OLED_ShowCN(32, 0, 33);
	OLED_ShowCN(48, 0, 34);
	OLED_ShowCN(64, 0, 35);
	OLED_ShowCN(80, 0, 50);
	OLED_ShowCN(96, 0, 51);
	while(1) {
		delay_ms(10);
		if(MI_OK==PcdRequest(0x52, Card_Type1))  //Ѱ������������ɹ�����MI_OK  ��ӡ��ο���
		{
			uint16_t cardType = (Card_Type1[0]<<8)|Card_Type1[1];//����ͬ�������� 0x0400
			printf("###########################  ��ȡȨ�� :\r\n");
			printf("�����ͣ�(0x%04X)\r\n",cardType);  //"Card Type(0x%04X):"
			switch(cardType){
			case 0x4400:
					printf("Mifare UltraLight\r\n");
					break;
			case 0x0400:
					printf("Mifare One(S50)\r\n");
					break;
			case 0x0200:
					printf("Mifare One(S70)\r\n");
					break;
			case 0x0800:
					printf("Mifare Pro(X)\r\n");
					break;
			case 0x4403:
					printf("Mifare DESFire\r\n");
					break;
			default:
					printf("Unknown Card\r\n");
					break;
			}
			
			//printf("###########################:%d%d%d%d\r\n",Card_ID[0],Card_ID[1],Card_ID[2],Card_ID[3]);
			status = PcdAnticoll(Card_ID);//����ײ ����ɹ�����MI_OK
			if(status != MI_OK){
					printf("Anticoll Error\r\n");
			} else {
					printf("Serial Number:%02X%02X%02X%02X\r\n",Card_ID[0],Card_ID[1],Card_ID[2],Card_ID[3]);
			}
			printf("**************:%d%d%d%d\r\n",Card_ID[0],Card_ID[1],Card_ID[2],Card_ID[3]);
			printf("CARD_ID @@@@@@@@@@@:%d%d%d%d\r\n",CARD_ID[0],CARD_ID[1],CARD_ID[2],CARD_ID[3]);

			status = PcdSelect(Card_ID);  //ѡ�� ����ɹ�����MI_OK
			if(status != MI_OK) {
					printf("Select Card Error\r\n");
			} else printf("Select Card OK\r\n");
			
			status = PcdHalt();  //��Ƭ��������״̬
			if(status != MI_OK){
					printf("PcdHalt Error\r\n");			
			} else{
					printf("PcdHalt OK\r\n");	
			}
			printf("##############################################  ��ȡȨ�� :\r\n");
			flag=0;
			for(i=0;i<4;i++){
				if(Card_ID[i]!=CARD_ID[i]) {
					flag=1;
					break;
				}
			}
			if (flag == 0) { // ������ͬ ���Ȩ��
				OLED_ShowCN(0, 2, 48); // �����֤�ɹ�
				OLED_ShowCN(16, 2, 49);
				OLED_ShowCN(32, 2, 50);
				OLED_ShowCN(48, 2, 51);
				OLED_ShowCN(64, 2, 61);
				OLED_ShowCN(80, 2, 62);
				GPIO_SetBits(GPIOB,GPIO_Pin_5);						 //����������
				delay_ms(1500);
				GPIO_ResetBits(GPIOB,GPIO_Pin_5);
				
				OLED_CLS();
			  new_login(); // ȥ�޸�����
				break;
			} else {
				OLED_ShowCN(0, 2, 48); // �����֤ʧ��
				OLED_ShowCN(16, 2, 49);
				OLED_ShowCN(32, 2, 50);
				OLED_ShowCN(48, 2, 51);
				OLED_ShowCN(64, 2, 52);
				OLED_ShowCN(80, 2, 53);
				GPIO_SetBits(GPIOB,GPIO_Pin_5);						 //����������
				delay_ms(1500);
				GPIO_ResetBits(GPIOB,GPIO_Pin_5);
				break;
			}
		}
	}
}

void Bee_Init(void) { // ��������ʼ������
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PA,PD�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //LED0-->PA.8 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.8
 GPIO_ResetBits(GPIOB,GPIO_Pin_5);						 //PA.8 �����
}

void KEY_Init1(void) { //������ʼ������
 	GPIO_InitTypeDef GPIO_InitStructure;
 	// ��B12 13 14 15
	// ��A8 11  12
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);//�ⲿ�жϣ���Ҫʹ��AFIOʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8|GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
 
 
u8 KEY_Scan(void){	//ɨ�躯��
	// ��B12 13 14 15
	// ��A8 11  12
	static u8 key_flag1=1,key_flag2=1,key_flag3=1,key_flag4=1;     //�����ɿ���־
	u8 KeyVal=12;
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);//��Ϊ�͵�ƽ
	GPIO_SetBits(GPIOB,GPIO_Pin_12);//��Ϊ�ߵ�ƽ

	if(key_flag1&&(Read_Lie1==1||Read_Lie2==1||Read_Lie3==1))
	{
		 key_flag1=0;
		     if (Read_Lie1==1){ KeyVal= 10; printf("key:*\r\n");}//#��
		else if (Read_Lie2==1) {KeyVal= 0; printf("key:0\r\n");}
		else if (Read_Lie3==1) {KeyVal= 11; printf("key:#\r\n");}
	}
	else if(Read_Lie1==0&&Read_Lie2==0&&Read_Lie3==0)key_flag1=1;
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);//��Ϊ�͵�ƽ
	GPIO_SetBits(GPIOB,GPIO_Pin_13);//��Ϊ�ߵ�ƽ
	if(key_flag2&&(Read_Lie1==1||Read_Lie2==1||Read_Lie3==1))
	{
		 key_flag2=0;
		     if (Read_Lie1==1){ KeyVal= 7; printf("key:7\r\n");}
		else if (Read_Lie2==1) {KeyVal= 8; printf("key:8\r\n");}
		else if (Read_Lie3==1) {KeyVal= 9; printf("key:9\r\n");}
	}
	else if(Read_Lie1==0&&Read_Lie2==0&&Read_Lie3==0)key_flag2=1;
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);//��Ϊ�͵�ƽ
	GPIO_SetBits(GPIOB,GPIO_Pin_14);//��Ϊ�ߵ�ƽ
	if(key_flag3&&(Read_Lie1==1||Read_Lie2==1||Read_Lie3==1))
	{
		key_flag3=0;
		     if (Read_Lie1==1){ KeyVal= 4; printf("key:4\r\n");}
		else if (Read_Lie2==1) {KeyVal= 5; printf("key:5\r\n");}
		else if (Read_Lie3==1) {KeyVal= 6; printf("key:6\r\n");}
	}
	else if(Read_Lie1==0&&Read_Lie2==0&&Read_Lie3==0)key_flag3=1;
		
	GPIO_ResetBits(GPIOB,GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);//��Ϊ�͵�ƽ
	GPIO_SetBits(GPIOB,GPIO_Pin_15);//��Ϊ�ߵ�ƽ
	if(key_flag4&&(Read_Lie1==1||Read_Lie2==1||Read_Lie3==1))
	 {
		 key_flag4=0;
		     if (Read_Lie1==1){ KeyVal= 1; printf("key:1\r\n");}
		else if (Read_Lie2==1) {KeyVal= 2; printf("key:2\r\n");}
		else if (Read_Lie3==1) {KeyVal= 3; printf("key:3\r\n");}
	 }
  else if(Read_Lie1==0 &&Read_Lie2==0 &&Read_Lie3==0 )key_flag4=1;
 
  return KeyVal;
}

int main(void){	 // ������
	u8 key=12;
	delay_init();	    	 //��ʱ������ʼ��	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
  uart_init(9600);	 //���ڳ�ʼ��Ϊ9600
	I2C_Configuration();//����CPU��Ӳ��I2C2 OLED
	I2C1_Configuration();//����CPU��Ӳ��I2C1 AT24C02
	OLED_Init();  //��ʼ��OLED
	KEY_Init1();  //��ʼ������
	Bee_Init();  //��ʼ��������
	
	RC522_IO_Init(); //��ʼ��IC
	PcdReset();  //��λRC522
	PcdAntennaOff();  //�ر�����
	delay_ms(100);
	PcdAntennaOn();  //��������
	 
	OLED_Fill(0x00);//ȫ����
	 
	GPIO_SetBits(GPIOB,GPIO_Pin_5);						 //����������
	delay_ms(1000);
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);
	 
	 
//	 new_card();
//	 while(1) {
//		 key=KEY_Scan();
//		 printf("PcdHalt OK: %d\r\n", key);	
//	 }
	write_more(0, "", 6);
	write_more(8, "", 6);
	while(1)
	{
		key=KEY_Scan();
		//if (key < 12) { // �������£����ѳ���
			read_more(0, AT24C02_BUFF, 6);
			read_more(8, CARD_ID, 6);
			//printf("read_AT24C02_BUFF: %s\n", AT24C02_BUFF);
		//printf("read_CARD_ID: %d%d%d%d\n", CARD_ID[0],CARD_ID[1],CARD_ID[2],CARD_ID[3]);
			
			if (strlen(AT24C02_BUFF) == 0) { // û���������룬ǰ����������, ���û���¼
				new_login();
				OLED_CLS();
				//¼��IC����Ϣ �洢����
				new_card();
				OLED_CLS();
			}
			
			// ��*����������
			OLED_ShowCN(0, 0, 45);
			OLED_ShowStr(16,0,"*",2);
			OLED_ShowCN(32, 0, 44);
			OLED_ShowCN(48, 0, 1);
			OLED_ShowCN(64, 0, 2);
			OLED_ShowCN(80, 0, 3);
			OLED_ShowCN(96, 0, 4);
			// ��#���޸�����
			OLED_ShowCN(0, 2, 45);
			OLED_ShowStr(16,2,"#",2);
			OLED_ShowCN(32, 2, 44);
			OLED_ShowCN(48, 2, 46);
			OLED_ShowCN(64, 2, 47);
			OLED_ShowCN(80, 2, 3);
			OLED_ShowCN(96, 2, 4);
			// ��0��ˢ������
			OLED_ShowCN(0, 4, 45);
			OLED_ShowStr(16,4,"0",2);
			OLED_ShowCN(32, 4, 44);
			OLED_ShowCN(48, 4, 32);
			OLED_ShowCN(64, 4, 33);
			OLED_ShowCN(80, 4, 20);
			OLED_ShowCN(96, 4, 21);
			
			if (key == 10) { // ��������
				OLED_CLS();
				login_lock();
				OLED_CLS();
			}
			
			if (key == 11) { // �޸��������������
				OLED_CLS();
				change_keyword();
				OLED_CLS();
			}
			
			if (key == 0) { // ˢ������
				OLED_CLS();
				OLED_ShowCN(0, 0, 32); // ˢIC�����н���
				OLED_ShowStr(16,0,"IC",2);
				OLED_ShowCN(32, 0, 33);
				OLED_ShowCN(48, 0, 34);
				OLED_ShowCN(64, 0, 35);
				OLED_ShowCN(80, 0, 20);
				OLED_ShowCN(96, 0, 21);
				
				login_by_card();
				OLED_CLS();
			}
			
		//	delay_ms(5000); // ��ʱ5�� ��OLED���� 
			
//		} else { // ����״̬
//			OLED_Fill(0x00);//ȫ����
//		}
	}
}
 




