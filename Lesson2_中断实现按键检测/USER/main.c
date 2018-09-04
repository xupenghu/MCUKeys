#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "stdlib.h"
#include "oled.h" 

int main(void)
{  
     eKEY_STATUS key_status = KEY_NONE;
	delay_init(168);		  //初始化延时函数
	LED_Init();		        //初始化LED端口
    key_init();                 //初始化按键
	OLED_Init();			//初始化OLED  
	OLED_Clear();
	OLED_ShowString(0,0,"   KEY TEST",16);
    while(1)
    {
        key_status = key_value;
        if(key_status != KEY_NONE)
        {
            switch(key_status)
            {
                case KEY1_DOWN:
                {
                    OLED_ShowString(0,16,"KEY1 DOWN",16);
                    delay_ms(200);
                    key_value = KEY_NONE;   //清掉
                }break;
                default:
                {
                    
                }break;
            }
        }
        OLED_ShowString(0,16,"                   ",16);
     }
}


	

 



