/**
 * @file Task_RobotControl.c
 * @author SJW
 * @brief 遥控器任务，处理遥控器输入
 * @version 1.0
 * @date 2026-01-24
 * @copyright Copyright (c) 2026
 */

#include "Task_RemoteControl.h"

void RemoteControl_Processing(void const *argument)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    const TickType_t TimeIncrement = pdMS_TO_TICKS(5);
    for (;;)
    {
        /*********** 机器人上下板通信 ************/
        Board2_To_1();
        osDelay(1);
        Board2_1_To_Board2_2();
        vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
    }
}
