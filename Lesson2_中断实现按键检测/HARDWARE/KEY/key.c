#include "key.h"
#include "sys.h"
#include "delay.h"
#include "exti.h"

extern eKEY_STATUS key_value = KEY_NONE;


/***********************************************************************
* 函数名称： key_init
* 函数功能： 初始化按键的IO口配置
* 输入参数：  无	 
* 返 回 值：   无
* 函数说明： 无
****************************************************************************/
void key_init(void )
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(KEY1_RCC, ENABLE);//使能GPIO时钟

  GPIO_InitStructure.GPIO_Pin = KEY1_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  //输入
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(KEY1_GPIO, &GPIO_InitStructure);//初始化GPIO

   EXTIX3_Init(); //外部中断初始化
    
}

/***********************************************************************
* 函数名称： get_key1_value
* 函数功能： 获取按键的状态
* 输入参数：  无	 
* 返 回 值：   按键按下返回1 否则返回0
* 函数说明： 我们规定，按键按下返回PRESS 否则返回LIFT 要注意实际的硬件接法，必要时要做一个逻辑转换，例如本函数就做了一个逻辑转换
****************************************************************************/
static uint8_t get_key1_value(void)
{
    uint8_t res  = 0;
    res = GPIO_ReadInputDataBit(KEY1_GPIO, KEY1_Pin);
    if(res == 0)
    {
        return PRESS;
    }
    else
    {
        return LIFT;
    }
}

/***********************************************************************
* 函数名称： key_init
* 函数功能： 初始化按键的IO口配置
* 输入参数：  无	 
* 返 回 值：   无
* 函数说明： 无
****************************************************************************/
uint8_t key_scan(void)
{

    uint8_t res = 0;
    res = get_key1_value();
    if(res == PRESS)
    {
        delay_ms(10);  //延时滤波
        res = get_key1_value();
        if(res == PRESS)
        {
            return KEY1_DOWN;
        }
    }
    return KEY_NONE;
}



//外部中断3服务程序
void EXTI3_IRQHandler(void)
{
     key_value = key_scan();
    
	 EXTI_ClearITPendingBit(EXTI_Line3);  //清除LINE3上的中断标志位  
}







