/**
 * @file Task_RobotControl.c
 * @author SJW
 * @brief
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
        ControlMes_board2.LF_track = -60;
        ControlMes_board2.LR_track = -60;
        ControlMes_board2.RR_track = -60;
        ControlMes_board2.RF_track = -60;
        SBUS_Handle();
        vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
    }
}
