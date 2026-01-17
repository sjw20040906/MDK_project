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

uint16_t DM_Frame_LF = 0;
uint16_t DM_Frame_LR = 0;
uint16_t DM_Frame_RR = 0;
uint16_t DM_Frame_RF = 0;

void DM_onlineCheck(void const *argument)
{

    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    const TickType_t TimeIncrement = pdMS_TO_TICKS(5);
    for (;;)
    {
        if (DM_Frame_LF == DM_Array[LF].InfoUpdateFrame)
        {
            DM_Enable(&hcan1, DM_LF);
        }
        DM_Frame_LF = DM_Array[LF].InfoUpdateFrame;
        osDelay(1);
        if (DM_Frame_LR == DM_Array[LR].InfoUpdateFrame)
        {
            DM_Enable(&hcan1, DM_LR);
        }
        DM_Frame_LR = DM_Array[LR].InfoUpdateFrame;
        osDelay(1);
        if (DM_Frame_RR == DM_Array[RR].InfoUpdateFrame)
        {
            DM_Enable(&hcan2, DM_RR);
        }
        DM_Frame_RR = DM_Array[RR].InfoUpdateFrame;
        osDelay(1);
        if (DM_Frame_RF == DM_Array[RF].InfoUpdateFrame)
        {
            DM_Enable(&hcan2, DM_RF);
        }
        DM_Frame_RF = DM_Array[RF].InfoUpdateFrame;
        vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
    }
}
