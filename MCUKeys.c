/*
 * Copyright (c) 2018-2020
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-06-01     xph      the first version
 */
#include "MCUKeys.h"

/*******************全局变量定义***********************/
t_keys_fifo keys_fifo; //按键缓冲FIFO
static key *head = NULL;

/**********************************函数实现********************************************/

/***********************************************************************
* 函数名称： key_in_fifo
* 函数功能： 将一个按键状态放入FIFO中
* 输入参数：  具体按键状态
* 返 回  值： 无
* 函数说明：无
****************************************************************************/
void key_in_fifo(event_key e_key)
{
    keys_fifo.fifo_buffer[keys_fifo.write].id = e_key.id;
    keys_fifo.fifo_buffer[keys_fifo.write].state = e_key.state;
    if (++keys_fifo.write >= KEY_FIFO_SIZE)
    {
        keys_fifo.write = 0;
    }
}

/***********************************************************************
* 函数名称： key_out_fifo
* 函数功能： 从按键FIFO中取出一个按键事件
* 输入参数：  无
* 返 回  值：  当前FIFO中缓冲的按键事件
* 函数说明：无
****************************************************************************/
event_key key_out_fifo(void)
{
    event_key ret;

    if (keys_fifo.read == keys_fifo.write)
    {
        ret.id = KEY_NONE;
        ret.state = KEY_NONE;
    }
    else
    {
        ret = keys_fifo.fifo_buffer[keys_fifo.read];
        if (++keys_fifo.read >= KEY_FIFO_SIZE)
        {
            keys_fifo.read = 0;
        }
    }
		return ret;
}

/***********************************************************************
* 函数名称： clear_keys_fifo
* 函数功能： 清除按键缓冲区
* 输入参数：  无
* 返 回  值：  无
* 函数说明：   无
****************************************************************************/
void clear_keys_fifo(void)
{
    keys_fifo.read = keys_fifo.write;
}

