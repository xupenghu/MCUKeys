
# MCUKeys

## 1  简介
MCUKeys完全使用C语言编写，是一个简洁小巧可配置的、灵活的、通用的的按键检测程序，全部源码都使用中文注释，方便阅读。有基于按键事件回调和按键缓冲区两种方式解决按键事件。截至到目前为止它可以检测如下输入事件：

-  按键按下
-  按键抬起
-   长按(时间可配置)
-  连击(时间可配置)
-  双击(时间可配置)
-  组合按键（可以实现以上五种状态）

同时，所有的事件都支持注册回调函数，在事件发生时执行。如果当前系统希望只检测按键按下事件，则可以屏蔽其他所有事件，对于其他按键事件亦然。


## 2  使用方法

### 2.1 先定义一个按键，如果是多个按键可以用数组来实现

```c
enum
{
    KEY_ID_UP = 0,
    KEY_ID_DOWN,
    KEY_ID_LEFT,
    KEY_ID_RIGHT,
    KEY_ID_ENTER,
    KEY_ID_MAX,
};
static key my_key[KEY_ID_MAX];
```
**KEY_ID**是按键的唯一标识符，不可重复，可以使用enum来实现。
### 2.2 初始化按键

```c
    for (int i = 0; i < (KEY_ID_MAX); i++)
    {
        key_init(&my_key[i]);
        my_key[i].e_key.id = i;
        my_key[i].key_event_cb = key_event_cb; // 屏蔽这行表示不使用事件回调函数
        my_key[i].get_key_status = get_key_state_cb;
        key_add(&my_key[i]);
    }
```
注意：这里的**get_key_state_cb**和硬件息息相关，是唯一需要移植的地方，我这里给出一个使用stm32HAL库开发的实例。

```c

uint8_t get_key_state_cb(const event_key e_key)
{
    uint8_t ret = KEY_LIFT;
    switch (e_key.id)
    {
    case KEY_ID_UP:
    {
        if (HAL_GPIO_ReadPin(GPIOD, KEY0_Pin) == GPIO_PIN_RESET)
        {
            ret = KEY_PRESS;
        }
        else
        {
            ret = KEY_LIFT;
        }
    }
    break;
    case KEY_ID_DOWN:
    {
        if (HAL_GPIO_ReadPin(GPIOD, KEY1_Pin) == GPIO_PIN_RESET)
        {
            ret = KEY_PRESS;
        }
        else
        {
            ret = KEY_LIFT;
        }
    }
    break;
    case KEY_ID_LEFT:
    {
        if (HAL_GPIO_ReadPin(GPIOD, KEY2_Pin) == GPIO_PIN_RESET)
        {
            ret = KEY_PRESS;
        }
        else
        {
            ret = KEY_LIFT;
        }
    }
    break;
    case KEY_ID_RIGHT:
    {
        if (HAL_GPIO_ReadPin(GPIOD, KEY3_Pin) == GPIO_PIN_RESET)
        {
            ret = KEY_PRESS;
        }
        else
        {
            ret = KEY_LIFT;
        }
    }
    break;
    case KEY_ID_ENTER:
    {
        if (HAL_GPIO_ReadPin(GPIOB, KEY4_Pin) == GPIO_PIN_RESET)
        {
            ret = KEY_PRESS;
        }
        else
        {
            ret = KEY_LIFT;
        }
    }
    break;
    default:
        break;
    }
    return ret;
}
```
其他类型的MCU需要根据自己的硬件来做相应的修改，我这里的硬件接法是按键按下低电平，按键抬起高电平。
### 2.3 在一个固定周期的函数中调用key_scan
这里的固定周期可以是定时器的中断服务函数，也可以是RTOS中的一个线程，周期应该是KEY_TICKS,

```c
/* 
 * 按键循环扫描周期(ms) key_scan()函数在哪个固定扫描周期中 该值就等于多少 一般设置为10ms
 */
#define KEY_TICKS 10
```
比如我在这里将它放在了RTOS的一个单独线程中：

```c
void keys_thread_entry(void *param)
{
    while (1)
    {
        key_scan();
        rt_thread_mdelay(KEY_TICKS);
    }
}
```

### 2.4 实现按键事件回调函数
如果初始化的时候，挂接了按键事件回调函数，则这里必须要实现该函数，当有对应的按键事件时，会回调该函数。

