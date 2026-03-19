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

void DM_onlineCheck(void const *argument)
{

    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    const TickType_t TimeIncrement = pdMS_TO_TICKS(2);
    for (;;)
    {
        if (!DM_Array[LF].state)
        {
            DM_Enable(&hcan1, DM_LF);
        }
        if (!DM_Array[LR].state)
        {
            DM_Enable(&hcan1, DM_LR);
        }
        osDelay(1);
        if (!DM_Array[RR].state)
        {
            DM_Enable(&hcan1, DM_RR);
        }
        if (!DM_Array[RF].state)
        {
            DM_Enable(&hcan1, DM_RF);
        }
        vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
    }
}
