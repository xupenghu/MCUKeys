/*
 * Copyright (c) 2018-2020
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-06-01     xph      the first version
 */
#ifndef _MCUKEYS__H
#define _MCUKEYS__H

#include <stdint.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

/* 
 * 按键循环扫描周期(ms) key_scan()函数在哪个固定扫描周期中 该值就等于多少 一般设置为10ms
 */
#define KEY_TICKS 10
#define KEY_LIFT (0)  /* 按键抬起 */
#define KEY_PRESS (1) /* 按键按下 */

#define KEY_FILTER_TIME 10  // 按键消抖时间 10ms
#define KEY_LONG_TIME 2000  // 长按时间 2s
#define KEY_REPEAT_TIME 200 // 连发间隔200ms
#define KEY_DOUBLE_MIN 100  // 双击按键最小间隔 100ms
#define KEY_DOUBLE_MAX 300  // 双击按键最大间隔 300ms

#define KEY_FIFO_SIZE 50 //按键FIFO缓冲大小

//上报事件标志
#define KEY_REPORT_DOWN (1 << 0)   //上报按键按下事件
#define KEY_REPORT_UP (1 << 1)     //上报按键抬起事件
#define KEY_REPORT_LONG (1 << 2)   //上报长按事件
#define KEY_REPORT_DOUBLE (1 << 3) //上报双击事件
#define KEY_REPORT_REPEAT (1 << 4) //上报连发事件

/* 按键事件的几种状态 */
enum
{
    KEY_NONE = 0, /* 没有按键按下 */
    KEY_DOWN,     /* 按键按下事件 */
    KEY_UP,       /* 按键抬起事件 */
    KEY_LONG,     /* 按键长按事件 */
    KEY_DOUBLE,   /* 按键双击事件 */
};

typedef struct event_key_t
{
    uint8_t id : 5;    /* 最大支持32个按键 如果按键数量更多则修改此值*/
    uint8_t state : 3; /* 存放按键状态 */
} event_key;

//按键状态机
typedef enum _e_key_machine
{
    KEY_MACHINE_NULL, //无按键按下
    KEY_MACHINE_DOWN,
    KEY_MACHINE_DOWN_RECHECK,
    KEY_MACHINE_UP,
    KEY_MACHINE_UP_RECHECK,
    KEY_MACHINE_LONG,
    KEY_MACHINE_REPEAT,

} e_key_machine;

//按键结构体
typedef struct _key_t
{
    event_key e_key;                                  /* 记录按键ID和状态*/
    uint8_t (*get_key_status)(const event_key e_key); /*  按键按下的判断函数 和硬件相关 */
    void (*key_event_cb)(const event_key e_key);      /* 按键事件处理回调函数 */
    uint16_t long_count;                              /* 长按计数器 */
    uint16_t long_time;                               /* 按键按下持续时间, 0表示不检测长按 */
    uint8_t state;                                    /* 按键当前状态（按下还是弹起） */
    uint16_t repeat_speed;                            /* 连续按键周期 */
    uint16_t repeat_count;                            /* 连续按键计数器 */
    uint16_t double_count;                            /* 双击计数器*/
    uint8_t report_flag;                              /* 上报事件标志*/
    e_key_machine key_machine;                        /* 按键状态机*/
    uint8_t prev_key_state;                           /* 上一次按键的状态 */
    struct _key_t *next;                              /* 按键链表 */

} key;

/* 按键FIFO用到变量 */
typedef struct _t_keys_fifo
{
    event_key fifo_buffer[KEY_FIFO_SIZE]; /* 键值缓冲区 */
    uint8_t read;                         /* 缓冲区读指针1 */
    uint8_t write;                        /* 缓冲区写指针 */

} t_keys_fifo;

/***********************************************************************
* 函数名称： key_scan
* 函数功能： 按键扫描
* 输入参数： 无
* 返 回  值：  无
* 函数说明：  应该放在一个1ms的周期函数中
****************************************************************************/
void key_scan(void);

/***********************************************************************
* 函数名称： key_add
* 函数功能： 按键模块初始化
* 输入参数：  无
* 返 回  值： 无
* 函数说明： 系统初始化时应当调用此函数,加入调度
****************************************************************************/
int key_add(struct _key_t *curr);

/***********************************************************************
* 函数名称： key_init
* 函数功能： 初始化按键
* 输入参数： 按键句柄
* 返 回  值： 0 增加成功 其他 失败
* 函数说明： 按键初始化时应当调用此函数
****************************************************************************/
void key_init(struct _key_t *curr);

/***********************************************************************
* 函数名称： key_out_fifo
* 函数功能： 从按键FIFO中取出一个按键事件
* 输入参数：  无
* 返 回  值： 当前FIFO中缓冲的按键ID和事件
* 函数说明：无
****************************************************************************/
event_key key_out_fifo(void);

#endif