```c
void key_event_cb(const event_key e_key)
{
    LOG_D("e_key id = %d state = %d", e_key.id, e_key.state);
}
```
这里传入的参数中, e_key.id就是我们初始化时定义的按键ID，e_key.state就是按键的事件，用户可以在这里实现按键回调的具体逻辑。
实测输出如下：

```c
[D/key] e_key id = 0 state = 2
[D/key] e_key id = 4 state = 1
[D/key] e_key id = 4 state = 2
[D/key] e_key id = 0 state = 1
[D/key] e_key id = 0 state = 2
[D/key] e_key id = 1 state = 1
[D/key] e_key id = 1 state = 2
[D/key] e_key id = 3 state = 1
[D/key] e_key id = 3 state = 2
[D/key] e_key id = 2 state = 1
[D/key] e_key id = 2 state = 2
[D/key] e_key id = 4 state = 1
[D/key] e_key id = 4 state = 2
[D/key] e_key id = 0 state = 1
[D/key] e_key id = 0 state = 1
[D/key] e_key id = 0 state = 1
[D/key] e_key id = 0 state = 1
[D/key] e_key id = 0 state = 1
[D/key] e_key id = 0 state = 1
[D/key] e_key id = 0 state = 2
[D/key] e_key id = 0 state = 1
[D/key] e_key id = 0 state = 2
[D/key] e_key id = 0 state = 1
[D/key] e_key id = 0 state = 2
[D/key] e_key id = 0 state = 1
[D/key] e_key id = 0 state = 2
[D/key] e_key id = 0 state = 1
[D/key] e_key id = 0 state = 2
[D/key] e_key id = 0 state = 1
[D/key] e_key id = 0 state = 2
[D/key] e_key id = 0 state = 1
[D/key] e_key id = 0 state = 2
[D/key] e_key id = 1 state = 1
[D/key] e_key id = 1 state = 2
[D/key] e_key id = 1 state = 1
[D/key] e_key id = 1 state = 2
[D/key] e_key id = 1 state = 4
[D/key] e_key id = 2 state = 1
[D/key] e_key id = 2 state = 1
[D/key] e_key id = 2 state = 1
[D/key] e_key id = 2 state = 1
[D/key] e_key id = 2 state = 1
[D/key] e_key id = 2 state = 1
[D/key] e_key id = 2 state = 2

```

### 2.5 在main函数中轮询按键状态
当我们不使用按键的事件回调机制时，还可以使用FIFO缓冲机制，这样就不会漏掉任何一个按键事件。

```c
int main(void)
{
  /* 必要的初始化代码...*/
  // ....
  event_key e_key;
  while (1)
  {
    e_key = key_out_fifo();
    if(e_key.state != KEY_NONE)
    {
      LOG_D( "e_key id = %d state = %d",e_key.id, e_key.state);
    }
    rt_thread_delay(800);
  }
}

```
实测输出：

```c
[D/main] e_key id = 0 state = 2
[D/main] e_key id = 1 state = 1
[D/main] e_key id = 1 state = 2
[D/main] e_key id = 2 state = 1
[D/main] e_key id = 2 state = 2
[D/main] e_key id = 3 state = 1
[D/main] e_key id = 3 state = 2
[D/main] e_key id = 0 state = 1
[D/main] e_key id = 0 state = 2
[D/main] e_key id = 0 state = 1
[D/main] e_key id = 0 state = 2
[D/main] e_key id = 0 state = 1
[D/main] e_key id = 0 state = 1
[D/main] e_key id = 0 state = 1
[D/main] e_key id = 0 state = 1
[D/main] e_key id = 0 state = 1
[D/main] e_key id = 0 state = 1
[D/main] e_key id = 0 state = 1
[D/main] e_key id = 0 state = 2
[D/main] e_key id = 1 state = 1
[D/main] e_key id = 1 state = 1
[D/main] e_key id = 1 state = 1
[D/main] e_key id = 1 state = 2

```

在上述代码中，我在main函数的while(1)中特意加入了800ms延时，实测没有漏掉一个按键状态，如果在使用中有漏按键的情况，请将FIFO缓冲区开大一点。默认是50，即缓冲50个按键状态。

```c
#define KEY_FIFO_SIZE 50 //按键FIFO缓冲大小
```
获取源码请点击[这里](https://github.com/xupenghu/MCUKeys)，如果有帮助到你，请给star支持！

联系作者：[xph](xupenghu@outlook.com)


