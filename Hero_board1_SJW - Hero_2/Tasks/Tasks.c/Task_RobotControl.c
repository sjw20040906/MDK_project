/**
 * @file Task_RobotControl.c
 * @author SJW
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
    portTickType xLastWakeTime;                        // 记录任务上次唤醒的时间点，用于精确周期控制。
    xLastWakeTime = xTaskGetTickCount();               // 获取当前系统时钟计数
    const TickType_t TimeIncrement = pdMS_TO_TICKS(2); // 每3毫秒强制进入数据发送

    for (;;)
    {
        Remote_Change();    // 变速小陀螺
        Cloud_Sport_Out();  // 云台运动控制
        Shoot_Processing(); // 发射机构

        /****************整合电流数据***************/
        uint8_t data1[8], data2[8], data3[8];
        M3508_setCurrent(M3508_Array[Fric_Left].outCurrent, M3508_Array[Fric_Right].outCurrent, M3508_Array[Dial_Wheel].outCurrent, 0, data1);
        J4310_setParameter(J4310s_Pitch.outPosition, J4310s_Pitch.outSpeed, J4310s_Pitch.outKp, J4310s_Pitch.outKd, J4310s_Pitch.outTorque, data2);
        J3519_setParameter(J3519_Array[J3519_Dail_Wheel].outPosition, J3519_Array[J3519_Dail_Wheel].outSpeed, J3519_Array[J3519_Dail_Wheel].outKp, J3519_Array[J3519_Dail_Wheel].outKd, J3519_Array[J3519_Dail_Wheel].outTorque, data3);
        /****************发送电流数据***************/
        CAN_SendData(CAN_SendHandle, &hcan1, CAN_ID_STD, M3508_SENDID_Fric_Dial, data1);
        CAN_SendData(CAN_SendHandle, &hcan2, CAN_ID_STD, J4310_SENDID_Pitch, data2);
        CAN_SendData(CAN_SendHandle, &hcan1, CAN_ID_STD, J3519_SENDID, data3);
        /****************发送电流数据 end***************/

        vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
    }
}
