# MCUKeys

## 1  简介
MCUKeys是一个简洁小巧可配置的、灵活的、通用的的按键检测程序，截至到目前为止它可以检测如下输入事件：

1. 按键按下
2. 按键抬起
3.  长按(时间可配置)
4.  连击(时间可配置)
5. 双击
6. 组合按键（可以实现以上五种状态）

同时，所有的事件都支持注册回调函数，在事件发生时执行。如果当前系统希望只检测按键按下事件，则可以屏蔽其他所有事件，对于其他按键事件亦然。


## 2  使用方法

1. 先定义一个变量用于获取按键状态

```
e_keys_status key_value = KEY_NONE;
```
2. 初始化按键，包括硬件IO配置和软件变量初始化两部分

```
keys_init();                 //初始化按键
```
3. 如果有需要回调函数，则注册，不需要可跳过，默认不支持回调函数

```
    keys[KEY1_ID].short_key_down = led2_on;   //注册回调函数
    keys[KEY1_ID].short_key_up = led2_off;
    keys[KEY1_ID].long_key_down = led3_on;
...
```
4. 设置一个周期性可调用的函数来调用按键扫描函数

```
//定时器3中断服务函数 KEY_TICKS周期中断一次
void TIM3_IRQHandler(void)
{

    if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
    {
        key_scan();  //按键扫描函数
    }
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}
```
5. 在主循环中可以取出按键事件并处理

```
	//main
    while(1)
    {
        key_value = key_out_fifo();
        if(key_value != KEY_NONE)
        {
            switch(key_value)
            {
            case KEY_1_DOWN:
            {
                //doing something
            }
            break;
            case KEY_1_UP:
            {
                //doing something
            }
            break;
            case KEY_1_LONG:
            {
                //doing something
            }
            break;
            case KEY_1_DOUBLE:
            {
                 //doing something
            }
            default:
            {

            } break;
            }
        }
    }
```


## 3 按键事件说明

事件 | 说明
---|---
KEY_DOWN | 按键按下，每次按下都触发，可选是否上报
KEY_UP | 按键弹起，每次松开都触发，可选是否上报
KEY_LONG | 长按触发，key.h中的KEY_LONG_TIME配置长按时间，同时每个按键可单独配置长按时间
KEY_REPEAT | 按下连发，key.h中的KEY_REPEAT_TIME配置连发时间，同时每个按键可单独配置连发时间
KEY_DOUBLE | 双击按键事件，key.h中的KEY_DOUBLE_MIN/MAX调整双击灵敏度


## 4 快速上手

MCUKeys提供完整的中文注释。同时，提供如下的例程供大家参考。相信很容易上手。

1. [Lesson1_轮询实现按键检测](https://github.com/xupenghu/MCUKeys/tree/master/Examples/Lesson1_%E8%BD%AE%E8%AF%A2%E5%AE%9E%E7%8E%B0%E6%8C%89%E9%94%AE%E6%A3%80%E6%B5%8B)
2. [Lesson2_中断实现按键检测](https://github.com/xupenghu/MCUKeys/tree/master/Examples/Lesson2_%E4%B8%AD%E6%96%AD%E5%AE%9E%E7%8E%B0%E6%8C%89%E9%94%AE%E6%A3%80%E6%B5%8B)
3. [Lesson3_轮询Fifo实现按键检测](https://github.com/xupenghu/MCUKeys/tree/master/Examples/Lesson3_%E8%BD%AE%E8%AF%A2Fifo%E5%AE%9E%E7%8E%B0%E6%8C%89%E9%94%AE%E6%A3%80%E6%B5%8B)
4. [Lesson4_基于FreeRTOS的按键检测](https://github.com/xupenghu/MCUKeys/tree/master/Examples/Lesson4_%E5%9F%BA%E4%BA%8EFreeRTOS%E7%9A%84%E6%8C%89%E9%94%AE%E6%A3%80%E6%B5%8B)
