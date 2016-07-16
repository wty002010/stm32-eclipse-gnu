
#include "lcdwty.h"
#include "Timer.h"
#include "led.h"
#include "GUI.h"
#include "Touch.h"
#include "SZ_STM32F107VC_LIB.h"
#include <stdio.h>

int main(void)
{
	NVIC_GroupConfig();
	RCC_Configuration();
	 SZ_STM32_COMInit(COM2, 9600);
	LED_Init();
	GPIO_SetBits(GPIOD,GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 |GPIO_Pin_7);
	timer_start();
	GPIO_SPI3_Init();
	GUI_Init();
	GUI_SetBkColor(GUI_BLUE);
	GUI_Clear();
	GUI_SetFont(&GUI_Font24_ASCII);
	GUI_SetColor(GUI_RED);
	GUI_DispStringAt("101Hello World",10,10);
	GUI_DispStringAt("101Hello World",10,34);
	GUI_CURSOR_Show();
  // Infinite loop
  while (1)
    {
       // Add your code here.
 		if(LCD_PEN==0)
		{
		   GUI_TOUCH_Exec();
		}

 		WM_Exec();
		timer_sleep(20);
    }
  return 0;
}

// ----------------------------------------------------------------------------
