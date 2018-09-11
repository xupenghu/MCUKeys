#ifndef _KEY__H
#define _KEY__H

#include "sys.h"

//KEY1 配置
#define KEY1_RCC    RCC_AHB1Periph_GPIOB
#define KEY1_GPIO   GPIOB
#define KEY1_Pin      GPIO_Pin_3


#define LIFT    (0)            /* 按键抬起 */
#define PRESS   (!LIFT) /* 按键按下 */

typedef enum
{
    KEY_NONE,
    
    KEY1_DOWN,
    KEY1_UP,
    
    KEY2_DOWN,
    KEY2_UP
}eKEY_STATUS;


extern eKEY_STATUS key_value;




void key_init(void);

/***********************************************************************
* 函数名称： key_init
* 函数功能： 初始化按键的IO口配置
* 输入参数：  无	 
* 返 回 值：   无
* 函数说明： 无
****************************************************************************/
uint8_t key_scan(void);

#endif


