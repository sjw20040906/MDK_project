/**
 * @file Task_RobotControl.c
 * @author rm_control_team
 * @brief
 * @version 0.1
 * @date 2023-08-30
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "Task_RobotControl.h"

void Robot_Control(void const *argument)
{

    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    const TickType_t TimeIncrement = pdMS_TO_TICKS(2);
    for (;;)
    {
			System_Reset();
        Remote_Change();
        Cloud_Sport_Out();
        Shoot_Processing();
        /****************整合电流数据***************/
        uint8_t data1[8], data2[8], data3[8];
        M3508_setCurrent(M3508_Array[FricL_Wheel].outCurrent, M3508_Array[FricR_Wheel].outCurrent, 0, 0, data1);
        M2006_setCurrent(M2006_Array[Dial_Motor].outCurrent, 0, 0, 0, data2);
        J4310_setParameter(J4310s_Pitch.outPosition, J4310s_Pitch.outSpeed, J4310s_Pitch.outKp, J4310s_Pitch.outKd, J4310s_Pitch.outTorque, data3);
        /****************发送电流数据***************/
        CAN_SendData(CAN_SendHandle, &hcan1, CAN_ID_STD, M3508_SENDID_Fric_Dial, data1);
        CAN_SendData(CAN_SendHandle, &hcan2, CAN_ID_STD, M2006_SENDID_Dial, data2);
        CAN_SendData(CAN_SendHandle, &hcan1, CAN_ID_STD, J4310_SENDID_Pitch, data3);

        vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
    }
}
