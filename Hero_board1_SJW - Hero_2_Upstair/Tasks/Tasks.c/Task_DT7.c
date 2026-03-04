/**
 * @file Task_RobotControl.c
 * @author ZS
 * @brief
 * @version 0.1
 * @date 2025-03-3
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "SBUS.h"

void SBUS_Control(void const *argument)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    const TickType_t TimeIncrement = pdMS_TO_TICKS(5); // 每5毫秒强制进入数据发送
    for (;;)
    {
        SBUS_Handle(); // 处理SBUS遥控器数据
        vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
    }
}
