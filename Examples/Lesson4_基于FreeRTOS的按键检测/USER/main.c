#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "stdlib.h"
#include "oled.h" 
#include "FreeRTOS.h"
#include "task.h"

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define LED0_TASK_PRIO		2
//任务堆栈大小	
#define LED0_STK_SIZE 		50  
//任务句柄
TaskHandle_t LED0Task_Handler;
//任务函数
void led0_task(void *pvParameters);

//任务优先级
#define OLEDRefreshGramTask_PRIO		3
//任务堆栈大小	
#define OLEDRefreshGramTask_STK_SIZE 		256  
//任务句柄
TaskHandle_t OLEDRefreshGramTask_Handler;
//任务函数
void OLEDRefreshGramTask(void *pvParameters);

int main(void)
{ 
 
	delay_init(168);		  //初始化延时函数
	LED_Init();		        //初始化LED端口
	OLED_Init();			//初始化OLED  
	OLED_Clear();
	OLED_ShowString(0,0,"MCUKeys with FreeRTOS",16);

	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
                
    return 0;
}



//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    //创建LED0任务
    xTaskCreate((TaskFunction_t )led0_task,     	
                (const char*    )"led0_task",   	
                (uint16_t       )LED0_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LED0_TASK_PRIO,	
                (TaskHandle_t*  )&LED0Task_Handler);   
    //创建LED1任务
    xTaskCreate((TaskFunction_t )OLEDRefreshGramTask,     
                (const char*    )"OLED_Refresh_Gram_task",   
                (uint16_t       )OLEDRefreshGramTask_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )OLEDRefreshGramTask_PRIO,
                (TaskHandle_t*  )&OLEDRefreshGramTask_Handler);        

    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//LED0任务函数 
void led0_task(void *pvParameters)
{
    while(1)
    {
        LED3=~LED3;
        vTaskDelay(100);
    }
}   

//OLED刷新任务
void OLEDRefreshGramTask(void *pvParameters)
{

    static portTickType xLastWakeTime;  
    const portTickType xFrequency = pdMS_TO_TICKS(20);  //50Hz刷新率
   
    // 使用当前时间初始化变量xLastWakeTime ,注意这和vTaskDelay()函数不同 
    xLastWakeTime = xTaskGetTickCount();
 
    while(1)
    {
        OLED_Refresh_Gram();//更新显示
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}


 



