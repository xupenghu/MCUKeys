#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "stdlib.h"
#include "oled.h" 
#include "timer.h"

void led3_on(void * arg)
{
    arg = NULL;
    LED3 = 1;
}
void led2_on(void * arg)
{
    arg = NULL;
    LED2 = 1;
}

void led2_off(void * arg)
{
    arg = NULL;
    LED2 = 0;
    LED3 = 0;
}

int main(void)
{  
    uint16_t i = 0;
    e_keys_status key_value = KEY_NONE;
	delay_init(168);		  //初始化延时函数
	LED_Init();		        //初始化LED端口
    keys_init();                 //初始化按键
    TIM3_Int_Init(1000, 84-1);    //1ms 定时
    keys[KEY1_ID].short_key_down = led2_on;   //注册回调函数
    keys[KEY1_ID].short_key_up = led2_off;
    keys[KEY1_ID].long_key_down = led3_on;
    
	OLED_Init();			//初始化OLED  
	OLED_Clear();
	OLED_ShowString(0,0,"   KEY TEST",16);

    while(1)
    {
        key_value = key_out_fifo();
        if(key_value != KEY_NONE)
        {
            switch(key_value)
            {
                case KEY_1_DOWN:
                {
                    OLED_ShowString(0,16,"KEY1 DOWN",16);
                     OLED_ShowNum(60, 32, i++, 2, 16);
                    delay_ms(20);
                }break;
                case KEY_1_UP:
                {
                    OLED_ShowString(0,16,"                   ",16);
                    OLED_ShowString(0,16,"KEY1 UP",16);
                   
                    delay_ms(20);
            
                }break;
                case KEY_1_LONG:
                {
                    OLED_ShowString(0,16,"KEY1 LONG",16);
                    delay_ms(20);
                 
                }break;               
                case KEY_1_DOUBLE:
                {
                     OLED_ShowString(0,16,"KEY1 DOUBLE",16);
                    delay_ms(20);               
                }
                default:
                {
                    
                }break;
            }
        }
        else
            OLED_ShowString(0,16,"                   ",16);
     }
}


	

 



