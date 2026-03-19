/**
 * @file Task_RobotControl.c
 * @author rm_control_team
 * @brief
 * @version 0.1
 * @date 2023-08-30
 *
 * @copyright Copyright (c) 2021
 *24+，13-
 */

#include "Task_RobotControl.h"

void Robot_Control(void const *argument)
{

    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    const TickType_t TimeIncrement = pdMS_TO_TICKS(5);
    for (;;)
    {
        /****************整合电流数据***************/
        uint8_t data1[8], data2[8], data3[8], data4[8];
        DM_setParameter(DM_Offset + ControlMes_board2.LF_track / 29.25f, DM_Speed, DM_KP, DM_KD, DM_Torque, data1);
        DM_setParameter(DM_Offset - ControlMes_board2.LR_track / 29.25f, DM_Speed, DM_KP, DM_KD, DM_Torque, data2); 
        DM_setParameter(DM_Offset + ControlMes_board2.RR_track / 29.25f, DM_Speed, DM_KP, DM_KD, DM_Torque, data3);
        DM_setParameter(DM_Offset - ControlMes_board2.RF_track / 29.25f, DM_Speed, DM_KP, DM_KD, DM_Torque, data4);
        /****************发送电流数据***************/
        CAN_SendData(CAN_SendHandle, &hcan1, CAN_ID_STD, DM_LF, data1);
        CAN_SendData(CAN_SendHandle, &hcan1, CAN_ID_STD, DM_LR, data2);
        osDelay(1);
        CAN_SendData(CAN_SendHandle, &hcan1, CAN_ID_STD, DM_RR, data3);
        CAN_SendData(CAN_SendHandle, &hcan1, CAN_ID_STD, DM_RF, data4);
        /****************发送电流数据end***************/
        vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
    }
}
