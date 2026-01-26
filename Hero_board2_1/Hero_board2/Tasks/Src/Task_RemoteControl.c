/**
 * @file Task_RobotControl.c
 * @author SJW
 * @brief
 * @version 1.0
 * @date 2026-01-24
 * @copyright Copyright (c) 2026
 */

#include "Task_RemoteControl.h"

int32_t output;
/**
 * 线性映射函数：将 [-671, 671] 范围的 int16_t 映射到 [-60, 60]
 * @param x 输入值（范围：-671 ~ 671）
 * @return 输出值（范围：-60 ~ 60）
 */
int16_t linear_map(int16_t x)
{
    int32_t x_32 = (int32_t)x;
    int32_t y_32 = (60 * x_32 + 671 / 2) / 671;
    return (int16_t)y_32;
}

void RemoteControl_Processing(void const *argument)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    const TickType_t TimeIncrement = pdMS_TO_TICKS(5);
    for (;;)
    {
        ControlMes_board2.LF_track = -60;
        ControlMes_board2.LR_track = linear_map(mappedData.Ch9);
        ControlMes_board2.RR_track = linear_map(mappedData.Ch9);
        ControlMes_board2.RF_track = -60;
        SBUS_Handle();
        vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
    }
}
