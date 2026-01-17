/**
 * @file Task_DM_onlineCheck.c
 * @author SJW
 * @brief 检测DM电机掉线并使能
 * @version 0.1
 * @date 2025-08-12
 *
 * @copyright Copyright (c)
 *
 */

#include "Task_DM_onlineCheck.h"

uint16_t DM_Frame = 0;

void DM_onlineCheck(void const *argument)
{

    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    const TickType_t TimeIncrement = pdMS_TO_TICKS(1);
    for (;;)
    {
        if (DM_Frame == DM_Array[0].InfoUpdateFrame)
        {
            DM_Enable(&hcan1, DM_SENDID_1);
        }
        DM_Frame = DM_Array[0].InfoUpdateFrame;
        vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
    }
}