/***********************************************************************
* 函数名称： detect_key
* 函数功能： 检测一个按键的状态并将状态值放入FIFO中
* 输入参数： 按键句柄
* 返 回  值：  无
* 函数说明：  
****************************************************************************/
static void detect_key(key *handle)
{
    uint8_t current_key_state = handle->get_key_status(handle->e_key); /* 获取按键状态 */
    switch (handle->key_machine)
    {
    case KEY_MACHINE_NULL:
    {
        /* 按键按下 */
        if (current_key_state == KEY_PRESS)
        {
            handle->key_machine = KEY_MACHINE_DOWN;
        }
        /* 双击事件计数 */
        handle->double_count += KEY_TICKS; 
        break;
    }
    case KEY_MACHINE_DOWN:
    {
        /* 如果状态还在保持 */
        if (current_key_state == handle->prev_key_state)
        {
            handle->key_machine = KEY_MACHINE_DOWN_RECHECK;
            handle->e_key.state = KEY_DOWN;
            /* 如果定义了按键按下上报功能 */
            if (handle->report_flag & KEY_REPORT_DOWN) 
            {
                /* 存入按键按下事件 */
                key_in_fifo(handle->e_key); 
            }
            /* 如果注册了回调函数 则执行 */
            if (handle->key_event_cb) 
            {
                handle->key_event_cb(handle->e_key);
            }
        }
        else
        {
            handle->key_machine = KEY_MACHINE_NULL;
        }
        break;
    }
    /* 长按和连发和按键抬起判断 */
    case KEY_MACHINE_DOWN_RECHECK:
    {
        /* 按键还在保持按下状态 */
        if (current_key_state == handle->prev_key_state)
        {
            if (handle->long_time > 0)
            {
                if ((handle->long_count += KEY_TICKS) == handle->long_time)
                {
                    handle->e_key.state = KEY_LONG;
                    if (handle->report_flag & KEY_REPORT_LONG)
                    {
                        /* 键值放入按键FIFO */
                        key_in_fifo(handle->e_key); 
                    }
                   /* 如果注册了回调函数 则执行 */
                    if (handle->key_event_cb) 
                    {
                        handle->key_event_cb(handle->e_key);
                    }
                }
                else
                {
                    if (handle->repeat_speed > 0)
                    {
                        if ((handle->repeat_count += KEY_TICKS) >= handle->repeat_speed)
                        {
                            handle->repeat_count = 0;
                            handle->e_key.state = KEY_DOWN;
                            if (handle->report_flag & KEY_REPORT_REPEAT) 
                            {
                                /* 长按按键后，每隔repeat_speed发送1个按键 */
                                key_in_fifo(handle->e_key);
                            }
                            /* 如果注册了回调函数 则执行 */
                            if (handle->key_event_cb) 
                            {
                                handle->key_event_cb(handle->e_key);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            /* 按键已经抬起 */
            handle->key_machine = KEY_MACHINE_UP;
        }
        break;
    }
    case KEY_MACHINE_UP:
    {
        if (current_key_state == handle->prev_key_state)
        {
            handle->key_machine = KEY_MACHINE_UP_RECHECK;
            handle->long_count = 0;   /* 长按计数清零 */
            handle->repeat_count = 0; /* 重复发送计数清零 */
            handle->e_key.state = KEY_UP;
            if (handle->report_flag & KEY_REPORT_UP)
            {
                /* 发送按钮弹起的消息 */
                key_in_fifo(handle->e_key);
            }
            /* 如果注册了回调函数 则执行 */
            if (handle->key_event_cb) 
            {
                handle->key_event_cb(handle->e_key);
            }
        }
        else
        {
            handle->key_machine = KEY_MACHINE_DOWN_RECHECK;
        }
        break;
    }
    case KEY_MACHINE_UP_RECHECK:
    {
        /* 满足双击要求 */
        if ((handle->double_count > KEY_DOUBLE_MIN) && (handle->double_count < KEY_DOUBLE_MAX))
        {
            handle->double_count = 0;
            handle->e_key.state = KEY_DOUBLE;
            if (handle->report_flag & KEY_REPORT_DOUBLE) 
            {
                key_in_fifo(handle->e_key); 
            }
            /* 如果注册了回调函数 则执行 */
            if (handle->key_event_cb) 
            {
                handle->key_event_cb(handle->e_key);
            }
        }
        else
        {
            /* 不满足双击要求 清零计数器 */
            handle->double_count = 0;
        }
        handle->key_machine = KEY_MACHINE_NULL;
    }
    default:
    {
        break;
    }
    }
    handle->prev_key_state = current_key_state;
}

/***********************************************************************
* 函数名称： key_scan
* 函数功能： 按键扫描
* 输入参数： 无
* 返 回  值：  无
* 函数说明：  应该放在一个固定的周期函数中
****************************************************************************/
void key_scan(void)
{
    struct _key_t *now_key;
    for (now_key = head; now_key != NULL; now_key = now_key->next)
    {
        detect_key(now_key);
    }
}

/***********************************************************************
* 函数名称： key_add
* 函数功能： 增加按键
* 输入参数： 按键句柄
* 返 回  值： 0 增加成功 其他 失败
* 函数说明： 按键初始化时应当调用此函数
****************************************************************************/
int key_add(struct _key_t *curr)
{
    key *tail = head;
    
    while(tail) 
    {
        if(tail == curr) 
        {
            return -1; /* 已经加入了 */
        }
        tail = tail->next;
    }
    
    curr->next = head;
    head = curr; /* 头插法 */ 
    
    return 0;
}

/***********************************************************************
* 函数名称： key_init
* 函数功能： 初始化按键，默认功能
* 输入参数： 按键句柄
* 返 回  值： 0 增加成功 其他 失败
* 函数说明： 按键初始化时应当调用此函数
****************************************************************************/
void key_init(struct _key_t *curr)
{
    curr->e_key.id = 0;
    curr->e_key.state = 0;
    curr->double_count = 0;
    curr->get_key_status = NULL;
    curr->key_event_cb = NULL;
    curr->long_count = 0;
    curr->long_time = KEY_LONG_TIME;
    curr->repeat_count = 0;
    curr->repeat_speed = KEY_REPEAT_TIME;
    curr->state = KEY_LIFT;
    curr->next = NULL;
    curr->report_flag = KEY_REPORT_DOUBLE|KEY_REPORT_DOWN|KEY_REPORT_UP|KEY_REPORT_REPEAT|KEY_REPORT_LONG;
    curr->prev_key_state = KEY_LIFT;
    curr->key_machine = KEY_MACHINE_NULL;

}
/***********************************************************************
* 函数名称： key_detach
* 函数功能： 销毁按键
* 输入参数： 按键句柄
* 返 回  值： 0 增加成功 其他 失败
* 函数说明： 销毁只是不参与调度 内存并没有释放 适当时候还是可以调用key_add加入调度的
*           后续根据反馈看是否增加
****************************************************************************/
int key_detach(struct _key_t *curr)
{
	return 0;
}



