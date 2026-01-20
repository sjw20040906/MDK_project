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
#include "DT7.h"

void DT7_Control(void const *argument)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    const TickType_t TimeIncrement = pdMS_TO_TICKS(5); 
    for (;;)
    {
        DT7_Handle();
        vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
    }
}
