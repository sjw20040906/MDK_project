/**
 * @file Task_J4310_onlineCheck.c
 * @author SJW
 * @brief 检测4310电机掉线并使能
 * @version 0.1
 * @date 2025-08-12
 *
 * @copyright Copyright (c)
 *
 */

#include "Task_J4310_onlineCheck.h"

void J4310_onlineCheck(void const *argument)
{

    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    const TickType_t TimeIncrement = pdMS_TO_TICKS(1);
    for (;;)
    {
        if (J4310s_Pitch.state == 0)
        {
            J4310_Enable();
        }
        if (J3519_Array[J3519_Dail_Wheel].state == 0)
        {
            J3519_Enable(&hcan1, 0x001);
        }
        vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
    }
}
