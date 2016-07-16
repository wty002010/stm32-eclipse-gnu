#include "Touch.h"

void GPIO_SPI3_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	SPI_InitTypeDef		SPI_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_10;	 //6本来是是输入但是这里配成复用输出也有效因为他虽然设成输出，但连接的是spi，没有连接输出部分，和输入电路等效
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
    //原来是GPIO_Mode_IN_FLOATING，记得换回来
    GPIO_InitStructure.GPIO_Pin = LCD_INT;	//LCD_TOUCH转换完成中断PC5
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 //GPIO_Mode_IN_FLOATING
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//LCD SPI3片选
	GPIO_InitStructure.GPIO_Pin = TP_CS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//wireless 片选
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
/*
typedef struct
{
	u16 SPI_Direction;
	u16 SPI_Mode;
	u16 SPI_DataSize;
	u16 SPI_CPOL;
	u16 SPI_CPHA;
	u16 SPI_NSS;
	u16 SPI_BaudRatePrescaler;
	u16 SPI_FirstBit;
	u16 SPI_CRCPolynomial;
} SPI_InitTypeDef;
*/
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS =	SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler	=  SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial	= 7;
	SPI_Init(SPI3, &SPI_InitStructure);

	GPIO_SetBits(GPIOD, GPIO_Pin_12); //wireless 片选
	GPIO_SetBits(GPIOC, TP_CS);  //LCD SPI3片选
	SPI_Cmd(SPI3, ENABLE);
//	debug("GPIO_SPI3_Init fin\r\n");
}

inline u8 SPI3_SendByte(u8 byte);
u8 SPI3_SendByte(u8 byte)
{
	while(SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPI3,byte);

	while(SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_RXNE)==RESET);
	return SPI_I2S_ReceiveData(SPI3);
}

static void SpiDelay(unsigned int DelayCnt);
void SpiDelay(unsigned int DelayCnt)
{
 	unsigned int i;
 	for(i=0;i<DelayCnt;i++);
}

inline u16 TPReadX(void);
u16 TPReadX(void)
{
	u16 x=0;
	Reset_TP_CS;
	SpiDelay(10);
	SPI3_SendByte(0x90);
	SpiDelay(10);
	x=SPI3_SendByte(0x00);
	x <<= 8;
	x += SPI3_SendByte(0x00);
	SpiDelay(10);
	Set_TP_CS;
	x >>= 3;
//	printf("ADX=%d\n\r",x);
	return (x);
}

inline u16 TPReadY(void);
u16 TPReadY(void)
{
 	u16 y=0;

 	Reset_TP_CS;
  	SpiDelay(10);
  	SPI3_SendByte(0xd0);
	SpiDelay(10);
	y=SPI3_SendByte(0x00);
	y<<=8;
	y+=SPI3_SendByte(0x00);
	SpiDelay(10);
	Set_TP_CS;
	y = y>>3;
//	printf("ADY=%d\n\r",y);
	return (y);
}

//static u32 last_x = 0;
//static u32 last_y = 0;

void GUI_TOUCH_X_ActivateX(void)
{

}

void GUI_TOUCH_X_ActivateY(void)
{
}

//void GUI_TOUCH_X_Disable(void);
int  GUI_TOUCH_X_MeasureX(void)
{
	unsigned char t=0,t1,count=0;
	unsigned short int databuffer[10]={5,7,9,3,2,6,4,0,3,1};//�����
	unsigned short temp=0,X=0;

	while(count<10)//ѭ������10��
	{
		databuffer[count]=TPReadX();
		count++;
	}
	if(count==10)//һ��Ҫ�� ��10�����,������
	{
	    do  //�����X��������
		{
			t1=0;
			for(t=0;t<count-1;t++)
			{
				if(databuffer[t]>databuffer[t+1])//��������
				{
					temp=databuffer[t+1];
					databuffer[t+1]=databuffer[t];
					databuffer[t]=temp;
					t1=1;
				}
			}
		}while(t1);
		X=(databuffer[3]+databuffer[4]+databuffer[5])/3;
	//	X=240-X/14;
	}
//	printf(" X���ADֵ��%d",X);
	return(X);
}

int  GUI_TOUCH_X_MeasureY(void)
{
  	unsigned char t=0,t1,count=0;
	unsigned short int databuffer[10]={5,7,9,3,2,6,4,0,3,1};//�����
	unsigned short temp=0,Y=0;

    while(count<10)	//ѭ������10��
	{
		databuffer[count]=TPReadY();
		count++;
	}
	if(count==10)//һ��Ҫ����10�����,������
	{
	    do//�����X��������
		{
			t1=0;
			for(t=0;t<count-1;t++)
			{
				if(databuffer[t]>databuffer[t+1])//��������
				{
					temp=databuffer[t+1];
					databuffer[t+1]=databuffer[t];
					databuffer[t]=temp;
					t1=1;
				}
			}
		}while(t1);

		Y=(databuffer[3]+databuffer[4]+databuffer[5])/3;

	}
	return(Y);
}

//this irq function is the LCD interrupt func
//there has a quesution when a screen touch is occur  the SPI3 data is not complete
//i have to think about this more.
void EXTI9_5_IRQHandler(void)
{
	while(1);
//	GPIO_SetBits
}
