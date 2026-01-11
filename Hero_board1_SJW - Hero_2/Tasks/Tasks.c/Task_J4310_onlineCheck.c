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

uint16_t pitch_Frame = 0;
uint16_t dial_Frame = 0;

void J4310_onlineCheck(void const *argument)
{

    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    const TickType_t TimeIncrement = pdMS_TO_TICKS(1);
    for (;;)
    {
        if (pitch_Frame == J4310s_Pitch.InfoUpdateFrame)
        {
            J4310_Fun.J4310_Enable();
        }
        pitch_Frame = J4310s_Pitch.InfoUpdateFrame;

        if (dial_Frame == J3519_Array[J3519_Dail_Wheel].InfoUpdateFrame)
        {
            J3519_Fun.J3519_Enable(&hcan1, 0x001);
        }
        dial_Frame = J3519_Array[J3519_Dail_Wheel].InfoUpdateFrame;

        vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
    }
}
